#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <vector.h>

#include "context.h"
#include "data.h"
#include "entity.h"
#include "fastRandom.h"
#include "font.h"
#include "game.h"
#include "matrix.h"
#include "menu.h"
#include "polygon.h"
#include "renderer.h"
#include "tests.h"
#include "util.h"
#include "vector.h"

#include "quickjs-libc.h"
#include "quickjs.h"

// #define RUN_BINARY_SCRIPT

#ifdef RUN_BINARY_SCRIPT
#include "../script.c"
#endif

#define MAX_IMAGES 16
static char imagePaths[MAX_IMAGES][256];
static SDL_Texture *images[MAX_IMAGES];

#define SCALE 3
#define WIDTH (320 * SCALE)
#define HEIGHT (240 * SCALE)

JSValue global_obj, app;

#define TX_TIMER_BEGIN                                                         \
  clock_t _start, _end;                                                        \
  double _cpu_time_used;                                                       \
  _start = clock();

#define TX_TIMER_RESET _start = clock();

#define TX_TIMER_END                                                           \
  _end = clock();                                                              \
  _cpu_time_used = ((double)(_end - _start)) / CLOCKS_PER_SEC;

typedef struct sprite_t {
  SDL_Texture *texture;
  int x;
  int y;
  int ax;
  int ay;
  int sx;
  int sy;
  int sw;
  int sh;
  int flipped;
  float rot;
} sprite_t;

#define MAX_SPRITES 1024
static sprite_t sprites[MAX_SPRITES];
static int spriteIndex = 0;

typedef struct text_t {
  char text[128];
  int x;
  int y;
  int size;
} text_t;

#define MAX_TEXTS 32
static text_t texts[MAX_TEXTS];
static int textIndex = 0;

SDL_Texture *_loadImage(context_t *context, char *path) {
  SDL_Renderer *renderer = (SDL_Renderer *)context->renderer;
  SDL_Surface *temp = IMG_Load(path);
  if (!temp) {
    printf("unable to load %s\n", path);
    return NULL;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, temp);
  SDL_FreeSurface(temp);
  if (!texture) {
    printf("unable to create texture %s\n", path);
    return NULL;
  }
  printf("loaded %s\n", path);
  return texture;
}

void _freeImage(void *context, void *image) {}

void _drawImage(context_t *context, sprite_t *sprite, vector_t pos) {
  vector_t vt = VectorTransformed(&pos, context->matrixStack.matrix);
  SDL_Renderer *renderer = (SDL_Renderer *)context->renderer;
  float scale = SCALE;

  // if (sprite->sw == 0) {
  // printf("%d %d\n", sprite->sw,sprite->sh);
  // }

  SDL_Rect dest;
  dest.x = vt.x * scale;
  dest.y = vt.y * scale;
  dest.w = (sprite->sw + 1.5) * scale;
  dest.h = (sprite->sh + 1.5) * scale;

  // // SDL_RenderCopy(renderer, image, NULL, &dest);
  SDL_Point center;
  center.x = 0; // sprite->ax * scale;
  center.y = 0; // sprite->ay * scale;

#if 1
  if (sprite->sw > 16 * 8) {
    return;
  }

#else
  vector_t p1, p2, p3, p4;
  p1.x = dest.x;
  p1.y = dest.y;
  p2.x = dest.x + sprite->sw * scale;
  p2.y = dest.y + sprite->sh * scale;
  p3 = p1;
  p3.x = p2.x;
  p4 = p1;
  p4.y = p2.y;
  ContextDrawLine(context, p1, p3);
  ContextDrawLine(context, p3, p2);
  ContextDrawLine(context, p2, p4);
  ContextDrawLine(context, p4, p1);

  return;
#endif
  // dest.x -= sprite->ax * scale;
  // dest.y -= sprite->ay * scale;

  SDL_Rect src;
  src.x = sprite->sx;
  src.y = sprite->sy;
  src.w = sprite->sw;
  src.h = sprite->sh;

  bool flipped = sprite->flipped == -1;
  SDL_RenderCopyEx(renderer, sprite->texture, &src, &dest, 0, &center,
                   flipped ? SDL_FLIP_HORIZONTAL : 0);
}

static JSRuntime *rt = 0;
static JSContext *ctx = 0;

static JSValue js_log(JSContext *ctx, JSValueConst this_val, int argc,
                      JSValueConst *argv) {
  printf("log: ");
  int i;
  const char *str;
  for (i = 0; i < argc; i++) {
    str = JS_ToCString(ctx, argv[i]);
    if (!str)
      return JS_EXCEPTION;
    printf("%s\n", str);
    JS_FreeCString(ctx, str);
  }
  return JS_UNDEFINED;
}

static JSValue js_load_image(JSContext *ctx, JSValueConst this_val, int argc,
                             JSValueConst *argv) {
  int id = -1;
  JS_ToInt32(ctx, &id, argv[0]);
  const char *str = JS_ToCString(ctx, argv[1]);
  if (!str)
    return JS_EXCEPTION;

  printf("load %d %s\n", id, str);
  strcpy(imagePaths[id], str);

  JS_FreeCString(ctx, str);
  return JS_UNDEFINED;
}

static JSValue js_draw_image(JSContext *ctx, JSValueConst this_val, int argc,
                             JSValueConst *argv) {
  sprite_t *sprite = &sprites[spriteIndex++];
  JS_ToInt32(ctx, &sprite->ax, argv[0]);
  JS_ToInt32(ctx, &sprite->ay, argv[1]);
  JS_ToInt32(ctx, &sprite->x, argv[2]);
  JS_ToInt32(ctx, &sprite->y, argv[3]);
  JS_ToInt32(ctx, &sprite->flipped, argv[4]);
  JS_ToInt32(ctx, &sprite->sx, argv[5]);
  JS_ToInt32(ctx, &sprite->sy, argv[6]);
  JS_ToInt32(ctx, &sprite->sw, argv[7]);
  JS_ToInt32(ctx, &sprite->sh, argv[8]);
  return JS_UNDEFINED;
}

/* also used to initialize the worker context */
static JSContext *JS_NewCustomContext(JSRuntime *rt) {
  JSContext *ctx;
  ctx = JS_NewContext(rt);
  if (!ctx)
    return NULL;
  /* system modules */
  js_init_module_std(ctx, "std");
  js_init_module_os(ctx, "os");
  return ctx;
}

void ScriptingInit() {
  // init scripting
  rt = JS_NewRuntime();
  JS_SetMaxStackSize(rt, JS_DEFAULT_STACK_SIZE * 2);
  js_std_set_worker_new_context_func(JS_NewCustomContext);
  js_std_init_handlers(rt);
  ctx = JS_NewCustomContext(rt);
  if (!ctx) {
    fprintf(stderr, "qjs: cannot allocate JS context\n");
    exit(2);
  }

  /* loader for ES6 modules */
  JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);

  // ctx = JS_NewContextRaw(rt);
  // JS_AddIntrinsicBaseObjects(ctx);
  // JS_AddIntrinsicEval(ctx);

  global_obj = JS_GetGlobalObject(ctx);
  app = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, app, "log", JS_NewCFunction(ctx, js_log, "log", 1));
  JS_SetPropertyStr(ctx, app, "loadImage",
                    JS_NewCFunction(ctx, js_load_image, "loadImage", 1));
  JS_SetPropertyStr(ctx, app, "drawImage",
                    JS_NewCFunction(ctx, js_draw_image, "drawImage", 1));
  JS_SetPropertyStr(ctx, global_obj, "app", app);
  JS_FreeValue(ctx, global_obj);

  JSValue ret;

  char *imports = "import * as std from 'std'; \
      import * as os from 'os'; \
      globalThis._os = os; \
      globalThis._std = std;";

  ret = JS_Eval(ctx, imports, strlen(imports), "<input>", JS_EVAL_TYPE_MODULE);
  if (JS_IsException(ret)) {
    printf("JS err : %s\n", JS_ToCString(ctx, JS_GetException(ctx)));
    js_std_dump_error(ctx);
    JS_ResetUncatchableError(ctx);
  }

  {
    char *input = "app.log('hello world');";
    JSValue ret =
        JS_Eval(ctx, input, strlen(input), "<input>", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(ret)) {
      js_std_dump_error(ctx);
      JS_ResetUncatchableError(ctx);
    }
  }
}

const char *keyA = "a";
const char *keyS = "s";
const char *keyZ = "z";
const char *keyX = "x";
const char *keyLeft = "ArrowLeft";
const char *keyRight = "ArrowRight";
const char *keyUp = "ArrowUp";
const char *keyDown = "ArrowDown";

const char *keyNames[] = {
    "",  "ArrowUp", "ArrowDown", "ArrowLeft", "ArrowRight", "a",
    "s", "z",       "x",         "Escape",    "Space",
};

const int keyCodes[] = {
    0, 38, 40, 37, 39, 65, 83, 90, 88, 27, 32,
};

static bool keyDownFuncsReady[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static bool keyUpFuncsReady[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static JSValue keyDownFuncs[10];
static JSValue keyUpFuncs[10];
static bool updateFuncReady = 0;
static JSValue updateFunc;

JSValue CompileFunction(const char *source) {
  JSValue func_val =
      JS_Eval(ctx, source, strlen(source), "<input>", JS_EVAL_TYPE_GLOBAL);
  if (JS_IsException(func_val)) {
    JS_FreeValue(ctx, func_val);
    return JS_EXCEPTION;
  }

  JSValue bytecode = JS_GetGlobalObject(ctx);
  JSValue func = JS_GetPropertyStr(ctx, bytecode, "main");
  JS_FreeValue(ctx, bytecode);

  return func;
}

void ScriptingShutdown() {
  for (int i = 0; i < 10; i++) {
    if (keyDownFuncsReady[i]) {
      JS_FreeValue(ctx, keyDownFuncs[i]);
    }
    if (keyUpFuncsReady[i]) {
      JS_FreeValue(ctx, keyUpFuncs[i]);
    }
  }
  if (updateFuncReady) {
    JS_FreeValue(ctx, updateFunc);
  }
  js_std_free_handlers(rt);
  JS_FreeContext(ctx);
  // JS_FreeRuntime(rt);
}

void ScriptUpdate() {
  if (!updateFuncReady) {
    char *script = "function main() { window.update(); }";
    updateFunc = CompileFunction(script);
    printf("%s\n", script);
    updateFuncReady = 1;
  }

  JSValue result = JS_Call(ctx, updateFunc, JS_UNDEFINED, 0, NULL);
  if (JS_IsException(result)) {
    js_std_dump_error(ctx);
    JS_ResetUncatchableError(ctx);
  }
}

void ScriptSendKeyDown(int key) {
  if (!keyDownFuncsReady[key]) {
    char script[128];
    sprintf(script,
            "function main() { window.onkeydown(new KeyEvent('%s', %d)); }",
            keyNames[key], keyCodes[key]);
    keyDownFuncs[key] = CompileFunction(script);
    printf("%s\n", script);
    keyDownFuncsReady[key] = 1;
  }

  JSValue result = JS_Call(ctx, keyDownFuncs[key], JS_UNDEFINED, 0, NULL);
  if (JS_IsException(result)) {
    js_std_dump_error(ctx);
    JS_ResetUncatchableError(ctx);
  }
}

void ScriptSendKeyUp(int key) {
  if (!keyUpFuncsReady[key]) {
    char script[128];
    sprintf(script,
            "function main() { window.onkeyup(new KeyEvent('%s', %d)); }",
            keyNames[key], keyCodes[key]);
    keyUpFuncs[key] = CompileFunction(script);
    printf("%s\n", script);
    keyUpFuncsReady[key] = 1;
  }

  JSValue result = JS_Call(ctx, keyUpFuncs[key], JS_UNDEFINED, 0, NULL);
  if (JS_IsException(result)) {
    js_std_dump_error(ctx);
    JS_ResetUncatchableError(ctx);
  }
}

void ScriptRun(char *code) {
  JSValue ret =
      JS_Eval(ctx, code, strlen(code), "<input>", JS_EVAL_TYPE_GLOBAL);
  if (JS_IsException(ret)) {
    js_std_dump_error(ctx);
    JS_ResetUncatchableError(ctx);
  }
}

void ScriptRunFile(char *path) {
  FILE *fp = fopen(path, "r");
  if (fp) {
    fseek(fp, 0, SEEK_END);
    size_t sz = ftell(fp);
    // fprintf(stderr, "%d\n", sz);
    fseek(fp, 0, SEEK_SET);
    char *content = (char *)malloc(sz);
    fread(content, sz, 1, fp);
    // fprintf(stderr, "%s\n", content);
    fclose(fp);
    ScriptRun(content);
    free(content);
  }
}

SDL_Window *window;

void _drawLine(void *ctx, vector_t v1, vector_t v2) {
  context_t *context = (context_t *)ctx;
  SDL_Renderer *renderer = (SDL_Renderer *)context->renderer;
  float alpha = context->state->dark ? 0.5 : 1;
  SDL_SetRenderDrawColor(renderer, context->state->r * alpha,
                         context->state->g * alpha, context->state->b * alpha,
                         255);
  SDL_RenderDrawLine(renderer, v1.x, v1.y, v2.x, v2.y);
}

int main(int argc, char **argv) {
  TX_TIMER_BEGIN

  for (int i = 0; i < MAX_IMAGES; i++) {
    imagePaths[i][0] = 0;
    images[i] = NULL;
  }
  FastRandomSeed(0);

  ScriptingInit();

  int width = WIDTH;
  int height = HEIGHT;
  context_t context;

  FontInit();

  game_t game;
  GameInit(&game);

  sceneTest_t testScene;
  TestSceneInit(&testScene);
  sceneMenu_t menuScene;
  MenuSceneInit(&menuScene);
  game.menu = (void *)&menuScene;
  GameEnterMenu(&game);

  // game.scene = &testScene;

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_EnableScreenSaver();
  SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

  atexit(SDL_Quit);

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  // width = dm.w * 0.75;
  // height = dm.h * 0.75;

  ContextInit(&context, width, height);

  window = SDL_CreateWindow(
      "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);

  SDL_SetWindowTitle(window, "SDL");
  SDL_ShowWindow(window);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  context.renderer = renderer;
  context.drawLine = _drawLine;

#ifdef RUN_BINARY_SCRIPT
  js_std_eval_binary(ctx, qjsc_index, qjsc_index_size, 0);
#else
  ScriptRunFile("./dist/index.js");
#endif

  // js_std_loop(ctx);

  int spriteCount = 0;
  int textCount = 0;
  int objectCount = 0;

  int lastTicks = SDL_GetTicks();
  int lastJSTicks = lastTicks;
  while (!game.done) {
    SDL_Event e;
    if (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT:
        game.done = true;
        break;
      case SDL_KEYUP:
      case SDL_KEYDOWN: {
        int down = e.type == SDL_KEYDOWN;
        switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          game.keys[ESCAPE] = down;
          break;
        case SDLK_LEFT:
          game.keys[LEFT] = down;
          break;
        case SDLK_RIGHT:
          game.keys[RIGHT] = down;
          break;
        case SDLK_UP:
          game.keys[UP] = down;
          break;
        case SDLK_DOWN:
          game.keys[DOWN] = down;
          break;
        case SDLK_SPACE:
          game.keys[SPACE] = down;
          break;
        case SDLK_z:
          game.keys[FIRE3] = down;
          break;
        case SDLK_x:
          game.keys[FIRE4] = down;
          break;
        case SDLK_a:
          game.keys[FIRE1] = down;
          break;
        case SDLK_s:
          game.keys[FIRE2] = down;
          break;
        }
      }
      }
    }

    int ticks = SDL_GetTicks();

    float dt = ticks - lastJSTicks;
    if (dt > 15) {
      lastJSTicks = ticks;
      spriteIndex = 0;

      {
        TX_TIMER_RESET
        // printf("====================\n");
        ScriptUpdate();
        js_std_loop(ctx);
        TX_TIMER_END

        if (_cpu_time_used > 0.05) {
          // printf("%fsecs\n", _cpu_time_used);
        }

        // JSValue oc = JS_GetPropertyStr(ctx, app, "objects");
        // JS_ToInt32(ctx, &objectCount, oc);

        JSValue sc = JS_GetPropertyStr(ctx, app, "spriteCount");
        JS_ToInt32(ctx, &spriteCount, sc);

        JSValue tc = JS_GetPropertyStr(ctx, app, "textCount");
        JS_ToInt32(ctx, &textCount, tc);

        #if 0
        if (spriteCount > 0) {
          spriteIndex = 0;
          JSValue sprs = JS_GetPropertyStr(ctx, app, "sprites");
          for (int i = 0; i < spriteCount; i++) {
            sprite_t *sprite = &sprites[spriteIndex++];
            JSValue spr = JS_GetPropertyUint32(ctx, sprs, i);
            JSValue prop = JS_GetPropertyUint32(ctx, spr, 0);
            JS_ToInt32(ctx, &sprite->ax, prop);
            prop = JS_GetPropertyUint32(ctx, spr, 1);
            JS_ToInt32(ctx, &sprite->ay, prop);
            prop = JS_GetPropertyUint32(ctx, spr, 2);
            JS_ToInt32(ctx, &sprite->x, prop);
            prop = JS_GetPropertyUint32(ctx, spr, 3);
            JS_ToInt32(ctx, &sprite->y, prop);
            prop = JS_GetPropertyUint32(ctx, spr, 4);
            JS_ToInt32(ctx, &sprite->flipped, prop);
            prop = JS_GetPropertyUint32(ctx, spr, 5);
            JS_ToInt32(ctx, &sprite->sx, prop);
            prop = JS_GetPropertyUint32(ctx, spr, 6);
            JS_ToInt32(ctx, &sprite->sy, prop);
            prop = JS_GetPropertyUint32(ctx, spr, 7);
            JS_ToInt32(ctx, &sprite->sw, prop);
            prop = JS_GetPropertyUint32(ctx, spr, 8);
            JS_ToInt32(ctx, &sprite->sh, prop);
            JS_FreeValue(ctx, spr);
            // printf("%d (%d, %d)\n", i, sprite->x, sprite->y);
          }
          JS_FreeValue(ctx, sprs);
          // printf(">%d\n", spriteCount);
        }
        #endif

        if (textCount > 0) {
          textIndex = 0;
          JSValue txts = JS_GetPropertyStr(ctx, app, "texts");
          for (int i = 0; i < textCount; i++) {
            text_t *text = &texts[textIndex++];
            JSValue txt = JS_GetPropertyUint32(ctx, txts, i);

            JSValue prop = JS_GetPropertyUint32(ctx, txt, 0); // font
            // JS_ToInt32(ctx, &sprite->ax, prop);
            prop = JS_GetPropertyUint32(ctx, txt, 1); // text
            char *str = JS_ToCString(ctx, prop);
            if (str) {
              strcpy(text->text, str);
              JS_FreeCString(ctx, str);
            }
            prop = JS_GetPropertyUint32(ctx, txt, 2); // x
            JS_ToInt32(ctx, &text->x, prop);
            prop = JS_GetPropertyUint32(ctx, txt, 3); // y
            JS_ToInt32(ctx, &text->y, prop);
            prop = JS_GetPropertyUint32(ctx, txt, 4); // size
            JS_ToInt32(ctx, &text->size, prop);

            JS_FreeValue(ctx, txt);
          }
          JS_FreeValue(ctx, txts);
        }
      }
    }

    // float dt = ticks - lastTicks;
    // if (dt < 12) {
    //   continue;
    // }
    // lastTicks = ticks;
    // if (dt > 64) {
    //   dt = 64;
    // }

    GameUpdate(&game, dt / 1000);
    if (game.done)
      break;

    // send keys
    for (int i = 0; i < KEYS_END; i++) {
      if (game.keysPressed[i]) {
        ScriptSendKeyDown(i);
        // js_std_loop(ctx);
      }
      if (game.keysReleased[i]) {
        ScriptSendKeyUp(i);
        // js_std_loop(ctx);
      }
    }

    if (spriteIndex == 0)
      continue;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    ContextSave(&context);
    GameRender(&game, &context);
    ContextRestore(&context);

    // single spritesheet
    if (images[0] == NULL && imagePaths[0][0] != 0) {
      images[0] = _loadImage(&context, imagePaths[0]);
    }

    for (int i = 0; i < spriteIndex; i++) {
      sprite_t *sprite = &sprites[i];
      sprite->texture = images[0];
      vector_t pos;
      VectorInit(&pos, sprite->x, sprite->y, 0);
      _drawImage(&context, sprite, pos);
    }

    {
      char text[128];
      sprintf(text, "%f  objects:%d  sprites:%d", _cpu_time_used, objectCount,
              spriteCount);
      vector_t pos;
      pos.x = 20;
      pos.y = 20;
      context.state->r = 255;
      context.state->g = 255;
      context.state->b = 255;
      ContextDrawText(&context, text, pos, 1, 1);
    }

    ContextSave(&context);
    for (int i = 0; i < textCount; i++) {
      text_t *text = &texts[i];
      vector_t pos;
      pos.x = text->x * SCALE;
      pos.y = text->y * SCALE + (16 * SCALE);
      context.state->r = 255;
      context.state->g = 255;
      context.state->b = 255;
      context.state->stroke = 4;
      ContextDrawText(&context, text->text, pos, (SCALE * 0.6), 1);
    }
    ContextRestore(&context);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  GameDestroy(&game);

  tx_stats();
  printf("bye\n");

  // freeup all images

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  ScriptingShutdown();
}
