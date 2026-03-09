#pragma once

#include "settings.h"
#include "gameObjects.h"
#include "inputs.h"

void startIO(int screenWidth, int screenHeight, int fps);
void updateIO();

void drawImage(image_t* image, int x, int y, int width, int height);
void clearDisplay(pixel_t color);
void pollInputs(inputStruct_t* inputs);
void awaitNextTick();
void setBuzz(uint8_t);

int getSeed();
