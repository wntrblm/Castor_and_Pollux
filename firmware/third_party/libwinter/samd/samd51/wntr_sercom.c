/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_sercom.h"
#include <stddef.h>

void wntr_sercom_init_clock(Sercom* inst, uint32_t gclk) {
    size_t gclk_id = 0;

    switch ((intptr_t)inst) {
        case (intptr_t)SERCOM0:
            MCLK->APBAMASK.reg |= MCLK_APBAMASK_SERCOM0;
            gclk_id = SERCOM0_GCLK_ID_CORE;
            break;
        case (intptr_t)SERCOM1:
            MCLK->APBAMASK.reg |= MCLK_APBAMASK_SERCOM1;
            gclk_id = SERCOM1_GCLK_ID_CORE;
            break;
        case (intptr_t)SERCOM2:
            MCLK->APBBMASK.reg |= MCLK_APBBMASK_SERCOM2;
            gclk_id = SERCOM2_GCLK_ID_CORE;
            break;
        case (intptr_t)SERCOM3:
            MCLK->APBBMASK.reg |= MCLK_APBBMASK_SERCOM3;
            gclk_id = SERCOM3_GCLK_ID_CORE;
            break;
        case (intptr_t)SERCOM4:
            MCLK->APBDMASK.reg |= MCLK_APBDMASK_SERCOM4;
            gclk_id = SERCOM4_GCLK_ID_CORE;
            break;
        case (intptr_t)SERCOM5:
            MCLK->APBDMASK.reg |= MCLK_APBDMASK_SERCOM5;
            gclk_id = SERCOM5_GCLK_ID_CORE;
            break;
        default:
            break;
    }

    GCLK->PCHCTRL[gclk_id].reg = GCLK_PCHCTRL_CHEN | gclk;
    while (!GCLK->PCHCTRL[gclk_id].bit.CHEN) {};
}
