#include "font.h"
#include "compositor.h"
#include "client/messages.h"
#include "client/types.h"
#include "minlib.h"
#include "vector.h"
#include "window.h"
#include <string.h>
#include <stdio.h>

constexpr Unit screen_width = 640;
constexpr Unit screen_height = 480;
constexpr Size screen_size = { screen_width, screen_height };

Vector<Window> windows;
size_t current_window = SIZE_MAX;
Compositor c(screen_size);

void poll_messages();
void switch_to_current_window();
void draw_bar_for_current_window();

int main()
{
    printf("[minwm] starting...\n");
    init_font("Perfect-DOS-VGA-437.sfn", 13);

    for(;;)
    {
        // Deal with messages
        poll_messages();

        // Re-draw window contents
        if (current_window != SIZE_MAX)
            c.redraw_window_framebuffer(windows[current_window]);
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
            Window* window = new Window(m->title, position, m->framebuffer, size, m->pid);
            window->position = screen_size / 2 - size / 2;

            // Make sure it's not too big
            if ((window->position + window->size()).x >= screen_size.x ||
                (window->position + window->size()).y >= screen_size.y)
            {
                printf("[minwm] failed to create window for PID %d - resolution too large\n", m->pid);
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

        else if (id == SET_WINDOW_TITLE_MESSAGE)
        {
            const auto* m = (SetWindowTitleMessage*)&message;
            for (size_t i = 0; i < windows.size(); ++i)
            {
                // Assume 1 window per process
                if (windows[i]->pid == m->pid)
                    strncpy(windows[i]->title, m->title, sizeof(windows[i]->title));

                // Current title changed; redraw
                if (current_window == i)
                {
                    draw_bar_for_current_window();
                    c.redraw_window_bar(windows[i]);
                }
            }
        }

        else printf("[minwm] unknown message %d\n", id);
    }
}

void switch_to_current_window()
{
    c.display_window(windows[current_window]);
    draw_bar_for_current_window();
}

void draw_bar_for_current_window()
{
    auto* window = windows[current_window];
    char message[255];
    snprintf(message, 255, "Workspace %d / %d - %s", current_window+1, windows.size(), window->title);
    c.display_bar(message);
}
