#include "multitask/syscalls.h"
#include "multitask/multitask.h"
#include "stdout/uart.h"

namespace Multitask
{
    static void _exit(Interrupts::StackFrameRegisters sRegisters); 

    void OnSyscall(Interrupts::StackFrameRegisters sRegisters)
    {
        // Get syscall id
        const uint32_t id = sRegisters.eax;

        switch (id)
        {
            case 0:
                _exit(sRegisters);
            break;

            default:
                UART::WriteString("[Syscall] Unexpected syscall ");
                UART::WriteNumber(id);
                UART::WriteString("\n");
        }
    }

    static void _exit(Interrupts::StackFrameRegisters sRegisters)
    {
        UART::WriteString("[Syscall] Task ");
        UART::WriteString(GetCurrentTask().m_sName);
        UART::WriteString(" exited with code ");
        UART::WriteNumber(sRegisters.ebx);
        UART::WriteString("\n");
        
        RemoveCurrentTask();
        OnPIT();
        bSwitchTasks = true;
    }

}