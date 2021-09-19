#include "hal.h"


void resetAllDioids(){
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void playAnimation() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_Delay(500);
	resetAllDioids();
}

void showCurrentCountOnDioids(int count){
    resetAllDioids();
    switch (count) {
        case 1:{
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
            break;
        }
        case 2:{
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
            break;
        }
        case 3:{
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
            break;
        }
        default: {
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
            break;
        }
    }
}

void showOverflow(int overFlow){
    resetAllDioids();
	playAnimation();
    HAL_Delay(500);
    for (int i =0;i < overFlow;i++){
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_Delay(500);
    }
}

int umain() {
    int shortPressTime = 300;
    int longPressTime = 3000000;
    int msTick = 0;
    bool state = false;
    int overflow = 0;
    int currentCount = 0;

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
    while (1) {
        state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
        if (!state) {
            msTick++;

        } else {
            if (msTick > longPressTime) {
                currentCount--;
				if (currentCount < 0){
					overflow--;
					if (overflow < 0)
						overflow = 0;
					showOverflow(overflow);
					currentCount = 3;
                    showCurrentCountOnDioids(currentCount);
				}
            } else {
                if (msTick > shortPressTime) {
                    currentCount++;
                    if (currentCount >= 4) {
                        overflow++;
                        showOverflow(overflow);
                        currentCount = 0;
                        showCurrentCountOnDioids(currentCount);
                    }
                    else{
                        showCurrentCountOnDioids(currentCount);
                    }
                }
            }
            msTick = 0;
        }

    }
    return 0;
}