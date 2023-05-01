#pragma once
#include "klib.h"

namespace uart
{
    void init();

    void write_char(const char c);
    void write_string(const char* string);
    void write_number(const uint64_t number);

    Optional<char> read_char();
}