#pragma once

#include "settings.h"
#include <stdbool.h>
#include <stdint.h>

#define interface struct

typedef struct {
    int16_t x;
    int16_t y;
} point_t;

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} rect_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} pixel_t;

struct letterRow {
    char cols[3];
};

struct letter {
    struct letterRow rows[5];
};

struct letterSet {
    const struct letter* letters;
    int letterMax;
};

const extern struct letter* const letters;
const extern struct letterSet basicFont;

int drawString(char* string, rect_t* rect, const struct letterSet font, pixel_t color, pixel_t backgroundColor, int charWidth, int charHeight);

typedef struct {
    int16_t id;
    uint16_t width;
    uint16_t height;
    char name[NAMESIZE];
    bool cacheAllowed;
    pixel_t pixels[];
} image_t;

typedef struct {
    float x;
    float y;
    uint16_t id;
    bool alive;
    uint8_t traitData[];
} gameObject_t;

typedef struct {
    image_t** images;
    uint16_t imageCount;
    uint16_t currentImage;
    rect_t imageClip;
    uint16_t width;
    uint16_t height;
    uint16_t ticksPerFrame;
} spriteSheet_t;

typedef struct {
    spriteSheet_t* spriteSheets;
    uint16_t sheetCount;
    uint16_t currentSheet;
} spriteSetTrait;

typedef struct {
    float xVelocity;
    float yVelocity;
    bool respectsGravity;
    float gravityOverride; // 0 if uses global gravity, some other number to define a specific gravity value.
    bool respectsAirResistance;
    float airResistanceOverride; // 0 if uses global air resistance, some other number to define a specific gravity value.
    float inertia;
} physicsTrait;

typedef struct {
    // TODO
} audioTrait;

enum collisionType {
    RECT,
    CIRCLE,
};

struct collisionTraitGeneric {
    uint16_t collisionType;
    void* collisionData; // checks out space in its gameObject
};

typedef struct collisionTraitGeneric collisionTraitGeneric;

typedef struct {
    uint16_t collisionType;
    rect_t* boundingBox;
} collisionTraitRect;

typedef union {
    collisionTraitGeneric generic;
    collisionTraitRect rect;
} collisionTrait;

// Basic interfaces that primitive functions use. "top-level interfaces", interface structs that do not compose any larger types, should be used as each object type.
// top-level interfaces should have init methods, and non top-level interfaces should be derived from those.
// Objects that are not the same top-level interface but can interact via sub-interfaces (all physics objects, for example) can derive and distribute interfaces into polymorphic lists.

interface drawable {
    gameObject_t* object;
    spriteSetTrait* sprites;
};
#define toDrawable(value) ((interface drawable) {(value).object, &(value).spriteSet})

void drawObject(interface drawable t);
void nextFrame(interface drawable t);
void lastFrame(interface drawable t);
void setSheet(interface drawable t, int sheet);
image_t* getFrame(interface drawable t);

interface physics {
    gameObject_t* object;
    physicsTrait* physics;
};
#define toPhysics(value) ((interface physics) {(value).object, &(value).physics})

void applyGravity(interface physics t);
void accelerate(interface physics t, float x, float y);
void applyAirResistance(interface physics t);
void move(interface physics t);
void applyGravity(interface physics t);

interface audio {
    gameObject_t* object;
    audioTrait* audio;
};
#define toAudio(value) ((interface audio) {(value).object, &(value).audio})

interface collision {
    gameObject_t* object;
    collisionTrait* collision;
};
#define toCollision(value) ((interface collision) {(value).object, &(value).collision})

bool rectsCollide(rect_t r1, rect_t r2);
void getCollisions(interface collision t, interface collision* others, int numOthers);
bool collidesWith(interface collision t, interface collision other);


interface physicsCollision {
    gameObject_t* object;
    collisionTrait* collision;
    physicsTrait* physics;
};
#define toPhysicsCollision(value) ((interface physicsCollision) {(value).object, &(value).collision, &(value).physics})

void moveWithCollisions(interface physicsCollision t, interface physicsCollision* others, int numOthers);

typedef struct {
    gameObject_t* object;
    collisionTrait collision;
    physicsTrait physics;
    spriteSetTrait spriteSet;
    audioTrait audio;
} entity_t;

entity_t initEntity(float x, float y, int width, int height, int* spriteSheetIndices, int* frameCounts, int sheetCount, image_t** imageList, float inertia, rect_t boundingBox);
