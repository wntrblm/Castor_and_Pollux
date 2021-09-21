/*
    An advanced imagemap using SVG.

    <object> element attributes:
    - data-is-svg-map: attaches this functionality.
    - data-list: element ID of a <datalist> containing <options> that map the
        IDs of SVG areas to descriptions.
    - data-stylesheet: href to stylesheet to apply inside of the SVG.

    The stylesheet should have styles for .hoverable, .hoverable:hover, and
    .info-text.
*/
class SVGMap {
    constructor(object_elem) {
        this.object_elem = object_elem;
        this.svg_doc = object_elem.contentDocument;

        this.svg_doc = object_elem.contentDocument;
        this.svg_elem = this.svg_doc.querySelector("svg");
        apply_safari_img_in_svg_fix(this.svg_elem);

        this.insert_stylesheet().then(() => {
            this.insert_help_text();
            this.insert_info_text();
            this.attach_event_listeners();
        });
    }

    insert_help_text() {
        const help_text_elem = document.createElement("p");
        help_text_elem.innerText = "Hover or tap an item";
        help_text_elem.classList.add("svgmap-help-text");
        this.object_elem.parentNode.insertBefore(
            help_text_elem,
            this.object_elem.nextSibling
        );
    }

    async insert_stylesheet() {
        const resp = await fetch(this.object_elem.dataset.stylesheet);
        const style_elem = this.svg_doc.createElementNS(
            "http://www.w3.org/2000/svg",
            "style"
        );
        style_elem.setAttribute("type", "text/css");
        style_elem.textContent = await resp.text();
        this.svg_elem.appendChild(style_elem);
    }

    insert_info_text() {
        const info_text_template = document.getElementById(
            this.object_elem.dataset.infoTextTemplate
        );
        let info_text_elem = null;

        if (info_text_template !== null) {
            /* Translate the HTML template into and SVG defs so the namespace is correct. */
            const svg_defs = this.svg_doc.createElementNS(
                "http://www.w3.org/2000/svg",
                "defs"
            );
            svg_defs.innerHTML = info_text_template.innerHTML;
            info_text_elem = svg_defs.firstElementChild;
        } else {
            info_text_elem = this.svg_doc.createElementNS(
                "http://www.w3.org/2000/svg",
                "text"
            );
            info_text_elem.setAttribute("id", "info-text");
        }
        this.svg_elem.appendChild(info_text_elem);

        /* We only need the text element from this point on. */
        this.info_text_container = info_text_elem;
        this.info_text = this.svg_doc.getElementById("info-text");
        this.info_text.textContent = "";

        /* Check if there's a rect that's set to follow the text's size. */
        this.rect = this.svg_doc.querySelector("rect[data-size-to]");
    }

    update_bounding_rect() {
        if (this.rect === null) return;
        const bb = this.info_text.getBBox();
        this.rect.setAttribute("x", bb.x);
        this.rect.setAttribute("y", bb.y);
        this.rect.setAttribute("width", bb.width);
        this.rect.setAttribute("height", bb.height);
    }

    attach_event_listeners() {
        const datalist = document.getElementById(this.object_elem.dataset.list);
        for (let item of datalist.options) {
            const item_elem = this.svg_doc.getElementById(item.value);
            item_elem.classList.add("hoverable");
            item_elem.addEventListener("mouseenter", () => {
                this.info_text.textContent = item.label;
                this.info_text_container.classList.add("visible");
                this.info_text_container.dataset.value = item.value;
                this.update_bounding_rect();
            });
            item_elem.addEventListener("mouseleave", () => {
                this.info_text_container.classList.remove("visible");
            });
        }
    }
}

window.addEventListener("load", () => {
    const svg_maps = document.querySelectorAll("object[data-is-svg-map]");
    for (const elem of svg_maps) {
        new SVGMap(elem);
    }
});

const is_safari = /apple/i.test(navigator.vendor);

function apply_safari_img_in_svg_fix(svg) {
    /* Fix for safari, which continues to be the most obnoxious browser. */
    if (is_safari) {
        for (let elem of svg.querySelectorAll("[*|href]")) {
            elem.setAttribute(
                "href",
                elem.getAttributeNS("http://www.w3.org/1999/xlink", "href")
            );
        }
    }
}
