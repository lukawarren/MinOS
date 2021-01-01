#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <stddef.h>
#include <stdint.h>

#define DO_SOUND_DEMO true

void OnTimerInterrupt();
uint32_t GetSeconds();
uint32_t GetSubseconds();

#endif