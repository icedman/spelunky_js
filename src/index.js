const { window, document, XMLHttpRequest, Image, Audio, } = require("./monkey_patch.js");
const { debug, patch } = require("./spelunky.js");

const _images = {};
ctx = {};
hyh = 0; // debug
requestAnimFrame = () => {
  //window.requestAnimFrame;
};

patch = {
  drawSprite: () => {},
  drawObject: () => {},
  disableRequestFrame: () => {
    requestAnimFrame = () => {};
  },
  loadImage: () => {},
};

Image.prototype.loadImage = (img) => {
  // console.log(img);
  patch.loadImage(img.id, img._src);
};

debug = {
  enable: () => (hyh = 1),
};

// qzi.prototype.tah
patch.drawObject = (obj, sprite, ax, ay, rot, x, y, f, g, h, i) => {
  let name = obj.rb;
  if (sprite && name.includes("sSnake")) {
    Object.keys(obj).forEach((k) => {
      console.log(obj[k]);
    });
    // console.log(name, ax, ay, rot, x, y, f, g, h, i);
  }
};

patch.drawText = (font, text, x, y, size, align) => {
  // console.log(text);
};

// vml
patch.drawSprite = (
  sprite,
  ax,
  ay,
  x,
  y,
  vol,
  wol,
  rot,
  aci,
  bci,
  lfi,
  mfi,
  gei
) => {
  // console.log(`${sprite['sxb']} ${sprite['txb']}`);
  console.log(`${ax}, ${ay}, ${x}, ${y}, ${vol}, ${wol}, ${rot}`);
};

if (globalThis.os || isNode) {
  patch.sprites = [];
  patch.spriteCount = 0;

  patch.disableRequestFrame();

  if (isNode) {
    patch.loadImage = () => {};
  } else {
    patch.loadImage = app.loadImage;
  }
  patch.drawObject = () => {};

  patch.drawText = (font, text, x, y, size, align) => {
    patch.texts.push([font, text, x + window.dx, y + window.dy]);
    return true;
  };

  patch.drawSprite = (
    sprite,
    ax,
    ay,
    x,
    y,
    flip,
    wol,
    rot,
    aci,
    bci,
    lfi,
    mfi,
    gei
  ) => {
    var sx = sprite["exb"];
    var sy = sprite["fxb"];
    var sw = sprite["sxb"];
    var sh = sprite["txb"];
    var ox = sprite["yxb"];
    var oy = sprite["zxb"];
    app.drawImage(
      ax,
      ay,
      x + window.dx + ox - ax,
      y + window.dy + oy - ay,
      flip,
      sx,
      sy,
      sw,
      sh
    );
    // return;
    // patch.sprites.push([
    //   ax,
    //   ay,
    //   x + window.dx + ox - ax,
    //   y + window.dy + oy - ay,
    //   flip,
    //   sx,
    //   sy,
    //   sw,
    //   sh,
    // ]);
    return true;
  };

  if (isNode) {
    patch.drawSprite = (
      sprite,
      ax,
      ay,
      x,
      y,
      flip,
      wol,
      rot,
      aci,
      bci,
      lfi,
      mfi,
      gei
    ) => {
      var sx = sprite["exb"];
      var sy = sprite["fxb"];
      var sw = sprite["sxb"];
      var sh = sprite["txb"];
      var ox = sprite["yxb"];
      var oy = sprite["zxb"];

      ctx.save();
      ctx.translate(ox - ax + window.dx, oy - ay + window.dy);

      if (false) {
        ctx.strokeStyle = "rgba(255,255,0,0.5)";
        ctx.beginPath();
        ctx.moveTo(x, y);
        // console.log([window.dx, window.dy])
        ctx.lineTo(x + sw, y);
        ctx.lineTo(x + sw, y + sh);
        ctx.lineTo(x, y + sh);
        ctx.lineTo(x, y);
        ctx.closePath();
        ctx.stroke();
      }

      var img = _images[0];
      if (img && sw <= 16) {
        // console.log(img);
        ctx.save();
        // ctx.scale(-1, 1);
        ctx.drawImage(img, sx, sy, sw, sh, x, y, sw, sh);
        ctx.restore();
      }

      ctx.restore();
    };
  }
}

window.skipUpdate = 0;
window.shouldSkip = () => {
  window.skipUpdate++;
  if (window.skipUpdate > 4) {
    window.skipUpdate = 0;
  }
  return window.skipUpdate != 0;
};

objCount = 0;
eicCount = 0;
window.frame = 0;
window.update = () => {
  // window.time = new Date().getTime();
  window.time += 1000;
  window.frame++;

  patch.sprites = [];
  patch.texts = [];

  eicCount = 0;
  zin();
  // console.log(eicCount);

  patch.spriteCount = patch.sprites.length;
  patch.textCount = patch.texts.length;
  if (globalThis.app) {
    app.sprites = patch.sprites;
    app.spriteCount = patch.spriteCount;
    app.texts = patch.texts;
    app.textCount = patch.textCount;
    // app.objects = yti.kvl.deh.length;
  }
};

// create event
if (false) {
  ob.bi.forEach((b) => {
    // create
    if (typeof b.gi == "function") {
      b._gi = b.gi;
      b.gi = function (s1, s2) {
        objCount++;
        // console.log(`create ${b.rb} ${objCount}`);
        return b._gi(s1, s2);
      };
    } else {
      b.gi = function (s1, s2) {
        objCount++;
        // console.log(`__create ${b.rb} ${objCount}`);
      };
    }
    // destroy
    if (typeof b.ki == "function") {
      b._ki = b.ki;
      b.ki = function (s1, s2) {
        objCount--;
        console.log(`destroy ${b.rb} ${objCount}`);
        return b._ki(s1, s2);
      };
    } else {
      b.ki = function (s1, s2) {
        objCount--;
        console.log(`__destroy ${b.rb} ${objCount}`);
      };
    }
  });
}
// skip frame updates
if (false) {
  ob.bi.forEach((b) => {
    if (
      [
        "oGame",
        "oGamepad",
        "oScreen",
        "oCharacter",
        "oPlayer1",
        "oWhip",
        "oWhipPre",
        "oIntro",
        "oTitle",
      ].includes(b.rb)
    ) {
      return;
    }
    // if (["oBlood", "oArrow", "oFlareSpark"].includes(b.rb)) {
    //   return;
    // }
    if ([!"oTreasure", "oJar", "oItem"].includes(b.rb)) {
      // console.log(`add frame skip ${b.rb}`);
      return;
    }

    // Step
    if (typeof b.mi == "function") {
      b._mi = b.mi.bind(b);
      b.mi = function (a1, a2) {
        // console.log(b.rb);
        // if (window.skipUpdate) return;
        if (window.shouldSkip()) {
          return;
        }
        this._mi(a1, a2);
      }.bind(b);
    }
  });
}

function JSON_stringify(obj) {
  let res = [];
  Object.keys(obj).forEach((k) => {
    let v = obj[k];
    if (typeof v != "object") {
      // res[k] = v;
      res.push(v);
    } else {
      // res[k] = 'object';
      res.push("object" + JSON.stringify(Object.keys(v)));
    }
  });
  return JSON.stringify(res);
}

const _eic = eic;
eic = function (wwh, lsh, msh, zuh, xwh, ywh) {
  // eicCount++;
  // console.log(JSON_stringify({wwh, lsh, msh, zuh, xwh, ywh}));
  return _eic(wwh, lsh, msh, zuh, xwh, ywh);
};

const keys = {
  up: [38, "ArrowUp"],
  down: [40, "ArrowDown"],
  left: [37, "ArrowLeft"],
  right: [39, "ArrowRight"],
  a: [65, "a"],
  s: [83, "s"],
  z: [90, "z"],
  x: [88, "x"],
  escape: [27, "Escape"],
  space: [32, "Space"],
};

if (isNode) {
  patch.loadImage = (id, src) => {
    loadImage(src).then((image) => {
      // ctx.drawImage(image, 50, 0, 70, 70)
      _images[id] = image;
    });
    console.log({ id, src });
  };
  width = 800;
  height = 600;
  run_id = 0;
  const sdl = required("@kmamal/sdl");
  const { createCanvas, loadImage } = required("canvas");
  const canvas = createCanvas(width, height);
  ctx = canvas.getContext("2d");
  const sdl_window = sdl.video.createWindow({
    title: "Hello, World!",
    width,
    height,
  });
  sdl_window.on("keyDown", (evt) => {
    if (evt.key == "escape") {
      clearInterval(run_id);
      sdl_window.destroy();
    }
    const key = keys[evt.key];
    if (!key) {
      console.log(evt);
      return;
    }
    window.event = new KeyEvent(key[1], key[0]);
    window.onkeydown(window.event);
  });
  sdl_window.on("keyUp", (evt) => {
    const key = keys[evt.key];
    if (!key) {
      console.log(evt);
      return;
    }
    window.event = new KeyEvent(key[1], key[0]);
    window.onkeyup(window.event);
  });

  _update = () => {
    _sz = 2.5;
    ctx.save();
    ctx.scale(_sz, _sz);
    // ctx.translate(window.dx, window.dy);

    ctx.fillStyle = "rgba(0,0,0,0.5)";
    // ctx.fillRect(0, 0, width, height);
    ctx.clearRect(0, 0, width, height);
    window.update();
    ctx.restore();
    // render
    // console.log(patch.sprites.length);
    // console.log(patch.texts.length);
    // patch.sprites.forEach((spr) => {
    //   console.log(JSON.stringify(spr));
    // });
    const buffer = canvas.toBuffer("raw");
    sdl_window.render(width, height, width * 4, "bgra32", buffer);
  };
  run_id = setInterval(_update, 0);
}

window.onload();
