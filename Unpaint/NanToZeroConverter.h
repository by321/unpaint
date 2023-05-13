#pragma once
#include "NanToZeroConverter.g.h"

namespace winrt::Unpaint::implementation
{
  struct NanToZeroConverter : NanToZeroConverterT<NanToZeroConverter>
  {
    NanToZeroConverter() = default;

    Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
  };
}
namespace winrt::Unpaint::factory_implementation
{
  struct NanToZeroConverter : NanToZeroConverterT<NanToZeroConverter, implementation::NanToZeroConverter>
  {
  };
}
