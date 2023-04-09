#include "pch.h"
#include "StableDiffustionInferer.h"
#include "VaeDecoder.h"
#include "OnnxModelStatistics.h"

using namespace DirectX;
using namespace Ort;
using namespace std;

namespace Axodox::MachineLearning
{
  StableDiffusionInferer::StableDiffusionInferer(OnnxEnvironment& environment) :
    _environment(environment),
    _session(nullptr)
  {
    _session = { _environment.Environment(), (_environment.RootPath() / L"unet/model.onnx").c_str(), _environment.DefaultSessionOptions() };
    OnnxPrintStatistics(_environment, _session);
  }

  Tensor StableDiffusionInferer::RunInference(const StableDiffusionOptions& options)
  {
    StableDiffusionContext context{
      .Options = options,
      .Random = minstd_rand{options.Seed}
    };

    list<Tensor> derivatives;

    auto latentSample = GenerateLatentSample(context);
    auto textEmbeddings = options.TextEmbeddings.ToOrtValue(_environment.MemoryInfo());

    auto steps = context.Scheduler.GetSteps(options.StepCount);

    IoBinding binding{ _session };
    binding.BindOutput("out_sample", _environment.MemoryInfo());
    binding.BindInput("encoder_hidden_states", textEmbeddings);

    for (size_t i = 0; i < steps.Timesteps.size(); i++)
    {
      printf("Step %zd\n", i);
      auto scaledSample = latentSample.Duplicate() / sqrt(steps.Sigmas[i] * steps.Sigmas[i] + 1);

      binding.BindInput("sample", scaledSample.ToOrtValue(_environment.MemoryInfo()));
      binding.BindInput("timestep", Tensor(steps.Timesteps[i]).ToOrtValue(_environment.MemoryInfo()));

      _session.Run({}, binding);

      auto outputs = binding.GetOutputValues();
      auto output = Tensor::FromOrtValue(outputs[0]);
      auto outputComponents = output.Split();

      auto& blankNoise = outputComponents[0];
      auto& textNoise = outputComponents[1];
      auto guidedNoise = blankNoise.BinaryOperation<float>(textNoise, [guidanceScale = options.GuidanceScale](float a, float b) 
        { return a + guidanceScale * (b - a); });

      latentSample = steps.ApplyStep(latentSample, guidedNoise, derivatives, context.Random, i);
    }

    latentSample = latentSample * (1.0f / 0.18215f);
    return latentSample;
  }
  
  Tensor StableDiffusionInferer::GenerateLatentSample(StableDiffusionContext& context)
  {
    Tensor::shape_t shape{ context.Options.BatchSize, 4, context.Options.Height / 8, context.Options.Width / 8 };
    return Tensor::CreateRandom(shape, context.Random, context.Scheduler.InitialNoiseSigma());
  }
}