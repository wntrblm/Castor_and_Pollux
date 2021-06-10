import * as Teeth from "./winterjs/teeth.js";
import { Uint8Array_to_hex } from "./winterjs/utils.js";
import GemSettings from "./gem_settings.js";

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
        console.log("Loading settings from device...");

        const timeout = new Promise((_, reject) => {
            setTimeout(() => reject("timeout"), 1000);
        });

        const response = await Promise.race([
            timeout,
            this.midi.transact(new Uint8Array([0xf0, 0x77, 0x18, 0xf7])),
        ]);

        const encoded_data = strip_response(response);
        const decoded_data = Teeth.decode(encoded_data);
        return GemSettings.unpack(decoded_data);
    }

    async save_settings(settings) {
        const settings_data = settings.pack();
        const encoded_data = Teeth.encode(settings_data);
        const midi_message = new Uint8Array(4 + encoded_data.length);
        midi_message.set([0xf0, 0x77, 0x19]);
        midi_message.set(encoded_data, 3);
        midi_message[midi_message.length - 1] = 0xf7;
        await this.midi.transact(midi_message);
    }
}
