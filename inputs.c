#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "iofuncs.h"
#include "inputs.h"
#include "settings.h"
#include "gameObjects.h"
#include "libraries/commonInterfaces/interfaces.h"
#include "libraries/commonInterfaces/objectstream.h"

uint8_t* deviceBuffer = NULL;
objectstream_t deviceBufferStream;
device_t** devices = NULL;
objectstream_t devicesStream;
write_i* controllers = NULL;
objectstream_t controllersStream;

const int deviceBufferInitialSize = 512;
const int deviceBufferBadSize = deviceBufferInitialSize * 8; // if the buffer is >= 4KB, compress devices.
const int devicesInitialSize = 2;
const int controllersInitialSize = 2;

bool initInputs() {
    deviceBuffer = malloc(deviceBufferInitialSize * sizeof *deviceBuffer);
    if (deviceBuffer == NULL) return false;
    objectstream_init(&deviceBufferStream, erase deviceBuffer, deviceBufferInitialSize * sizeof *deviceBuffer, alignof *deviceBuffer);
    devices = malloc(devicesInitialSize * sizeof *devices);
    if (devices == NULL) return false;
    objectstream_init(&devicesStream, erase devices, devicesInitialSize * sizeof *devices, alignof *devices);
    controllers = malloc(controllersInitialSize * sizeof *controllers);
    if (controllers == NULL) return false;
    objectstream_init(&controllersStream, erase controllers, controllersInitialSize * sizeof *controllers, alignof *controllers);
    return true;
}

bool addDevice(device_t inital) {
}

void destroyInputs() {
    if (deviceBuffer) free(deviceBuffer);
    deviceBuffer = NULL;
    if (devices) free(devices);
    devices = NULL;
    if (controllers) free(controllers);
    controllers = NULL;
}

fullInput_t getEntry(int deviceId, int deviceCode, int deviceValue) {
}

bool sendEvent(int deviceId, int deviceCode, int deviceValue) {
    if (devices == NULL) return false;
    if (controllers == NULL) return false;
    if (deviceId < 0 || deviceId >= devicesStream.length) return false;
    device_t* dev = devices[deviceId];
    if (deviceCode < dev->startIndex) return false;
    if (deviceCode > dev->endIndex) return false;
    input_t* entries = (input_t*) (dev->entries + dev->hotTableIndex);
    fullInput_t input = entries[deviceCode];
    write_i controllerBuffer = controllers[entry.controllerIndex];
    struct event newEvent = {
        .generic.index = entry.controllerCode
    };
    if (entry.type == schemaButton) newEvent = (struct event) {
        .type=schemaButton,
        .button.value=deviceValue,
    };
    else if (entry.type == schemaAxisMapDown) newEvent = (struct event) {
        .type=schemaAxis,
        .axis.value=0
    };
    else if (entry.type == schemaAxisMapUp) newEvent = (struct event) {
        .type=schemaAxis,
        .axis.value=0
    };
    controllerBuffer.write->write(controllerBuffer.base, erase &newEvent, sizeof newEvent);
    return true;
}

void registerInput(struct schemaEntry* schema, int schemaLength, write_i buffer, write_i* schemaOut) {
    for (int i = 0; i < schemaLength; i++) {
        struct schemaEntry entry = schema[i];
        char hint[128];
        sscanf(entry.hints, "%s", hint);
        int code;
    }
    //controllers[controllerCount] = buffer;
}

