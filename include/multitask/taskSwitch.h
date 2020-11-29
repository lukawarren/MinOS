#pragma once
#ifndef TASK_SWITCH_H
#define TASK_SWITCH_H

#include <stddef.h>
#include <stdint.h>

extern uint32_t oldTaskStack;
extern uint32_t newTaskStack;

extern "C"
{
    extern void PerformTaskSwitch();
}

#endif