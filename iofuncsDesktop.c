#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <time.h>
#include <stdlib.h>

#include "settings.h"
#include "gameObjects.h"
#include "sdl_wrapper.h"
#include "iofuncs.h"
#include "inputs.h"

pixel_t black = {.r=0, .g=0, .b=0};
struct screenPackage s;
int globalfps = 0;

uint32_t nextTick;

void startIO(int screenWidth, int screenHeight, int fps) {
    globalfps = fps;
    s = initVideo(SCREENWIDTH, SCREENHEIGHT, 64, 3);
    nextTick = SDL_GetTicks() + (1000 / fps);
    initInputs();
}

const int KEYBOARDID = 0;
void pollInputs() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit(0);
        }
        int active;
        if (e.type == SDL_KEYDOWN) {
            active = 1;
        }
        else if (e.type == SDL_KEYUP) {
            active = 0;
        }
        else continue;
        int scancode = e.key.keysym.scancode;
        int deviceId = KEYBOARDID;
        sendEvent(deviceId, scancode, active);
    }
}

void updateIO() {
    pollInputs();
    updateScreen(s);
}

SDL_Texture* getUncachedTexture(image_t* image, bool willCache) {
    static SDL_Texture* permTexture;
    SDL_Texture* texture;
    SDL_Surface* surface = NULL;
    if (permTexture != NULL) SDL_DestroyTexture(permTexture);
    permTexture = NULL;
    surface = SDL_CreateRGBSurfaceWithFormatFrom(
            image->pixels,
            image->width,
            image->height,
            32,
            image->width * 4,
            SDL_PIXELFORMAT_ABGR8888
            );
    if (surface == NULL) {
        fprintf(stderr, "%s: %s\n", __func__, SDL_GetError());
        return NULL;
    }
    texture = SDL_CreateTextureFromSurface(s.renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == NULL) {
        fprintf(stderr, "%s: %s\n", __func__, SDL_GetError());
        return NULL;
    }
    if (!willCache) permTexture = texture;
    return texture;
}

SDL_Texture* getCachedTexture(image_t* image) {
    SDL_Texture* texture;
    if (image->id >= s.textureCapacity) {
        int oldCapacity = s.textureCapacity;
        s.textureCapacity = image->id + 1;
        SDL_Texture** newTextures = realloc(s.textures, s.textureCapacity * sizeof (*s.textures));
        if (newTextures == NULL) {
            fprintf(stderr, "%s: %s\n", __func__, strerror(errno));
            return NULL;
        }
        s.textures = newTextures;
        memset(s.textures + oldCapacity, 0, (s.textureCapacity - oldCapacity) * sizeof *s.textures);
    }
    if (s.textures[image->id] == NULL) {
        s.textures[image->id] = getUncachedTexture(image, true);
    }
    texture = s.textures[image->id];
    return texture;
}

SDL_Texture* getTexture(image_t* image) {
    if (image->cacheAllowed) return getCachedTexture(image);
    else return getUncachedTexture(image, false);
}

void clearDisplay(pixel_t color) {
    clearScreen(s, color);
}

void drawImage(image_t* image, int x, int y, int width, int height) {
    SDL_Texture* texture = getTexture(image);
    SDL_Rect destination = (SDL_Rect) {
        .x = x,
        .y = y,
        .w = width,
        .h = height,
    };
    SDL_RenderCopy(s.renderer, texture, NULL, &destination);
}

void awaitNextTick() {
    // busy wait is fine because the desktop binding is mainly meant to test for the embedded binding
    while (SDL_GetTicks() < nextTick);
    nextTick += 1000 / globalfps;
}

int getSeed() {
    return time(NULL);
}
