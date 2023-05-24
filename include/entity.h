#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "context.h"
#include "util.h"
#include "vector.h"

typedef struct entity_t {
  vector_t position;
  vector_t direction;
  vector_t velocity;
  float radius;
  float life;
  int type;
  bool invisible;
  void (*onUpdate)(struct entity_t *, float delta_time);
  void (*onRender)(struct entity_t *, context_t *context);
  void *data;
} entity_t;

void EntityInit(entity_t *entity);
void EntitiesUpdate(list_t *entityList, float dt);
void EntitiesCreateParticles(list_t *entityList, vector_t position, int count,
                             float speed);
void EntitiesCreateExplosion(list_t *entityList, vector_t position);
void EntitiesCreateFloatingText(list_t *entityList, char *text,
                                vector_t position);

#endif // _ENTITY_H_