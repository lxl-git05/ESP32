#include <stdio.h>
#include "ai_model.h"
#include "model_data.h"

// 不再用 all_ops_resolver
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
// #include "tensorflow/lite/version.h"

/************************************************************
 *                 1?? 归一化参数
 ************************************************************/
#define TEMP_MEAN   22.95f
#define TEMP_STD    2.98f

#define HUMI_MEAN   50.12f
#define HUMI_STD    9.87f

#define GRAY_MEAN   0.198f
#define GRAY_STD    0.14f

#define NORM_TEMP(x)   (((x) - TEMP_MEAN) / TEMP_STD)
#define NORM_HUMI(x)   (((x) - HUMI_MEAN) / HUMI_STD)
#define NORM_GRAY(x)   (((x) - GRAY_MEAN) / GRAY_STD)

/************************************************************
 *                 2?? Tensor Arena
 ************************************************************/
#define TENSOR_ARENA_SIZE (15 * 1024)
static uint8_t tensor_arena[TENSOR_ARENA_SIZE];

/************************************************************
 *                 3?? TFLite 变量
 ************************************************************/
static const tflite::Model* model = NULL;
static tflite::MicroInterpreter* interpreter = NULL;
static TfLiteTensor* input = NULL;
static TfLiteTensor* output = NULL;

/************************************************************
 *                 4?? 自定义 Resolver
 ************************************************************/
static tflite::MicroMutableOpResolver<5> resolver; // 5 表示最多注册5个算子

void RegisterOps() {
    resolver.AddFullyConnected();
    resolver.AddSoftmax();
    resolver.AddRelu();   // 如果你的模型有 ReLU 激活
    // resolver.AddAdd(); // 如果模型里有加法也可以注册
}

/************************************************************
 *                 5?? 初始化函数
 ************************************************************/
void ai_model_init(void)
{
    model = tflite::GetModel(model_data);

    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        printf("Model schema mismatch!\n");
        return;
    }

    // 注册算子
    RegisterOps();

    static tflite::MicroInterpreter static_interpreter(
        model,
        resolver,
        tensor_arena,
        TENSOR_ARENA_SIZE
    );

    interpreter = &static_interpreter;

    if (interpreter->AllocateTensors() != kTfLiteOk)
    {
        printf("AllocateTensors failed\n");
        return;
    }

    input = interpreter->input(0);
    output = interpreter->output(0);

    printf("AI Model Init Done\n");
}

/************************************************************
 *                 6?? 推理函数
 ************************************************************/
int ai_predict_level(float temp, float humi, float gray)
{
    if (input == NULL || output == NULL)
        return -1;

    /************ 归一化 ************/
    float t_norm = NORM_TEMP(temp);
    float h_norm = NORM_HUMI(humi);
    float g_norm = NORM_GRAY(gray);

    /************ 填充输入 ************/
    input->data.f[0] = t_norm;
    input->data.f[1] = h_norm;
    input->data.f[2] = g_norm;

    /************ 执行推理 ************/
    if (interpreter->Invoke() != kTfLiteOk)
        return -1;

    /************ 读取输出（softmax三分类） ************/
    float p0 = output->data.f[0];
    float p1 = output->data.f[1];
    float p2 = output->data.f[2];

    /************ 取最大概率 ************/
    if (p0 >= p1 && p0 >= p2)
        return 0;
    else if (p1 >= p0 && p1 >= p2)
        return 1;
    else
        return 2;
}
