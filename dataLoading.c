#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/param.h>
#include "gameObjects.h"
#include "settings.h"
#include "assets.h"

int loadImages(image_t** imageList, int listCapacity, uint8_t* buffer) {
    image_t* image;
    int imageSize;
    for(int imageCount = 0; imageCount < listCapacity; (imageCount++, buffer += imageSize)) {
        image = (void*) buffer;
        if (image->id == -1) {
            return imageCount;
        }
        printf("sprite %d loaded. name: %s\n", image->id, image->name);
        imageSize = image->width * image->height * sizeof(image->pixels[0]) + sizeof(image_t);
        imageList[imageCount] = image;
    }
    return -1;
}

void loadStage(point_t* points, int pointCount, entity_t* stageTiles, int tileCount, image_t** imageList) {
    for (int i = 0; i < MIN(pointCount, tileCount); i++) {
        point_t p = points[i];
        stageTiles[i] = initEntity(p.x, p.y, 28, 28, (int[1]){2}, (int[1]){1}, 1, imageList, 0, (rect_t) {0, 0, 28, 28});
    }
}
