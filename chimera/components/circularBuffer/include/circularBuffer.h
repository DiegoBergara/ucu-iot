typedef struct {
    uint16_t data[100];
    uint16_t checksum;
    int front;
    int rear;
    int count;
} CircularBuffer;

CircularBuffer* createBuffer();

void destroyBuffer(CircularBuffer* buffer);

int isBufferEmpty(CircularBuffer* buffer);

int isBufferFull(CircularBuffer* buffer);

void updateChecksum(CircularBuffer* buffer);

void insertElement(CircularBuffer* buffer, uint16_t element);

uint16_t getLatestElement(CircularBuffer* buffer);

uint16_t removeLatestElement(CircularBuffer* buffer);

bool isBufferCorrupted(CircularBuffer* buffer);

void saveBufferToFlash(CircularBuffer* buffer);

CircularBuffer* loadBufferFromFlash();