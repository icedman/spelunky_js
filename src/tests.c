#include "tests.h"
#include "renderer.h"
#include "game.h"

float zoom = 1.0;

void testEnter(game_t *gm, scene_t *scn) {
  GameDestroy(gm);
}

void testUpdate(game_t *gm, scene_t *scn, float delta_time) {
  if (gm->keysPressed[ESCAPE]) {
    gm->done = true;
    return;
  }

  if (gm->keysPressed[SPACE]) {
    GameDestroy(gm);
  }

  if (gm->keys[UP]) {
    zoom += 0.5;
    if (zoom > 40) {
      zoom = 40;
    }
  }

  if (gm->keys[DOWN]) {
    zoom -= 0.5;
    if (zoom < 2) {
      zoom = 2;
    }
  }

  if (gm->keys[LEFT]) {
  }

  if (gm->keys[RIGHT]) {
  }
}

void testRender(game_t *gm, scene_t *scn, context_t *context) {
  int width = context->width;
  int height = context->height;
  ContextScale(context, zoom);

}

void TestSceneInit(sceneTest_t *scene) {
  SceneInit(scene);
  scene->scene.onEnter = testEnter;
  scene->scene.onUpdate = testUpdate;
  scene->scene.onRender = testRender;
}