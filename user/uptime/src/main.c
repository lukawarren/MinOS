#include <stdint.h>
#include <stddef.h>

#include "interrupts/syscall.h"
#include "stdlib.h"
#include "task.h"
#include "events.h"

int main()
{
    uint32_t wm = getProcess("wm");

    TaskEvent event;
    event.id = CREATE_WINDOW_EVENT;
    strncpy((char*)event.data, "uptime", 6);
    pushEvent(wm, &event);

    sysexit();
    return 0;
}