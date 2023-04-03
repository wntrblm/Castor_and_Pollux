# Castor & Pollux kit build guide

[TOC]

## Before you build

Welcome to the build guide for Castor & Pollux's DIY kit. We hope you have a great time putting this module together and a wonderful time using it.

Please **read all instructions** thoroughly before starting. If you have questions or run into trouble please reach out to us on [discord] or drop us an email at support@winterbloom.com

This build is a **intermediate level** kit. You should be comfortable soldering through-hole components and working near surface mount components. If you're not, we encourage you to try out some other kits first, like our [Big Honking Button kit](https://winterbloom.com/store/winterbloom-big-honking-button-kit). Even if you've got some experience, we recommend taking a look at [Adafruit's guide to excellent soldering](https://learn.adafruit.com/adafruit-guide-excellent-soldering) to refresh your knowledge.

By the way, we program, test, and calibrate the main circuit board before sending it to you, so you don't have to worry about any of that- once you're done building you'll be good to go!

This build takes around **one hour** to complete.

[discord]: https://discord.gg/UpfqghQ

## Tools and materials required

Before jumping in, make sure you have:

* Safety glasses. Yes, really.
* Proper ventilation - at least open a window or two.
* A soldering iron, like [this one](https://www.adafruit.com/product/180).
* Solder, we can recommend [Adafruit's 60/40 **no-clean** rosin core solder](https://www.adafruit.com/product/145).
* A small flat head screwdriver, like [this one](https://lovemyswitches.com/2mm-flat-head-screwdriver-for-knob-set-screws/).


!!! fairy
    We suggest using solder with "no clean" flux. If you use a different kind of flux, be sure to carefully clean the flux residue off based on the guidelines provided by the manufacturer of your solder. Take special care with the LEDs and potentiometers, as they can be damaged by water and flux cleaners.

## Kit contents

Your kit should contain the following items. If any are missing please email us at support@winterbloom.com.

<object
    alt="Kit contents"
    data-is-svg-map
    data-list="kit-contents-img-datalist"
    data-stylesheet="/styles/kit-contents-svgmap.css"
    data-info-text-template="kit-contents-img-info-text"
    data="../images/kit/kit-contents.svg"
    height="100%"
    id="kit-contents-img"
    type="image/svg+xml"
    width="100%">
</object>
<template id="kit-contents-img-info-text">
    <g id="info-text-container">
        <rect data-size-to="info-text"></rect>
        <text id="info-text">Test test test</text>
    </g>
</template>
<datalist id="kit-contents-img-datalist">
    <option value="faceplate">Faceplate</option>
    <option value="mainboard">Mainboard</option>
    <option value="expander-board">Expander board</option>
    <option value="expander-faceplate">Expander faceplate</option>
    <option value="rubber-bands">Rubber bands (2)</option>
    <option value="expander-cable">Expander cable</option>
    <option value="button">Tactile switch</option>
    <option value="cap">Tactile switch cap</option>
    <option value="power-header">Eurorack power header</option>
    <option value="jack-nuts">Nuts for 1/8" jacks (13)</option>
    <option value="jacks">1/8" jacks (13)</option>
    <option value="small-knobs">Small knobs (4)</option>
    <option value="big-knobs">Big knobs (2)</option>
    <option value="trimpots">Tall trimmer pots (6)</option>
    <option value="pots">9mm pots (6)</option>
    <option value="pot-washers-and-nuts">Washers and nuts for 9mm pots (6)</option>
</datalist>
<script type="module" src="/scripts/svgmap.js"></script>
<link rel="stylesheet" href="/styles/svgmap.css"/>

- (1) Mainboard
- (1) Faceplate
- (1) Expander board
- (1) Expander faceplate
- (1) Expander cable
- (13) 1/8" jacks
- (13) Hex nuts for the 1/8" jacks
- (6) Tall trimmer pots
- (6) 9mm pots
- (6) Washers and nuts for the 9mm pots
- (4) Small knobs
- (2) Large knobs
- (1) Tactile switch
- (1) Tactile switch cap
- (1) Eurorack power header
- (2) Rubber bands

## Power header

Your first task is to solder the 10-pin Eurorack power connector to the mainboard.

![Power header and mainboard](images/kit/4%20-%20mainboard%20and%20power%20header.webp)

The power connector goes on the **back** side of the board. When placing **note the notch in the outline on the board**. You'll need to make sure the **slot on the connector matches where the notch is on the outline**.

![Power connector placed](images/kit/5%20-%20power%20header.webp)

Once placed, make sure to push it flush against the board and then solder the 10 pins on the front side of the board. Be careful here and avoid touching the small components near the pins with your iron.

<figure data-layered>
  <img title="1" src="../images/kit/6 - power header.webp" class="active">
  <img title="2" src="../images/kit/7 - power header.webp">
  <img title="3" src="../images/kit/8 - power header.webp">
</figure>

!!! warning "Watch out for those LEDs!"
    Avoid touching the LEDs with your iron- they really don't like being melted and they're very hard to replace.

## 9mm pots

The next task is placing and soldering the six 9mm pots on the mainboard.

![Pots and mainboard](images/kit/11%20-%20mainboard%20and%20pots.webp)

This step requires **special care and attention** to make sure that the front panel aligns correctly. Please make sure everything is correct before soldering anything in place.

One of the six pots has been modified - its pins are shorter and bent outwards. Find this one and place it aside for the moment.

![Special pot](images/kit/10%20-%20special%20pot.webp)

Place five of the six pots onto the mainboard in the spots labeled `pitch`, `duty`, and `lfo`. You may need to bend or straighten the mounting legs on the pots to get them in place.

<figure data-layered>
  <img src="../images/kit/12 - pots.webp" class="active">
  <img src="../images/kit/13 - pots.webp">
  <img src="../images/kit/14 - pots.webp">
  <img src="../images/kit/15 - pots.webp">
  <img src="../images/kit/16 - pots.webp">
  <img src="../images/kit/17 - pots.webp">
</figure>

Take the remaining, modified pot and place it on the spot labeled `crossfade`. This pot will not be as secure as the others, but make sure the pins line up with the pads on the as board shown below.

![Special pot placed](images/kit/18%20-%20special%20pot.webp)

Next, take one of the 1/8" jacks and place it into the spot at the center bottom edge of the board labeled `mix`:

<figure data-layered>
  <img src="../images/kit/19 - jack.webp" class="active">
  <img src="../images/kit/20 - jack.webp">
</figure>

Next, carefully place the faceplate onto the front of the module. Make sure all of the pots and the jack are aligned and resting in their respective holes.

<figure data-layered>
  <img src="../images/kit/21 - panel.webp" class="active">
  <img src="../images/kit/30 - panel.webp">
  <img src="../images/kit/22 - panel.webp">
</figure>

Take one of the rubber bands and wrap it around the board and faceplate twice to hold the faceplate in place during the next few steps.

<figure data-layered>
  <img src="../images/kit/23 - rubber band.webp" class="active">
  <img src="../images/kit/24 - rubber band.webp">
</figure>

Flip the whole thing upside-down and solder the pots and jack in place.

<figure data-layered>
  <img src="../images/kit/25 - soldering.webp" class="active">
  <img src="../images/kit/26 - soldering.webp">
  <img src="../images/kit/27 - soldering.webp">
  <img src="../images/kit/28 - soldering.webp">
  <img src="../images/kit/29 - soldering jack.webp">
</figure>

!!! warning
    Take extra care not to hit any of the surface mount components with your iron.

Next, flip the assembly rightside-up and remove the rubber bands and faceplate.

![Removing the faceplate](images/kit/30%20-%20panel.webp)

Finally, solder the legs of the modified pot to the pads on the board. Take care not to accidentally bridge the legs together. If you find that the legs are misaligned, you can flip the board back over and melt the solder on the mounting legs and twist it back into alignment.

<figure data-layered>
  <img src="../images/kit/31 - special pot.webp" class="active">
  <img src="../images/kit/32 - soldering special pot.webp">
  <img src="../images/kit/33 - soldering special pot.webp">
</figure>

## Tactile switch

Next up is the tactile switch and its cap.

![Tactile switch and cap](images/kit/34%20-%20button.webp)

Place the tactile switch on the spot labeled `btn` on the mainboard. Make sure it's fully inserted against the board.

![Tactile switch placed](images/kit/35%20-%20button.webp)

Next, place the cap on top of the tactile switch and press in firmly to completely seat it on the switch.

<figure data-layered>
  <img src="../images/kit/36 - button.webp" class="active">
  <img src="../images/kit/37 - button.webp">
</figure>

## Jacks and trimpots

Next up is all six trimpots and six of the 1/8" jacks.

![Jacks and trimpots](images/kit/38%20-%20jacks%20and%20pots.webp)

Start from the **right** side of the board and place the pots and jacks for `ramp`, `pulse`, `duty`, `sub`, `pitch`, and `out`.

<figure data-layered>
  <img src="../images/kit/39 - trimpots.webp" class="active">
  <img src="../images/kit/40 - trimpots.webp">
  <img src="../images/kit/41 - jacks.webp">
  <img src="../images/kit/42 - trimpots.webp">
  <img src="../images/kit/43 - jacks.webp">
  <img src="../images/kit/44 - jacks.webp">
</figure>

Continue on the left side with the pots and jacks for `pulse`, `ramp`, `sub`, `duty`, `out`, and `pitch`.

<figure data-layered>
  <img src="../images/kit/45 - trimpots.webp" class="active">
  <img src="../images/kit/46 - trimpots.webp">
  <img src="../images/kit/47 - placements.webp">
</figure>

Next, place the faceplate and rubber band just as you did earlier.

<figure data-layered>
  <img src="../images/kit/48 - panel.webp" class="active">
  <img src="../images/kit/49 - rubber band.webp">
  <img src="../images/kit/50 - rubber band.webp">
  <img src="../images/kit/51 - rubber band.webp">
</figure>

Flip the module upside-down and solder the pots and jacks into place.

<figure data-layered>
  <img src="../images/kit/52 - soldering.webp" class="active">
  <img src="../images/kit/53 - soldering.webp">
</figure>

## Nuts

Now that everything is soldered in place the next step is to secure the faceplate using nuts. Start with the six larger nuts and washers for the 9mm pots.

![9mm pot nuts and washers](images/kit/54%20-%20nuts%20and%20washers.webp)

Place a washer on the shaft of each of the six 9mm pots.

<figure data-layered>
  <img src="../images/kit/55 - washer.webp" class="active">
  <img src="../images/kit/56 - washer.webp">
  <img src="../images/kit/57 - washers.webp">
</figure>

With the washers in place, place the nuts onto the shafts and tighten them in place.

<figure data-layered>
  <img src="../images/kit/58 - nut.webp" class="active">
  <img src="../images/kit/59 - nut.webp">
  <img src="../images/kit/60 - nuts.webp">
</figure>

Next up is the seven hex nuts for the 1/8 jacks.

![Hex nuts for 1/8" jacks](images/kit/61%20-%20nuts.webp)

Place the nuts on each jack and tighten them in place.

<figure data-layered>
  <img src="../images/kit/62 - nut.webp" class="active">
  <img src="../images/kit/63 - nut.webp">
  <img src="../images/kit/64 - nuts.webp">
</figure>


## Knobs

The final step for the main module is attaching the six knobs.

![Two large knobs and two small knobs](images/kit/65%20-%20knobs.webp)

Start by loosening the set screw on each of the knobs using a small flat head screwdriver.

![Loosening the set screw](images/kit/66%20-%20knob.webp)

Next, turn all of the potentiometers fully counterclockwise. Place the two large knobs on the top two shafts with the indicator line at the 7 o' clock position. Tighten the set screws to secure the knobs in place.

<figure data-layered>
  <img src="../images/kit/67 - knob.webp" class="active">
  <img src="../images/kit/68 - knob.webp">
  <img src="../images/kit/69 - knobs.webp">
</figure>

Repeat the same process for the four smaller knobs.

![Placing the smaller knobs](images/kit/70%20-%20knob.webp)

## Module completed

Congrats, you finished building your very own Castor & Pollux!

![Completed module](images/kit/71%20-%20finished.webp)

Continue reading to finish assembling the expander if you'd like. Don't forget to go check out the [User's Guide](/). We'd love to see your work, feel free to tag us on social media - we're `@wntrblm` on [Twitter](https://twitter.com/wntrblm) and [Instagram](https://instagram.com/wntrblm).

## Expander

To assemble the expander, you'll need the expander faceplate, six 1/8" jacks, six nuts for the jacks, and the expander board.

![Expander parts](images/kit/72%20-%20expander%20parts.webp)

Start by placing all six of the 1/8" jacks onto the front side of the expander board.

<figure data-layered>
  <img src="../images/kit/73 - jack.webp" class="active">
  <img src="../images/kit/74 - jacks.webp">
</figure>

Next, place the faceplate onto the jacks and temporarily hold it in place using a rubber band.

<figure data-layered>
  <img src="../images/kit/75 - panel.webp" class="active">
  <img src="../images/kit/76 - rubber band.webp">
</figure>

Flip it upside-down and solder all of the jacks into place.

<figure data-layered>
  <img src="../images/kit/77 - soldering.webp" class="active">
  <img src="../images/kit/78 - soldering.webp">
</figure>

Flip it rightside-up and secure the faceplate using the hex nuts.

<figure data-layered>
  <img src="../images/kit/79 - nut.webp" class="active">
  <img src="../images/kit/80 - nuts.webp">
</figure>

& you're finished!

## All done

Congrats on building your very own Castor & Pollux, we hope you had a lovely time! Don't forget to go check out the [User's Guide](/). We'd love to see your work, feel free to tag us on social media - we're `@wntrblm` on [Twitter](https://twitter.com/wntrblm) and [Instagram](https://instagram.com/wntrblm).

If you have any feedback or ran into any issues, feel free to drop us an email at support@winterbloom.com or file a issue on [GitHub](https://github.com/wntrblm/Castor_and_Pollux).

<script type="module" src="../scripts/layered.js"></script>
<link rel="stylesheet" href="../styles/layered.css" />
