const { debug, patch } = require('./spelunky.js');

// qzi.prototype.tah
patch.drawObject = (obj, sprite, ax, ay, rot, x, y, f, g, h, i) => {
  let name = obj.rb;
  if (sprite && name.includes('sSnake')) {
    Object.keys(obj).forEach((k) => {
      console.log(obj[k])
    });
    // console.log(name, ax, ay, rot, x, y, f, g, h, i);
  }
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
  // console.log(`${ax}, ${ay}, ${x}, ${y}, ${vol}, ${wol}, ${rot}`);
};

// debug.enable();

if (globalThis.os) {
  patch.sprites = [];
  patch.spriteCount = 0;

  patch.disableRequestFrame();

  if (globalThis.app) {
    patch.loadImage = app.loadImage;
  }

  patch.drawObject = () => {};

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
    var sx = sprite['exb'];
    var sy = sprite['fxb'];
    var sw = sprite['sxb'];
    var sh = sprite['txb'];
    var ox = sprite['yxb'];
    var oy = sprite['zxb'];
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

window.update = () => {
  // window.time = new Date().getTime();
  window.time += 1000;
  patch.sprites = [];
  zin();
  patch.spriteCount = patch.sprites.length;
  if (globalThis.app) {
    app.sprites = patch.sprites;
    app.spriteCount = patch.spriteCount;
    app.objects = yti.kvl.deh.length;
  }
};

window.onload();
