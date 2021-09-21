/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

import { $e, $on, DOMHelpers, ObjectHelpers } from "./utils.js";

/*
    Two-way databinding for a form input.

    Whenever the input changes, `data[key]` is updated with the value from the
    form.

    To go the other direction - update the form when `data[key]` changes, call
    `update_value()`.

    The html structure needed for this matches bulma's. If there's a .field
    container with a .validation-message somewhere, this will show validation
    messages there.
*/
export class InputBinding {
    constructor(elem, data, key = undefined) {
        this.elem = $e(elem);
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

    update_value(validate = true) {
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
        this.field_container = this.elem.closest(".field");
        if (this.field_container !== null) {
            this.validation_message = this.field_container.querySelector(
                ".validation-message"
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
        this.type = elem.dataset.bindingType;
        this.placeholder = this.elem.getAttribute("placeholder");

        if (options === undefined) {
            const options_key = elem.dataset.bindingOptions;
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
        DOMHelpers.remove_all_children(this.elem);
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

        console.log("placeholding");

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
    Bind the controls in the given `form` to the given `data`.

    The form controls must have a `data-binding-type` attribute with one of the
    following values:

    * text
    * int
    * float
    * checkbox
    * select
*/
export class Form {
    constructor(form_elem, data) {
        this.elem = $e(form_elem);
        this.submit_btn = this.elem.querySelector("button[type=submit]");
        this.bindings = [];
        this.fields = {};

        this._setup_submit();
        this._bind_all(data);
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
            "input[data-binding-type], select[data-binding-type], textarea[data-binding-type]"
        )) {
            this.bind_one(elem, data);
        }
    }

    bind_one(elem, data) {
        let binding = null;

        switch (elem.dataset.bindingType) {
            case "text":
                binding = new TextInputBinding(elem, data);
                break;

            case "int":
                if (elem.tagName === "SELECT") {
                    binding = new IntInputBinding(elem, data);
                } else {
                    binding = new SelectInputBinding(elem, data);
                }
                break;

            case "float":
                binding = new FloatInputBinding(
                    elem,
                    data,
                    undefined,
                    elem.dataset.bindingPrecision
                );
                break;

            case "checkbox":
                binding = new CheckboxInputBinding(elem, data);
                break;

            case "select": {
                binding = new SelectInputBinding(elem, data);
                break;
            }

            default:
        }

        if (binding === null) {
            console.error(
                `Unimplemented binding type ${elem.dataset.bindingType}`
            );
            return;
        }

        this.bindings.push(binding);
        this.fields[binding.name] = binding;
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

    get classList() {
        return this.elem.classList;
    }

    addEventListener(event, callback) {
        this.elem.addEventListener(event, callback);
    }
}

/*
    A UI helper for diplaying the value of an input.

    This is useful for range or number displays that want to show their value.

    Example HTML structure:

        <input name="range_example" type="range" value="0.6" step="0.01" min="0.33" max="1.0" />
        <span class="form-unit">
            <span id="range_example_value_display"></span> percent
        </span>

    And JS usage:

        new ValueDisplay(document.querySelector("input[name=range_example]"), (elem) => elem.value);
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
