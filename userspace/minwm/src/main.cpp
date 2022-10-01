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
Compositor c(screen_size);

void poll_messages();

int main()
{
    printf("[minwm] starting...\n");
    init_font("Gidole-Regular.sfn", 16);

    for(;;)
    {
        // Deal with messages
        poll_messages();

        // Re-draw window contents
        windows.for_each([](auto* w) {
            c.redraw_window(w);
        });

        struct timeval val;
        gettimeofday(&val, NULL);
        static int bob = 0;
        if (val.tv_sec == 1 && !bob)
        {
            bob = true;
            c.move_window(windows[0], { windows[0]->position.x + 50, windows[0]->position.y });
        }
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

            Window* window = new Window(m->title, position, m->framebuffer, size);
            windows.push(window);
            c.display_window(windows[0]);
        }

        else printf("[minwm] unknown message %d\n", id);
    }
}

