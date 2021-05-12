#pragma once
#ifndef MODULE_H
#define MODULE_H

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"

namespace Modules
{
    void Init(const multiboot_info_t* pMultiboot);
    void PostInit();
}

#endif