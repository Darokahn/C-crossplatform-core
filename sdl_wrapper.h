#pragma once

#include "gameObjects.h"

#include <SDL2/SDL.h>
#include <stdint.h>

struct screenPackage {
    SDL_Window *window;
    SDL_Texture* canvas;
    SDL_Renderer *renderer;
    uint16_t width;
    uint16_t height;
    bool initialized;
    SDL_Texture** textures;
    int textureCapacity;
};

typedef SDL_Event event;

struct screenPackage initVideo(int width, int height, int initialTextureCapacity, int windowScale);
void destroyVideo(struct screenPackage screen);

void setPixel(struct screenPackage screen, point_t p, pixel_t c);
void updateScreen(struct screenPackage screen);
void clearScreen(struct screenPackage screen, pixel_t c);

#define pollEvent SDL_PollEvent
