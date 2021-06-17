#include <minlib.h>
#include "compositor.h"
#include "mouse.h"

extern Graphics::Window* window;

int main()
{
    Graphics::Compositor compositor;
    Input::Mouse mouse = Input::Mouse(compositor.m_screenWidth / 2, compositor.m_screenHeight / 2);
    
    while (1)
    {   
        /*
        uint32_t oldX = window->m_X;
        uint32_t oldY = window->m_Y;
        uint32_t oldEndX = oldX + window->m_Width;
        uint32_t oldEndY = oldY + window->m_Height;
        
        window->m_X = mousePosition.x;
        window->m_Y = mousePosition.y;
        window->Redraw();

        uint32_t chosenX = MIN(oldX, window->m_X);
        uint32_t chosenY = MIN(oldY, window->m_Y);
        uint32_t chosenEndX = MAX(oldEndX, window->m_Width + window->m_X);
        uint32_t chosenEndY = MAX(oldEndY, window->m_Height + window->m_Y);
        */

        //compositor.DrawRegion(chosenX, chosenY, chosenEndX - chosenX, chosenEndY - chosenY);
        //Graphics::DrawRegion(0, 0, Graphics::screenWidth, Graphics::screenHeight);
        
        Message message;
        message.data[0] = 'b';
        message.data[1] = 'o';
        message.data[2] = 'b';
        message.data[3] = '\0';
        sendmessage(&message, getpid());
        
        message.data[0] = 'd';
        message.data[1] = 'e';
        message.data[2] = 'd';
        message.data[3] = '\0';
        sendmessage(&message, getpid());
        
        printf("-----------------\n");
        
        memset(&message, 0, sizeof(Message));
        while (getmessage(&message))
        {
            printf("Message recieved from PID %lu: %s\n", message.sourcePID, (const char*)message.data);
        }
        
        printf("-----------------\n");
        
        compositor.DrawMouse(mouse);
    }
    
    return 0;
}
