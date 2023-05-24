#include "renderer.h"
#include "data.h"
#include "game.h"
#include "polygon.h"

#include <math.h>
#include <stdio.h>

void RenderShape(context_t *context, float *points, vector_t pos, float angle,
                 float radius, bool close) {
  points += 3;

  polygon_t p;
  p.sides = 0;
  for (int i = 0; i < POLYGON_MAX_SIDES; i++) {
    if (points[0] == -1 && points[1] == -1 && points[2] == -1) {
      break;
    }
    p.points[i].x = points[0];
    p.points[i].y = points[1];
    p.points[i].z = points[2];
    p.sides++;
    points += 3;
  }
  ContextDrawPolygon(context, &p, pos, angle, radius, close);
}

void RenderParticle(entity_t *e, context_t *context) {
  vector_t v = VectorScaled(&e->velocity, 0.1);
  vector_t v2 = VectorSubtracted(&e->position, &v);
  ContextDrawLine(context, e->position, v2);
}

void RenderExplosion(entity_t *entity, context_t *context) {
  ContextSave(context);
  ContextSetColor(context, 50, 100, 50);
  ContextDrawPolygon(context, &polygon_circle, entity->position,
                     VectorAngle(&entity->direction), entity->radius, true);
  ContextRestore(context);
}

void RenderFloatingText(entity_t *entity, context_t *context) {
  ContextDrawText(context, (char *)entity->data, entity->position, 0.025, 0);
}

void RenderEntities(list_t *entityList, context_t *context) {
  node_t *n = entityList->first;
  while (n) {
    entity_t *e = (entity_t *)n->data;
    if (e->onRender) {
      e->onRender(e, context);
    } else {
      RenderParticle(e, context);
    }
    n = n->next;
  }
}

