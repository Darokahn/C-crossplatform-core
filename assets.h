#pragma once
#include <stdint.h>
#include "gameObjects.h"

extern uint8_t imageBlob[];
extern point_t points[];

int loadImages(image_t** imageList, int listCapacity, uint8_t* buffer);

void loadStage(point_t* points, int pointCount, entity_t* stageTiles, int tileCount, image_t** imageList);
