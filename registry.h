#pragma once

// a description of an object which can be used to retrieve its traits at runtime

// Because registries are usually lazy and bad, but sometimes useful enough to justify:

// uncomment this if you want to use them
// #define registries OK

#ifdef registries

typedef struct {
    // The base object of the registry. For use when the registry is divorced from the original context of the entity it registers.
    void* base;
    int* traitIds;
    void** traitOffsets;
} registry_t;

#endif

// TODO
// I haven't actually written registries yet
