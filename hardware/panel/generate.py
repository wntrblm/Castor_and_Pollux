#!/usr/bin/env python3
# Generate the panels' KiCAD files from the design.

from wintertools.affinity2kicad import Converter, helpers, SVGDocument, PCB, FancyText


def generate():
    doc = SVGDocument("panel.svg")
    pcb = PCB(title="Castor & Pollux", rev="v1")
    pcb.comment2 = "CC BY-SA 4.0"

    converter = Converter(doc, pcb)
    converter.convert()

    # Add standard slotted Eurorack mounting holes.
    helpers.add_eurorack_mounting_holes(converter)
    helpers.add_eurorack_mounting_holes(converter, hp=11)

    # Add board info to backside, right above the bottom mounting hole.
    fancytext = FancyText(font="Overpass", size=1.5, line_spacing=1.2)
    text = fancytext.generate(
        f"Castor + Pollux DIY Panel\n"
        f"{pcb.date} {pcb.rev}\n"
        "Winterbloom\n"
        "CC BY-SA 4.0",
        layer="B.Mask",
    )
    pcb.add_mod(text, 70.8 / 2, 121)

    pcb.write("panel.kicad_pcb")

    print("Finished!")


if __name__ == "__main__":
    generate()
