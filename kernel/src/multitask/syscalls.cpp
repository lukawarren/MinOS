#include "multitask/syscalls.h"
#include "multitask/multitask.h"
#include "stdout/uart.h"
#include "cpu/pic.h"

namespace Multitask
{
    static void _exit(const Interrupts::StackFrameRegisters sRegisters); 

    int OnSyscall(const Interrupts::StackFrameRegisters sRegisters)
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
                UART::WriteString(" by task ");
                UART::WriteString(Multitask::GetCurrentTask().m_sName);
                UART::WriteString("\n");
                
                RemoveCurrentTask();
                OnTaskSwitch(false);
                Interrupts::bSwitchTasks = true;
            break;
        }

        PIC::EndInterrupt(0x80);
        return 0; // Stops compiler sullying our stack
    }

    static void _exit(const Interrupts::StackFrameRegisters sRegisters)
    {
        UART::WriteString("[Syscall] Task ");
        UART::WriteString(GetCurrentTask().m_sName);
        UART::WriteString(" exited with code ");
        UART::WriteNumber(sRegisters.ebx);
        UART::WriteString("\n");

        RemoveCurrentTask();
        OnTaskSwitch(false);
        Interrupts::bSwitchTasks = true;
    }

}