import * as Teeth from "./winterjs/teeth.js";
import { Uint8Array_to_hex } from "./winterjs/utils.js";
import GemSettings from "./gem_settings.js";

const settings_chunk_size = 10;
const settings_chunk_count =
    Teeth.encoded_length(GemSettings.packed_size) / settings_chunk_size;

function strip_response(response) {
    return response.data.slice(3, -1);
}

export default class Gemini {
    constructor(midi) {
        this.midi = midi;
    }

    async get_version() {
        /* (command 0x01 - hello) */
        let response = await this.midi.transact(
            new Uint8Array([0xf0, 0x77, 0x01, 0xf7])
        );
        return new TextDecoder("ascii").decode(strip_response(response));
    }

    async get_serial_number() {
        /* (command 0x0f - get serial number) */
        const response = await this.midi.transact(
            new Uint8Array([0xf0, 0x77, 0x0f, 0xf7])
        );
        return Uint8Array_to_hex(Teeth.decode(strip_response(response)));
    }

    async load_settings() {
        let encoded_data = new Uint8Array(
            Teeth.encoded_length(GemSettings.packed_size)
        );

        for (let n = 0; n < settings_chunk_count; n++) {
            /* (command 0x08 - load settings chunk) */
            const response = await this.midi.transact(
                new Uint8Array([0xf0, 0x77, 0x08, n, 0xf7])
            );

            const chunk = strip_response(response);

            if (chunk.length != settings_chunk_size) {
                throw `Invalid settings chunk data! ${response.data}`;
            }

            for (let x = 0; x < settings_chunk_size; x++) {
                encoded_data[settings_chunk_size * n + x] = chunk[x];
            }
        }

        const decoded_data = Teeth.decode(encoded_data);
        return GemSettings.unpack(decoded_data);
    }

    async save_settings(settings) {
        const settings_data = settings.pack();

        let encoded_data = new Uint8Array(
            Teeth.encoded_length(GemSettings.packed_size)
        );

        encoded_data.set(Teeth.encode(settings_data));

        for (let n = 0; n < settings_chunk_count; n++) {
            /* (command 0x09 - save settings chunk) */
            let midi_message = new Uint8Array(5 + settings_chunk_size);
            midi_message.set([0xf0, 0x77, 0x09, n]);
            for (let x = 0; x < settings_chunk_size; x++) {
                midi_message[4 + x] = encoded_data[settings_chunk_size * n + x];
            }
            midi_message[5 + settings_chunk_size - 1] = 0xf7;
            await this.midi.transact(midi_message);
        }
    }
}
