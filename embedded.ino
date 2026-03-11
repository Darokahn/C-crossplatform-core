#include <TFT_eSPI.h>

#define ALPHAIGNORETHRESHOLD 10

#define FULLLCDWIDTH 320
#define FULLLCDHEIGHT 240

/*
#define LCDWIDTH 265
#define LCDHEIGHT 208
*/

#define LCDWIDTH 320
#define LCDHEIGHT 240

#define TFT_MISO -1
#define TFT_MOSI  5
#define TFT_SCLK 18
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    4  // Data Command control pin
#define TFT_RST   2  // Reset pin (could connect to RST pin)
#define TFT_LED  19

#define THUMBSTICK_VCC 26
#define THUMBSTICK_GND 27

#define THUMBSTICK_X    33
#define THUMBSTICK_Y    25
#define THUMBSTICK_SW   32
#define SW_PIN 23
#define SW_GND 22

int globalfps;
int frameInterval;

TFT_eSPI tft = TFT_eSPI(FULLLCDWIDTH, FULLLCDWIDTH);
TFT_eSprite tftSpriteUpper = TFT_eSprite(&tft);
TFT_eSprite tftSpriteLower = TFT_eSprite(&tft);

extern "C" {
    #include "gameObjects.h"
    #include "iofuncs.h"
    #include "settings.h"
}


int nextTick;
void awaitNextTick() {
    while (millis() < nextTick);
    nextTick += frameInterval;
}

extern "C" int main();
const int TOPHEIGHT = 170;
const int BOTTOMHEIGHT = FULLLCDHEIGHT - 170;

extern "C" void startIO(int screenWidth, int screenHeight, int fps) {
    globalfps = fps;
    frameInterval  = 1000 / globalfps;
    pinMode(THUMBSTICK_X, INPUT_PULLUP);
    pinMode(THUMBSTICK_Y, INPUT_PULLUP);
    pinMode(THUMBSTICK_SW, INPUT_PULLUP);
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, HIGH);
    pinMode(THUMBSTICK_VCC, OUTPUT);
    digitalWrite(THUMBSTICK_VCC, HIGH);
    pinMode(THUMBSTICK_GND, OUTPUT);
    digitalWrite(THUMBSTICK_GND, LOW);
    pinMode(SW_PIN, INPUT_PULLUP);
    pinMode(SW_GND, OUTPUT);
    digitalWrite(SW_GND, LOW);
    tft.init();
    tft.setRotation(2);
    tftSpriteUpper.createSprite(FULLLCDWIDTH, TOPHEIGHT);
    tftSpriteLower.createSprite(FULLLCDWIDTH, BOTTOMHEIGHT);
    tft.fillScreen(TFT_BLUE);
    nextTick = millis() + frameInterval;
}

int ADC_CENTER = -1;       // Midpoint of 12-bit ADC
const int DEADZONE  = 200;         // Joystick deadzone threshold

bool inputs[6];
extern "C" void updateIO() {
    tftSpriteUpper.pushSprite(0, 0);
    tftSpriteLower.pushSprite(0, TOPHEIGHT);
    int vrx = analogRead(THUMBSTICK_X);
    int vry = analogRead(THUMBSTICK_Y);
    bool sw0  = digitalRead(THUMBSTICK_SW) == LOW;  // Active-low button
    bool sw1  = digitalRead(SW_PIN) == LOW;  // Active-low button
    if (ADC_CENTER == -1) ADC_CENTER = vry;
    machineLog("x: %d, y: %d, sw0: %d, sw1: %d\n\r", vrx, vry, sw0, sw1);

    inputs[0] = vry > ADC_CENTER + DEADZONE;
    inputs[1] = vrx > ADC_CENTER + DEADZONE;
    inputs[2] = vry < ADC_CENTER - DEADZONE;
    inputs[3] = vrx < ADC_CENTER - DEADZONE;
    inputs[4] = sw0;
    inputs[5] = sw1;
    return;
}

extern "C" void* mallocDMA(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_DMA);
}

unsigned long nativeColor(pixel_t color) {
    return (color.r << 11) | ((color.g & 077) << 5) | ((color.b & 037));
}

void clearDisplay(pixel_t color) {
    tftSpriteUpper.fillScreen(nativeColor(color));
    tftSpriteLower.fillScreen(nativeColor(color));
}

static inline void drawPixel(int x, int y, uint16_t color) {
    static TFT_eSprite* halves[] = {&tftSpriteUpper, &tftSpriteLower};
    TFT_eSprite* screenHalf = halves[y >= TOPHEIGHT];
    y -= (TOPHEIGHT * (y >= TOPHEIGHT));
    screenHalf->drawPixel(x, y, color);
}

rect_t screenRect = (rect_t) {0, 0, LCDWIDTH, LCDHEIGHT};
extern "C" void drawImage(image_t* image, int x, int y, int width, int height) {
    rect_t destination = (rect_t) {
        .x = x,
        .y = y,
        .width = width,
        .height = height,
    };
    if (!rectsCollide(destination, screenRect)) return;
    float xStride = (float)image->width / destination.width;
    float yStride = (float)image->height / destination.height;
    int pixelSize = sizeof(pixel_t);
    int rowSize = image->width * pixelSize;
    uint8_t* bytes = (uint8_t*) image->pixels;
    for (int y = 0; y < destination.height; y++) {
        for (int x = 0; x < destination.width; x++) {
            int column = x * xStride;
            int row = y * yStride;
            bytes = (uint8_t*)image->pixels + (row * rowSize) + (column * pixelSize);
            uint8_t red = *(bytes++) * 31 / 255;
            uint8_t green = *(bytes++) * 63 / 255;
            uint8_t blue = *(bytes++) * 31 / 255;
            uint8_t alpha = *(bytes++);
            if (alpha <= ALPHAIGNORETHRESHOLD) continue;
            uint16_t color = red << 11 | green << 5 | blue;
            int drawX = x + destination.x;
            int drawY = y + destination.y;
            drawPixel(drawX, drawY, color);
        }
    }
}

extern "C" int getInput(int index) {
    return inputs[index];
}

void pollInputs(inputStruct_t* inputs) {
    inputs->yAxis = getInput(2) - getInput(0);
    inputs->xAxis = getInput(3) - getInput(1);
    inputs->action1 = getInput(4);
    inputs->action2 = getInput(5);
}

int getSeed() {
    return esp_random();
}

extern "C" int machineLog(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = Serial.vprintf(fmt, args);
    va_end(args);
    return n;
}

void loop() {
}

void setup() {
    delay(1000);
    Serial.begin(115200);
    machineLog("got here\n\n\n\r");
    main();
}
