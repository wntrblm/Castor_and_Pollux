/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

import { $e, $on } from "./utils.js";

/*
    A UI helper for diplaying the value of an input.

    This is useful for range or number displays that want to show their value.

    Example HTML structure:

        <input name="range_example" type="range" value="0.6" step="0.01" min="0.33" max="1.0" />
        <span class="form-unit">
            <span id="range_example_value_display"></span> percent
        </span>

    And JS usage:

        ValueDisplay(document.querySelector("input[name=range_example]"), (elem) => elem.value);
*/
export class ValueDisplay {
    constructor(elem, formatter, display_elem) {
        this.elem = $e(elem);

        if (formatter === undefined) {
            formatter = (input) => input.value;
        }
        if (display_elem === undefined) {
            display_elem = $e(`${this.elem.name}_value_display`);
        }
        if (typeof display_elem === "string") {
            display_elem = $e(display_elem);
        }

        const update = () => {
            display_elem.innerText = formatter(this.elem);
        };

        $on(elem, "input", update);

        // Call it once to update it from the default value.
        update();
    }
}

/*
    Two-way databinding for a form input.

    Whenever the input changes, `data[key]` is updated with the value from the
    form.

    To go the other direction - update the form when `data[key]` changes, call
    `update_value()`.
*/
export class InputBinding {
    constructor(elem, data, key = undefined) {
        this.elem = $e(elem);

        this.data = data;

        if (key === undefined) {
            key = this.elem.name;
        }

        this.key = key;

        this.bind();
    }

    bind() {
        $on(this.elem, "input", () => {
            this.update_data();
        });

        $on(this.elem, "data_update", () => {
            this.update_value();
        });

        this.update_value();
    }

    value_to_data(value) {
        return value;
    }

    data_to_value(value) {
        return value;
    }

    update_value() {
        this.elem.value = this.data_to_value(this.data[this.key]);
    }

    update_data() {
        this.data[this.key] = this.value_to_data(this.elem.value);
    }
}

/* Two-way databinding for number inputs with a min & max property. */
export class MixMaxInputBinding extends InputBinding {
    update_data() {
        const min = parseFloat(this.elem.min);
        const max = parseFloat(this.elem.max);

        if (!isNaN(min) && !isNaN(max)) {
            const value = this.elem.valueAsNumber;
            if (value < min) {
                this.elem.value = min;
            }
            if (value > max) {
                this.elem.value = max;
            }
        }

        this.data[this.key] = this.value_to_data(this.elem.value);
    }
}

export class IntInputBinding extends MixMaxInputBinding {
    value_to_data(value) {
        return parseInt(value);
    }
}

export class FloatInputBinding extends MixMaxInputBinding {
    constructor(elem, data, key, precision) {
        super(elem, data, key);
        this.precision = parseInt(precision || 2);
        this.update_value();
    }

    value_to_data(value) {
        return parseFloat(value);
    }

    data_to_value(value) {
        return value.toFixed(this.precision);
    }
}

/* Two-way databinding for checkbox inputs.  */
export class CheckboxInputBinding extends InputBinding {
    update_value() {
        this.elem.checked = this.data[this.key] ? true : false;
    }

    update_data() {
        this.data[this.key] = this.elem.checked;
    }
}

/*
    Bind the controls in the given `form` to the given `data`.

    The form controls must have a `data-binding-type` attribute with one of the
    following values:

    * int
    * float
    * checkbox
*/
export function bind(form, data) {
    for (const elem of form.querySelectorAll("input[data-binding-type=int]")) {
        new IntInputBinding(elem, data);
    }

    for (const elem of form.querySelectorAll(
        "input[data-binding-type=float]"
    )) {
        new FloatInputBinding(
            elem,
            data,
            undefined,
            elem.dataset.bindingPrecision
        );
    }

    for (const elem of form.querySelectorAll(
        "input[data-binding-type=checkbox]"
    )) {
        new CheckboxInputBinding(elem, data);
    }
}

/*
    Call this to update the form's fields whenever modifying the binded
    `data`.
*/
export function update_values(form) {
    for (const elem of form.querySelectorAll("[data-binding-type]")) {
        elem.dispatchEvent(new CustomEvent("data_update"));
    }
}

/*
    Enables value displays for all input elements with a `data-value-display`
    property.
*/
export function bind_value_displays(form) {
    for (const elem of form.querySelectorAll("[data-value-display]")) {
        switch (elem.dataset.valueDisplay) {
            case "":
                new ValueDisplay(elem);
                break;
            case "percent":
                new ValueDisplay(elem, (input) =>
                    Math.round(input.value * 100)
                );
                break;
            case "float":
                new ValueDisplay(elem, (input) =>
                    input.valueAsNumber.toFixed(
                        elem.dataset.valueDisplayPrecision
                    )
                );
                break;
            default:
                new ValueDisplay(
                    elem,
                    new Function(
                        "input",
                        `"use strict"; return ${elem.dataset.valueDisplay}`
                    )
                );
                break;
        }
    }
}
