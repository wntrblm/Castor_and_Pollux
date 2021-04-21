/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Checks/sets SAMD21 fuses (NVM USER ROW) */

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
} USER_ROW_Type;
// clang-format on

typedef struct {
    __IO USER_ROW_Type USER_ROW;
} NVM_USER_Type;

#define NVM_USER ((NVM_USER_Type*)NVMCTRL_USER)

void gem_fuses_check();
void gem_fuses_write(NVM_USER_Type fuses);
