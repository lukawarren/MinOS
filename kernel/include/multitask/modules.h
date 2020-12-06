#pragma once
#ifndef MODULES_H
#define MODULES_H

#include <stdint.h>
#include <stddef.h>

#include "../multiboot.h"

void MoveGrubModules(multiboot_info_t* pMultiboot);
void LoadGrubModules(multiboot_info_t* pMultiboot);

#endif