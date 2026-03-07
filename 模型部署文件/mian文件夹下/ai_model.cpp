#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/c/common.h"

#include "ai_model.h"
#include "model.h"

// ============== global没有变动 =============== 
// Globals, used for compatibility with Arduino-style sketches.
namespace {
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
int inference_count = 0;  // 记录已经做了多少次推理,可删 // 可改

constexpr int kTensorArenaSize = 10 * 1024;  // 如果模型大了，这个值(内存区域)要增大 // 可改
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// ============== 变量 ==============


// ============== 函数 ==============
void HandleOutput(float temp, float humi, float gray, float risk_score)
{
    MicroPrintf("T:%.2f H:%.2f L:%.2f Risk:%.2f\n", temp, humi, gray, risk_score);
}

// ============== AI模型初始化 =============== 

// The name of this function is important for Arduino compatibility.
void Ai_Init(void) 
{
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_model);    // 模型导入 // 可改:g_model改成自己的模型的extern数组名字
  if (model->version() != TFLITE_SCHEMA_VERSION)  // 检查模型版本
  {
    MicroPrintf("Model provided is schema version %d not equal to supported "
                "version %d.", model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // ======================= 改动区域 =======================
  // Pull in only the operation implementations we need.
  // 换模型时,如果模型中有其他操作(如 Conv2D、ReLU),这里要 AddConv2D(),AddRelu() 等
  static tflite::MicroMutableOpResolver<2> resolver;  // 可改,新模型可能需要卷积、激活函数、池化等操作，需要在这里全部注册，否则解释器会报错 op not supported,反正是看有几个(不是几种)模型就注册几个
  // 改
  resolver.AddRelu();    // 如果模型有 relu
  // resolver.AddTanh();    // 如果模型有 tanh
  if (resolver.AddFullyConnected() != kTfLiteOk) // 可改 , 可增
  {
    return;
  }
  // ======================= 改动区域end =======================

  // 分配内存
  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  // 保存模型指针,方便后续使用
  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) 
  {
    MicroPrintf("AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);

  // Keep track of how many inferences we have performed.
  inference_count = 0;
}

// ============== AI模型loop =============== 

float Ai_Predict(float temp , float humi , float gray) // py不含量化版
{
  // ---------------- 输入 ----------------
  // FP32 模型直接写入 float
  input->data.f[0] = temp;
  input->data.f[1] = humi;
  input->data.f[2] = gray;

  // ---------------- 推理 ----------------
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) 
  {
    MicroPrintf("Invoke failed on\n");
    return -1.0f;
  }

  // ---------------- 输出 ----------------
  // FP32 模型直接读取 float
  float y = output->data.f[0];
  if (y < 0.0f) y = 0.0f;        // 不允许负数

  // 打印结果
  HandleOutput(temp , humi , gray , y );

  return y;
}

// void Ai_Predict_test(void)  // 原汁原味的原函数
// {
//   // Calculate an x value to feed into the model. We compare the current
//   // inference_count to the number of inferences per cycle to determine
//   // our position within the range of possible x values the model was
//   // trained on, and use this to calculate a value.
//   float position = static_cast<float>(inference_count) /
//                    static_cast<float>(kInferencesPerCycle);
//   float x = position * kXrange; 

//   // Quantize the input from floating-point to integer
//   int8_t x_quantized = x / input->params.scale + input->params.zero_point;
//   // Place the quantized input in the model's input tensor
//   input->data.int8[0] = x_quantized;

//   // Run inference, and report any error
//   TfLiteStatus invoke_status = interpreter->Invoke();
//   if (invoke_status != kTfLiteOk) {
//     MicroPrintf("Invoke failed on x: %f\n",
//                          static_cast<double>(x));
//     return ;
//   }

//   // Obtain the quantized output from model's output tensor
//   int8_t y_quantized = output->data.int8[0];
//   // Dequantize the output from integer to floating-point
//   float y = (y_quantized - output->params.zero_point) * output->params.scale;

//   // Output the results. A custom HandleOutput function can be implemented
//   // for each supported hardware target.
//   // HandleOutput(x, y);

//   // Increment the inference_counter, and reset it if we have reached
//   // the total number per cycle
//   inference_count += 1;
//   if (inference_count >= kInferencesPerCycle) inference_count = 0;
// }

