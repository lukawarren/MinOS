#include "cli.h"

#include "io/vga.h"

CLI::CLI(void (*_OnCommand)(char*)) { OnCommand = _OnCommand; }

void CLI::Update(uint8_t scancode)
{
    // Beggining of prompt
    commandBuffer[0] = '$';
    commandBuffer[1] = ' ';
    commandBuffer[MAX_COMMAND_LENGTH-1] = '\0';

    auto PrintPrompt = [&]()
    {
        VGA_ROW = 0;
        VGA_printf(commandBuffer, false);

        // Insert null terminator after prompt to print it seperately
        char lastChar = commandBuffer[2];
        commandBuffer[2] = '\0';
        VGA_ROW = 0;
        VGA_printf(commandBuffer, false, VGA_COLOUR_LIGHT_GREEN);
        commandBuffer[2] = lastChar;
        
        VGA_MoveCursor(bufferCount, VGA_COLUMN);
    };

    if (scancode == '\0') { PrintPrompt(); return; }

    // Backspace
    if (scancode == '\r' && bufferCount > CLI_BEGIN)
    {
        commandBuffer[bufferCount-1] = ' ';
        bufferCount--;
        PrintPrompt();
        return;
    } else if (scancode == '\r') { return; }

    // If newline, it's a command!
    if (scancode == '\n')
    {
        // Execute command
        VGA_printf("");
        OnCommand(commandBuffer);

        // Clear buffer, new line
        for (int i = 2; i < MAX_COMMAND_LENGTH; ++i) commandBuffer[i] = '\0';
        bufferCount = 2;
        VGA_COLUMN++;
        PrintPrompt();
        return;
    }

    // Append command to command buffer if within current line
    if (bufferCount >= VGA_WIDTH) return;
    commandBuffer[bufferCount] = scancode;
    bufferCount++;

    // Print command
    PrintPrompt();
}

CLI::~CLI() { }