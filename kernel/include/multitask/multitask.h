#pragma once
#ifndef MULTITASK_H
#define MULTITASK_H

#include <stdint.h>
#include <stddef.h>

#include "task.h"

#define MAX_TASK_EVENTS 63-sizeof(TaskEvent)
// ^^^ -1 to account for uint32_t nEvents, and temp event

struct TaskEventQueue
{
    TaskEvent events[MAX_TASK_EVENTS];
    TaskEvent returnEventBuffer;
    uint32_t nEvents = 0;
} __attribute__((packed));

struct Task
{
    char sName[32];
    uint32_t processID;
    uint32_t size;
    uint32_t location;
    uint32_t* pStack;
    uint32_t* pOriginalStack;
    Task* pPrevTask = nullptr;
    Task* pNextTask = nullptr;
    TaskEventQueue* pEventQueue = nullptr;
    bool bSubscribeToStdout = false;
    bool bSubscribeToSysexit = false;
    bool bSubscribeToKeyboard = false;
    uint32_t parentID = 0;
    bool bBlocked = false;
};

void EnableScheduler();
void DisableScheduler();

void OnMultitaskPIT();

uint32_t GetNumberOfTasks();

void TaskExit();

void TaskGrow(uint32_t size);

TaskEvent* GetNextEvent();
int PushEvent(uint32_t processID, TaskEvent* event);
int PopLastEvent();

void SubscribeToStdout(bool subscribe);
void OnStdout(const char* message);
void OnStdout(uint32_t number, bool hex);

void SubscribeToSysexit(bool subscribe);
void OnSysexit();

void SubscribeToKeyboard(bool subscribe);
void OnKeyEvent();

void OnProcessBlock();

uint32_t GetProcess(const char* sName);

enum TaskType
{
    KERNEL_TASK,
    USER_TASK
};

Task* CreateTask(char const* sName, uint32_t entry, uint32_t size = 0, uint32_t location = 0, uint32_t parentID = 0);
Task* CreateChildTask(char const* sName, uint32_t entry, uint32_t size = 0, uint32_t location = 0);

#endif