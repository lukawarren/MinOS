#ifndef MESSAGE_H
#define MESSAGE_H

struct sMessage // 32 bytes
{
    uint32_t sourcePID;
    uint8_t data[28];
};

typedef struct sMessage Message;

#endif
