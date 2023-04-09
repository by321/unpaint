// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

#include "UI/Window.h"
#include "Infrastructure/Events.h"
#include "Graphics/Swap Chains/HwndSwapChain.h"
#include "Storage/FileStream.h"
#include "Storage/FileIO.h"

#include "MachineLearning/TextTokenizer.h"
#include "MachineLearning/TextEncoder.h"
#include "MachineLearning/StableDiffustionInferer.h"
#include "MachineLearning/VaeDecoder.h"

using namespace Axodox::Graphics;
using namespace Axodox::Infrastructure;
using namespace Axodox::UI;
using namespace Axodox::Storage;
using namespace Axodox::MachineLearning;
using namespace std;
using namespace winrt;


TextureData ReadPfm(const std::filesystem::path& path)
{
  //Create stream
  file_stream stream{ path, file_mode::read };

  //Read header
  auto magic = stream.read_line();
  auto resolution = stream.read_line();
  uint32_t width, height;
  sscanf_s(resolution.c_str(), "%u %u", &width, &height);

  auto bias = stream.read_line();

  //Read depth data
  TextureData result{ width, height, DXGI_FORMAT_R32_FLOAT };
  stream.read(result.Buffer);

  return result;
}

void test()
{
  std::cout << "Hello World!\n";

  auto textureData = TextureData::FromBuffer(read_file(L"C:\\dev\\ai\\MiDaS\\input\\test.png"));
  auto depthData = ReadPfm(L"C:\\dev\\ai\\MiDaS\\output\\test-dpt_beit_large_512.pfm");

  auto depthBuffer = depthData.Cast<float>();

  Window window{ L"Hello world!" };

  GraphicsDevice device{};
  HwndSwapChain swapChain{ device, *window };

  window.Painting(no_revoke, [&](Window*) {
    swapChain.BackBuffer()->Clear();
    swapChain.Present();
    });

  window.Resized(no_revoke, [&](Window*) {
    swapChain.Resize();
    });

  window.Show();
  window.Run();
}

int main()
{
  winrt::init_apartment();

  OnnxEnvironment onnxEnvironment{ L"C:/dev/ai/realistic_vision_v1.4-fp16-vram" };
  //OnnxEnvironment onnxEnvironment{ L"C:/dev/StableDiffusion/StableDiffusion" };

  //Create text embeddings
  Tensor textEmbeddings{ TensorType::Single, 2, 77, 768 };
  {
    //Encode text
    TextTokenizer textTokenizer{ onnxEnvironment };
    TextEncoder textEncoder{ onnxEnvironment };

    auto tokenizedBlank = textTokenizer.GetUnconditionalTokens();
    auto encodedBlank = textEncoder.EncodeText(tokenizedBlank);

    auto tokenizedText = textTokenizer.TokenizeText("a stag standing in a misty forest at dawn, closeup");
    auto encodedText = textEncoder.EncodeText(tokenizedText);

    auto pSourceBlank = encodedBlank.AsPointer<float>();
    auto pSourceText = encodedText.AsPointer<float>();
    auto pTargetBlank = textEmbeddings.AsPointer<float>(0);
    auto pTargetText = textEmbeddings.AsPointer<float>(1);

    auto size = encodedText.Size();
    memcpy(pTargetBlank, pSourceBlank, size * 4);
    memcpy(pTargetText, pSourceText, size * 4);
  }

  //Run stable diffusion
  Tensor latentResult;
  {
    StableDiffusionInferer stableDiffusion{ onnxEnvironment };

    StableDiffusionOptions options{
      .StepCount = 20,
      .Width = 768,
      .Height = 768,
      .Seed = 60,
      .TextEmbeddings = textEmbeddings
    };

    latentResult = stableDiffusion.RunInference(options);
  }

  //Decode VAE
  {
    VaeDecoder vaeDecoder{ onnxEnvironment };
    auto imageTensor = vaeDecoder.DecodeVae(latentResult);

    auto imageTexture = imageTensor.ToTextureData();
    auto pngBuffer = imageTexture[0].ToBuffer();
    write_file(L"bin/test.png", pngBuffer);
  }

  //Done
  printf("done.");

  return 0;
}