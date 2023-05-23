class Context2d {
    transform() {}
    setTransform() {}
    save() {}
    restore() {}
    fillRect() {}
    strokeRect() {}
    beginPath() {}
    arc() {}
    stroke() {}
    closePath() {}
    lineWidth() {}
    moveTo() {}
    lineTo() {}
    fill() {}
    fill() {
      console.log('fill');
    }
    drawImage() {}
    // drawImage() {console.log(arguments);}
    // getImageData() {}
    getImageData() {
      console.log('getImageData');
    }
    createImageData() {}
    createImageData() {
      console.log('createImageData');
    }
    putImageData() {}
    putImageData() {
      console.log('putImageData');
    }
    clip() {}
    rect() {}
  
    getParameter() {}
    createProgram() {}
    getProgramParameter() {}
    createShader() {  }
    shaderSource() {}
    shaderSource() {console.log(arguments)}
    compileShader() {console.log(arguments)}
    getShaderParameter() {}
    getShaderInfoLog() {}
    linkProgram() {}
    getError() {}
  }
  
  class XMLHttpRequest {
    open(method, url) {
      console.log(url);
    }
  }
  
  class Elm {
    style = {};
    value = '';
  
    setAttribute(attr, val) {
      this[attr] = val;
      // console.log(`${attr} ${val}`);
      if (attr == 'id') {
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

  let imageId = 0xff00;
  class Image extends Elm {
    _src = '';
    get src() {
      return this._src;
    }
    set src(url) {
      this.imageId = imageId++;
      this._src = url;
      setTimeout(() => {
        let onload = this.onload.bind(this);
        onload();
      }, 0);
    }
  }
  
  class Canvas extends Elm {
    getContext(type) {
      if (type == '2d') {
        return new Context2d();
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
    location: 'local',
    setTimeout: setTimeout,
    requestAnimFrame: () => {},
    XMLHttpRequest: () => {},
  };
  
  navigator = {
    userAgent: 'Chrome',
    platform: 'Linux',
    appVersion: '?',
  };
  
  document = {
    body: new Elm('body'),
  
    getElementById: (id) => {
      // console.log(`getElementById ${id}`);
      return elements[id];
    },
    createElement: (e) => {
      // console.log(`createElement ${e}`);
      switch (e) {
        case 'canvas': {
          c = new Canvas();
          elements['canvas'] = c;
          return c;
        }
      }
      return new Elm();
    },
  };
  
  alert = console.log;

module.exports  = {
    XMLHttpRequest,
    Audio,
    Image,
    Canvas,
    Context2d,
    navigator,
    window,
    document
  }