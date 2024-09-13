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
  patch.loadImage(img.id, img.src);
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

if (globalThis.os) {
  patch.sprites = [];
  patch.spriteCount = 0;

  if (globalThis.app) {
    patch.disableRequestFrame();
    patch.loadImage = app.loadImage;
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
      // app.drawImage(ax, ay, x, y, flip, sx, sy, sw, sh);
      patch.sprites.push([
        ax,
        ay,
        x + window.dx + ox - ax,
        y + window.dy + oy - ay,
        flip,
        sx,
        sy,
        sw,
        sh,
      ]);
      return true;
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

eicCount = 0;
window.frame = 0;
window.update = () => {
  // window.time = new Date().getTime();
  window.time += 1000;
  window.frame++;

  patch.sprites = [];
  patch.texts = [];

  eicCount = 0;
  _uK2();
  console.log(patch.sprites.length);
  // console.log(patch.drawSprite);

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

window._DL2 = () => {};
GameMaker_Init();
