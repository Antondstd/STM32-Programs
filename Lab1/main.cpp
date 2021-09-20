#include "hal.h"

#include "hal_driver.h"

void showCurrentCountOnDiodes(int count) {
    resetAllDioids();
    switch (count) {
        case 1: {
            setLightRedDiode();
            break;
        }
        case 2: {
            setLightGreenDiode();
            break;
        }
        case 3: {
            setLightRedDiode();
            setLightGreenDiode();
            break;
        }
        default: {
            setOffGreenDiode();
            setOffRedDiode();
            break;
        }
    }
}

void showOverflow(int overFlow) {
    resetAllDioids();
    playAnimation();
    for (int i = 0; i < overFlow; i++) {
        blinkGreenDiode();
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
        state = getButtonState();
        if (!state) {
            msTick++;
        } else {
            if (msTick > longPressTime) {
                currentCount--;
                if (currentCount < 0) {
                    overflow--;
                    if (overflow < 0)
                        overflow = 0;
                    showOverflow(overflow);
                    currentCount = 3;
                }
            } else {
                if (msTick > shortPressTime) {
                    currentCount++;
                    if (currentCount >= 4) {
                        overflow++;
                        showOverflow(overflow);
                        currentCount = 0;
                    }
                }
            }
            msTick = 0;
			showCurrentCountOnDiodes(currentCount);
        }

    }
    return 0;
}