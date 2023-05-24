const { debug, patch } = require('./spelunky.js');

// qzi.prototype.tah
patch.drawObject = (obj,sprite,ax,ay,rot,x,y,f,g,h) => {
    let name = obj.rb;
    if (sprite && name.includes('sRun')) {
        // console.log(name);
        // console.log(sprite);
        console.log({name,ax,ay,rot,x,y,f,g,h});
    }
}

// vml
patch.drawSprite = (sprite, ax, ay, x, y, vol, wol, rot, aci, bci, lfi, mfi, gei) => {
    // console.log(sprite);
    // console.log(`${sprite['sxb']} ${sprite['txb']}`);
    // console.log(`${ax}, ${ay}, ${x}, ${y}, ${vol}, ${wol}, ${rot}`);
}

debug.enable();

if (globalThis.os) {
    patch.disableRequestFrame();

    patch.loadImage = (img) => {
        app.loadImage(img.id, img.src);
    };

    patch.drawObject = () => {};

    patch.drawSprite = (sprite, ax, ay, x, y, flip, wol, rot, aci, bci, lfi, mfi, gei) => {
        var sx = sprite['exb'];
        var sy = sprite['fxb'];
        var sw = sprite['sxb'];
        var sh = sprite['txb'];
        app.drawImage(ax, ay, x, y, flip, sx, sy, sw, sh);
    };
}

window.onload();
