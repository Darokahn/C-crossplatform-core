#include "sdl_wrapper.h"
#include "gameObjects.h"

#include <SDL2/SDL.h>
#include <math.h>

struct screenPackage initVideo(int width, int height, int initialTextureCapacity, int windowScale) {
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        fprintf(stderr, "%s: %s\n", __func__, SDL_GetError());
        exit(1);
    }
    struct screenPackage screen;
    screen.width = width;
    screen.height = height;

    screen.window = SDL_CreateWindow( "Video", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * windowScale, height * windowScale, SDL_WINDOW_SHOWN );
    if ( screen.window == NULL ) {
        fprintf(stderr, "%s: %s\n", __func__, SDL_GetError());
        exit(1);
    }
    screen.renderer = SDL_CreateRenderer(screen.window, -1, SDL_RENDERER_ACCELERATED);
    if (screen.renderer == 0) {
        fprintf(stderr, "%s: %s\n", __func__, SDL_GetError());
        exit(1);
    }
    SDL_RenderSetLogicalSize(screen.renderer, width, height);
    screen.initialized = true;
    screen.textureCapacity = initialTextureCapacity;
    screen.textures = (malloc(sizeof(*screen.textures) * screen.textureCapacity));
    screen.canvas = SDL_CreateTexture(
        screen.renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_TARGET,
        screen.width,
        screen.height
    );
    SDL_SetRenderTarget(screen.renderer, screen.canvas);
    for (int i = 0; i < screen.textureCapacity; i++) {
        screen.textures[i] = NULL;
    }
    return screen;
}

void destroyVideo(struct screenPackage screen) {
    SDL_DestroyRenderer(screen.renderer);
    SDL_DestroyWindow(screen.window);
    SDL_Quit();
}

void updateScreen(struct screenPackage screen) {
    SDL_SetRenderTarget(screen.renderer, NULL);
    SDL_RenderCopy(screen.renderer, screen.canvas, NULL, NULL);
    SDL_RenderPresent(screen.renderer);
    SDL_SetRenderTarget(screen.renderer, screen.canvas);
}

void clearScreen(struct screenPackage screen, pixel_t c) {
    SDL_SetRenderDrawColor(screen.renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(screen.renderer);
}

void setPixel(struct screenPackage screen, point_t p, pixel_t c) {
    SDL_SetRenderDrawColor(screen.renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoint(screen.renderer, p.x, p.y);
}
