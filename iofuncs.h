#pragma once

#include "settings.h"
#include "gameObjects.h"

void startIO(int screenWidth, int screenHeight, int fps);
void updateIO();

void drawImage(image_t* image, int x, int y, int width, int height);
void clearDisplay(pixel_t color);
void awaitNextTick();
int getSeed();

// input suite of iofuncs
#include "inputs.h"

