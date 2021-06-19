#include <minlib.h>
#include "events.h"

constexpr unsigned int nWidth = 600;
constexpr unsigned int nHeight = 400;
constexpr unsigned int nPadding = 5;

int main()
{
    eWindowCreate(nWidth, nHeight, "Notepad");
    ePanelCreate(nWidth-nPadding*2, nHeight-nPadding*2, nPadding, nPadding, 0xffaaaaaa);

    while(1) {}
    
    return 0;
}
