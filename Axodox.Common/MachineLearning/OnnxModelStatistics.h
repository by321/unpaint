#pragma once
#ifdef USE_ONNX
#include "OnnxEnvironment.h"

namespace Axodox::MachineLearning
{
  void OnnxPrintStatistics(OnnxEnvironment& environment, Ort::Session& session);
}
#endif