//
// Created by ad3st on 19.09.2021.
//

#ifndef LAB1_HAL_DRIVER_H
#define LAB1_HAL_DRIVER_H

void resetAllDioids();

void setLightGreenDiode();

void setOffGreenDiode();

void setLightRedDiode();

void setOffRedDiode();

void setLightYellowDiode();

void setOffYellowDiode();

void delay(int ms);

void playAnimation();

bool getButtonState();

void blinkGreenDiode();

#endif //LAB1_HAL_DRIVER_H
