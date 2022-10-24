/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "wntr_system_clocks.h"
#include "sam.h"

/* Hubble's clock configuration after hubble_clocks_init() is:

- GCLK0: 120 MHz from DPLL0 (used by CPU)
- GCLK1: 48 MHz from DFLL48M (used by USB, ADC)
- GCLK2: Unused
- GCLK3: 32.768 kHz from XOSC32K (used as ref. clock for DFLL48M)
- GCLK4: 12 MHz from DFLL48M (used by the DAC & SPI)
- GCLK5: 1 MHz from DFLL48M (used as ref. clock for DPLL0)

*/

/* Forward declarations */

static void init_osc32k() __attribute__((unused));
static void init_xosc32k() __attribute__((unused));
static void init_dfll48m_open_loop() __attribute__((unused));
static void init_dfll48m_closed_loop() __attribute__((unused));
static void init_dfll48m_gclks();
static void init_dpll0();
static void setup_cpu_osc32k();
static void setup_cpu_dpll0();

#ifndef WNTR_SYSTEM_CLOCKS_USE_32K_CRYSTAL
#error "Set WNTR_SYSTEM_CLOCKS_USE_32K_CRYSTAL to 1 or 0."
#endif

/* Public functions */

void wntr_system_clocks_init() {
    /* Enable automatic flash wait states. */
    NVMCTRL->CTRLA.reg |= NVMCTRL_CTRLA_RWS(0) | NVMCTRL_CTRLA_AUTOWS;

    /* Reset the clock system. */
    GCLK->CTRLA.bit.SWRST = 1;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_SWRST) {}

/* Configure 32kHz Oscillator */
#if WNTR_SYSTEM_CLOCKS_USE_32K_CRYSTAL
    init_xosc32k();
#else
    init_osc32k();
#endif

    /* Temporarily switch CPU to the internal 32kHz osc while
       configuring the clocks. */
    setup_cpu_osc32k();

/* Configure DFLL (48Mhz) & DPLL (120Mhz) */
#ifdef WNTR_SYSTEM_CLOCKS_USE_32K_CRYSTAL
    init_dfll48m_closed_loop();
#else
    init_dfll48m_open_loop();
#endif
    init_dfll48m_gclks();
    init_dpll0();

    /* Switch CPU to DPLL0 (120MHz) */
    setup_cpu_dpll0();
}

/* Private functions */

static void init_osc32k() {
    /* OSCULP32K to GCLK3 */
    GCLK->GENCTRL[3].reg = GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL3) {}
}

static void init_xosc32k() {
    /* Configure XOSC32K: 32kHz output enabled, standard speed, using a crystal. */
    OSC32KCTRL->XOSC32K.reg =
        OSC32KCTRL_XOSC32K_ENABLE | OSC32KCTRL_XOSC32K_EN32K | OSC32KCTRL_XOSC32K_CGM_XT | OSC32KCTRL_XOSC32K_XTALEN;
    while (!(OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY)) {}

    /* XOSC32K to GCLK3 */
    GCLK->GENCTRL[3].reg = GCLK_GENCTRL_SRC_XOSC32K | GCLK_GENCTRL_IDC | GCLK_GENCTRL_OE | GCLK_GENCTRL_GENEN;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL3) {}
}

static void init_dfll48m_open_loop() {
    /*
        DFLL is used to generate a 48 MHz clock.

        This implementation uses open loop and USB clock recovery mode.
    */

    // Disable it while we're configuring it.
    OSCCTRL->DFLLCTRLA.reg = 0;
    while (OSCCTRL->DFLLSYNC.bit.ENABLE) {}

    OSCCTRL->DFLLCTRLB.reg = OSCCTRL_DFLLCTRLB_WAITLOCK | OSCCTRL_DFLLCTRLB_CCDIS | OSCCTRL_DFLLCTRLB_USBCRM;
    while (OSCCTRL->DFLLSYNC.bit.DFLLCTRLB) {}

    OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_CSTEP(0x1) | OSCCTRL_DFLLMUL_FSTEP(0x1) | OSCCTRL_DFLLMUL_MUL(0);
    while (OSCCTRL->DFLLSYNC.bit.DFLLMUL) {}

    // Re-enable and wait for it to be ready.
    OSCCTRL->DFLLCTRLA.reg |= OSCCTRL_DFLLCTRLA_ENABLE;
    while (OSCCTRL->DFLLSYNC.bit.ENABLE) {}
    while (!OSCCTRL->STATUS.bit.DFLLRDY) {}
}

static void init_dfll48m_closed_loop() {
    /*
        DFLL is used to generate a 48 MHz clock.

        This implementation uses the external 32.768 kHz crystal.
    */

    // Disable it while we're configuring it.
    OSCCTRL->DFLLCTRLA.reg = 0;
    while (OSCCTRL->DFLLSYNC.bit.ENABLE) {}

    // Set the reference clock to the 32.768 kHz crystal (GCLK3)
    GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK3;
    while (!GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].bit.CHEN) {};

    // freq = DFLLMUL.MUL × ref_freq
    // 48 MHz = MUL * 32.768 kHz
    OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_CSTEP(0x1) | OSCCTRL_DFLLMUL_FSTEP(0x1) | OSCCTRL_DFLLMUL_MUL(1465);
    while (OSCCTRL->DFLLSYNC.bit.DFLLMUL) {}

    OSCCTRL->DFLLCTRLB.reg =
        OSCCTRL_DFLLCTRLB_WAITLOCK | OSCCTRL_DFLLCTRLB_CCDIS | OSCCTRL_DFLLCTRLB_QLDIS | OSCCTRL_DFLLCTRLB_MODE;
    while (OSCCTRL->DFLLSYNC.bit.DFLLCTRLB) {}

    // Re-enable and wait for it to be ready.
    OSCCTRL->DFLLCTRLA.reg |= OSCCTRL_DFLLCTRLA_ENABLE;
    while (OSCCTRL->DFLLSYNC.bit.ENABLE) {}

    // Errata DLL101-4: "In Close Loop mode, the STATUS.DFLLRDY bit does not rise before lock fine occurs."
    // Wait for locks instead.
    while (OSCCTRL->STATUS.bit.DFLLLCKC == 0 || OSCCTRL->STATUS.bit.DFLLLCKF == 0) {}
}

static void init_dfll48m_gclks() {
    /* GCLK1: 48 MHz */
    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_IDC | GCLK_GENCTRL_OE | GCLK_GENCTRL_GENEN;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL1) {}

    /* GCLK4: 12 MHz */
    GCLK->GENCTRL[4].reg =
        GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_DIV(4) | GCLK_GENCTRL_IDC | GCLK_GENCTRL_OE | GCLK_GENCTRL_GENEN;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL4) {}

    /* GCLK5: 1 MHz */
    GCLK->GENCTRL[5].reg = GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_DIV(48u) | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL4) {}
}

static void init_dpll0() {
    /* configures DPLL0 @ 120MHz for GCLK0 */

    /* Reference clock is GCLK5 (1MHz) */
    GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK5;

    /* Note: must use LBYPASS as per errata */
    OSCCTRL->Dpll[0].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK_GCLK | OSCCTRL_DPLLCTRLB_LBYPASS;

    /*
        Output frequency formula (datasheet section 28.6.5):

            DPLL_freq = REF_freq * (LDR + 1 + (LDRFRAC / 32))

        Integer-only mode LDR formula:

            LDR = (DPLL_freq / REF_freq) - 1

        So for 120 MHz from 1 MHz LDR should be 120, set to 119 here to prevent
        overclocking.
    */
    OSCCTRL->Dpll[0].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDRFRAC(0) | OSCCTRL_DPLLRATIO_LDR(119);
    while (OSCCTRL->Dpll[0].DPLLSYNCBUSY.bit.DPLLRATIO) {}

    /* Enable and wait for clock ready */
    OSCCTRL->Dpll[0].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE;
    while (OSCCTRL->Dpll[0].DPLLSTATUS.bit.CLKRDY == 0 || OSCCTRL->Dpll[0].DPLLSTATUS.bit.LOCK == 0) {};

    /*
        Delay 10ms as per errata 2.13.1. May not be needed now that a higher
        frequency reference clock is being used.

        CPU clock is (currently) at 32kHz, so at least 320 cycles using the cycle
        counter.
    */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    while (DWT->CYCCNT < 320) {};
}

static void setup_cpu_osc32k() {
    MCLK->CPUDIV.reg = MCLK_CPUDIV_DIV_DIV1;
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_GENEN;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL0) {}
    SystemCoreClock = 32768;
}

static void setup_cpu_dpll0() {
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_SRC_DPLL0 | GCLK_GENCTRL_IDC | GCLK_GENCTRL_OE | GCLK_GENCTRL_GENEN;
    while (GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL0) {}
    SystemCoreClock = 120000000;
}
