globalThis.process = { env: { NODE_ENV: "development" } };
if (!globalThis.console) {
  globalThis.console = app ? app : { log: () => {} };
}
if (!globalThis.setTimeout) {
  globalThis.setTimeout = globalThis._os ? globalThis._os.setTimeout : () => {};
}

class Context2d {
  transform() {}
  setTransform(a, b, c, d, e, f) {
    window.dx = e / a;
    window.dy = f / a;
  }
  save() {}
  restore() {}
  fillRect() {}
  strokeRect() {}
  beginPath() {}
  arc() {}
  stroke() {}
  closePath() {}
  lineWidth() {}
  moveTo(x, y) {
    // console.log(`moveTo: ${x} ${y}`)
  }
  lineTo() {}
  fill() {}
  fill() {
    console.log("fill");
  }
  drawImage() {}
  // drawImage() {console.log(arguments);}
  // getImageData() {}
  getImageData() {
    console.log("getImageData");
  }
  createImageData() {}
  createImageData() {
    console.log("createImageData");
  }
  putImageData() {}
  putImageData() {
    console.log("putImageData");
  }
  clip() {}
  rect() {}

  getParameter() {}
  createProgram() {}
  getProgramParameter() {}
  createShader() {}
  shaderSource() {}
  shaderSource() {
    console.log(arguments);
  }
  compileShader() {
    console.log(arguments);
  }
  getShaderParameter() {}
  getShaderInfoLog() {}
  linkProgram() {}
  getError() {}
}

class XMLHttpRequest {
  open(method, url) {
    console.log(`open: ${url}`);
  }
}

class Elm {
  style = {};
  value = "";

  setAttribute(attr, val) {
    this[attr] = val;
    // console.log(`${attr} ${val}`);
    if (attr == "id") {
      elements[val] = this;
    }
  }

  insertBefore() {
    // console.log('insert');
    // console.log(arguments);
  }

  appendChild() {
    // console.log('append');
    // console.log(arguments);
  }

  removeChild() {
    // console.log('remove');
    // console.log(arguments);
  }

  addEventListener(e, l) {
    // console.log(`add event listener ${e}`);
  }
}

class Audio extends Elm {
  load() {
    // console.log(`audio load`);
  }
}

let imageId = 0;
class HTMLImageElement extends Elm {
  _src = "";
  get src() {
    return this._src;
  }
  set src(url) {
    this.id = imageId++;
    this._src = url;
    this.loadImage(this);
    setTimeout(() => {
      let onload = this.onload.bind(this);
      onload();
    }, 0);
  }
  loadImage(img) {
    // ...
  }
}

class Image extends HTMLImageElement {}

class Canvas extends Elm {
  getContext(type) {
    if (type == "2d") {
      var ctx = new Context2d();
      ctx.canvas = this;
      return ctx;
    }
    return null;
  }
  width = 320;
  height = 240;
  parentNode = new Elm();
}

elements = {
  canvas: new Canvas(),
  GM4HTML5_loadingscreen: new Elm(),
  loadingscreen: new Elm(),
};

window = {
  performance: {},
  location: "local",
  addEventListener: (evt) => {
    console.log(evt);
  },
  update: () => {},
  onkeyup: () => {},
  onkeydown: () => {},
  onload: () => {},
  setTimeout: setTimeout,
  requestAnimFrame: () => {},
  XMLHttpRequest: () => {},
  getTime: () => {
    return window.time;
    // return new Date().getTime()
  },
};
window.time = 0;

navigator = {
  userAgent: "Chrome",
  platform: "Linux",
  appVersion: "?",
};

document = {
  body: new Elm("body"),

  addEventListener: (evt) => {
    console.log(evt);
  },

  getElementById: (id) => {
    // console.log(`getElementById ${id}`);
    return elements[id];
  },
  createElement: (e) => {
    // console.log(`createElement ${e}`);
    switch (e) {
      case "canvas": {
        c = new Canvas();
        elements["canvas"] = c;
        return c;
      }
    }
    return new Elm();
  },
};
window.document = document;

alert = console.log;

class KeyEvent {
  constructor(k, w) {
    this.key = k;
    this.code = k;
    this.keyCode = w;
    this.which = w;
    window.event = this;
  }
  preventDefault() {}
}

debug = (c) => {
  console.log(c);
};

// function isElectron() {
//   return false;
// }

module.exports = {
  XMLHttpRequest,
  Audio,
  Image,
  Canvas,
  Context2d,
  navigator,
  window,
  document,
};
