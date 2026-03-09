enum controlType {
    startMove,
    stopMove,
    jump,
    startCrouch,
    stopCrouch,
};

typedef struct {
    enum controlType type;
    int length;
    uint8_t data[];
} control_t;

typedef struct {
    int xAxis;
    int yAxis;
    int xPointer;
    int yPointer;
    bool action1;
    bool action2;
} inputStruct_t;

