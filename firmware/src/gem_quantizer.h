/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include "fix16.h"
#include <stdbool.h>
#include <stdint.h>

/* Implementation of the optional pitch quantizer */

/*
    The quantizer operates on a user-selectable scale, defaulting to 12-tone
    equal temperament if the user does not upload a replacement scale.

    In order to provide maximum flexibility, the scale is stored as a table of
    pairs {voltage threshold, output voltage}. This says that any input CV
    between the threshold of this bin and the threshold of the *next* bin,
    will be quantized to the specified output.

    This design has two main ideas:

    * The use of a table, rather than a more constrained scheme, allows
      a wide variety of scales to be supported, including ones with different
      numbers of notes per octave or with unevenly spaced notes.

    * The separation of thresholds and outputs allows the user to map the
      desired scale onto controllers which may not output appropriate voltages
      directly.

      An example would be using a standard MIDI keyboard and a MIDI-to-CV
      converter which outputs voltages spaced by exactly 1/12 V. For this use
      case, the threshold voltages could be placed in between those steps in
      order to correctly capture the pressed key, while the output voltages
      could be an unevenly spaced scale, or even a non-octave-based scale.

    TODO: Could the thresholds be represented in a 16-bit format such as i3.12
          to save space?
*/
struct GemQuantizerTableEntry {
    /* Minimum input voltage for this bin, not accounting for hysteresis */
    fix16_t threshold;
    /* Output voltage if the input falls into this bin */
    fix16_t output;
};

/*
    Note on table size:
    * The nominal input voltage is 0V - 6V
    * After offsetting and clamping, the range is 0V - 7V
    * So the number of table entries needed is 7 * (notes_per_octave) + 1

    As the quantization table will need to be stored in flash, it makes sense
    to have a fixed upper size limit which is a power of 2 (or that minus 1).
    We can also store the actual number of table entries, so that generator programs
    don't have to generate any more entries than needed for the desired scale.

    The maximum table size could reasonably be either:
    MAX_TABLE_SIZE = 127 or 128 => max notes per octave = 18
    MAX_TABLE_SIZE = 255 or 256 => max notes per octave = 36

    We pick 255 here so that the "number of notes" field can be encoded in one byte.
*/
#define MAX_QUANTIZER_TABLE_SIZE 255

struct GemQuantizerConfig {
    /*
        Once a note has been selected, if the input CV moves slightly outside of
        the selection range for that note, we do not immediately switch to a new note.
        Instead, we require the input CV to go at least a certain amount outside of the
        range for the current note before changing.

        This behavior avoids a problem where the input CV is hovering right at the
        boundary between two notes, and keeps moving from one to the other due to noise.

        Without hysteresis, this would cause the output pitch to flap between the two
        notes. This can manifest as either the pitch switching between two notes, or
        as a noisy output, depending on the input conditions.

        On the other hand, with hysteresis, if this occurs then we will select one of
        the two notes and remain there, as long as the amplitude of the noise is not
        too high. However, which note we select can still be inconsistent if the range
        of input CVs seen spans a note boundary.

        In either case, the solution is for the end user to adjust either their CV
        source or the base offset on Castor & Pollux, so that the CVs are better
        centered. However, with hysteresis, the user-facing symptoms are much easier
        to explain.
    */
    fix16_t hysteresis;

    uint32_t notes_len;
    struct GemQuantizerTableEntry notes[MAX_QUANTIZER_TABLE_SIZE];

    // TODO: Add checksum
};

// Maximum size of a packed quantizer table
// Size is 4 bytes for hysteresis, 1 byte for number of notes, then 255*8 bytes for notes
// Note: This size could be reduced by storing values in i4.12 format, rather than
// the current `fix16` (i16.16) format, as the extra range and precision are not
// needed here
#define GEMQUANTIZER_PACKED_SIZE 2045

extern struct GemQuantizerConfig gem_quantizer_config;

void gem_quantizer_init();

// Search the quantizer table to figure out which bin the incoming pitch CV
// falls into.
// Returns the index of the quantizer bin.
uint32_t GemQuantizer_search_table(fix16_t pitch_cv);

void GemQuantizer_erase();
bool GemQuantizer_unpack(struct GemQuantizerConfig* config, const uint8_t* data);
bool GemQuantizer_pack(const struct GemQuantizerConfig* config, uint8_t* data);
