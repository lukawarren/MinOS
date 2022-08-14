#pragma once
#include "klib.h"

namespace uart
{
    void init();
    void write_char(const char c);
    void write_string(const char* string);
    void write_number(const uint32_t number);
}