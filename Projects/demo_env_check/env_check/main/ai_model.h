#ifndef AI_MODEL_H
#define AI_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

void Ai_Init(void);
float Ai_Predict(float temp, float humi, float gray);
void Ai_Predict_test(void) ;

#ifdef __cplusplus
}
#endif

#endif
