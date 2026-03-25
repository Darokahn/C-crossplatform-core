#pragma once

#include "libraries/commonInterfaces/interfaces.h"
#include "libraries/table/table.h"
#include <stdint.h>

enum streamType {
    GENERIC,
    STDIN,
    STDOUT,
    OPEN,
    TCP
};

static const uint16_t axisMidpoint = ((int)UINT16_MAX + 1) / 2;
enum inputType {
    schemaButton,
    schemaAxis,
    schemaAxisMapUp, // for mapping a button to the positive direction of an axis
    schemaAxisMapDown, // negative
    // special pre-defined schemas
    WASD,
    UPDOWNLEFTRIGHT,
    XYAB,
    KEYBOARD // Entire scancode range
};

struct schemaEntry {
    enum inputType type;
    uint16_t index;
    char* hints;
};

struct event {
    enum inputType type;
    union {
        struct {
            uint16_t index; // corresponds to the value specified at register-time
            bool value; // 0 for UP; 1 for DOWN.
        } button;
        struct {
            uint16_t index;
            uint16_t value; // (0 - 65535)
        } axis;
        struct {
            uint16_t index;
            uint16_t value;
        } generic;
    };
};

typedef struct {
    uint8_t type;               // which type of input (button, axis, ...)
    uint8_t controllerIndex;    // which controller this input corresponds to
    uint8_t controllerCode;     // the code the controller expects from this input
    uint8_t deviceCode;         // which slot in its device it belongs to
} fullInput_t;

typedef struct {
    uint8_t type;
    uint8_t controllerCode;
} input_t;

typedef struct {
    uint16_t inputCount;
    uint8_t thisMode;
} inputHeader_t;

typedef struct {
    bool complexDevice;                  // If true, this struct is embedded inside a `complexDevice_t`.
    int deviceType;
    table_t hints;
    int inputCount;                     // the actual size of the flat lookup table
    int inputCapacity;                  // how many bytes are available to expand `entries` into
    input_t entries[];                  // caution: invalid if complexDevice is true.
} device_t;

typedef struct {
    device_t device;
    int virtualSize;                    // the range of input codes handled by this device
    int startIndex;                     // the beginning of the actual handled range (virtualRange[startIndex] = entries[hotTableIndex])
    int hotTableIndex;                  // index to the inputs that are placed sparsely as a lookup table
                                        // entries[0..hotTableIndex] (not inclusive) is the sparse, packed memory for input structs. A run of inputs starts with a header describing how many entries that run spans.
                                        // entries[hotTableIndex..hotTableIndex+actualSize] is a flat lookup table.
                                        // when modes are switched, a run of entries from somewhere in [0..hotTableIndex] is copied into [hotTableIndex..hotTableIndex+actualSize] so that they can be looked up in O(1).

    union {fullInput_t entry; inputHeader_t header;} entries[];
} complexDevice_t;

bool initInputs();

bool addDevice();

// get a readable interface from the system, with `hint` as the intended identity of interface for the system to provide. System is free to ignore hint or return NULL.
// remaining arguments should be consumed by the hint handler.
readseek_i getInputStream(enum streamType hint, ...);

// get a writable interface from the system, with `hint` as the intended identity of interface for the system to provide.
// A given stream may expect structured packets once opened, which will be entirely up to that stream to interpret.
writeseek_i getOutputStream(enum streamType hint, ...);

// register `buffer` as the recipient for system events described by `schema`. `hints` specifies the hints in priority order as shown in the schema below. Writes schema entries back to `schemaOut` with `hints` pointing inside the string at the accepted hint (or NULL if none).
void registerInput(struct schemaEntry* schema, int schemaLength, write_i buffer, write_i* schemaOut);

bool sendEvent(int id, int code, int value);

/* 
 * hints:
 *      hint ...
 *      hint ..., hints
 *      hint ...,hints
 *
 * hint:
 *      (hints are identifiers among the standard set of input devices. The following are examples, not comprehensive)
 *      generic
 *      joystick1X
 *      joystick1Y
 *      joystick2X
 *      joystick2Y
 *      X
 *      Y
 *      A
 *      B
 *      MLEFT
 *      MRIGHT
 *      MSCROLLBTN
 *      MSCROLL
 *      UP
 *      DOWN
 *      LEFT
 *      RIGHT
 *      W
 *      A
 *      S
 *      D
 *      SELECT
 *      START
 *      cursorX
 *      cursorY
 *
 * ...:
 *      (hints can have arbitrary extra data between their first word and the next comma or end of string)
 */

// Once per update, `buffer` can expect to be handed structs as binary blobs according to the updates the system has to give, via its `write` function. The system may write input events at whatever frequency it likes. structs are guaranteed to be aligned to the union of `struct buttonEvent` and `struct axisEvent` when passed as pointers to `write`.

typedef void (*hintHandler)(char* hint);
hintHandler getHintHandler(char* name);
