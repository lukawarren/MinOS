#ifndef MESSAGE_H
#define MESSAGE_H

struct sMessage // 256 bytes
{
    uint32_t sourcePID;
    uint8_t data[252];
};

typedef struct sMessage Message;

#endif
