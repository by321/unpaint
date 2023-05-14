﻿#pragma once
#include "InferenceViewModel.g.h"
#include "ModelRepository.h"
#include "StableDiffusionModelExecutor.h"

namespace winrt::Unpaint::implementation
{
  struct InferenceViewModel : InferenceViewModelT<InferenceViewModel>
  {
    InferenceViewModel();

    hstring PositivePromptPlaceholder();
    hstring PositivePrompt();
    void PositivePrompt(hstring const& value);

    hstring NegativePromptPlaceholder();
    hstring NegativePrompt();
    void NegativePrompt(hstring const& value);

    Windows::Foundation::Collections::IObservableVector<hstring> Models();
    int32_t SelectedModelIndex();
    void SelectedModelIndex(int32_t value);

    Windows::Foundation::Collections::IObservableVector<Windows::Graphics::SizeInt32> Resolutions();
    int32_t SelectedResolutionIndex();
    void SelectedResolutionIndex(int32_t value);

    float GuidanceStrength();
    void GuidanceStrength(float value);

    float DenoisingStrength();
    void DenoisingStrength(float value);
    
    uint32_t SamplingSteps();
    void SamplingSteps(uint32_t value);

    uint32_t RandomSeed();
    void RandomSeed(uint32_t value);

    bool IsSeedFrozen();
    void IsSeedFrozen(bool value);

    hstring Status();
    void Status(hstring const& value);

    float Progress();
    void Progress(float value);

    Windows::UI::Xaml::Media::ImageSource OutputImage();
    void OutputImage(Windows::UI::Xaml::Media::ImageSource const& value);

    fire_and_forget GenerateImage();

    event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value);
    void PropertyChanged(event_token const& token);

  private:
    std::shared_ptr<ModelRepository> _modelRepository;
    std::shared_ptr<StableDiffusionModelExecutor> _modelExecutor;

    std::minstd_rand _random;
    std::uniform_int_distribution<uint32_t> _seedDistribution;

    event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> _propertyChanged;

    hstring _positivePrompt, _negativePrompt;
    
    Windows::Foundation::Collections::IObservableVector<hstring> _models;
    int32_t _selectedModelIndex;

    Windows::Foundation::Collections::IObservableVector<Windows::Graphics::SizeInt32> _resolutions;
    int32_t _selectedResolutionIndex;
    
    float _guidanceStrength, _denoisingStrength;
    uint32_t _samplingSteps, _randomSeed;
    bool _isSeedFrozen;

    hstring _status;
    float _progress;

    Windows::UI::Xaml::Media::ImageSource _outputImage;
  };
}

namespace winrt::Unpaint::factory_implementation
{
  struct InferenceViewModel : InferenceViewModelT<InferenceViewModel, implementation::InferenceViewModel>
  {
  };
}
