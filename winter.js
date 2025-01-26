var __defProp = Object.defineProperty;
var __getOwnPropDesc = Object.getOwnPropertyDescriptor;
var __name = (target, value) => __defProp(target, "name", { value, configurable: true });
var __export = (target, all) => {
  for (var name in all)
    __defProp(target, name, { get: all[name], enumerable: true });
};
var __decorateClass = (decorators, target, key, kind) => {
  var result = kind > 1 ? void 0 : kind ? __getOwnPropDesc(target, key) : target;
  for (var i4 = decorators.length - 1, decorator; i4 >= 0; i4--)
    if (decorator = decorators[i4])
      result = (kind ? decorator(target, key, result) : decorator(result)) || result;
  if (kind && result)
    __defProp(target, key, result);
  return result;
};

// src/scripts/teeth.ts
var Teeth = class {
  static {
    __name(this, "Teeth");
  }
  static encodedLength(src_len) {
    if (src_len instanceof Uint8Array) {
      src_len = src_len.length;
    }
    return Math.floor((src_len + 4 - 1) / 4) * 5;
  }
  static encode(src) {
    const src_len = src.length;
    let dst = new Uint8Array(this.encodedLength(src_len));
    let src_idx = 0;
    let dst_idx = 0;
    while (src_idx < src_len) {
      if (src_idx + 4 <= src_len) {
        dst[dst_idx] = 64 | (src[src_idx] & 128) >> 4 | (src[src_idx + 1] & 128) >> 5 | (src[src_idx + 2] & 128) >> 6 | (src[src_idx + 3] & 128) >> 7;
        dst[dst_idx + 1] = src[src_idx] & 127;
        dst[dst_idx + 2] = src[src_idx + 1] & 127;
        dst[dst_idx + 3] = src[src_idx + 2] & 127;
        dst[dst_idx + 4] = src[src_idx + 3] & 127;
        dst_idx += 5;
        src_idx += 4;
      } else if (src_idx + 3 == src_len) {
        dst[dst_idx] = 48 | (src[src_idx] & 128) >> 4 | (src[src_idx + 1] & 128) >> 5 | (src[src_idx + 2] & 128) >> 6;
        dst[dst_idx + 1] = src[src_idx] & 127;
        dst[dst_idx + 2] = src[src_idx + 1] & 127;
        dst[dst_idx + 3] = src[src_idx + 2] & 127;
        dst_idx += 4;
        src_idx += 3;
      } else if (src_idx + 2 == src_len) {
        dst[dst_idx] = 32 | (src[src_idx] & 128) >> 4 | (src[src_idx + 1] & 128) >> 5;
        dst[dst_idx + 1] = src[src_idx] & 127;
        dst[dst_idx + 2] = src[src_idx + 1] & 127;
        dst_idx += 3;
        src_idx += 2;
      } else if (src_idx + 1 == src_len) {
        dst[dst_idx] = 16 | (src[src_idx] & 128) >> 4;
        dst[dst_idx + 1] = src[src_idx] & 127;
        dst_idx += 2;
        src_idx += 1;
      } else {
        break;
      }
    }
    return dst;
  }
  static decode(src) {
    const src_len = src.length;
    const dst_len = src_len / 5 * 4;
    let dst = new Uint8Array(dst_len);
    let src_idx = 0;
    let dst_idx = 0;
    while (src_idx < src_len) {
      dst[dst_idx] = (src[src_idx] & 8) << 4 | src[src_idx + 1];
      dst[dst_idx + 1] = (src[src_idx] & 4) << 5 | src[src_idx + 2];
      dst[dst_idx + 2] = (src[src_idx] & 2) << 6 | src[src_idx + 3];
      dst[dst_idx + 3] = (src[src_idx] & 1) << 7 | src[src_idx + 4];
      let len_marker = src[src_idx] >> 4;
      dst_idx += len_marker;
      src_idx += 5;
    }
    return dst.slice(0, dst_idx);
  }
};

// src/scripts/audio/midi.ts
var MIDI = class {
  constructor(portName) {
    this.portName = portName;
  }
  static {
    __name(this, "MIDI");
  }
  async connect() {
    let access = await navigator.requestMIDIAccess({ sysex: true });
    for (const port of access.inputs.values()) {
      if (port.name === this.portName) {
        this.input = port;
      }
    }
    for (const port of access.outputs.values()) {
      if (port.name === this.portName) {
        this.output = port;
      }
    }
    if (this.input == void 0 || this.output == void 0) {
      throw `Unable to connect to ${this.portName}`;
    }
  }
  send(data) {
    this.output.send(data);
  }
  async receive() {
    const done = new Promise((resolve) => {
      this.input.onmidimessage = function(msg) {
        resolve(msg);
      };
    });
    return await done;
  }
  async transact(data) {
    const done = new Promise((resolve) => {
      this.input.onmidimessage = function(msg) {
        resolve(msg);
      };
      this.output.send(data);
    });
    return await done;
  }
};

// src/scripts/base/dom.ts
var dom_exports = {};
__export(dom_exports, {
  $: () => $,
  $$: () => $$,
  isHTMLElement: () => isHTMLElement,
  removeAllChildren: () => removeAllChildren
});

// src/scripts/base/types.ts
function isString(value) {
  return typeof value === "string";
}
__name(isString, "isString");

// src/scripts/base/dom.ts
function isHTMLElement(v2) {
  return typeof HTMLElement === "object" && v2 instanceof HTMLElement;
}
__name(isHTMLElement, "isHTMLElement");
function $(x2) {
  if (isHTMLElement(x2)) {
    return x2;
  } else if (isString(x2)) {
    return document.getElementById(x2) ?? document.querySelector(x2);
  }
  return x2;
}
__name($, "$");
function $$(selectorOrParent, selector) {
  if (isHTMLElement(selectorOrParent)) {
    return selectorOrParent.querySelectorAll(selector);
  } else {
    return document.querySelectorAll(selectorOrParent);
  }
}
__name($$, "$$");
function removeAllChildren(node) {
  var range = document.createRange();
  range.selectNodeContents(node);
  range.deleteContents();
}
__name(removeAllChildren, "removeAllChildren");

// node_modules/@lit/reactive-element/css-tag.js
var t = globalThis;
var e = t.ShadowRoot && (void 0 === t.ShadyCSS || t.ShadyCSS.nativeShadow) && "adoptedStyleSheets" in Document.prototype && "replace" in CSSStyleSheet.prototype;
var s = Symbol();
var o = /* @__PURE__ */ new WeakMap();
var n = class {
  static {
    __name(this, "n");
  }
  constructor(t4, e7, o6) {
    if (this._$cssResult$ = true, o6 !== s)
      throw Error("CSSResult is not constructable. Use `unsafeCSS` or `css` instead.");
    this.cssText = t4, this.t = e7;
  }
  get styleSheet() {
    let t4 = this.o;
    const s4 = this.t;
    if (e && void 0 === t4) {
      const e7 = void 0 !== s4 && 1 === s4.length;
      e7 && (t4 = o.get(s4)), void 0 === t4 && ((this.o = t4 = new CSSStyleSheet()).replaceSync(this.cssText), e7 && o.set(s4, t4));
    }
    return t4;
  }
  toString() {
    return this.cssText;
  }
};
var r = /* @__PURE__ */ __name((t4) => new n("string" == typeof t4 ? t4 : t4 + "", void 0, s), "r");
var i = /* @__PURE__ */ __name((t4, ...e7) => {
  const o6 = 1 === t4.length ? t4[0] : e7.reduce((e8, s4, o7) => e8 + ((t5) => {
    if (true === t5._$cssResult$)
      return t5.cssText;
    if ("number" == typeof t5)
      return t5;
    throw Error("Value passed to 'css' function must be a 'css' function result: " + t5 + ". Use 'unsafeCSS' to pass non-literal values, but take care to ensure page security.");
  })(s4) + t4[o7 + 1], t4[0]);
  return new n(o6, t4, s);
}, "i");
var S = /* @__PURE__ */ __name((s4, o6) => {
  if (e)
    s4.adoptedStyleSheets = o6.map((t4) => t4 instanceof CSSStyleSheet ? t4 : t4.styleSheet);
  else
    for (const e7 of o6) {
      const o7 = document.createElement("style"), n6 = t.litNonce;
      void 0 !== n6 && o7.setAttribute("nonce", n6), o7.textContent = e7.cssText, s4.appendChild(o7);
    }
}, "S");
var c = e ? (t4) => t4 : (t4) => t4 instanceof CSSStyleSheet ? ((t5) => {
  let e7 = "";
  for (const s4 of t5.cssRules)
    e7 += s4.cssText;
  return r(e7);
})(t4) : t4;

// node_modules/@lit/reactive-element/reactive-element.js
var { is: i2, defineProperty: e2, getOwnPropertyDescriptor: r2, getOwnPropertyNames: h, getOwnPropertySymbols: o2, getPrototypeOf: n2 } = Object;
var a = globalThis;
var c2 = a.trustedTypes;
var l = c2 ? c2.emptyScript : "";
var p = a.reactiveElementPolyfillSupport;
var d = /* @__PURE__ */ __name((t4, s4) => t4, "d");
var u = { toAttribute(t4, s4) {
  switch (s4) {
    case Boolean:
      t4 = t4 ? l : null;
      break;
    case Object:
    case Array:
      t4 = null == t4 ? t4 : JSON.stringify(t4);
  }
  return t4;
}, fromAttribute(t4, s4) {
  let i4 = t4;
  switch (s4) {
    case Boolean:
      i4 = null !== t4;
      break;
    case Number:
      i4 = null === t4 ? null : Number(t4);
      break;
    case Object:
    case Array:
      try {
        i4 = JSON.parse(t4);
      } catch (t5) {
        i4 = null;
      }
  }
  return i4;
} };
var f = /* @__PURE__ */ __name((t4, s4) => !i2(t4, s4), "f");
var y = { attribute: true, type: String, converter: u, reflect: false, hasChanged: f };
Symbol.metadata ??= Symbol("metadata"), a.litPropertyMetadata ??= /* @__PURE__ */ new WeakMap();
var b = class extends HTMLElement {
  static {
    __name(this, "b");
  }
  static addInitializer(t4) {
    this._$Ei(), (this.l ??= []).push(t4);
  }
  static get observedAttributes() {
    return this.finalize(), this._$Eh && [...this._$Eh.keys()];
  }
  static createProperty(t4, s4 = y) {
    if (s4.state && (s4.attribute = false), this._$Ei(), this.elementProperties.set(t4, s4), !s4.noAccessor) {
      const i4 = Symbol(), r7 = this.getPropertyDescriptor(t4, i4, s4);
      void 0 !== r7 && e2(this.prototype, t4, r7);
    }
  }
  static getPropertyDescriptor(t4, s4, i4) {
    const { get: e7, set: h3 } = r2(this.prototype, t4) ?? { get() {
      return this[s4];
    }, set(t5) {
      this[s4] = t5;
    } };
    return { get() {
      return e7?.call(this);
    }, set(s5) {
      const r7 = e7?.call(this);
      h3.call(this, s5), this.requestUpdate(t4, r7, i4);
    }, configurable: true, enumerable: true };
  }
  static getPropertyOptions(t4) {
    return this.elementProperties.get(t4) ?? y;
  }
  static _$Ei() {
    if (this.hasOwnProperty(d("elementProperties")))
      return;
    const t4 = n2(this);
    t4.finalize(), void 0 !== t4.l && (this.l = [...t4.l]), this.elementProperties = new Map(t4.elementProperties);
  }
  static finalize() {
    if (this.hasOwnProperty(d("finalized")))
      return;
    if (this.finalized = true, this._$Ei(), this.hasOwnProperty(d("properties"))) {
      const t5 = this.properties, s4 = [...h(t5), ...o2(t5)];
      for (const i4 of s4)
        this.createProperty(i4, t5[i4]);
    }
    const t4 = this[Symbol.metadata];
    if (null !== t4) {
      const s4 = litPropertyMetadata.get(t4);
      if (void 0 !== s4)
        for (const [t5, i4] of s4)
          this.elementProperties.set(t5, i4);
    }
    this._$Eh = /* @__PURE__ */ new Map();
    for (const [t5, s4] of this.elementProperties) {
      const i4 = this._$Eu(t5, s4);
      void 0 !== i4 && this._$Eh.set(i4, t5);
    }
    this.elementStyles = this.finalizeStyles(this.styles);
  }
  static finalizeStyles(s4) {
    const i4 = [];
    if (Array.isArray(s4)) {
      const e7 = new Set(s4.flat(1 / 0).reverse());
      for (const s5 of e7)
        i4.unshift(c(s5));
    } else
      void 0 !== s4 && i4.push(c(s4));
    return i4;
  }
  static _$Eu(t4, s4) {
    const i4 = s4.attribute;
    return false === i4 ? void 0 : "string" == typeof i4 ? i4 : "string" == typeof t4 ? t4.toLowerCase() : void 0;
  }
  constructor() {
    super(), this._$Ep = void 0, this.isUpdatePending = false, this.hasUpdated = false, this._$Em = null, this._$Ev();
  }
  _$Ev() {
    this._$Eg = new Promise((t4) => this.enableUpdating = t4), this._$AL = /* @__PURE__ */ new Map(), this._$ES(), this.requestUpdate(), this.constructor.l?.forEach((t4) => t4(this));
  }
  addController(t4) {
    (this._$E_ ??= /* @__PURE__ */ new Set()).add(t4), void 0 !== this.renderRoot && this.isConnected && t4.hostConnected?.();
  }
  removeController(t4) {
    this._$E_?.delete(t4);
  }
  _$ES() {
    const t4 = /* @__PURE__ */ new Map(), s4 = this.constructor.elementProperties;
    for (const i4 of s4.keys())
      this.hasOwnProperty(i4) && (t4.set(i4, this[i4]), delete this[i4]);
    t4.size > 0 && (this._$Ep = t4);
  }
  createRenderRoot() {
    const t4 = this.shadowRoot ?? this.attachShadow(this.constructor.shadowRootOptions);
    return S(t4, this.constructor.elementStyles), t4;
  }
  connectedCallback() {
    this.renderRoot ??= this.createRenderRoot(), this.enableUpdating(true), this._$E_?.forEach((t4) => t4.hostConnected?.());
  }
  enableUpdating(t4) {
  }
  disconnectedCallback() {
    this._$E_?.forEach((t4) => t4.hostDisconnected?.());
  }
  attributeChangedCallback(t4, s4, i4) {
    this._$AK(t4, i4);
  }
  _$EO(t4, s4) {
    const i4 = this.constructor.elementProperties.get(t4), e7 = this.constructor._$Eu(t4, i4);
    if (void 0 !== e7 && true === i4.reflect) {
      const r7 = (void 0 !== i4.converter?.toAttribute ? i4.converter : u).toAttribute(s4, i4.type);
      this._$Em = t4, null == r7 ? this.removeAttribute(e7) : this.setAttribute(e7, r7), this._$Em = null;
    }
  }
  _$AK(t4, s4) {
    const i4 = this.constructor, e7 = i4._$Eh.get(t4);
    if (void 0 !== e7 && this._$Em !== e7) {
      const t5 = i4.getPropertyOptions(e7), r7 = "function" == typeof t5.converter ? { fromAttribute: t5.converter } : void 0 !== t5.converter?.fromAttribute ? t5.converter : u;
      this._$Em = e7, this[e7] = r7.fromAttribute(s4, t5.type), this._$Em = null;
    }
  }
  requestUpdate(t4, s4, i4) {
    if (void 0 !== t4) {
      if (i4 ??= this.constructor.getPropertyOptions(t4), !(i4.hasChanged ?? f)(this[t4], s4))
        return;
      this.C(t4, s4, i4);
    }
    false === this.isUpdatePending && (this._$Eg = this._$EP());
  }
  C(t4, s4, i4) {
    this._$AL.has(t4) || this._$AL.set(t4, s4), true === i4.reflect && this._$Em !== t4 && (this._$ET ??= /* @__PURE__ */ new Set()).add(t4);
  }
  async _$EP() {
    this.isUpdatePending = true;
    try {
      await this._$Eg;
    } catch (t5) {
      Promise.reject(t5);
    }
    const t4 = this.scheduleUpdate();
    return null != t4 && await t4, !this.isUpdatePending;
  }
  scheduleUpdate() {
    return this.performUpdate();
  }
  performUpdate() {
    if (!this.isUpdatePending)
      return;
    if (!this.hasUpdated) {
      if (this.renderRoot ??= this.createRenderRoot(), this._$Ep) {
        for (const [t6, s5] of this._$Ep)
          this[t6] = s5;
        this._$Ep = void 0;
      }
      const t5 = this.constructor.elementProperties;
      if (t5.size > 0)
        for (const [s5, i4] of t5)
          true !== i4.wrapped || this._$AL.has(s5) || void 0 === this[s5] || this.C(s5, this[s5], i4);
    }
    let t4 = false;
    const s4 = this._$AL;
    try {
      t4 = this.shouldUpdate(s4), t4 ? (this.willUpdate(s4), this._$E_?.forEach((t5) => t5.hostUpdate?.()), this.update(s4)) : this._$Ej();
    } catch (s5) {
      throw t4 = false, this._$Ej(), s5;
    }
    t4 && this._$AE(s4);
  }
  willUpdate(t4) {
  }
  _$AE(t4) {
    this._$E_?.forEach((t5) => t5.hostUpdated?.()), this.hasUpdated || (this.hasUpdated = true, this.firstUpdated(t4)), this.updated(t4);
  }
  _$Ej() {
    this._$AL = /* @__PURE__ */ new Map(), this.isUpdatePending = false;
  }
  get updateComplete() {
    return this.getUpdateComplete();
  }
  getUpdateComplete() {
    return this._$Eg;
  }
  shouldUpdate(t4) {
    return true;
  }
  update(t4) {
    this._$ET &&= this._$ET.forEach((t5) => this._$EO(t5, this[t5])), this._$Ej();
  }
  updated(t4) {
  }
  firstUpdated(t4) {
  }
};
b.elementStyles = [], b.shadowRootOptions = { mode: "open" }, b[d("elementProperties")] = /* @__PURE__ */ new Map(), b[d("finalized")] = /* @__PURE__ */ new Map(), p?.({ ReactiveElement: b }), (a.reactiveElementVersions ??= []).push("2.0.3");

// node_modules/lit-html/lit-html.js
var t2 = globalThis;
var i3 = t2.trustedTypes;
var s2 = i3 ? i3.createPolicy("lit-html", { createHTML: (t4) => t4 }) : void 0;
var e3 = "$lit$";
var h2 = `lit$${(Math.random() + "").slice(9)}$`;
var o3 = "?" + h2;
var n3 = `<${o3}>`;
var r3 = document;
var l2 = /* @__PURE__ */ __name(() => r3.createComment(""), "l");
var c3 = /* @__PURE__ */ __name((t4) => null === t4 || "object" != typeof t4 && "function" != typeof t4, "c");
var a2 = Array.isArray;
var u2 = /* @__PURE__ */ __name((t4) => a2(t4) || "function" == typeof t4?.[Symbol.iterator], "u");
var d2 = "[ 	\n\f\r]";
var f2 = /<(?:(!--|\/[^a-zA-Z])|(\/?[a-zA-Z][^>\s]*)|(\/?$))/g;
var v = /-->/g;
var _ = />/g;
var m = RegExp(`>|${d2}(?:([^\\s"'>=/]+)(${d2}*=${d2}*(?:[^ 	
\f\r"'\`<>=]|("|')|))|$)`, "g");
var p2 = /'/g;
var g = /"/g;
var $2 = /^(?:script|style|textarea|title)$/i;
var y2 = /* @__PURE__ */ __name((t4) => (i4, ...s4) => ({ _$litType$: t4, strings: i4, values: s4 }), "y");
var x = y2(1);
var b2 = y2(2);
var w = Symbol.for("lit-noChange");
var T = Symbol.for("lit-nothing");
var A = /* @__PURE__ */ new WeakMap();
var E = r3.createTreeWalker(r3, 129);
function C(t4, i4) {
  if (!Array.isArray(t4) || !t4.hasOwnProperty("raw"))
    throw Error("invalid template strings array");
  return void 0 !== s2 ? s2.createHTML(i4) : i4;
}
__name(C, "C");
var P = /* @__PURE__ */ __name((t4, i4) => {
  const s4 = t4.length - 1, o6 = [];
  let r7, l4 = 2 === i4 ? "<svg>" : "", c4 = f2;
  for (let i5 = 0; i5 < s4; i5++) {
    const s5 = t4[i5];
    let a4, u4, d3 = -1, y3 = 0;
    for (; y3 < s5.length && (c4.lastIndex = y3, u4 = c4.exec(s5), null !== u4); )
      y3 = c4.lastIndex, c4 === f2 ? "!--" === u4[1] ? c4 = v : void 0 !== u4[1] ? c4 = _ : void 0 !== u4[2] ? ($2.test(u4[2]) && (r7 = RegExp("</" + u4[2], "g")), c4 = m) : void 0 !== u4[3] && (c4 = m) : c4 === m ? ">" === u4[0] ? (c4 = r7 ?? f2, d3 = -1) : void 0 === u4[1] ? d3 = -2 : (d3 = c4.lastIndex - u4[2].length, a4 = u4[1], c4 = void 0 === u4[3] ? m : '"' === u4[3] ? g : p2) : c4 === g || c4 === p2 ? c4 = m : c4 === v || c4 === _ ? c4 = f2 : (c4 = m, r7 = void 0);
    const x2 = c4 === m && t4[i5 + 1].startsWith("/>") ? " " : "";
    l4 += c4 === f2 ? s5 + n3 : d3 >= 0 ? (o6.push(a4), s5.slice(0, d3) + e3 + s5.slice(d3) + h2 + x2) : s5 + h2 + (-2 === d3 ? i5 : x2);
  }
  return [C(t4, l4 + (t4[s4] || "<?>") + (2 === i4 ? "</svg>" : "")), o6];
}, "P");
var V = class _V {
  static {
    __name(this, "V");
  }
  constructor({ strings: t4, _$litType$: s4 }, n6) {
    let r7;
    this.parts = [];
    let c4 = 0, a4 = 0;
    const u4 = t4.length - 1, d3 = this.parts, [f3, v2] = P(t4, s4);
    if (this.el = _V.createElement(f3, n6), E.currentNode = this.el.content, 2 === s4) {
      const t5 = this.el.content.firstChild;
      t5.replaceWith(...t5.childNodes);
    }
    for (; null !== (r7 = E.nextNode()) && d3.length < u4; ) {
      if (1 === r7.nodeType) {
        if (r7.hasAttributes())
          for (const t5 of r7.getAttributeNames())
            if (t5.endsWith(e3)) {
              const i4 = v2[a4++], s5 = r7.getAttribute(t5).split(h2), e7 = /([.?@])?(.*)/.exec(i4);
              d3.push({ type: 1, index: c4, name: e7[2], strings: s5, ctor: "." === e7[1] ? k : "?" === e7[1] ? H : "@" === e7[1] ? I : R }), r7.removeAttribute(t5);
            } else
              t5.startsWith(h2) && (d3.push({ type: 6, index: c4 }), r7.removeAttribute(t5));
        if ($2.test(r7.tagName)) {
          const t5 = r7.textContent.split(h2), s5 = t5.length - 1;
          if (s5 > 0) {
            r7.textContent = i3 ? i3.emptyScript : "";
            for (let i4 = 0; i4 < s5; i4++)
              r7.append(t5[i4], l2()), E.nextNode(), d3.push({ type: 2, index: ++c4 });
            r7.append(t5[s5], l2());
          }
        }
      } else if (8 === r7.nodeType)
        if (r7.data === o3)
          d3.push({ type: 2, index: c4 });
        else {
          let t5 = -1;
          for (; -1 !== (t5 = r7.data.indexOf(h2, t5 + 1)); )
            d3.push({ type: 7, index: c4 }), t5 += h2.length - 1;
        }
      c4++;
    }
  }
  static createElement(t4, i4) {
    const s4 = r3.createElement("template");
    return s4.innerHTML = t4, s4;
  }
};
function N(t4, i4, s4 = t4, e7) {
  if (i4 === w)
    return i4;
  let h3 = void 0 !== e7 ? s4._$Co?.[e7] : s4._$Cl;
  const o6 = c3(i4) ? void 0 : i4._$litDirective$;
  return h3?.constructor !== o6 && (h3?._$AO?.(false), void 0 === o6 ? h3 = void 0 : (h3 = new o6(t4), h3._$AT(t4, s4, e7)), void 0 !== e7 ? (s4._$Co ??= [])[e7] = h3 : s4._$Cl = h3), void 0 !== h3 && (i4 = N(t4, h3._$AS(t4, i4.values), h3, e7)), i4;
}
__name(N, "N");
var S2 = class {
  static {
    __name(this, "S");
  }
  constructor(t4, i4) {
    this._$AV = [], this._$AN = void 0, this._$AD = t4, this._$AM = i4;
  }
  get parentNode() {
    return this._$AM.parentNode;
  }
  get _$AU() {
    return this._$AM._$AU;
  }
  u(t4) {
    const { el: { content: i4 }, parts: s4 } = this._$AD, e7 = (t4?.creationScope ?? r3).importNode(i4, true);
    E.currentNode = e7;
    let h3 = E.nextNode(), o6 = 0, n6 = 0, l4 = s4[0];
    for (; void 0 !== l4; ) {
      if (o6 === l4.index) {
        let i5;
        2 === l4.type ? i5 = new M(h3, h3.nextSibling, this, t4) : 1 === l4.type ? i5 = new l4.ctor(h3, l4.name, l4.strings, this, t4) : 6 === l4.type && (i5 = new L(h3, this, t4)), this._$AV.push(i5), l4 = s4[++n6];
      }
      o6 !== l4?.index && (h3 = E.nextNode(), o6++);
    }
    return E.currentNode = r3, e7;
  }
  p(t4) {
    let i4 = 0;
    for (const s4 of this._$AV)
      void 0 !== s4 && (void 0 !== s4.strings ? (s4._$AI(t4, s4, i4), i4 += s4.strings.length - 2) : s4._$AI(t4[i4])), i4++;
  }
};
var M = class _M {
  static {
    __name(this, "M");
  }
  get _$AU() {
    return this._$AM?._$AU ?? this._$Cv;
  }
  constructor(t4, i4, s4, e7) {
    this.type = 2, this._$AH = T, this._$AN = void 0, this._$AA = t4, this._$AB = i4, this._$AM = s4, this.options = e7, this._$Cv = e7?.isConnected ?? true;
  }
  get parentNode() {
    let t4 = this._$AA.parentNode;
    const i4 = this._$AM;
    return void 0 !== i4 && 11 === t4?.nodeType && (t4 = i4.parentNode), t4;
  }
  get startNode() {
    return this._$AA;
  }
  get endNode() {
    return this._$AB;
  }
  _$AI(t4, i4 = this) {
    t4 = N(this, t4, i4), c3(t4) ? t4 === T || null == t4 || "" === t4 ? (this._$AH !== T && this._$AR(), this._$AH = T) : t4 !== this._$AH && t4 !== w && this._(t4) : void 0 !== t4._$litType$ ? this.g(t4) : void 0 !== t4.nodeType ? this.$(t4) : u2(t4) ? this.T(t4) : this._(t4);
  }
  k(t4) {
    return this._$AA.parentNode.insertBefore(t4, this._$AB);
  }
  $(t4) {
    this._$AH !== t4 && (this._$AR(), this._$AH = this.k(t4));
  }
  _(t4) {
    this._$AH !== T && c3(this._$AH) ? this._$AA.nextSibling.data = t4 : this.$(r3.createTextNode(t4)), this._$AH = t4;
  }
  g(t4) {
    const { values: i4, _$litType$: s4 } = t4, e7 = "number" == typeof s4 ? this._$AC(t4) : (void 0 === s4.el && (s4.el = V.createElement(C(s4.h, s4.h[0]), this.options)), s4);
    if (this._$AH?._$AD === e7)
      this._$AH.p(i4);
    else {
      const t5 = new S2(e7, this), s5 = t5.u(this.options);
      t5.p(i4), this.$(s5), this._$AH = t5;
    }
  }
  _$AC(t4) {
    let i4 = A.get(t4.strings);
    return void 0 === i4 && A.set(t4.strings, i4 = new V(t4)), i4;
  }
  T(t4) {
    a2(this._$AH) || (this._$AH = [], this._$AR());
    const i4 = this._$AH;
    let s4, e7 = 0;
    for (const h3 of t4)
      e7 === i4.length ? i4.push(s4 = new _M(this.k(l2()), this.k(l2()), this, this.options)) : s4 = i4[e7], s4._$AI(h3), e7++;
    e7 < i4.length && (this._$AR(s4 && s4._$AB.nextSibling, e7), i4.length = e7);
  }
  _$AR(t4 = this._$AA.nextSibling, i4) {
    for (this._$AP?.(false, true, i4); t4 && t4 !== this._$AB; ) {
      const i5 = t4.nextSibling;
      t4.remove(), t4 = i5;
    }
  }
  setConnected(t4) {
    void 0 === this._$AM && (this._$Cv = t4, this._$AP?.(t4));
  }
};
var R = class {
  static {
    __name(this, "R");
  }
  get tagName() {
    return this.element.tagName;
  }
  get _$AU() {
    return this._$AM._$AU;
  }
  constructor(t4, i4, s4, e7, h3) {
    this.type = 1, this._$AH = T, this._$AN = void 0, this.element = t4, this.name = i4, this._$AM = e7, this.options = h3, s4.length > 2 || "" !== s4[0] || "" !== s4[1] ? (this._$AH = Array(s4.length - 1).fill(new String()), this.strings = s4) : this._$AH = T;
  }
  _$AI(t4, i4 = this, s4, e7) {
    const h3 = this.strings;
    let o6 = false;
    if (void 0 === h3)
      t4 = N(this, t4, i4, 0), o6 = !c3(t4) || t4 !== this._$AH && t4 !== w, o6 && (this._$AH = t4);
    else {
      const e8 = t4;
      let n6, r7;
      for (t4 = h3[0], n6 = 0; n6 < h3.length - 1; n6++)
        r7 = N(this, e8[s4 + n6], i4, n6), r7 === w && (r7 = this._$AH[n6]), o6 ||= !c3(r7) || r7 !== this._$AH[n6], r7 === T ? t4 = T : t4 !== T && (t4 += (r7 ?? "") + h3[n6 + 1]), this._$AH[n6] = r7;
    }
    o6 && !e7 && this.O(t4);
  }
  O(t4) {
    t4 === T ? this.element.removeAttribute(this.name) : this.element.setAttribute(this.name, t4 ?? "");
  }
};
var k = class extends R {
  static {
    __name(this, "k");
  }
  constructor() {
    super(...arguments), this.type = 3;
  }
  O(t4) {
    this.element[this.name] = t4 === T ? void 0 : t4;
  }
};
var H = class extends R {
  static {
    __name(this, "H");
  }
  constructor() {
    super(...arguments), this.type = 4;
  }
  O(t4) {
    this.element.toggleAttribute(this.name, !!t4 && t4 !== T);
  }
};
var I = class extends R {
  static {
    __name(this, "I");
  }
  constructor(t4, i4, s4, e7, h3) {
    super(t4, i4, s4, e7, h3), this.type = 5;
  }
  _$AI(t4, i4 = this) {
    if ((t4 = N(this, t4, i4, 0) ?? T) === w)
      return;
    const s4 = this._$AH, e7 = t4 === T && s4 !== T || t4.capture !== s4.capture || t4.once !== s4.once || t4.passive !== s4.passive, h3 = t4 !== T && (s4 === T || e7);
    e7 && this.element.removeEventListener(this.name, this, s4), h3 && this.element.addEventListener(this.name, this, t4), this._$AH = t4;
  }
  handleEvent(t4) {
    "function" == typeof this._$AH ? this._$AH.call(this.options?.host ?? this.element, t4) : this._$AH.handleEvent(t4);
  }
};
var L = class {
  static {
    __name(this, "L");
  }
  constructor(t4, i4, s4) {
    this.element = t4, this.type = 6, this._$AN = void 0, this._$AM = i4, this.options = s4;
  }
  get _$AU() {
    return this._$AM._$AU;
  }
  _$AI(t4) {
    N(this, t4);
  }
};
var Z = t2.litHtmlPolyfillSupport;
Z?.(V, M), (t2.litHtmlVersions ??= []).push("3.1.1");
var j = /* @__PURE__ */ __name((t4, i4, s4) => {
  const e7 = s4?.renderBefore ?? i4;
  let h3 = e7._$litPart$;
  if (void 0 === h3) {
    const t5 = s4?.renderBefore ?? null;
    e7._$litPart$ = h3 = new M(i4.insertBefore(l2(), t5), t5, void 0, s4 ?? {});
  }
  return h3._$AI(t4), h3;
}, "j");

// node_modules/lit-element/lit-element.js
var s3 = class extends b {
  static {
    __name(this, "s");
  }
  constructor() {
    super(...arguments), this.renderOptions = { host: this }, this._$Do = void 0;
  }
  createRenderRoot() {
    const t4 = super.createRenderRoot();
    return this.renderOptions.renderBefore ??= t4.firstChild, t4;
  }
  update(t4) {
    const i4 = this.render();
    this.hasUpdated || (this.renderOptions.isConnected = this.isConnected), super.update(t4), this._$Do = j(i4, this.renderRoot, this.renderOptions);
  }
  connectedCallback() {
    super.connectedCallback(), this._$Do?.setConnected(true);
  }
  disconnectedCallback() {
    super.disconnectedCallback(), this._$Do?.setConnected(false);
  }
  render() {
    return w;
  }
};
s3._$litElement$ = true, s3["finalized", "finalized"] = true, globalThis.litElementHydrateSupport?.({ LitElement: s3 });
var r4 = globalThis.litElementPolyfillSupport;
r4?.({ LitElement: s3 });
(globalThis.litElementVersions ??= []).push("4.0.3");

// node_modules/lit-html/static.js
var e4 = Symbol.for("");
var o4 = /* @__PURE__ */ __name((t4) => {
  if (t4?.r === e4)
    return t4?._$litStatic$;
}, "o");
var a3 = /* @__PURE__ */ new Map();
var l3 = /* @__PURE__ */ __name((t4) => (r7, ...e7) => {
  const i4 = e7.length;
  let s4, l4;
  const n6 = [], u4 = [];
  let c4, $3 = 0, f3 = false;
  for (; $3 < i4; ) {
    for (c4 = r7[$3]; $3 < i4 && void 0 !== (l4 = e7[$3], s4 = o4(l4)); )
      c4 += s4 + r7[++$3], f3 = true;
    $3 !== i4 && u4.push(l4), n6.push(c4), $3++;
  }
  if ($3 === i4 && n6.push(r7[i4]), f3) {
    const t5 = n6.join("$$lit$$");
    void 0 === (r7 = a3.get(t5)) && (n6.raw = n6, a3.set(t5, r7 = n6)), e7 = u4;
  }
  return t4(r7, ...e7);
}, "l");
var n4 = l3(x);
var u3 = l3(b2);

// node_modules/@lit/reactive-element/decorators/custom-element.js
var t3 = /* @__PURE__ */ __name((t4) => (e7, o6) => {
  void 0 !== o6 ? o6.addInitializer(() => {
    customElements.define(t4, e7);
  }) : customElements.define(t4, e7);
}, "t");

// node_modules/@lit/reactive-element/decorators/property.js
var o5 = { attribute: true, type: String, converter: u, reflect: false, hasChanged: f };
var r5 = /* @__PURE__ */ __name((t4 = o5, e7, r7) => {
  const { kind: n6, metadata: i4 } = r7;
  let s4 = globalThis.litPropertyMetadata.get(i4);
  if (void 0 === s4 && globalThis.litPropertyMetadata.set(i4, s4 = /* @__PURE__ */ new Map()), s4.set(r7.name, t4), "accessor" === n6) {
    const { name: o6 } = r7;
    return { set(r8) {
      const n7 = e7.get.call(this);
      e7.set.call(this, r8), this.requestUpdate(o6, n7, t4);
    }, init(e8) {
      return void 0 !== e8 && this.C(o6, void 0, t4), e8;
    } };
  }
  if ("setter" === n6) {
    const { name: o6 } = r7;
    return function(r8) {
      const n7 = this[o6];
      e7.call(this, r8), this.requestUpdate(o6, n7, t4);
    };
  }
  throw Error("Unsupported decorator location: " + n6);
}, "r");
function n5(t4) {
  return (e7, o6) => "object" == typeof o6 ? r5(t4, e7, o6) : ((t5, e8, o7) => {
    const r7 = e8.hasOwnProperty(o7);
    return e8.constructor.createProperty(o7, r7 ? { ...t5, wrapped: true } : t5), r7 ? Object.getOwnPropertyDescriptor(e8, o7) : void 0;
  })(t4, e7, o6);
}
__name(n5, "n");

// node_modules/@lit/reactive-element/decorators/state.js
function r6(r7) {
  return n5({ ...r7, state: true, attribute: false });
}
__name(r6, "r");

// node_modules/@lit/reactive-element/decorators/base.js
var e5 = /* @__PURE__ */ __name((e7, t4, c4) => (c4.configurable = true, c4.enumerable = true, Reflect.decorate && "object" != typeof t4 && Object.defineProperty(e7, t4, c4), c4), "e");

// node_modules/@lit/reactive-element/decorators/query.js
function e6(e7, r7) {
  return (n6, s4, i4) => {
    const o6 = /* @__PURE__ */ __name((t4) => t4.renderRoot?.querySelector(e7) ?? null, "o");
    if (r7) {
      const { get: e8, set: r8 } = "object" == typeof s4 ? n6 : i4 ?? (() => {
        const t4 = Symbol();
        return { get() {
          return this[t4];
        }, set(e9) {
          this[t4] = e9;
        } };
      })();
      return e5(n6, s4, { get() {
        let t4 = e8.call(this);
        return void 0 === t4 && (t4 = o6(this), (null !== t4 || this.hasUpdated) && r8.call(this, t4)), t4;
      } });
    }
    return e5(n6, s4, { get() {
      return o6(this);
    } });
  };
}
__name(e6, "e");

// src/scripts/audio/context-manager.ts
var FFT_SIZE = 4096;
var HAS_AUDIO_CONTEXT = window.AudioContext !== void 0;
var AudioContextManager = class {
  static {
    __name(this, "AudioContextManager");
  }
  constructor() {
    if (HAS_AUDIO_CONTEXT) {
      this.createContext();
    }
  }
  static get instance() {
    if (!this._instance) {
      this._instance = new this();
    }
    return this._instance;
  }
  createContext() {
    this.context = new window.AudioContext();
    this.analyser = this.context.createAnalyser();
    this.analyser.smoothingTimeConstant = 1;
    this.analyser.fftSize = FFT_SIZE;
    this.analyser.connect(this.context.destination);
    this.fftData = new Uint8Array(this.analyser.frequencyBinCount);
    this.activeSource = void 0;
  }
  checkContext() {
    if (!this.context) {
      throw new Error("Audio context unavailable");
    }
  }
  createSource(mediaElement) {
    this.checkContext();
    return this.context.createMediaElementSource(mediaElement);
  }
  get activeSource() {
    return this._activeSource;
  }
  set activeSource(src) {
    this.checkContext();
    if (this.activeSource) {
      this.activeSource.disconnect();
    }
    this._activeSource = src;
    if (this._activeSource) {
      this._activeSource.connect(this.analyser);
      this.context.resume();
    }
  }
  get analyzerData() {
    this.analyser.getByteTimeDomainData(this.fftData);
    return this.fftData;
  }
  static findFirstPositiveZeroCrossing(data) {
    let n6 = -1;
    for (let i4 = 0; i4 < data.length; i4++) {
      const val = data[i4];
      if (val < 128) {
        n6 = i4;
      } else if (val >= 128 && n6 > -1) {
        return n6;
      }
    }
    return 0;
  }
};

// src/scripts/base/media-queries.ts
var prefersReducedMotion = window.matchMedia(
  "(prefers-reduced-motion: reduce)"
);
var prefersDarkScheme = window.matchMedia(
  "(prefers-color-scheme: dark)"
);

// src/scripts/components/styles.ts
var styles_default = i`
    :host {
        box-sizing: border-box;
        display: block;
    }

    :host *,
    :host *::before,
    :host *::after {
        box-sizing: inherit;
    }

    [hidden] {
        display: none !important;
    }

    .elevated {
        box-shadow: var(--elevation-shadow, none);
    }
`;

// src/scripts/components/audio-oscilloscope.ts
var WinterAudioOscilloscopeElement = class extends s3 {
  start() {
    this.scope.start();
  }
  stop() {
    this.scope.stop();
  }
  render() {
    const width = Math.floor(800 * window.devicePixelRatio);
    const height = Math.floor(100 * window.devicePixelRatio);
    return x`<canvas width="${width}" height="${height}"></canvas>`;
  }
  firstUpdated(_changedProperties) {
    const computedStyles = window.getComputedStyle(this);
    this.ctx = this.canvas.getContext("2d", { alpha: false });
    this.scope = new Oscilloscope(
      this.ctx,
      computedStyles.backgroundColor ?? "#333333",
      computedStyles.color ?? "#FF0000",
      6
    );
  }
};
__name(WinterAudioOscilloscopeElement, "WinterAudioOscilloscopeElement");
WinterAudioOscilloscopeElement.styles = [
  styles_default,
  i`
            :host {
                display: block;
                width: 100%;
                aspect-ratio: 8 / 1;
            }

            canvas {
                width: 100%;
                height: 100%;
            }
        `
];
__decorateClass([
  e6("canvas")
], WinterAudioOscilloscopeElement.prototype, "canvas", 2);
WinterAudioOscilloscopeElement = __decorateClass([
  t3("winter-audio-oscilloscope")
], WinterAudioOscilloscopeElement);
var Oscilloscope = class {
  constructor(ctx, backgroundColor, strokeColor, strokeWidth) {
    this.ctx = ctx;
    this.backgroundColor = backgroundColor;
    this.strokeColor = strokeColor;
    this.strokeWidth = strokeWidth;
    this.active = false;
    this.sineOffset = 0;
    this.width = ctx.canvas.width;
    this.height = ctx.canvas.height;
    this.clear();
    this.drawSine();
  }
  static {
    __name(this, "Oscilloscope");
  }
  start() {
    this.active = true;
    window.requestAnimationFrame(() => this.draw());
  }
  stop() {
    this.active = false;
  }
  clear() {
    this.ctx.beginPath();
    this.ctx.fillStyle = this.backgroundColor;
    this.ctx.fillRect(0, 0, this.width, this.height);
  }
  draw() {
    if (!prefersReducedMotion.matches) {
      this.drawAnalyzer();
    } else {
      this.drawSine();
    }
  }
  drawAnalyzer() {
    const data = AudioContextManager.instance.analyzerData;
    const zeroCrossing = AudioContextManager.findFirstPositiveZeroCrossing(data);
    this.clear();
    this.ctx.lineJoin = "round";
    this.ctx.lineWidth = this.strokeWidth;
    this.ctx.strokeStyle = this.strokeColor;
    this.ctx.beginPath();
    const ymid = this.height / 2;
    for (let x2 = 0; x2 < this.width; x2++) {
      let sampleIdx = Math.round(x2 / this.width * data.length);
      sampleIdx = (zeroCrossing + sampleIdx) % data.length;
      const sample = data[sampleIdx] ?? 0;
      const sampleNorm = (sample - 127) / 127;
      const y3 = ymid + sampleNorm * 1.6 * ymid;
      if (x2 === 0) {
        this.ctx.moveTo(x2, y3);
      } else {
        this.ctx.lineTo(x2, y3);
      }
    }
    this.ctx.lineTo(this.width + 10, ymid);
    this.ctx.stroke();
    if (this.active) {
      window.requestAnimationFrame(() => this.drawAnalyzer());
    }
  }
  drawSine() {
    this.clear();
    this.ctx.lineWidth = this.strokeWidth;
    this.ctx.strokeStyle = this.strokeColor;
    this.ctx.beginPath();
    const divider = 4;
    const width = this.width / divider;
    for (let i4 = -2; i4 < width + 2; i4++) {
      var v2 = Math.sin(8 * Math.PI * (i4 / width) + this.sineOffset / 8);
      var y3 = this.height / 2 + v2 * this.height / 2 * 0.9;
      if (i4 === -2) {
        this.ctx.moveTo(i4 * divider, y3);
      } else {
        this.ctx.lineTo(i4 * divider, y3);
      }
    }
    this.ctx.stroke();
    this.sineOffset++;
    if (this.active) {
      window.requestAnimationFrame(() => this.drawSine());
    }
  }
};

// src/scripts/components/audio-player.ts
var WinterAudioPlayerElement = class extends s3 {
  get currentTime() {
    return this.audio?.currentTime;
  }
  set currentTime(value) {
    this.audio.currentTime = value;
  }
  get duration() {
    return this.audio?.duration;
  }
  get ended() {
    return this.audio?.ended;
  }
  get networkState() {
    return this.audio?.networkState;
  }
  get paused() {
    return this.audio?.paused;
  }
  get readyState() {
    return this.audio?.readyState;
  }
  get volume() {
    return this.audio?.volume;
  }
  set volume(value) {
    this.audio.volume = value;
  }
  play() {
    this.audio?.play();
  }
  pause() {
    this.audio?.pause();
  }
  pauseOthers() {
    const staticThis = this.constructor;
    for (const instance of staticThis.instances) {
      if (instance !== this) {
        instance.pause();
      }
    }
  }
  connectSource() {
    const manager = AudioContextManager.instance;
    if (!this.audioSourceNode) {
      this.audioSourceNode = manager.createSource(this.audio);
    }
    manager.activeSource = this.audioSourceNode;
  }
  onAudioPlay() {
    this.pauseOthers();
    this.connectSource();
    this.oscilloscope.start();
  }
  onAudioPauseOrEnd() {
    this.oscilloscope.stop();
  }
  connectedCallback() {
    super.connectedCallback();
    const staticThis = this.constructor;
    staticThis.instances.add(this);
  }
  disconnectedCallback() {
    super.disconnectedCallback();
    const staticThis = this.constructor;
    staticThis.instances.delete(this);
  }
  render() {
    let sources = this.querySelectorAll("source");
    if (!sources.length) {
      sources = n4`<source src="${this.src}" />`;
    }
    return x`<div class="elevated">
            <winter-audio-oscilloscope></winter-audio-oscilloscope>
            <audio
                controls
                crossorigin="anonymous"
                ?loop=${this.loop}
                title="${this.title}"
                @play=${this.onAudioPlay}
                @pause=${this.onAudioPauseOrEnd}
                @ended=${this.onAudioPauseOrEnd}>
                ${sources}
            </audio>
            <p>${this.title}</p>
        </div>`;
  }
};
__name(WinterAudioPlayerElement, "WinterAudioPlayerElement");
WinterAudioPlayerElement.instances = /* @__PURE__ */ new Set();
WinterAudioPlayerElement.styles = [
  styles_default,
  i`
            div {
                display: flex;
                flex-direction: column;
                width: 100%;
                max-width: 100%;
                padding: 0;
                color: white;
                background-color: #408d94;
                border: 0px solid transparent;
                border-radius: 0.2em;
            }

            audio {
                display: block;
                width: 90%;
                margin: 1em auto;
            }

            winter-audio-oscilloscope {
                background-color: inherit;
                color: inherit;
                margin: 2em 0 0.5em 0;
            }

            p {
                font-size: 1.25em;
                text-align: center;
                margin: 0 0 0.75em 0;
            }
        `
];
__decorateClass([
  e6("audio")
], WinterAudioPlayerElement.prototype, "audio", 2);
__decorateClass([
  e6("winter-audio-oscilloscope")
], WinterAudioPlayerElement.prototype, "oscilloscope", 2);
__decorateClass([
  n5()
], WinterAudioPlayerElement.prototype, "src", 2);
__decorateClass([
  n5({ type: Boolean })
], WinterAudioPlayerElement.prototype, "loop", 2);
WinterAudioPlayerElement = __decorateClass([
  t3("winter-audio-player")
], WinterAudioPlayerElement);

// src/scripts/base/iterator.ts
function* map(iterable, callback) {
  let n6 = 0;
  for (const i4 of iterable) {
    yield callback(i4, n6);
    n6++;
  }
}
__name(map, "map");

// src/scripts/components/carousel.ts
var WinterCarouselElement = class extends s3 {
  constructor() {
    super(...arguments);
    this._activeIndex = 0;
  }
  set activeIndex(val) {
    this.activeImage?.classList.remove("--is-active");
    this.images[val]?.classList.add("--is-active");
    this._activeIndex = val;
  }
  get activeIndex() {
    return this._activeIndex;
  }
  onNavigationClick(e7) {
    const index = parseInt(e7.target.dataset["index"], 10);
    this.activeIndex = index;
  }
  get images() {
    return this.querySelectorAll("img");
  }
  get activeImage() {
    return this.querySelector("img.--is-active");
  }
  render() {
    return x`<div class="elevated">
                <slot></slot>
            </div>
            <div @click="${this.onNavigationClick}" class="navigation elevated">
                ${map(this.images, (image, n6) => {
      return x`<button
                        type="button"
                        data-index="${n6}"
                        class="${this.activeIndex == n6 ? "--is-active" : ""}">
                        ${n6 + 1}
                    </button>`;
    })}
            </div>`;
  }
  firstUpdated(_changedProperties) {
    const images = this.images;
    if (images.length) {
      images[0]?.classList.add("--is-active");
    }
  }
};
__name(WinterCarouselElement, "WinterCarouselElement");
WinterCarouselElement.styles = [
  styles_default,
  i`
            :host {
                width: 100%;
                --button-fg: white;
                --button-bg: rgb(94, 64, 158);
                --button-bg-active: rgb(163, 138, 214);
                display: flex;
                flex-direction: column;
                align-items: center;
            }

            .navigation {
                margin: -0.5em 0 0em 0;
                width: fit-content;
                display: flex;
                flex-direction: row;
                justify-content: center;
                border-radius: 0.2rem;
            }

            .navigation button {
                font-family: inherit;
                line-height: 1.5;
                font-size: 1em;
                color: var(--button-fg);
                background: var(--button-bg);
                border: 1px solid transparent;
                padding: 0.2em 1em;
                transition: background 300ms ease;
            }

            .navigation button:hover,
            .navigation button:focus,
            .navigation button.--is-active {
                background: var(--button-bg-active);
            }

            .navigation button:first-child {
                border-top-left-radius: 0.2em;
                border-bottom-left-radius: 0.2em;
            }

            .navigation button:last-child {
                border-top-right-radius: 0.2em;
                border-bottom-right-radius: 0.2em;
            }

            ::slotted(img) {
                width: 100%;
                display: none !important;
            }

            ::slotted(img.--is-active) {
                display: block !important;
            }
        `
];
__decorateClass([
  n5({ type: Number })
], WinterCarouselElement.prototype, "activeIndex", 1);
WinterCarouselElement = __decorateClass([
  t3("winter-carousel")
], WinterCarouselElement);

// src/scripts/components/icon.ts
var WinterIconElement = class extends s3 {
  render() {
    return x`<slot></slot>`;
  }
};
__name(WinterIconElement, "WinterIconElement");
WinterIconElement.styles = [
  i`
            :host {
                display: inline-block;
                box-sizing: content-box;
                font-family: "Material Symbols Outlined";
                font-weight: normal;
                font-style: normal;
                font-size: inherit;
                line-height: 1;
                letter-spacing: normal;
                text-transform: none;
                white-space: nowrap;
                word-wrap: normal;
                direction: ltr;
                -webkit-font-feature-settings: "liga";
                -moz-font-feature-settings: "liga";
                font-feature-settings: "liga";
                -webkit-font-smoothing: antialiased;
                user-select: none;
            }
        `
];
WinterIconElement = __decorateClass([
  t3("winter-icon")
], WinterIconElement);

// src/scripts/components/dark-mode.ts
var WinterDarkModeElement = class extends s3 {
  connectedCallback() {
    super.connectedCallback();
    const mediaPrefersDark = prefersDarkScheme.matches;
    const storedPreference = window.localStorage.getItem("color-scheme");
    let useDark = storedPreference === "dark" || storedPreference !== "light" && mediaPrefersDark;
    if (useDark) {
      document.documentElement.classList.add("dark");
    }
  }
  onClick(e7) {
    if (document.documentElement.classList.contains("dark")) {
      document.documentElement.classList.remove("dark");
      localStorage.setItem("color-scheme", "light");
    } else {
      document.documentElement.classList.add("dark");
      localStorage.setItem("color-scheme", "dark");
    }
  }
  render() {
    return x`
            <button
                @click=${this.onClick}
                type="button"
                aria-label="switch between dark and light mode">
                <winter-icon>dark_mode</winter-icon>
            </button>
        `;
  }
};
__name(WinterDarkModeElement, "WinterDarkModeElement");
WinterDarkModeElement.styles = [
  styles_default,
  i`
            :host {
                color: inherit;
                font-family: inherit;
                font-size: inherit;
                display: inline-block;
                line-height: 0;
            }

            button {
                background: transparent;
                display: inline-block;
                border: none;
                font-family: inherit;
                font-size: inherit;
                padding: 0;
                align-items: unset;
                color: inherit;
                line-height: inherit;
            }

            button:hover {
                font-size: inherit;
                cursor: pointer;
            }

            winter-icon {
                font-size: inherit;
            }
        `
];
WinterDarkModeElement = __decorateClass([
  t3("winter-dark-mode")
], WinterDarkModeElement);

// src/scripts/components/image-map.ts
var WinterImageMapElement = class extends s3 {
  constructor() {
    super(...arguments);
    this.activeText = "test";
    this.idToDescription = /* @__PURE__ */ new Map();
  }
  get items() {
    return this.querySelectorAll(
      "winter-image-map-item"
    );
  }
  connectedCallback() {
    super.connectedCallback();
    if (this.src) {
      this.loadSrc();
    }
  }
  async loadSrc() {
    const resp = await fetch(this.src, {
      mode: "same-origin"
    });
    if (!resp.ok) {
      console.error(
        `Could not fetch ${this.src}: ${resp.status} ${resp.statusText}`
      );
    }
    const svgContents = await resp.text();
    const parser = new DOMParser();
    this.svg = parser.parseFromString(
      svgContents,
      "image/svg+xml"
    ).documentElement;
    this.prepSvg();
    this.requestUpdate();
  }
  prepSvg() {
    for (const item of this.items) {
      this.idToDescription.set(item.id, item.innerText);
      const itemArea = this.svg.querySelector(
        `[id=${item.id}]`
      );
      if (!itemArea) {
        console.warn(`Area ${item.id} not found in ${this.src}`);
        continue;
      }
      itemArea.classList.add("--is-interactive-area");
      if (!this.activeArea) {
        this.setActiveArea(itemArea);
      }
    }
    this.svg.addEventListener("mouseover", (e7) => {
      const target = e7.target;
      if (target.classList.contains("--is-interactive-area")) {
        this.setActiveArea(target);
      }
    });
  }
  setActiveArea(areaElem) {
    if (this.activeArea === areaElem) {
      return;
    }
    this.activeArea?.classList.remove("--is-active");
    this.activeArea = areaElem;
    this.activeArea.classList.add("--is-active");
    this.activeText = this.idToDescription.get(areaElem.id);
  }
  render() {
    return x`<div>${this.svg}</div>
            <h3 class="elevated">${this.activeText}</h3>`;
  }
};
__name(WinterImageMapElement, "WinterImageMapElement");
WinterImageMapElement.styles = [
  styles_default,
  i`
            :host {
                width: 100%;
                display: flex;
                flex-direction: column;
                align-items: center;
            }

            .--is-interactive-area {
                fill: #1ba8b800 !important;
                stroke-width: 10 !important;
                stroke: #1ba8b800 !important;
                mix-blend-mode: color !important;
                filter: blur(15px) !important;
                transition: 0.1s;
            }

            .--is-interactive-area.--is-active {
                fill: #1ba8b8 !important;
                stroke: #1ba8b800 !important;
            }

            div {
                width: 100%;
            }

            h3 {
                line-height: 1.5;
                font-size: 1em;
                font-weight: normal;
                border: 1px solid transparent;
                padding: 0.2em 1em;
                margin: -1em 0 0em 0;
                width: 60%;
                display: flex;
                flex-direction: row;
                justify-content: center;
                border-radius: 0.2rem;
                color: white;
                background: #1ba8b8;
            }
        `
];
__decorateClass([
  n5()
], WinterImageMapElement.prototype, "src", 2);
__decorateClass([
  r6()
], WinterImageMapElement.prototype, "activeText", 2);
WinterImageMapElement = __decorateClass([
  t3("winter-image-map")
], WinterImageMapElement);
var WinterImageMapItemElement = class extends s3 {
};
__name(WinterImageMapItemElement, "WinterImageMapItemElement");
WinterImageMapItemElement.styles = [
  i`
            :host {
                display: none;
                visibility: hidden;
            }
        `
];
WinterImageMapItemElement = __decorateClass([
  t3("winter-image-map-item")
], WinterImageMapItemElement);

// src/scripts/components/pcb-assembly.ts
var WinterPCBAssemblyElement = class extends s3 {
  createRenderRoot() {
    return this;
  }
  connectedCallback() {
    super.connectedCallback();
    if (this.src) {
      this.loadSrc();
    }
  }
  async loadSrc() {
    const resp = await fetch(this.src, {
      mode: "same-origin"
    });
    if (!resp.ok) {
      console.error(
        `Could not fetch ${this.src}: ${resp.status} ${resp.statusText}`
      );
    }
    this.pcbData = await resp.json();
    this.prepData();
    this.requestUpdate();
  }
  prepData() {
    this.bom = new BOM(this.pcbData);
    this.renderer = new PCBRenderer(this, this.pcbData, this.rotate);
  }
  render() {
    if (!this.pcbData) {
      return x`<p>loading...</p>`;
    }
    return x`
            ${this.renderer.front.canvas}
            <winter-pcb-assembly-bom-table
                .renderer=${this.renderer}
                .items=${this.bom.frontItemsGroupedByValue}></winter-pcb-assembly-bom-table>
            ${this.renderer.back.canvas}
            <winter-pcb-assembly-bom-table
                .renderer=${this.renderer}
                .items=${this.bom.backItemsGroupedByValue}></winter-pcb-assembly-bom-table>
        `;
  }
};
__name(WinterPCBAssemblyElement, "WinterPCBAssemblyElement");
__decorateClass([
  n5()
], WinterPCBAssemblyElement.prototype, "src", 2);
__decorateClass([
  n5({ type: Boolean })
], WinterPCBAssemblyElement.prototype, "rotate", 2);
WinterPCBAssemblyElement = __decorateClass([
  t3("winter-pcb-assembly")
], WinterPCBAssemblyElement);
var WinterPCBAssemblyBOMTableElement = class extends s3 {
  createRenderRoot() {
    return this;
  }
  connectedCallback() {
    super.connectedCallback();
    this.addEventListener("click", (e7) => {
      const target = e7.target;
      const row = target.closest("tr");
      if (!row) {
        return;
      }
      const itemValue = row.dataset["bomItemValue"];
      const items = this.items.get(itemValue);
      if (!items) {
        return;
      }
      const refs = items.map((x2) => x2.ref);
      this.renderer.highlight(refs);
      for (const tr of this.querySelectorAll("tr[aria-selected]")) {
        tr.ariaSelected = null;
      }
      row.ariaSelected = "true";
    });
  }
  render() {
    const rows = [];
    for (const [value, items] of this.items.entries()) {
      const refs = items.map((x2) => x2.ref).join(", ");
      rows.push(
        x`<tr data-bom-item-value=${value}>
                    <td>
                        ${x`<input
                            type="checkbox"
                            class="checkbox"
                            aria-label="Mark row ${rows.length + 1} complete" />`}
                    </td>
                    <td>${refs}</td>
                    <td>${value}</td>
                    <td>${items[0]?.rating}</td>
                </tr>`
      );
    }
    return x`<table
            class="bom-table is-high-density"
            aria-label="Table of components">
            <thead>
                <tr>
                    <th aria-label="Completed"></th>
                    <th>References</th>
                    <th>Value</th>
                    <th>Rating</th>
                </tr>
            </thead>
            <tbody>
                ${rows}
            </tbody>
        </table>`;
  }
};
__name(WinterPCBAssemblyBOMTableElement, "WinterPCBAssemblyBOMTableElement");
__decorateClass([
  n5({ attribute: false })
], WinterPCBAssemblyBOMTableElement.prototype, "renderer", 2);
__decorateClass([
  n5({ attribute: false })
], WinterPCBAssemblyBOMTableElement.prototype, "items", 2);
WinterPCBAssemblyBOMTableElement = __decorateClass([
  t3("winter-pcb-assembly-bom-table")
], WinterPCBAssemblyBOMTableElement);
var BOM = class {
  constructor(pcbData) {
    this.pcbData = pcbData;
    this.frontItems = [];
    this.backItems = [];
    this.frontItemsGroupedByValue = /* @__PURE__ */ new Map();
    this.backItemsGroupedByValue = /* @__PURE__ */ new Map();
    this.extract();
  }
  static {
    __name(this, "BOM");
  }
  extract() {
    const frontRefs = /* @__PURE__ */ new Set();
    for (const group of this.pcbData.bom.F) {
      for (const [ref, _2] of group) {
        frontRefs.add(ref);
      }
    }
    for (const group of this.pcbData.bom.both) {
      for (const [ref, index] of group) {
        const fields = this.pcbData.bom.fields[index];
        const dstList = frontRefs.has(ref) ? this.frontItems : this.backItems;
        const dstMap = frontRefs.has(ref) ? this.frontItemsGroupedByValue : this.backItemsGroupedByValue;
        const item = {
          ref,
          value: fields[0],
          footprint: fields[1],
          rating: fields[2],
          side: frontRefs.has(ref) ? "front" : "back"
        };
        dstList.push(item);
        if (!dstMap.has(item.value)) {
          dstMap.set(item.value, []);
        }
        dstMap.get(item.value)?.push(item);
      }
    }
  }
};
var PCBRenderer = class {
  constructor(parent, pcbData, rotate = false) {
    this.parent = parent;
    this.pcbData = pcbData;
    this.rotate = rotate;
    this.scale = 10;
    this.colors = {
      edge_cuts: "#ff99ce",
      board: "#ffb9dd",
      pad: "gold",
      hole: "white",
      pin1: "#6bd280",
      silk: "black",
      highlight_stroke: "#7ce4f4",
      highlight_fill: "#7ce4f488"
    };
    this.highlighted = /* @__PURE__ */ new Set();
    this.width = this.pcbData.edges_bbox.maxx - this.pcbData.edges_bbox.minx;
    this.height = this.pcbData.edges_bbox.maxy - this.pcbData.edges_bbox.miny;
    if (this.rotate) {
      const height = this.height;
      this.height = this.width;
      this.width = height;
    }
    this.back = new PCBDraw(this.makeCanvas("back"), this.colors);
    this.front = new PCBDraw(this.makeCanvas("front"), this.colors);
    this.draw();
  }
  static {
    __name(this, "PCBRenderer");
  }
  makeCanvas(className, scale = 10) {
    const canvas = document.createElement("canvas");
    canvas.classList.add(className);
    canvas.width = this.width * window.devicePixelRatio * scale;
    canvas.height = this.height * window.devicePixelRatio * scale;
    canvas.dataset["scale"] = `${scale}`;
    return canvas;
  }
  highlight(refs) {
    this.highlighted.clear();
    for (const ref of refs) {
      this.highlighted.add(ref);
    }
    this.draw();
  }
  /* Drawing */
  draw() {
    this.setTransforms(this.front.canvas);
    this.front.clear();
    this.front.items(this.pcbData.drawings.silkscreen.F, this.colors.silk);
    this.front.footprints(this.pcbData.footprints, "F", this.highlighted);
    this.front.items(this.pcbData.edges, this.colors.edge_cuts);
    this.setTransforms(this.back.canvas);
    this.back.clear();
    this.back.items(this.pcbData.drawings.silkscreen.B, this.colors.silk);
    this.back.footprints(this.pcbData.footprints, "B", this.highlighted);
    this.back.items(this.pcbData.edges, this.colors.edge_cuts);
  }
  setTransforms(canvas) {
    var ctx = canvas.getContext("2d");
    ctx.setTransform(1, 0, 0, 1, 0, 0);
    ctx.scale(
      this.scale * window.devicePixelRatio,
      this.scale * window.devicePixelRatio
    );
    if (this.rotate) {
      const x2 = this.width / 2;
      const y3 = this.height / 2;
      ctx.translate(x2, y3);
      ctx.rotate(deg2rad(90));
      ctx.translate(-y3, -x2);
    }
    ctx.translate(
      -this.pcbData.edges_bbox.minx,
      -this.pcbData.edges_bbox.miny
    );
  }
};
var PCBDraw = class {
  constructor(canvas, colors) {
    this.canvas = canvas;
    this.colors = colors;
    this.ctx = canvas.getContext("2d");
  }
  static {
    __name(this, "PCBDraw");
  }
  /* Public methods */
  clear() {
    this.ctx.save();
    this.ctx.setTransform(1, 0, 0, 1, 0, 0);
    this.ctx.fillStyle = this.colors.board;
    this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
    this.ctx.restore();
  }
  footprints(footprints, layer, highlighted) {
    const ctx = this.ctx;
    ctx.lineWidth = 1 / 4;
    for (let i4 = 0; i4 < footprints.length; i4++) {
      const mod = footprints[i4];
      this.footprint(layer, mod, highlighted.has(mod.ref));
    }
  }
  items(items, color) {
    for (const item of items) {
      this.item(item, color);
    }
  }
  /* Drawing commands. */
  item(item, color) {
    if (item.text) {
      console.warn("pcb-assembly doesn't support text items", item);
    } else if (item.type == "polygon") {
      this.polygon(item, color);
    } else if (item.type !== void 0) {
      this.edge(item, color);
    } else if (Object.hasOwn(item, "svgpath")) {
      this.svgpath(item, color);
    } else if (Object.hasOwn(item, "polygons")) {
      this.polygons(item, color);
    } else {
      console.error("Unknown drawing item", item);
    }
  }
  footprint(layer, footprint, highlight) {
    const ctx = this.ctx;
    const highlightPinOne = footprint.ref.startsWith("D") || footprint.ref.startsWith("U") || footprint.ref.startsWith("CP");
    for (var item of footprint.drawings) {
      if (item.layer == layer) {
        this.item(item, this.colors.pad);
      }
    }
    for (var pad of footprint.pads) {
      if (pad.layers.includes(layer)) {
        this.pad(pad, this.colors.pad, false);
        if (pad.pin1 && highlightPinOne) {
          this.pad(pad, this.colors.pin1, true);
        }
      }
    }
    for (var pad of footprint.pads) {
      this.padHole(pad, this.colors.hole);
    }
    if (highlight) {
      if (footprint.layer == layer) {
        ctx.save();
        ctx.translate(...footprint.bbox.pos);
        ctx.rotate(deg2rad(-footprint.bbox.angle));
        ctx.translate(...footprint.bbox.relpos);
        ctx.fillStyle = this.colors.highlight_fill;
        ctx.fillRect(0, 0, ...footprint.bbox.size);
        ctx.strokeStyle = this.colors.highlight_stroke;
        ctx.strokeRect(0, 0, ...footprint.bbox.size);
        ctx.restore();
      }
    }
  }
  edge(edge, color) {
    const ctx = this.ctx;
    ctx.strokeStyle = color;
    ctx.fillStyle = color;
    ctx.lineWidth = edge.width;
    ctx.lineCap = "round";
    ctx.lineJoin = "round";
    ctx.beginPath();
    if (edge.type == "segment") {
      ctx.moveTo(...edge.start);
      ctx.lineTo(...edge.end);
    }
    if (edge.type == "rect") {
      ctx.moveTo(...edge.start);
      ctx.lineTo(edge.start[0], edge.end[1]);
      ctx.lineTo(...edge.end);
      ctx.lineTo(edge.end[0], edge.start[1]);
      ctx.lineTo(...edge.start);
    }
    if (edge.type == "arc") {
      ctx.arc(
        ...edge.start,
        edge.radius,
        deg2rad(edge.startangle),
        deg2rad(edge.endangle)
      );
    }
    if (edge.type == "circle") {
      ctx.arc(...edge.start, edge.radius, 0, 2 * Math.PI);
      ctx.closePath();
    }
    if (edge.type == "curve") {
      ctx.moveTo(...edge.start);
      ctx.bezierCurveTo(...edge.cpa, ...edge.cpb, ...edge.end);
    }
    if ("filled" in edge && edge.filled) {
      ctx.fill();
    } else {
      ctx.stroke();
    }
  }
  polygon(shape, color) {
    const ctx = this.ctx;
    ctx.save();
    ctx.translate(...shape.pos);
    ctx.rotate(deg2rad(-shape.angle));
    if ("filled" in shape && !shape.filled) {
      ctx.strokeStyle = color;
      ctx.lineWidth = shape.width;
      ctx.lineCap = "round";
      ctx.lineJoin = "round";
      ctx.stroke(this.makePolygonPath(shape));
    } else {
      ctx.fillStyle = color;
      ctx.fill(this.makePolygonPath(shape));
    }
    ctx.restore();
  }
  polygons(item, color) {
    this.ctx.save();
    this.ctx.strokeStyle = color;
    this.ctx.fillStyle = color;
    this.ctx.lineCap = "round";
    this.ctx.lineJoin = "round";
    this.ctx.lineWidth = item.thickness;
    var path = new Path2D();
    for (var polygon of item.polygons) {
      path.moveTo(...polygon[0]);
      for (var i4 = 1; i4 < polygon.length; i4++) {
        path.lineTo(...polygon[i4]);
      }
      path.closePath();
    }
    item.filled !== false ? this.ctx.fill(path) : this.ctx.stroke(path);
    this.ctx.restore();
  }
  svgpath(item, color) {
    this.ctx.save();
    this.ctx.strokeStyle = color;
    this.ctx.lineCap = "round";
    this.ctx.lineJoin = "round";
    this.ctx.lineWidth = item.thickness;
    this.ctx.stroke(new Path2D(item.svgpath));
    this.ctx.restore();
  }
  pad(pad, color, outline) {
    const ctx = this.ctx;
    ctx.save();
    ctx.translate(...pad.pos);
    ctx.rotate(deg2rad(pad.angle));
    if (pad.offset) {
      ctx.translate(...pad.offset);
    }
    ctx.fillStyle = color;
    ctx.strokeStyle = color;
    var path = this.makePadPath(pad);
    if (outline) {
      ctx.stroke(path);
    } else {
      ctx.fill(path);
    }
    ctx.restore();
  }
  padHole(pad, color) {
    const ctx = this.ctx;
    if (pad.type != "th")
      return;
    ctx.save();
    ctx.translate(...pad.pos);
    ctx.rotate(deg2rad(pad.angle));
    ctx.fillStyle = color;
    if (pad.drillshape == "oblong") {
      ctx.fill(this.makeOblongPath(pad.drillsize));
    } else {
      ctx.fill(this.makeCirclePath(pad.drillsize[0] / 2));
    }
    ctx.restore();
  }
  /* Path generation commands */
  makeChamferedRectPath(size, radius, chamfpos, chamfratio) {
    var path = new Path2D();
    var width = size[0];
    var height = size[1];
    var x2 = width * -0.5;
    var y3 = height * -0.5;
    var chamfOffset = Math.min(width, height) * chamfratio;
    path.moveTo(x2, 0);
    if (chamfpos & 4) {
      path.lineTo(x2, y3 + height - chamfOffset);
      path.lineTo(x2 + chamfOffset, y3 + height);
      path.lineTo(0, y3 + height);
    } else {
      path.arcTo(x2, y3 + height, x2 + width, y3 + height, radius);
    }
    if (chamfpos & 8) {
      path.lineTo(x2 + width - chamfOffset, y3 + height);
      path.lineTo(x2 + width, y3 + height - chamfOffset);
      path.lineTo(x2 + width, 0);
    } else {
      path.arcTo(x2 + width, y3 + height, x2 + width, y3, radius);
    }
    if (chamfpos & 2) {
      path.lineTo(x2 + width, y3 + chamfOffset);
      path.lineTo(x2 + width - chamfOffset, y3);
      path.lineTo(0, y3);
    } else {
      path.arcTo(x2 + width, y3, x2, y3, radius);
    }
    if (chamfpos & 1) {
      path.lineTo(x2 + chamfOffset, y3);
      path.lineTo(x2, y3 + chamfOffset);
      path.lineTo(x2, 0);
    } else {
      path.arcTo(x2, y3, x2, y3 + height, radius);
    }
    path.closePath();
    return path;
  }
  makeOblongPath(size) {
    return this.makeChamferedRectPath(
      size,
      Math.min(size[0], size[1]) / 2,
      0,
      0
    );
  }
  makePolygonPath(shape) {
    if (shape.path2d) {
      return shape.path2d;
    }
    const path = new Path2D();
    for (const polygon of shape.polygons) {
      path.moveTo(...polygon[0]);
      for (let i4 = 1; i4 < polygon.length; i4++) {
        path.lineTo(...polygon[i4]);
      }
      path.closePath();
    }
    shape.path2d = path;
    return shape.path2d;
  }
  makeCirclePath(radius) {
    var path = new Path2D();
    path.arc(0, 0, radius, 0, 2 * Math.PI);
    path.closePath();
    return path;
  }
  makePadPath(pad) {
    if (pad.path2d) {
      return pad.path2d;
    }
    if (pad.shape == "rect") {
      const start = pad.size.map((c4) => -c4 * 0.5);
      pad.path2d = new Path2D();
      pad.path2d.rect(...start, ...pad.size);
    } else if (pad.shape == "oval") {
      pad.path2d = this.makeOblongPath(pad.size);
    } else if (pad.shape == "circle") {
      pad.path2d = this.makeCirclePath(pad.size[0] / 2);
    } else if (pad.shape == "roundrect") {
      pad.path2d = this.makeChamferedRectPath(pad.size, pad.radius, 0, 0);
    } else if (pad.shape == "chamfrect") {
      pad.path2d = this.makeChamferedRectPath(
        pad.size,
        pad.radius,
        pad.chamfpos,
        pad.chamfratio
      );
    } else if (pad.shape == "custom") {
      pad.path2d = this.makePolygonPath(pad);
    }
    return pad.path2d;
  }
};
function deg2rad(deg) {
  return deg * Math.PI / 180;
}
__name(deg2rad, "deg2rad");

// src/scripts/index.ts
var VERSION = "head";
export {
  dom_exports as DOM,
  MIDI,
  Teeth,
  VERSION,
  WinterAudioPlayerElement,
  WinterCarouselElement,
  WinterDarkModeElement,
  WinterIconElement,
  WinterImageMapElement,
  WinterPCBAssemblyElement
};
/*! Bundled license information:

@lit/reactive-element/css-tag.js:
  (**
   * @license
   * Copyright 2019 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/reactive-element.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

lit-html/lit-html.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

lit-element/lit-element.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

lit-html/is-server.js:
  (**
   * @license
   * Copyright 2022 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

lit-html/static.js:
  (**
   * @license
   * Copyright 2020 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/custom-element.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/property.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/state.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/event-options.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/base.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/query.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/query-all.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/query-async.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/query-assigned-elements.js:
  (**
   * @license
   * Copyright 2021 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)

@lit/reactive-element/decorators/query-assigned-nodes.js:
  (**
   * @license
   * Copyright 2017 Google LLC
   * SPDX-License-Identifier: BSD-3-Clause
   *)
*/
//# sourceMappingURL=winter.js.map
