#ifndef AI_MODEL_H
#define AI_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

void Ai_Init(void);
float Ai_Predict(float x);
void Ai_Predict_test(void) ;

#ifdef __cplusplus
}
#endif

#endif
