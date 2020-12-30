#include "multitask.h"
#include "../interrupts/interrupts.h"
#include "../memory/paging.h"
#include "../gfx/vga.h"
#include "stdlib.h"
#include "taskSwitch.h"

bool bEnableMultitasking = false;

// Linked list of tasks
Task* pTaskListHead = nullptr;
Task* pTaskListTail = nullptr;
Task* pCurrentTask = nullptr;
size_t nTasks = 0;

// Ring 0 vs Ring 3
static uint32_t lastUserTaskPages = 0;
static uint32_t processIDCount = 1;

Task* CreateTask(char const* sName, uint32_t entry, uint32_t size, uint32_t location, uint32_t parentID)
{
    // Create new task in memory and linked list
    Task* task = (Task*) kmalloc(sizeof(Task), USER_PAGE);
    task->processID = processIDCount;
    task->parentID = parentID;
    strncpy(task->sName, sName, 32);
    task->bBlocked = false;

    // Round task to nearest page
    uint32_t originalSize = size;
    uint32_t roundedSize = originalSize;
    uint32_t remainder = roundedSize % PAGE_SIZE;
    if (remainder != 0) roundedSize += PAGE_SIZE - remainder;
    task->size = roundedSize;
    task->location = location;

    // Allocate stack
    uint32_t stack = (uint32_t)kmalloc(4096, USER_PAGE, false);
    task->pStack = (uint32_t*)(stack + 4096 - 16); // Stack grows downwards
    uint32_t* pStackTop = task->pStack;
    task->pOriginalStack = (uint32_t*)stack;
    
    // Allocate event queue
    task->pEventQueue = (TaskEventQueue*) kmalloc(sizeof(TaskEventQueue), USER_PAGE, false);
    task->pEventQueue->nEvents = 0;

    // Push blank registers onto the stack
    *--task->pStack = 0x00;   // stack alignment (if any)
    *--task->pStack = 0x23;   // stack segment (ss)
    *--task->pStack = (uint32_t) pStackTop; // esp
    *--task->pStack = 0x202; // eflags - default value with interrupts enabled
    *--task->pStack = 0x1B;    // cs (iret uses a 32-bit pop - don't panic!)
    *--task->pStack = entry;  // eip
    *--task->pStack = 0;      // eax
    *--task->pStack = 0;      // ebx
    *--task->pStack = 0;      // ecx
    *--task->pStack = 0;      // edx
    *--task->pStack = 0;      // esi
    *--task->pStack = 0;      // edi
    *--task->pStack = (uint32_t) pStackTop; // ebp?

    // Segment registers
    *--task->pStack = 0x23; // ds
    *--task->pStack = 0x23; // fs
    *--task->pStack = 0x23; // es
    *--task->pStack = 0x23; // gs

    // SSE, x87 FPU and MMX states - 512 bytes
    for (unsigned int i = 0; i < 512/sizeof(uint32_t); ++i) *--task->pStack = 0;

    // Linked list stuff
    Task* oldHead = pTaskListHead;
    pTaskListHead = task;
    task->pPrevTask = oldHead;
    oldHead->pNextTask = task;
    
    if (pTaskListTail == nullptr) pTaskListTail = task;
    if (task->pNextTask == nullptr) task->pNextTask = pTaskListTail;

    nTasks++;
    processIDCount++;

    return task;
}

Task* CreateChildTask(char const* sName, uint32_t entry, uint32_t size, uint32_t location)
{
    return CreateTask(sName, entry, size, location, pCurrentTask->processID);
}

void EnableScheduler()              { bEnableMultitasking = true; asm volatile("sti"); }
void DisableScheduler()             { bEnableMultitasking = false; }

static void MapNewUserTask(Task* task)
{
    // Unmap current task so its memory can't be read or written to accidentally
    for (uint32_t i = 0; i < lastUserTaskPages; ++i)
    {
        DeallocatePage(0x40000000 + i*PAGE_SIZE);
    } 

    // Setup paging so task begins at 0x40000000
    for (uint32_t i = 0; i < task->size / PAGE_SIZE; ++i)
    {
        AllocatePage(task->location + i * PAGE_SIZE, 0x40000000 + i * PAGE_SIZE, USER_PAGE, false);
    }

    lastUserTaskPages = task->size / PAGE_SIZE;
}

void OnMultitaskPIT()
{
    if (nTasks == 0 || !bEnableMultitasking) { bIRQShouldJump = false; return; }
    
    // If one task, switch to it if nessecary
    if (nTasks == 1 && pCurrentTask == nullptr) 
    {
        pCurrentTask = pTaskListHead;
        oldTaskStack = 0;
        newTaskStack = (uint32_t) &pCurrentTask->pStack;
        MapNewUserTask(pCurrentTask);
        bIRQShouldJump = true; // Will tell the following IRQ 0 to switch tasks
    }
    else if (nTasks > 1)
    {
        // If no task has previously ran, avoid
        // sullying the non-existent "old task"
        if (pCurrentTask == nullptr)
        {
            pCurrentTask = pTaskListTail;
            Task* newTask = pCurrentTask;

            while (newTask->bBlocked && nTasks > 1) newTask = newTask->pNextTask;

            oldTaskStack = 0;
            newTaskStack = (uint32_t) &newTask->pStack;
            MapNewUserTask(newTask);
            bIRQShouldJump = true; // Will tell the following IRQ 0 to switch tasks
        }

        // Otherwise continue cycling
        else
        {
            Task* oldTask = pCurrentTask;
            Task* newTask = pCurrentTask->pNextTask;

            while (newTask->bBlocked && nTasks > 1) newTask = newTask->pNextTask;

            pCurrentTask = newTask;
            oldTaskStack = (uint32_t) &oldTask->pStack;
            newTaskStack = (uint32_t) &newTask->pStack;
            MapNewUserTask(newTask);
            bIRQShouldJump = true; // Will tell the following IRQ 0 to switch tasks
        }
    }
}

uint32_t GetNumberOfTasks()
{
    return nTasks;
}

void TaskExit()
{
    Task* task = pCurrentTask;

    // Case 1: There is one element in the list
    if (nTasks == 1)
    {
        pCurrentTask = nullptr;
        pTaskListHead = nullptr;
        pTaskListTail = nullptr;
    }

    // Case 2: The task is the first one in the list
    else if (task == pTaskListTail)
    {
        pTaskListTail = task->pNextTask;
        pTaskListHead->pNextTask = pTaskListTail;
    }

    // Case 3: The task is the last one in the list
    else if (task == pTaskListHead)
    {
        pTaskListHead = task->pPrevTask;
        pTaskListHead->pNextTask = pTaskListTail;
    }

    // Case 4: The task lies somewhere in the middle
    else
    {
        task->pPrevTask->pNextTask = task->pNextTask;
        task->pNextTask->pPrevTask = task->pPrevTask;
    }

    pCurrentTask = nullptr;

    // Unallocate all memory
    kfree(task->pOriginalStack, 4096); // stack
    if (task->size != 0) kfree((void*)task->location, task->size); // memory
    kfree(task, sizeof(Task)); // task struct
    
    // Switch to new task
    nTasks--;
    bSysexitCall = true;
    OnMultitaskPIT();
}

void TaskGrow(uint32_t size)
{
    pCurrentTask->size += size;
    lastUserTaskPages = pCurrentTask->size / PAGE_SIZE;
}

TaskEvent* GetNextEvent()
{
    if (pCurrentTask->pEventQueue->nEvents == 0) return (TaskEvent*)nullptr;

    // Get bottom event
    memcpy(&pCurrentTask->pEventQueue->returnEventBuffer, &pCurrentTask->pEventQueue->events[0], sizeof(TaskEvent));

    // Shift all events down by one
    for (uint32_t i = 0; i < pCurrentTask->pEventQueue->nEvents; ++i)
    {
        memcpy(&pCurrentTask->pEventQueue->events[i], &pCurrentTask->pEventQueue->events[i+1], sizeof(TaskEvent));
    }

    pCurrentTask->pEventQueue->nEvents--;

    return &pCurrentTask->pEventQueue->returnEventBuffer;
}

static Task* GetTaskWithProcessID(uint32_t id)
{
    if (id == 0) return (Task*)nullptr;

    Task* task = pTaskListTail;
    unsigned int count = 0;
    while (count < nTasks)
    {
        if (task->processID == id) return task;
        task = task->pNextTask;
        count++;
    }

    return (Task*)nullptr;
}

int PushEvent(uint32_t processID, TaskEvent* event)
{

    // Find process in question
    Task* task = GetTaskWithProcessID(processID);
    if (task == nullptr) return -1;

    // Check event queue is not full
    if (task->pEventQueue->nEvents >= MAX_TASK_EVENTS-1) return -1;

    // Push event
    memcpy(&task->pEventQueue->events[task->pEventQueue->nEvents], event, sizeof(TaskEvent));
    task->pEventQueue->events[task->pEventQueue->nEvents].id = event->id;
    task->pEventQueue->events[task->pEventQueue->nEvents].source = pCurrentTask->processID;
    task->pEventQueue->nEvents++;

    // Unblock process
    task->bBlocked = false;

    return 0;
}

int PopLastEvent()
{
    if (pCurrentTask->pEventQueue->nEvents == 0) return -1;

    // Get top event
    TaskEvent* event = &pCurrentTask->pEventQueue->events[pCurrentTask->pEventQueue->nEvents-1];

    // Zero it out
    memset(event, 0, sizeof(TaskEvent));

    // Decrement event counter
    pCurrentTask->pEventQueue->nEvents--;

    return 0;
}

void SubscribeToStdout(bool subscribe)
{
    pCurrentTask->bSubscribeToStdout = subscribe;
}

void OnStdout(const char* message)
{
    // Walk up process tree before a subscriber of stdout is found
    Task* task = GetTaskWithProcessID(pCurrentTask->parentID);
    bool bFound = false;

    while (task != nullptr)
    {
        if (task->bSubscribeToStdout)
        {
            // Found subscriber, dispatch events in the form of 31 chars at a time (plus null terminator)
            for (uint32_t i = 0; i < strlen(message); i+=31)
            {
                TaskEvent event;
                
                event.id = EVENT_QUEUE_PRINTF;
                for (uint32_t c = 0; c < 31; ++c)
                {
                    event.data[c] = message[i+c];
                    if (message[i+c] == '\0') break; // Break if null terminator
                }
                event.data[31] = '\0';

                PushEvent(task->processID, &event);
            }

            bFound = true;
        }
        task = GetTaskWithProcessID(task->parentID);
    }

    // Else, just print to kernel screen
    if (!bFound) VGA_printf(message, false);
}

void OnStdout(uint32_t data, bool hex)
{
    // Get number of digits
    size_t i = data;
    size_t nDigits = 1;
    while (i/=(hex ? 16 : 10)) nDigits++;

    auto digitToASCII = [](const size_t number) { return (char)('0' + number); };
    auto hexToASCII = [](const size_t number) 
    {
        char value = number % 16 + 48;
        if (value > 57) value += 7;
        return value;
    };
    auto getNthDigit = [](const size_t number, const size_t digit, const size_t base) { return int((number / pow(base, digit)) % base); };
   
    if (hex) OnStdout("0x");

    char buffer[2];
    buffer[1] = '\0';

    if (hex) 
    { 
        for (size_t d = 0; d < nDigits; ++d) 
        {
            buffer[0] = (hexToASCII(getNthDigit(data, nDigits - d - 1, 16)));
            OnStdout(buffer);
        }
    }
    else
    {
        for (size_t d = 0; d < nDigits; ++d) 
        {
            buffer[0] = (digitToASCII(getNthDigit(data, nDigits - d - 1, 10)));
            OnStdout(buffer);
        }
    }
}

void SubscribeToSysexit(bool subscribe)
{
    pCurrentTask->bSubscribeToSysexit = subscribe;
}

void OnSysexit()
{
    // Walk up process tree before a subscriber of sysexit is found
    Task* task = GetTaskWithProcessID(pCurrentTask->parentID);

    while (task != nullptr)
    {
        if (task->bSubscribeToSysexit)
        {
            // Found subscriber, dispatch events in the form of 15 chars at a time (plus null terminator)
            TaskEvent event;     
            event.id = EVENT_QUEUE_SYSEXIT;
            PushEvent(task->processID, &event);
        }
        task = GetTaskWithProcessID(task->parentID);
    }
}

void SubscribeToKeyboard(bool subscribe)
{
    pCurrentTask->bSubscribeToKeyboard = subscribe;
}

void OnKeyEvent()
{
    // Walk through each task and sent event if applicable
    unsigned int count = 0;
    Task* task = pTaskListTail;
    while (task != nullptr && count < nTasks)
    {
        if (task->bSubscribeToKeyboard)
        {
            /*
                Only send event if no other key event
                has already been sent to avoid saturating buffer
            */
            bool bAlreadySaturated = false;
            for (unsigned int i = 0; i < task->pEventQueue->nEvents; ++i)
                if (task->pEventQueue->events[i].id == EVENT_QUEUE_KEY_PRESS)
                    bAlreadySaturated = true;

            if (!bAlreadySaturated)
            {
                TaskEvent event;
                event.id = EVENT_QUEUE_KEY_PRESS;
                PushEvent(task->processID, &event);
            }
        }

        task = task->pNextTask;
        ++count;
    }
}

void OnProcessBlock()
{
    pCurrentTask->bBlocked = true;
    OnMultitaskPIT();
}

uint32_t GetProcess(const char* sName)
{
    Task* task = pTaskListTail;
    unsigned int count = 0;
    while (count < nTasks)
    {
        if (strcmp(task->sName, sName)) return task->processID;
        task = task->pNextTask;
        count++;
    }

    return -1;
}