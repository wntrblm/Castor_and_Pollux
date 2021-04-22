/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Helpers for interacting with the SAMD21 hardware fuses */

#include "sam.h"
#include <stdint.h>

/*
    Struct mapping for the User Row, since it's not included in the normal CMSIS headers for the SAMD21.
    See datasheet section 10.3.1 for full details.
*/
// clang-format off
typedef union {
  struct {
    uint64_t BOOTPROT:3;        /*!< bit:  0.. 2  Used to select one of eight different bootloader sizes.                                                                                       */
    uint64_t :1;                /*!< bit:      3  Reserved                                                                                                                                      */
    uint64_t EEPROM:3;          /*!< bit:  4.. 6  Used to select one of eight different EEPROM sizes.                                                                                           */
    uint64_t :1;                /*!< bit:      7  Reserved                                                                                                                                      */
    uint64_t BOD33_LEVEL:6;     /*!< bit:  8..13  BOD33 threshold Level at power on.                                                                                                            */
    uint64_t BOD33_ENABLE:1;    /*!< bit:     14  BOD33 enable at power on.                                                                                                                     */
    uint64_t BOD33_ACTION:2;    /*!< bit: 15..16  BOD33 action at power on.                                                                                                                     */
    uint64_t :8;                /*!< bit: 17..24  Reserved: Voltage Regulator Internal BOD (BOD12) configuration. These bits are written in production and must not be changed.                 */
    uint64_t WDT_ENABLE:1;      /*!< bit:     25  WDT enable at power on.                                                                                                                       */
    uint64_t WDT_ALWAYSON:1;    /*!< bit:     26  WDT always on at power on.                                                                                                                    */
    uint64_t WDT_PERIOD:4;      /*!< bit: 27..30  WDT period at power on.                                                                                                                       */
    uint64_t WDT_WINDOW:4;      /*!< bit: 31..34  WDT window at power on.                                                                                                                       */
    uint64_t WDT_EWOFFSET:4;    /*!< bit: 35..38  WDT early warning interrupt time at power on.                                                                                                 */
    uint64_t WDT_WEN:1;         /*!< bit:     39  WDT timer window mode enable at power on.                                                                                                     */
    uint64_t BOD33_HYST:1;      /*!< bit:     40  BOD33 hysteresis configuration at power on.                                                                                                   */
    uint64_t :1;                /*!< bit:     41  Reserved: Voltage Regulator Internal BOD(BOD12) configuration. This bit is written in production and must not be changed.                     */
    uint64_t :6;                /*!< bit: 42..47  Reserved                                                                                                                                      */
    uint64_t LOCK:16;           /*!< bit: 48..63  NVM Region Lock Bits.                                                                                                                         */
  } bit;
  uint64_t reg;
} USER_FUSES_Type;
// clang-format on

typedef struct {
    __IO USER_FUSES_Type FUSES;
} NVM_USER_ROW_Type;

#define NVM_USER_ROW ((NVM_USER_ROW_Type*)NVMCTRL_USER)

/*
    Struct mapping for the OTP4 software calibration fuses, See datasheet
    section 10.3.2 for full details.
*/
// clang-format off
typedef union {
  struct {
    uint64_t :3;                    /*!< bit:  0.. 2  Reserved                                                        */
    uint64_t :12;                   /*!< bit:  3..14  Reserved                                                        */
    uint64_t :12;                   /*!< bit: 15..26  Reserved                                                        */
    uint64_t ADC_LINEARITY:8;       /*!< bit: 27..34  ADC Linearity calibration                                       */
    uint64_t ADC_BIASCAL:3;         /*!< bit: 35..37  ADC Bias calibration                                            */
    uint64_t OSC32K_CAL:7;          /*!< bit: 38..44  OSC32K calibration                                              */
    uint64_t USB_TRANSN:5;          /*!< bit: 45..49  USB TRANSN calibration value                                    */
    uint64_t USB_TRANSP:5;          /*!< bit: 50..54  USB TRANSP calibration value                                    */
    uint64_t USB_TRIM:3;            /*!< bit: 55..57  USB TRIM calibration value                                      */
    uint64_t DFLL48M_COARSE_CAL:6;  /*!< bit: 58..63  DFLL48M Coarse calibration value                                */
  } bit;
  uint64_t reg;
} OTP4_FUSES0_Type;
typedef union {
  struct {
      /* This isn't documented in the datasheet, but it is present in nvmctrl.h */
      uint64_t DFLL48M_FINE_CAL:10; /*!< bit:  0.. 9  DFLL48M Coarse calibration value  */
      uint64_t :53;                 /*!< bit:  9..63  Reserved */
  } bit;
  uint64_t reg;
} OTP4_FUSES1_Type;
// clang-format on

typedef struct {
    // RoReg8 Reserved1[0x4];
    __IO OTP4_FUSES0_Type FUSES0;
    __IO OTP4_FUSES1_Type FUSES1;
} NVM_OTP4_Type;

#define OTP4_FUSES ((NVM_OTP4_Type*)NVMCTRL_OTP4)

void wntr_check_bootprot_fuse();
void wntr_fuses_write(NVM_USER_ROW_Type fuses);
