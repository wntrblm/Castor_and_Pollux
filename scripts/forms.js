/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

import { DOM } from "/winter.js";
import { $on, ObjectHelpers } from "./utils.js";

const $ = DOM.$;

/*
    Two-way databinding for a form input.

    Whenever the input changes, `data[key]` is updated with the value from the
    form.

    To go the other direction - update the form when `data[key]` changes, call
    `update_value()`.

    This can also display validation messages. It looks for an element with
    data-validation-message-for="${id|name}" within the containing <form>.
*/
export class InputBinding {
    constructor(elem, data, key = undefined) {
        this.elem = $(elem);
        this.name = this.elem.name || this.elem.id;
        this.data = data;

        if (key === undefined) {
            key = this.elem.name;
        }

        this.key = key;

        this.bind();

        this.setup_validation();
    }

    bind() {
        this.update_value();

        $on(this.elem, "input", () => {
            this.update_data();
        });
    }

    value_to_data(value) {
        return value;
    }

    data_to_value(value) {
        return value;
    }

    update_value() {
        const value = ObjectHelpers.get_property_by_path(
            this.data,
            this.key,
            false
        );
        if (value === undefined) {
            return;
        }
        this.elem.value = this.data_to_value(value);
        this.update_validation();
        this.elem.dispatchEvent(
            new Event("winterjs:update", { bubbles: false })
        );
    }

    update_data() {
        ObjectHelpers.set_property_by_path(
            this.data,
            this.key,
            this.value_to_data(this.elem.value),
            false
        );
    }

    get validation_enabled() {
        return this.elem.classList.contains("is-validation-enabled");
    }

    set validation_enabled(value) {
        if (value) {
            this.elem.classList.add("is-validation-enabled");
        } else {
            this.elem.classList.remove("is-validation-enabled");
        }
    }

    setup_validation() {
        this.validation_enabled = false;
        const form = this.elem.closest("form");
        if (form !== null) {
            this.validation_message =
                form.querySelector(
                    `[data-validation-message-for="${this.elem.id}"]`
                ) ||
                form.querySelector(
                    `[data-validation-message-for="${this.elem.name}"]`
                );
        }

        $on(this.elem, "blur", () => {
            this.validation_enabled = true;
            this.set_backend_error("");
            this.update_validation(this.elem.checkValidity());
        });

        $on(this.elem, "invalid", () => {
            this.update_validation(false);
        });
    }

    set_backend_error(message) {
        this.validation_enabled = true;
        this.elem.setCustomValidity(message);
        this.update_validation(false);
    }

    update_validation(valid) {
        if (!this.validation_message || !this.validation_enabled) {
            return;
        }

        if (valid === undefined) {
            valid = this.elem.checkValidity();
        }

        if (valid) {
            this.validation_message.innerText = "";
        } else {
            this.validation_message.innerText = this.elem.validationMessage;
        }
    }
}

/* Two-way databinding for plain-o-text fields */
export class TextInputBinding extends InputBinding {}

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

        ObjectHelpers.set_property_by_path(
            this.data,
            this.key,
            this.value_to_data(this.elem.value),
            false
        );
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
        this.elem.checked = ObjectHelpers.get_property_by_path(
            this.data,
            this.key
        )
            ? true
            : false;
        this.elem.dispatchEvent(
            new Event("winterjs:update", { bubbles: false })
        );
    }

    update_data() {
        ObjectHelpers.set_property_by_path(
            this.data,
            this.key,
            this.elem.checked
        );
    }
}

/* Two-day databinding for select fields */
export class SelectInputBinding extends InputBinding {
    constructor(elem, data, options) {
        super(elem, data);
        this.type = elem.dataset.bindType;
        this.placeholder = this.elem.getAttribute("placeholder");

        if (options === undefined) {
            const options_key = elem.dataset.bindOptions;
            if (options_key !== undefined) {
                this.options = ObjectHelpers.get_property_by_path(
                    data,
                    options_key
                );
                this.update_options();
            }
        } else {
            this.options = options;
            this.update_options();
        }

        this.update_value();
        this.setup_placeholder();
    }

    update_options() {
        DOM.removeAllChildren(this.elem);
        if (this.placeholder) {
            const option = document.createElement("option");
            option.value = "";
            option.disabled = true;
            option.selected = true;
            option.hidden = true;
            option.innerText = this.placeholder;
            this.elem.appendChild(option);
        }

        for (const [val, name] of Object.entries(this.options)) {
            const option = document.createElement("option");
            option.value = val;
            option.innerText = name;
            this.elem.appendChild(option);
        }
    }

    /* God, placeholders for <select> elements is nonsense. */
    setup_placeholder() {
        if (!this.placeholder) {
            return;
        }

        const update_class = () => {
            if (this.elem.value === "") {
                this.elem.classList.add("placeholder");
            } else {
                this.elem.classList.remove("placeholder");
            }
        };

        $on(this.elem, "change", update_class);
        update_class();
    }

    data_to_value(data) {
        data = super.data_to_value(data);
        if (data === undefined || data === null) {
            data = "";
        }
        return data;
    }

    value_to_data(value) {
        if (this.type === "int") {
            return parseInt(value, 10);
        } else {
            return super.value_to_data(value);
        }
    }
}

/*
    A UI helper for diplaying the value of an input.

    This is useful for range or number displays that want to show their value.

    Example HTML structure:

        <input name="range_example" type="range" value="0.6" step="0.01" min="0.33" max="1.0" />
        <span class="form-unit">
            <span data-display-value-for="range_example"></span> percent
        </span>

    `data-display-format` can be:

    - `float`, with `data-display-precision` controlling rounding.
    - `percent`

    If advanced formatting is needed, use `data-display-formatter`:

        data-display-formatter="input.valueAsNumber * 2"
*/
export class ValueDisplay {
    constructor(display_elem, formatter = null, target_elem = null) {
        this.display_elem = $(display_elem);

        if (target_elem) {
            target_elem = $(target_elem);
        } else {
            const target_id = this.display_elem.dataset.displayValueFor;
            target_elem =
                $(target_id) || document.querySelector(`[name="${target_id}"]`);
        }
        if (!target_elem) {
            console.error(
                "Could not find target element for ValueDisplay",
                this.display_elem
            );
        }
        this.target_elem = target_elem;

        this.setup_formatter(formatter);

        $on(target_elem, "input", () => this.update());
        $on(target_elem, "winterjs:update", () => this.update());
        this.update();
    }

    setup_formatter(formatter) {
        if (formatter) {
            this.formatter = formatter;
            return;
        }
        switch (this.display_elem.dataset.displayFormat) {
            case "float": {
                let precision =
                    parseInt(this.display_elem.dataset.displayPrecision, 10) ||
                    2;
                this.formatter = (input) =>
                    input.valueAsNumber.toFixed(precision);
                break;
            }
            case "percent":
                this.formatter = (input) => Math.round(input.value * 100);
                break;
            default:
                this.formatter = (input) => input.value;
                break;
        }
        if (this.display_elem.dataset.displayFormatter) {
            this.formatter = new Function(
                "input",
                `"use strict"; return ${this.display_elem.dataset.displayFormatter}`
            );
        }
    }

    update() {
        this.display_elem.innerText = this.formatter(this.target_elem);
    }
}

/*
    Bind the controls in the given `form` to the given `data`.

    The form controls must have a `data-bind` attribute to be wired up. For
    input type="text" the `data-bind-type` attribute can be set to `text`, `int`,
    or `float`.

    Also binds any value displays (elements with `data-display-value-for`).
*/
export class Form {
    constructor(form_elem, data) {
        this.elem = $(form_elem);
        this.submit_btn = this.elem.querySelector("button[type=submit]");
        this.bindings = [];
        this.fields = {};

        this._setup_submit();
        this._bind_all(data);
        this._bind_all_displays();
    }

    _setup_submit() {
        $on(this.elem, "submit", (e) => {
            if (this.elem.dataset.noSubmit !== undefined) {
                e.preventDefault();
            }
            if (this.submit_btn !== null) {
                this.submit_btn.classList.add("is-validation-enabled");
            }
            for (const binding of this.bindings) {
                binding.validation_enabled = true;
            }
        });
    }

    _bind_all(data) {
        for (const elem of this.elem.querySelectorAll(
            "input[data-bind], select[data-bind], textarea[data-bind]"
        )) {
            this.bind_one(elem, data);
        }
    }

    bind_one(elem, data) {
        let binding = null;

        switch (elem.tagName) {
            case "INPUT": {
                if (elem.type === "checkbox") {
                    binding = new CheckboxInputBinding(elem, data);
                } else {
                    switch (elem.dataset.bindType) {
                        case "int":
                            binding = new IntInputBinding(elem, data);
                            break;
                        case "float":
                            binding = new FloatInputBinding(elem, data);
                            break;
                        default:
                            binding = new TextInputBinding(elem, data);
                            break;
                    }
                }
                break;
            }

            case "TEXTAREA": {
                binding = new TextInputBinding(elem, data);
                break;
            }

            case "SELECT": {
                binding = new SelectInputBinding(elem, data);
                break;
            }

            default:
        }

        if (binding === null) {
            console.error(`Unimplemented databinding for element`, elem);
            return;
        }

        this.bindings.push(binding);
        this.fields[binding.name] = binding;
    }

    _bind_all_displays() {
        for (const elem of this.elem.querySelectorAll(
            "[data-display-value-for]"
        )) {
            new ValueDisplay(elem);
        }
    }

    /*
        Call this to update the form's fields whenever modifying the bound
        `data`.
    */
    update() {
        for (const binding of this.bindings) {
            binding.update_value();
        }
        this.elem.dispatchEvent(new Event("change", { bubbles: true }));
    }

    get valid() {
        return this.elem.checkValidity();
    }

    set custom_validity(val) {
        if (val) {
            this.elem.classList.remove("is-invalid");
        } else {
            this.elem.classList.add("is-invalid");
        }
    }

    get custom_validity() {
        return !this.elem.classList.contains("is-invalid");
    }

    get classList() {
        return this.elem.classList;
    }

    addEventListener(event, callback) {
        this.elem.addEventListener(event, callback);
    }
}
