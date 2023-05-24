#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
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

#define MAX_IMAGES 16
static char imagePaths[MAX_IMAGES][256];
static SDL_Texture *images[MAX_IMAGES];

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

SDL_Texture *_loadImage(context_t *context, char *path) {
  SDL_Renderer *renderer = (SDL_Renderer *)context->renderer;
  SDL_Surface *temp = IMG_Load(path);
  if (!temp) {
    printf("unable to load %s\n", path);
    return;
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
  float scale = 2;

  // if (sprite->sw == 0) {
    // printf("%d %d\n", sprite->sw,sprite->sh);
  // }

  if (sprite->sw > 16 * 10) return;

  // int sw, sh;
  // SDL_QueryTexture(sprite->texture, NULL, NULL, &sw, &sh);
  SDL_Rect dest;
  dest.x = vt.x * scale;
  dest.y = vt.y * scale;
  dest.w = sprite->sw * scale;
  dest.h = sprite->sh * scale;

  // // SDL_RenderCopy(renderer, image, NULL, &dest);
  SDL_Point center;
  center.x = 0;//sprite->ax;
  center.y = 0;//sprite->ay;

  SDL_Rect src;
  src.x = sprite->sx;
  src.y = sprite->sy;
  src.w = sprite->sw;
  src.h = sprite->sh;

  bool flipped = sprite->flipped == -1;
  SDL_RenderCopyEx(renderer, sprite->texture, &src, &dest, 0, &center,
                   flipped ? SDL_FLIP_HORIZONTAL : 0);

  // vector_t p1, p2;
  // p1.x = dest.x;
  // p1.y = dest.y;
  // p2.x = dest.x + 32;
  // p2.y = dest.y + 32;
  // ContextDrawLine(context, p1, p2);
}

static JSRuntime* rt = 0;
static JSContext* ctx = 0;

static JSValue js_log(JSContext* ctx, JSValueConst this_val,
    int argc, JSValueConst* argv)
{
    printf("log: ");
    int i;
    const char* str;
    for (i = 0; i < argc; i++) {
        str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;
        printf("%s\n", str);
        JS_FreeCString(ctx, str);
    }
    return JS_UNDEFINED;
}

static JSValue js_load_image(JSContext* ctx, JSValueConst this_val,
    int argc, JSValueConst* argv)
{
    int id = -1;
    JS_ToInt32(ctx, &id, argv[0]);
    const char* str = JS_ToCString(ctx, argv[1]);
    if (!str)
      return JS_EXCEPTION;

    printf("load %d %s\n", id, str);
    strcpy(imagePaths[id], str);

    JS_FreeCString(ctx, str);
    return JS_UNDEFINED;
}

static JSValue js_draw_image(JSContext* ctx, JSValueConst this_val,
    int argc, JSValueConst* argv)
{
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
static JSContext* JS_NewCustomContext(JSRuntime* rt)
{
    JSContext* ctx;
    ctx = JS_NewContext(rt);
    if (!ctx)
        return NULL;
    /* system modules */
    js_init_module_std(ctx, "std");
    js_init_module_os(ctx, "os");
    return ctx;
}

void ScriptingInit()
{
    // init scripting
    rt = JS_NewRuntime();
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

    JSValue global_obj, app;

    global_obj = JS_GetGlobalObject(ctx);

    app = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, app, "log", JS_NewCFunction(ctx, js_log, "log", 1));
    JS_SetPropertyStr(ctx, app, "loadImage", JS_NewCFunction(ctx, js_load_image, "loadImage", 1));
    JS_SetPropertyStr(ctx, app, "drawImage", JS_NewCFunction(ctx, js_draw_image, "drawImage", 1));
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
        JSValue ret = JS_Eval(ctx, input, strlen(input), "<input>", JS_EVAL_TYPE_GLOBAL);
        if (JS_IsException(ret)) {
            js_std_dump_error(ctx);
            JS_ResetUncatchableError(ctx);
        }
    }
}

void ScriptingShutdown() {
    js_std_free_handlers(rt);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}

void ScriptUpdate() {
    char *input = "window.update();";
    JSValue ret = JS_Eval(ctx, input, strlen(input), "<input>", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(ret)) {
        js_std_dump_error(ctx);
        JS_ResetUncatchableError(ctx);
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
  "",
  "ArrowUp",
  "ArrowDown",
  "ArrowLeft",
  "ArrowRight",
  "a",
  "s",
  "z",
  "x",
  "Escape",
  "Space",
};

const int keyCodes[] = {
  0,
  38,
  40,
  37,
  39,
  65,
  83,
  90,
  88,
  27,
  32,
};

void ScriptSendKeyDown(int key) {
  char script[64];
  sprintf(script, "window.onkeydown(new KeyEvent('%s', %d));", keyNames[key], keyCodes[key]);
  JSValue ret = JS_Eval(ctx, script, strlen(script), "<input>", JS_EVAL_TYPE_GLOBAL);
  if (JS_IsException(ret)) {
      js_std_dump_error(ctx);
      JS_ResetUncatchableError(ctx);
  }
}

void ScriptSendKeyUp(int key) {
  char script[64];
  sprintf(script, "window.onkeyup(new KeyEvent('%s', %d));", keyNames[key], keyCodes[key]);
  JSValue ret = JS_Eval(ctx, script, strlen(script), "<input>", JS_EVAL_TYPE_GLOBAL);
  if (JS_IsException(ret)) {
      js_std_dump_error(ctx);
      JS_ResetUncatchableError(ctx);
  }
}

// {
//     char *input = "window.update();";
//     JSValue ret = JS_Eval(ctx, input, strlen(input), "<input>", JS_EVAL_TYPE_GLOBAL);
//     if (JS_IsException(ret)) {
//         js_std_dump_error(ctx);
//         JS_ResetUncatchableError(ctx);
//     }
// }


void ScriptRunFile(char *path) {
  FILE *fp = fopen(path, "r");
  if (fp) {
    fseek(fp, 0, SEEK_END);
    size_t sz = ftell(fp);
    // fprintf(stderr, "%d\n", sz);
    fseek(fp, 0, SEEK_SET);
    char *content = (char*)malloc(sz);
    fread(content, sz, 1, fp);
    // fprintf(stderr, "%s\n", content);
    fclose(fp);
    JSValue ret = JS_Eval(ctx, content, sz, path, JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(ret)) {
        js_std_dump_error(ctx);
        JS_ResetUncatchableError(ctx);
    }
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
  // printf(">%d\n", context->state->r);
  SDL_RenderDrawLine(renderer, v1.x, v1.y, v2.x, v2.y);
}

int main(int argc, char **argv) {
  for(int i=0; i<MAX_IMAGES; i++) {
    imagePaths[i][0] = 0;
    images[i] = NULL;
  }
  FastRandomInit(0);

  ScriptingInit();
  
  int width = 800;
  int height = 600;
  context_t context;

  FontInit();

  game_t game;
  GameInit(&game);

  sceneTest_t testScene;
  TestSceneInit(&testScene);
  sceneMenu_t menuScene;
  MenuSceneInit(&menuScene);
  game.menu = &menuScene;
  GameEnterMenu(&game);

  // game.scene = &testScene;

  int frames = 0;

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_EnableScreenSaver();
  SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  atexit(SDL_Quit);

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  width = dm.w * 0.75;
  height = dm.h * 0.75;

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

  rect_t *tr = 0;
  vector_t tv;
  int trIdx = -1;

  ScriptRunFile("./dist/index.js");
    
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

    {
      float dt = ticks - lastJSTicks;
      if (dt > 16) {
        lastJSTicks = ticks;
        spriteIndex = 0;
        ScriptUpdate();
        js_std_loop(ctx);
      }
    }

    float dt = ticks - lastTicks;
    if (dt < 12) {
      continue;
    }
    lastTicks = ticks;
    if (dt > 64) {
      dt = 64;
    }

    GameUpdate(&game, dt / 1000);
    if (game.done)
      break;

    // send keys
    for(int i=0; i<KEYS_END; i++) {
      if (game.keysPressed[i]) {
        ScriptSendKeyDown(i);
      }
      if (game.keysReleased[i]) {
        ScriptSendKeyUp(i);
      }
    }

    if (spriteIndex == 0) continue;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    ContextSave(&context);
    GameRender(&game, &context);
    ContextRestore(&context);

    if (images[0] == NULL && imagePaths[0][0] != 0) {
      images[0] = _loadImage(&context, imagePaths[0]);
    }

    for(int i=0; i<spriteIndex; i++) {
      sprite_t *sprite = &sprites[i];
      sprite->texture = images[0];
      vector_t pos;
      VectorInit(&pos, sprite->x, sprite->y, 0);
      _drawImage(&context, sprite, pos);
    }

    SDL_RenderPresent(renderer);
  }

  GameDestroy(&game);

  tx_stats();
  printf("bye\n");

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  ScriptingShutdown();
}
