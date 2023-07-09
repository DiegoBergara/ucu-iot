typedef struct {
    uint16_t value;
    char* timestamp;
} CircularBufferElement;

typedef struct {
    CircularBufferElement data[100];
    uint16_t checksum;
    int front;
    int rear;
    int count;
    int sent;
} CircularBuffer;

CircularBuffer* createBuffer();

void destroyBuffer(CircularBuffer* buffer);

int isBufferEmpty(CircularBuffer* buffer);

int isBufferFull(CircularBuffer* buffer);

void updateChecksum(CircularBuffer* buffer);

void insertElement(CircularBuffer* buffer, CircularBufferElement element);

bool isBufferCorrupted(CircularBuffer* buffer);

void saveBufferToFlash(CircularBuffer* buffer);

CircularBuffer* loadBufferFromFlash();