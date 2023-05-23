const { debug, patch } = require('./spelunky.js');

// qzi.prototype.tah
// patch.drawObject = (obj,sprite,ax,ay,rot,x,y,f,g,h) => {
//     let name = obj.rb;
//     if (sprite && name.includes('sRun')) {
//         console.log(name);
//         console.log(sprite);
//     }
//     console.log({name,ax,ay,rot,x,y,f,g,h});
// }

// vml
patch.drawSprite = (sprite, ax, ay, x, y, vol, wol, rot, aci, bci, lfi, mfi, gei) => {
    console.log(sprite);
    // console.log({ax, ay, x, y, vol, wol, rot});
}

// debug.enable();
window.onload();
