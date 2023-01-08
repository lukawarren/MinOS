#include "font.h"
#include "compositor.h"
#include "client/messages.h"
#include "client/types.h"
#include "minlib.h"
#include "vector.h"
#include "window.h"
#include <string.h>
#include <stdio.h>

constexpr Unit screen_width = 1280;
constexpr Unit screen_height = 720;
constexpr Size screen_size = { screen_width, screen_height };

struct Workspace
{
    Vector<Window> windows;
    Optional<size_t> current_window;
};

Vector<Workspace> workspaces;
size_t current_workspace;

Compositor c(screen_size);

void poll_messages();
void switch_to_current_window();
void draw_bar_for_current_workspace();

int main()
{
    printf("[minwm] starting...\n");
    init_font("Perfect-DOS-VGA-437.sfn", 13);

    // Initial workspace
    workspaces.push(new Workspace());
    current_workspace = 0;
    draw_bar_for_current_workspace();

    for(;;)
    {
        // Deal with messages
        poll_messages();

        // Re-draw window contents
        auto workspace = workspaces[current_workspace];
        workspace->windows.for_each([](Window* w) {
            c.redraw_window_framebuffer(w);
        });
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
        auto workspace = workspaces[current_workspace];

        if (id == CREATE_WINOW_MESSAGE)
        {
            const auto* m = (CreateWindowMessage*)&message;

            const Size size = { m->width, m->height };
            Position position = screen_size/2 - size/2;

            static int bob = 0;
            if (bob++ == 0)
            {
                position = Position { 100, 100 };
            }
            else if (bob++ == 2)
            {
                position = Position { 500, 720/2 - size.y/2 };
            }

            // Create and centre window
            Window* window = new Window(m->title, position, m->framebuffer, size, m->pid);

            // Make sure it's not too big
            if ((window->position + window->size()).x >= screen_size.x ||
                (window->position + window->size()).y >= screen_size.y)
            {
                printf("[minwm] failed to create window for PID %d - resolution too large\n", m->pid);
                delete window;
                continue;
            }

            // Push to compositor
            workspace->windows.push(window);
            workspace->current_window = workspace->windows.size()-1;
            switch_to_current_window();
        }

        else if (id == SWITCH_WINOW_MESSAGE)
        {
            if (workspace->windows.size() == 0) return;

            if (!workspace->current_window.contains_data)
                workspace->current_window = 0;

            ++(workspace->current_window.data) %= workspace->windows.size();
            switch_to_current_window();
        }

        else if (id == SET_WINDOW_TITLE_MESSAGE)
        {
            const auto* m = (SetWindowTitleMessage*)&message;
            for (size_t w = 0; w < workspaces.size(); ++w)
            {
                for (size_t i = 0; i < workspaces[w]->windows.size(); ++i)
                {
                    // Assume 1 window per process
                    if (workspaces[w]->windows[i]->pid == m->pid)
                        strncpy(workspaces[w]->windows[i]->title, m->title, sizeof(workspaces[w]->windows[i]->title));

                    // Redraw if visible
                    if (current_workspace == w)
                        c.redraw_window_bar(workspace->windows[i]);
                }
            }
        }

        else printf("[minwm] unknown message %d\n", id);
    }
}

void switch_to_current_window()
{
    c.display_window(workspaces[current_workspace]->windows[
        *workspaces[current_workspace]->current_window
    ]);
}

void draw_bar_for_current_workspace()
{
    char message[255];
    snprintf(message, 255, "Workspace %d / %d", current_workspace+1, workspaces.size());
    c.display_bar(message);
}
