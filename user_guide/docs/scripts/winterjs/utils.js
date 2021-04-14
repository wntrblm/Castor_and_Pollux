/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

export function Uint8Array_to_hex(buf) {
    return Array.prototype.map
        .call(buf, (x) => ("00" + x.toString(16)).slice(-2))
        .join("");
}

/* Returns a HTMLElement give the ID or the element itself. */
export function $e(x) {
    if (typeof x === "string") {
        return document.getElementById(x);
    }
    return x;
}

/* Adds an event listener. */
export function $on(elem, event, callback) {
    elem.addEventListener(event, callback);
}
