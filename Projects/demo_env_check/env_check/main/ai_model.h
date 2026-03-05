#ifndef AI_MODEL_H
#define AI_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

/************ 初始化模型 ************/
void ai_model_init(void);

/************ 返回风险等级 ************/
/*
0 = 安全
1 = 轻度风险
2 = 高风险
-1 = 错误
*/
int ai_predict_level(float temp, float humi, float gray);

#ifdef __cplusplus
}
#endif

#endif