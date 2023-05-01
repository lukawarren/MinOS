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
constexpr Unit bottom_bar_height = 32;

struct Workspace
{
    Vector<Window> windows;
    Optional<size_t> current_window;
};

Vector<Workspace> workspaces;
size_t current_workspace;

Compositor c(screen_size, bottom_bar_height);

// Event loop
void poll_messages();
void switch_to_current_window();

// Window placement
const Unit min_window_margin = 10;
Unit total_window_width(const Vector<Window>& windows);
bool new_workspace_needed();
Position get_new_window_position(const Size new_window_size);

// Drawing
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

            // Work out new position - may mutate current_workspace :)
            const Size size = { m->width, m->height };
            const Position position = get_new_window_position(size);
            workspace = workspaces[current_workspace];

            // Create window
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

        else if (id == SWITCH_WINDOW_MESSAGE)
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

Unit total_window_width(const Vector<Window>& windows)
{
    Unit total_width = 0;
    for (size_t i = 0; i < windows.size(); ++i)
        total_width += windows[i]->size().x;
    return total_width;
}

// Determines if the current workspace has space for the new window
bool new_workspace_needed()
{
    const auto& windows = workspaces[current_workspace]->windows;
    return (total_window_width(windows) + min_window_margin * (windows.size()+1) >=
            screen_size.x);
}

Position get_new_window_position(const Size new_window_size)
{
    const auto& windows = workspaces[current_workspace]->windows;
    const bool new_workspace = new_workspace_needed();
    const auto centre_height = [&]() {
        return (screen_size.y - bar_height) / 2 - new_window_size.y / 2;
    };

    if (new_workspace || windows.size() == 0)
    {
        if (new_workspace)
        {
            current_workspace++;
            workspaces.push(new Workspace());
            c.blit_background();
            draw_bar_for_current_workspace();
        }

        return Position {
            screen_size.x / 2 - new_window_size.x / 2,
            centre_height()
        };
    }
    else
    {
        // Work out how much margin to have on side of all the windows
        const Unit total_width = total_window_width(windows) + new_window_size.x;
        const Unit margin = (screen_size.x - total_width) / (windows.size()+2);
        Unit consumed_width = margin;

        for (size_t i = 0; i < windows.size(); ++i)
        {
            windows[i]->position.x = consumed_width;
            consumed_width += windows[i]->size().x;
            consumed_width += margin;
        }

        // Re-draw all affected windows (new window hasn't been drawn yet)
        c.blit_background();
        draw_bar_for_current_workspace();
        for (size_t i = 0; i < windows.size(); ++i)
        {
            c.redraw_window(windows[i]);
        }

        return { consumed_width, centre_height() };
    }
}

void draw_bar_for_current_workspace()
{
    char message[255];
    snprintf(message, 255, "Workspace %d / %d", current_workspace+1, workspaces.size());
    c.display_bar(message);
}
