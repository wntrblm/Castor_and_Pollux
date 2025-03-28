/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

import { DOM } from "/winter.js";

export function Uint8Array_to_hex(buf) {
    return Array.prototype.map
        .call(buf, (x) => ("00" + x.toString(16)).slice(-2))
        .join("");
}

export const $e = DOM.$;
export const $s = DOM.$$;

export function $make(tag_name, properties = {}) {
    const elem = document.createElement(tag_name);
    for (const [name, value] of Object.entries(properties)) {
        if (name === "children") {
            for (const child of value) {
                elem.appendChild(child);
            }
            continue;
        }
        if (name === "innerText") {
            elem.innerText = value;
            continue;
        }
        elem.setAttribute(name, value);
    }
    return elem;
}

/* Adds an event listener. */
export function $on(elem, event, callback, strict = true) {
    if (!strict && (elem === null || elem === undefined)) {
        return;
    }
    elem.addEventListener(event, callback);
}

/* Helper for working with template elements with simple interpolation. */
export class TemplateElement {
    constructor(id) {
        this._elem = $e(id);
        this._parent = this._elem.parentNode;
    }

    hide() {
        this._parent.classList.add("hidden");
    }

    show() {
        this._parent.classList.remove("hidden");
    }

    render(ctx) {
        let content = this._elem.innerHTML;
        content = content.replace(/\${(.*?)}/g, (_, g) => {
            return ObjectHelpers.get_property_by_path(ctx, g) || "";
        });
        const temp = document.createElement("template");
        temp.innerHTML = content;
        return temp.content.cloneNode(true);
    }

    render_to(elem, ctx) {
        DOM.removeAllChildren($e(elem));
        $e(elem).appendChild(this.render(ctx));
    }

    render_to_parent(ctx) {
        DOM.removeAllChildren(this._parent);
        this._parent.appendChild(this.render(ctx));
    }

    render_all_to_parent(ctxes) {
        DOM.removeAllChildren(this._parent);
        for (const [n, ctx] of ctxes.entries()) {
            ctx.$index = n;
            this._parent.appendChild(this.render(ctx));
        }
    }
}

export const ObjectHelpers = {
    /* Why is it so hard to check if something is a string in JavaScript? */
    is_string(val) {
        return typeof val === "string" || val instanceof String;
    },

    /*
        Like Object.assign, but only updates existing properties in target- it
        does not add any new ones.
    */
    assign_only_existing_properties: (target, source) => {
        for (const prop of Object.getOwnPropertyNames(target)) {
            if (Object.prototype.hasOwnProperty.call(source, prop)) {
                target[prop] = source[prop];
            }
        }
    },
    /* Access nested object properties using a string, e.g. "order.email" */
    get_property_by_path: (obj, key, strict = true) => {
        const key_parts = key.split(".");
        let value = obj;
        for (const part of key_parts) {
            if (!strict && (value === undefined || value == null)) {
                return undefined;
            }
            value = value[part];
        }
        return value;
    },

    set_property_by_path: (obj, key, value, strict = true) => {
        const key_parts = key.split(".");
        let current_obj = obj;
        for (const part of key_parts.slice(0, -1)) {
            if (!strict && (part === undefined || part == null)) {
                return;
            }
            current_obj = current_obj[part];
        }

        current_obj[key_parts.pop()] = value;
    },
};

/*
    Handles re-constructing native objects, like Number and Date objects
    from plain-o-JSON objects with sentinel keys like __decimal__ and
    __datetime__ fields.
*/
export class ObjectReviver {
    constructor() {
        this._revivers = {};
    }

    add(sentinel_key, reviver) {
        this._revivers[sentinel_key] = reviver;
    }

    revive() {
        return (key, value) => {
            if (
                value === null ||
                value === undefined ||
                typeof value !== "object"
            ) {
                return value;
            }
            for (const [sentinel_key, revive] of Object.entries(
                this._revivers
            )) {
                if (Object.prototype.hasOwnProperty.call(value, sentinel_key)) {
                    return revive(value);
                }
            }
            return value;
        };
    }
}

ObjectReviver.default = new ObjectReviver();
ObjectReviver.default.add("__decimal__", (val) => new Number(val.value));
ObjectReviver.default.add("__datetime__", (val) => new Date(val.value));
