#ifndef __CAR_H
#define __CAR_H
#include "main.h"

void Car_Forward(float target_rpm);
void Car_Backward(float target_rpm);
void Car_Turn_Left(float target_rpm);
void Car_Turn_Right(float target_rpm);
void Car_Translate_Left(float target_rpm);
void Car_Translate_Right(float target_rpm);
void Car_Stop(void);
void Car_Tast(float target_rpm);
#endif



