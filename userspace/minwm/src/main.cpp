#include "font.h"
#include "compositor.h"
#include "messages.h"
#include "common.h"
#include "minlib.h"
#include "vector.h"
#include "window.h"
#include <string.h>
#include <stdio.h>
#include <sys/time.h> // DO NOT STAGE

constexpr Unit screen_width = 640;
constexpr Unit screen_height = 480;
constexpr Size screen_size = { screen_width, screen_height };

Vector<Window> windows;
int current_window = -1;
Compositor c(screen_size);

void poll_messages();
void switch_to_current_window();

int main()
{
    printf("[minwm] starting...\n");
    init_font("Gidole-Regular.sfn", 16);

    for(;;)
    {
        // Deal with messages
        poll_messages();

        // Re-draw window contents
        if (current_window != -1)
            c.redraw_window(windows[current_window]);
    }

    free_font();
    return 0;
}

void poll_messages()
{
    Message message;
    while (get_messages(&message, 1))
    {
        const int id = *(int*)message.data;

        if (id == CREATE_WINOW_MESSAGE)
        {
            const auto* m = (CreateWindowMessage*)&message;

            const Size size = { m->width, m->height };
            const Position position = screen_size/2 - size/2;

            // Create and centre window
            Window* window = new Window(m->title, position, m->framebuffer, size);
            window->position = screen_size / 2 - size / 2;

            // Make sure it's not too big
            if ((window->position + window->size()).x >= screen_size.x ||
                (window->position + window->size()).y >= screen_size.y)
            {
                delete window;
                continue;
            }

            // Push to compositor
            windows.push(window);
            current_window = windows.size()-1;
            switch_to_current_window();
        }

        else if (id == SWITCH_WINOW_MESSAGE)
        {
            if (windows.size() == 0) return;
            ++current_window %= windows.size();
            switch_to_current_window();
        }

        else printf("[minwm] unknown message %d\n", id);
    }
}

void switch_to_current_window()
{
    auto* window = windows[current_window];
    char message[255];
    snprintf(message, 255, "Workspace %d / %d - %s", current_window+1, windows.size(), window->title);
    c.display_window(window);
    c.display_bar(message);
}