//
//  main.c
//  Extension
//
//

#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

#include "context.h"
#include "data.h"
#include "entity.h"
#include "font.h"
#include "game.h"
#include "matrix.h"
#include "menu.h"
#include "renderer.h"
#include "vector.h"

#define PATTERN_IMPLEMENTATION
#include "pattern.h"

static int update(void *userdata);
const char *fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont *font = NULL;

int width = 400;
int height = 240;

context_t context;

sceneMenu_t menuScene;
// sceneRace_t gameScene;
game_t game;

void _drawLine(void *ctx, vector_t v1, vector_t v2) {
  context_t *context = (context_t *)ctx;
  PlaydateAPI *pd = (PlaydateAPI *)context->renderer;
  int x1 = (int)v1.x;
  int y1 = (int)v1.y;
  int x2 = (int)v2.x;
  int y2 = (int)v2.y;
  int width = (int)context->state->stroke;
  LCDColor kColor = kColorWhite;
  if (context->state->g == 155) {
    kColor = (LCDColor)get_pattern(gray);
  }
  if (context->state->g == 100) {
    kColor = (LCDColor)get_pattern(gray_3);
  }
  if (context->state->dark) {
    kColor = (LCDColor)get_pattern(darkgray_2);
  }
  pd->graphics->drawLine(x1, y1, x2, y2, width, kColor);
}

#ifdef _WINDLL
__declspec(dllexport)
#endif
    int eventHandler(PlaydateAPI *pd, PDSystemEvent event, uint32_t arg) {
  (void)arg; // arg is currently only used for event = kEventKeyPressed

  if (event == kEventInit) {
    ContextInit(&context, width, height);
    FontInit();

    GameInit(&game);
    // RaceSceneInit(&gameScene);
    MenuSceneInit(&menuScene);

    game.gamePad = true;
    game.menu = (void *)&menuScene;
    GameEnterMenu(&game);
    // GameEnterRace(&game);

    const char *err;
    font = pd->graphics->loadFont(fontpath, &err);

    if (font == NULL)
      pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__,
                        fontpath, err);

    // Note: If you set an update callback in the kEventInit handler, the system
    // assumes the game is pure C and doesn't run any Lua code in the game
    pd->system->setUpdateCallback(update, pd);
  }

  return 0;
}

static int update(void *userdata) {
  PlaydateAPI *pd = userdata;
  context.renderer = pd;
  context.drawLine = _drawLine;

  // float dt = pd->system->getElapsedTime()/1000;
  float dt = 0.04;

  PDButtons current;
  pd->system->getButtonState(&current, NULL, NULL);

  game.keys[UP] = (current & kButtonUp);
  game.keys[DOWN] = (current & kButtonDown);
  game.keys[LEFT] = (current & kButtonLeft);
  game.keys[RIGHT] = (current & kButtonRight);
  game.keys[FIRE1] = (current & kButtonA);
  game.keys[FIRE2] = (current & kButtonB);

  GameUpdate(&game, dt);

  pd->graphics->clear(kColorBlack);
  pd->graphics->setFont(font);

  GameRender(&game, &context);

  pd->system->drawFPS(0, 0);

  return 1;
}
