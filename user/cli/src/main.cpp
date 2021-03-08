#include "interrupts/syscall.h"
#include "stdlib.h"

#include "font.h"

int main();

void ClearScreen();
void DrawChar(char c, bool notDrawingPrompt, const uint32_t x, const uint32_t y);
void DrawChar(char c, bool notDrawingPrompt = true);
int DrawNumber(unsigned int data, const uint32_t x, const uint32_t y);

// Colours
constexpr uint32_t GetColour(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 0xff) { return a << 24 | r << 16 | g << 8 | b; }
constexpr uint32_t TEXT_COLOUR = GetColour(0xff, 0xff, 0xff);
constexpr uint32_t PROMPT_COLOUR = GetColour(0xeb, 0xeb, 0xeb);
constexpr uint32_t BACKGROUND_COLOUR = GetColour(0, 0, 0);
constexpr uint32_t TERMINAL_COLOUR = GetColour(0xaa, 0xaa, 0xaa);
constexpr uint32_t BAR_COLOUR = GetColour(0, 0, 200);

// Background
inline uint32_t GetBackgroundColour(const uint32_t x, const uint32_t y);

// Framebuffer
uint32_t nWidth, nHeight, nRows, nColumns;
uint32_t* pFramebuffer;

// Padding
constexpr uint32_t nBorder = CHAR_WIDTH*3;
constexpr uint32_t nTerminalBorder = 1;
constexpr uint32_t nShadowPixels = 2;

// Text buffer
constexpr uint32_t promptWidth = 2;
char textBuffer[1024]; // (good enough)
size_t nCharacter = 0;
size_t nVisualCharacter = 0;
size_t nVisualRow = 0;

// Running commands
bool bInCommand = false;
uint32_t pid;

// Recieving and handling commands
void HandleSpecialKeys(char character);
void HandleRegularKeys(char character);
void OnCommand();
void OnCommandFinish();
void DrawPrompt();

// Top bar
constexpr uint32_t barPadding = 2;
constexpr uint32_t barHeight = CHAR_HEIGHT*2 + barPadding*2;
void DrawTopBar();

int main()
{
    // Subscribe to events
    subscribeToStdout(true);
    subscribeToSysexit(true);
    subscribeToKeyboard(true);
    printf("Starting window manager...\n");

    // Screen dimensions and adress
    nWidth = getFramebufferWidth(); nHeight = getFramebufferHeight();
    nRows = (nHeight-nBorder*2) / (CHAR_HEIGHT*CHAR_SCALE) - 1; nColumns = (nWidth-nBorder*2) / CHAR_WIDTH;
    nColumns -= promptWidth;
    pFramebuffer = (uint32_t*)getFramebufferAddr();

    // Clear screen
    ClearScreen();
    DrawPrompt();

    while(1)
    {
        // Deal with events
        TaskEvent* event = getNextEvent();
        while (event != nullptr)
        {
            // Key input
            if (event->id == EVENT_QUEUE_KEY_PRESS)
            {
                const char character = event->data[0];
                const bool special = event->data[1];

                // Delegate input
                if (special) HandleSpecialKeys(character);
                else if (!bInCommand && character == '\n') OnCommand();
                else if (!bInCommand && nCharacter < sizeof(textBuffer) / sizeof(textBuffer[0]) - 1) HandleRegularKeys(character);
            }

            // Stdout
            if (event->id == EVENT_QUEUE_PRINTF && bInCommand)
            {
                const char* message = (const char*)event->data;
                size_t len = strlen(message);
                for (size_t i = 0; i < len; ++i)
                {
                    if (message[i] == '\n') { nVisualRow++; nVisualCharacter = 0; }
                    else
                    {
                        DrawChar(message[i], false);
                        nVisualCharacter++;

                        if (nVisualCharacter >= nColumns)
                        {
                            nVisualCharacter = 0;
                            nVisualRow++;

                            if (nVisualRow >= nRows)
                            {
                                nVisualRow = 0;
                                ClearScreen();
                            }
                        }
                    }
                }
            }

            // Sysexits
            if (event->id == EVENT_QUEUE_SYSEXIT) OnCommandFinish();

            event = getNextEvent();
        }
        
        DrawTopBar();

        // We *must not* block, as if both processes block, we'll be in drouble trouble!
        //block();
    }

    sysexit();
    return 0;
}

void ClearScreen()
{
    for (uint32_t x = 0; x < nWidth; ++x)
    {
        for (uint32_t y = 0; y < nHeight; ++y)
        {
            *(pFramebuffer + y*nWidth + x) = GetBackgroundColour(x, y);
        }
    }
}

inline uint32_t GetBackgroundColour(const uint32_t x, const uint32_t y)
{
    // Figure out which section we're in
    const bool withinTerminal = x > nBorder && x < nWidth-nBorder && y > nBorder && y < nHeight-nBorder;
    const bool withinTerminalShadow = 
        (x >= nWidth - nBorder && x < nWidth - nBorder + nShadowPixels && y > nBorder + nShadowPixels && y < nHeight - nBorder + nShadowPixels) || // Right side
        (x > nBorder + nShadowPixels && x < nWidth - nBorder + nShadowPixels && y >= nHeight - nBorder && y <= nHeight-nBorder+nShadowPixels); // Bottom side
    
    // Colour appropriately
    if (withinTerminal) return TERMINAL_COLOUR;
    else if (withinTerminalShadow) return TERMINAL_COLOUR - GetColour(50, 50, 50);
    return y < barHeight ? BAR_COLOUR : BACKGROUND_COLOUR;
}

void DrawChar(char c, bool notDrawingPrompt, const uint32_t x, const uint32_t y)
{
    const uint8_t* bitmap = GetFontFromChar(c);

    for (int w = 0; w < CHAR_WIDTH; ++w)
    {
        for (int h = 0; h < CHAR_HEIGHT*CHAR_SCALE; ++h)
        {
            uint8_t mask = 1 << (w);

            size_t xPos = x + w;
            size_t yPos = y + h;
            
            if (bitmap[h/CHAR_SCALE] & mask) *(pFramebuffer + yPos*nWidth+xPos) = notDrawingPrompt ? TEXT_COLOUR : PROMPT_COLOUR; 
            else *(pFramebuffer + yPos*nWidth+xPos) = GetBackgroundColour(xPos, yPos);
        }
    }
}

void DrawChar(char c, bool notDrawingPrompt)
{
    const uint32_t x = nBorder + (nVisualCharacter+nTerminalBorder+promptWidth*notDrawingPrompt)*CHAR_WIDTH;
    const uint32_t y = nBorder + nVisualRow*CHAR_HEIGHT*CHAR_SCALE+CHAR_HEIGHT*nTerminalBorder;
    DrawChar(c, notDrawingPrompt, x, y);
}

int DrawNumber(unsigned int data, const uint32_t x, const uint32_t y)
{
    auto digitToASCII = [](const size_t number) { return (char)('0' + number); };
    auto getNthDigit = [](const size_t number, const size_t digit, const size_t base) { return int((number / pow(base, digit)) % base); };

    // Get number of digits
    size_t nDigits = 0;
    size_t i = data;
    nDigits = 1;
    while (i/=10) nDigits++;
    
    for (size_t d = 0; d < nDigits; ++d)
        DrawChar(digitToASCII(getNthDigit(data, nDigits - d - 1, 10)), false, x + d * CHAR_WIDTH, y);

    return nDigits;
}

void HandleSpecialKeys(char character)
{
    if (!bInCommand && character == KEY_EVENT_BACKSPACE && nCharacter > 0 && nVisualCharacter > 0) // Backspace
    {
        if (nVisualCharacter-- == 0) { nVisualCharacter = nColumns-1; nVisualRow--; }
        nCharacter--;
        textBuffer[nCharacter] = '\0';
        DrawChar(' ');
    }

    if (character == KEY_EVENT_CTRL && bInCommand) // Force quit
    {
        kill(pid);
        OnCommandFinish();
    }
}

void HandleRegularKeys(char character)
{
    // Else draw character and advance
    DrawChar(character);
    textBuffer[nCharacter] = character;

    // Advance cursor
    nCharacter++;
    nVisualCharacter++;
    if (nVisualCharacter >= nColumns-nTerminalBorder-1) 
    {
        nVisualCharacter = 0; nVisualRow++;
        if (nVisualRow >= nRows)
        {
            nVisualRow = 0;
            ClearScreen();
        }
    }
}

void OnCommand()
{
    // Advance cursor
    nCharacter = 0;
    nVisualCharacter = 0;
    nVisualRow++;

    if (nVisualRow >= nRows)
    {
        nVisualRow = 0;
        ClearScreen();
    }

    bInCommand = true;

    // Get command
    pid = loadProgram(textBuffer);
    if ((int32_t)pid == -1)
    {
        const char* errorMessage = "Unknown command";
        for (unsigned int i = 0; i < 15; ++i)
        {
            DrawChar(errorMessage[i]);
            nVisualCharacter++;
        }

        nVisualRow++; // OnCommandFinish() will santitise it

        OnCommandFinish();
    }
}

void OnCommandFinish()
{
    bInCommand = false;

    // Advance cursor
    nVisualCharacter = 0;
    nCharacter = 0;
    //nVisualRow++;
       
    if (nVisualRow >= nRows)
    {
        nVisualRow = 0;
        ClearScreen();
    }

    DrawPrompt();
}

void DrawPrompt()
{
    DrawChar('$', false);
    nVisualCharacter++;
    DrawChar(' ', false);
    nVisualCharacter--;

    // Also reset text buffer
    memset(textBuffer, '\0', sizeof(textBuffer)/sizeof(textBuffer[0]));
}

void DrawTopBar()
{
    uint32_t x = barPadding;

    // Uptime
    const char* sUptime = "Uptime: ";
    for (size_t i = 0; i < strlen(sUptime); ++i) DrawChar(sUptime[i], true, barPadding + CHAR_WIDTH*i, barPadding);
    x = barPadding + CHAR_WIDTH*strlen(sUptime);
    x += DrawNumber(getSeconds(), x, barPadding) * CHAR_WIDTH;

    // Pages
    const char* sPages = ", pages: ";
    for (size_t i = 0; i < strlen(sPages); ++i) DrawChar(sPages[i], true, x + CHAR_WIDTH*i, barPadding);
    x += CHAR_WIDTH*strlen(sPages);
    x += DrawNumber(nPages(), x, barPadding) * CHAR_WIDTH;

    // Tasks
    const char* sTasks = ", tasks: ";
    for (size_t i = 0; i < strlen(sTasks); ++i) DrawChar(sTasks[i], true, x + CHAR_WIDTH*i, barPadding);
    x += CHAR_WIDTH*strlen(sTasks);
    x += DrawNumber(nTasks(), x, barPadding) * CHAR_WIDTH;
}
