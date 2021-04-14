/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

import _struct from "./struct.mjs";

class Struct {
    constructor(values) {
        for (const field of this.constructor._fields) {
            if (values[field.name] === undefined) {
                this[field.name] = field.default;
            } else {
                this[field.name] = values[field.name];
            }
        }
    }

    static get _struct() {
        return _struct(this._pack_string);
    }

    pack() {
        const values = [];

        for (const field of this.constructor._fields) {
            if (field.kind == "fix16") {
                let val = this[field.name] * 0x00010000;
                val += val >= 0 ? 0.5 : -0.5;
                values.push(val);
            } else {
                values.push(this[field.name]);
            }
        }
        return new Uint8Array(this.constructor._struct.pack(...values));
    }

    static unpack(buf) {
        if (buf instanceof Uint8Array) {
            buf = buf.buffer;
        }

        const unpacked = this._struct.unpack(buf);
        const result = new this();

        for (const field of this._fields) {
            if (field.kind == "fix16") {
                result[field.name] = unpacked.shift() / 0x00010000;
            } else {
                result[field.name] = unpacked.shift();
            }
        }

        return result;
    }
}

export default Struct;
