#include <time.h>
#include <stdio.h>

#include "settings.h"
#include "gameObjects.h"
#include "iofuncs.h"

#include "emscripten.h"

double nextTick;

EM_JS(double, getMillis, (), {
    return performance.now();
});

EM_JS(void, _startIO, (int screenWidth, int screenHeight, int fps), {
    canvas = document.querySelector("#canvas");
    canvas.width = screenWidth;
    canvas.height = screenHeight;
    ctx = canvas.getContext("2d", { alpha: true });
    imageCache = {};
});

EM_JS(void, _updateIO, (), {
});

EM_JS(void, _clearDisplay, (int r, int g, int b), {
    ctx.fillStyle = "rgb(" + r + ", " + g + ", " + b + ")"; // set the color
    ctx.fillRect(0, 0, canvas.width, canvas.height); // fill the entire canvas
})

EM_JS(void, _drawObject, (int x, int y, int width, int height, int xResolution, int yResolution, int id, bool cacheAllowed, void* pixels), {
    if (cacheAllowed && imageCache[id] != undefined) {
        ctx.drawImage(imageCache[id], x, y, width, height);
        return;
    }
    let size = width * height * 4;
    let pixelArray = new Uint8ClampedArray(Module.HEAPU8.buffer, pixels, size);
    let imageData = new ImageData(pixelArray, width, height);
    createImageBitmap(imageData).then(image => {
        if (cacheAllowed) imageCache[id] = image;
        ctx.drawImage(image, x, y, width, height);
    });
});

EM_JS(void, _awaitNextTick, (), {
});

int getSeed() {
    return time(NULL);
}

void pollInputs(inputStruct_t* input) {
    return;
}

int globalfps;

void startIO(int screenWidth, int screenHeight, int fps) {
    globalfps = fps;
    nextTick = getMillis() + (1000.0f / fps);
    _startIO(screenWidth, screenHeight, fps);
}

void updateIO() {
    _updateIO();
}

void clearDisplay(pixel_t color) {
    _clearDisplay(color.r, color.g, color.b);
}

void drawImage(image_t* image, int x, int y, int width, int height) {
    _drawObject(x, y, width, height, width, height, image->id, image->cacheAllowed, image->pixels);
}

void awaitNextTick() {
    _awaitNextTick();
    emscripten_sleep(nextTick - getMillis());
    nextTick += 1000.0f / globalfps;
}
