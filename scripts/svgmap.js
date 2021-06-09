document.addEventListener("DOMContentLoaded", () => {
    const svg_elem = document.getElementById("kitsvg");
    document.getElementById("kitsvg-hover-text").classList.remove("hidden");
    svg_elem.addEventListener("load", () => {
        const svg = svg_elem.contentDocument;

        /* Fix for safari, which continues to be the most obnoxious browser. */
        const is_safari = /apple/i.test(navigator.vendor);

        if (is_safari) {
            for (let elem of svg.querySelectorAll("[*|href]")) {
                console.log(elem);
                elem.setAttribute(
                    "href",
                    elem.getAttributeNS("http://www.w3.org/1999/xlink", "href")
                );
            }
        }

        /* Create the custom stylesheet */
        const style_elem = svg.createElementNS(
            "http://www.w3.org/2000/svg",
            "style"
        );
        style_elem.setAttribute("type", "text/css");
        style_elem.textContent = `
            .hoverable {
                fill: rgba(0, 0, 0, 0) !important;
                stroke: none !important;
                mix-blend-mode: color !important;
                filter: blur(20px) !important;
                transition: fill 0.1s;
            }
            .hoverable:hover {
                fill: #66ADB5 !important;
            }
            .info-text {
                font-family: Overpass, sans-serif;
                font-size: 300%; fill: white;
                transition: fill 0.1s;
            }
        `;
        svg.querySelector("svg").appendChild(style_elem);

        /* Create the text element to hold the description text */
        const info_text = svg.createElementNS(
            "http://www.w3.org/2000/svg",
            "text"
        );
        info_text.setAttribute("class", "info-text");
        info_text.setAttribute("x", "3%");
        info_text.setAttribute("y", "95%");
        info_text.textContent = "";
        svg.querySelector("svg").appendChild(info_text);

        /* Now attach event listeners to all of the hoverable elements */
        const items = [
            {
                id: "mainboard",
                description: "Mainboard (1)",
            },
            {
                id: "jackboard",
                description: "Jackboard PCB (1)",
            },
            {
                id: "panel",
                description: "Front panel (1)",
            },
            {
                id: "pin-sockets",
                description: '20-pin 2.54" pin sockets (2)',
            },
            {
                id: "jacks",
                description: '1/8" jacks (13)',
            },
            {
                id: "nuts",
                description: 'Hex nuts for the 1/8" jacks (13)',
            },
            {
                id: "pots",
                description: "Potentiometers (13)",
            },
            {
                id: "switch",
                description: "Tactile switch (1)",
            },
            {
                id: "switch-cap",
                description: "Tactile switch cap (1)",
            },
            {
                id: "screw",
                description: "M3x18 screw (1)",
            },
            {
                id: "spacer",
                description: "M3x11 spacer (1)",
            },
            {
                id: "nut",
                description: "M3 nut (1)",
            },
            {
                id: "power-header",
                description: "Eurorack power header (1)",
            },
            {
                id: "power-cable",
                description: "Eurorack power cable (1)",
            },
            {
                id: "mounting-screws",
                description: "M3x6 screws (4)",
            },
            {
                id: "rubber-bands",
                description: "Rubber bands (2)",
            },
        ];
        for (let item of items) {
            const item_elem = svg.getElementById(item.id);
            item_elem.classList.add("hoverable");
            item_elem.addEventListener("mouseenter", () => {
                info_text.textContent = item.description;
                info_text.style = "fill: white;";
            });
            item_elem.addEventListener("mouseleave", () => {
                info_text.style = "fill: transparent;";
            });
        }
    });
});
