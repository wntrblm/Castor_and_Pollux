/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

/* TODO:
    * Write scripts to generate a few useful scales

    * Allocate space in flash, implement load/save functions
      * Integrate with sysexes
      * Use separate buffer (stack-allocated?), no need to share
        with sysex buffers

    * Implement the other side of the upload/download process

    * Update user guide
      * With some example tuning tables?
*/

#include "gem_quantizer.h"
#include "wntr_pack.h"
#include <string.h>

/* Static variables */

/* Forward declarations */

static const struct GemQuantizerConfig default_quantizer_config;

/* Public functions */

struct GemQuantizerConfig gem_quantizer_config;

void gem_quantizer_init() {
    // TODO: Load from flash
    gem_quantizer_config = default_quantizer_config;
}

uint32_t GemQuantizer_search_table(fix16_t pitch_cv) {
    const uint32_t notes_len = gem_quantizer_config.notes_len;
    const struct GemQuantizerTableEntry* notes = &gem_quantizer_config.notes[0];

    /*
        Check against outer limits of the table
    */
    if (pitch_cv < notes[0].threshold) {
        return 0;
    } else if (pitch_cv >= notes[notes_len - 1].threshold) {
        return notes_len - 1;
    }

    /*
        Binary search through the quantizer table to find which bin the input falls into
    */
    uint32_t lo = 0;
    uint32_t hi = notes_len - 1;

    while (hi != lo) {
        /*
            Note: Since hi > lo here, this calculation of `mid` always gives mid > lo as well.
            This is important in ensuring that the loop always terminates.
            TODO: Test
        */
        uint32_t mid = (lo + hi + 1) >> 1;
        if (pitch_cv >= notes[mid].threshold) {
            lo = mid; /* > previous value of lo */
        } else {
            hi = mid - 1; /* < previous value of hi */
        }
    }

    return lo;
}

void GemQuantizer_erase() {
    // TODO: Clear stored table instead
    gem_quantizer_config = default_quantizer_config;
}

bool GemQuantizer_unpack(struct GemQuantizerConfig* config, const uint8_t* data) {
    config->hysteresis = (fix16_t)WNTR_UNPACK_32(data, 0);
    config->notes_len = (uint32_t)data[4];
    for (uint32_t i = 0; i < config->notes_len; i++) {
        config->notes[i].threshold = (fix16_t)WNTR_UNPACK_32(data, 5 + 8 * i + 0);
        config->notes[i].output = (fix16_t)WNTR_UNPACK_32(data, 5 + 8 * i + 4);
    }
    // TODO: Validation
    return true;
}

bool GemQuantizer_pack(const struct GemQuantizerConfig* config, uint8_t* data) {
    // Clear the output buffer, as we may not necessarily write to every byte
    memset(data, 0, GEMQUANTIZER_PACKED_SIZE);

    WNTR_PACK_32(config->hysteresis, data, 0);
    data[4] = (uint8_t)config->notes_len;
    for (uint32_t i = 0; i < config->notes_len; i++) {
        WNTR_PACK_32(config->notes[i].threshold, data, 5 + 8 * i + 0);
        WNTR_PACK_32(config->notes[i].output, data, 5 + 8 * i + 4);
    }
    // TODO: Validation
    return true;
}

/* Private functions */

/*
    The default configuration for the quantizer is 12-tone equal temperament.
    7 octaves * 12 notes/octave + 1 fencepost entry = 85 total entries
*/
#define TABLE_ENTRY(n)                                                                                                 \
    { F16((n) / 12. - 1 / 24.), F16((n) / 12.) }

static const struct GemQuantizerConfig default_quantizer_config = {
    .hysteresis = F16(0.005),  // 5mV - TODO: Tune
    .notes_len = 85,
    .notes = {
        TABLE_ENTRY(0),  TABLE_ENTRY(1),  TABLE_ENTRY(2),  TABLE_ENTRY(3),  TABLE_ENTRY(4),  TABLE_ENTRY(5),
        TABLE_ENTRY(6),  TABLE_ENTRY(7),  TABLE_ENTRY(8),  TABLE_ENTRY(9),  TABLE_ENTRY(10), TABLE_ENTRY(11),
        TABLE_ENTRY(12), TABLE_ENTRY(13), TABLE_ENTRY(14), TABLE_ENTRY(15), TABLE_ENTRY(16), TABLE_ENTRY(17),
        TABLE_ENTRY(18), TABLE_ENTRY(19), TABLE_ENTRY(20), TABLE_ENTRY(21), TABLE_ENTRY(22), TABLE_ENTRY(23),
        TABLE_ENTRY(24), TABLE_ENTRY(25), TABLE_ENTRY(26), TABLE_ENTRY(27), TABLE_ENTRY(28), TABLE_ENTRY(29),
        TABLE_ENTRY(30), TABLE_ENTRY(31), TABLE_ENTRY(32), TABLE_ENTRY(33), TABLE_ENTRY(34), TABLE_ENTRY(35),
        TABLE_ENTRY(36), TABLE_ENTRY(37), TABLE_ENTRY(38), TABLE_ENTRY(39), TABLE_ENTRY(40), TABLE_ENTRY(41),
        TABLE_ENTRY(42), TABLE_ENTRY(43), TABLE_ENTRY(44), TABLE_ENTRY(45), TABLE_ENTRY(46), TABLE_ENTRY(47),
        TABLE_ENTRY(48), TABLE_ENTRY(49), TABLE_ENTRY(50), TABLE_ENTRY(51), TABLE_ENTRY(52), TABLE_ENTRY(53),
        TABLE_ENTRY(54), TABLE_ENTRY(55), TABLE_ENTRY(56), TABLE_ENTRY(57), TABLE_ENTRY(58), TABLE_ENTRY(59),
        TABLE_ENTRY(60), TABLE_ENTRY(61), TABLE_ENTRY(62), TABLE_ENTRY(63), TABLE_ENTRY(64), TABLE_ENTRY(65),
        TABLE_ENTRY(66), TABLE_ENTRY(67), TABLE_ENTRY(68), TABLE_ENTRY(69), TABLE_ENTRY(70), TABLE_ENTRY(71),
        TABLE_ENTRY(72), TABLE_ENTRY(73), TABLE_ENTRY(74), TABLE_ENTRY(75), TABLE_ENTRY(76), TABLE_ENTRY(77),
        TABLE_ENTRY(78), TABLE_ENTRY(79), TABLE_ENTRY(80), TABLE_ENTRY(81), TABLE_ENTRY(82), TABLE_ENTRY(83),
        TABLE_ENTRY(84)
        // Remaining entries are filled with zeros
    }};
