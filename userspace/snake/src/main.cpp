#include <minlib.h>
#include <client/window.h>
#include <client/drawing.h>
#include <sys/time.h>
#include <assert.h>
#include <stdio.h>

// Window
constexpr Vector2<Unit> screen_size = { 300, 300 };
Window window("Snake", screen_size);
FILE* keyboard;

// Tiles
constexpr int tiles_per_side = 10;
constexpr Unit tile_size = screen_size.x / tiles_per_side;

// Snake
Vector<Position> body;
enum class Direction { Right, Left, Up, Down } direction;
Position tail_position;
bool did_grow = false;
bool did_die = false;

// Food
Position food_position = {};

// Colours
constexpr uint32_t snake_colour = 0xff00ff00;
constexpr uint32_t food_colour = 0xffff0000;

void init_draw();
void draw();
void input();
void advance();
void spawn_food();
bool game_over();

int main()
{
    keyboard = fopen("keyboard", "r");
    assert(keyboard);

    body.push(new Position(0, 0));
    spawn_food();
    init_draw();

    for (;;)
    {
        // Process input
        input();

        if (!game_over())
        {
            // Game logic
            advance();
            draw();
        }
        else if (!did_die)
        {
            window.set_title("Game over!");
            did_die = true;
            break;
        }

        // Sleep - TODO: ammend if/when sleep syscall added
        const auto get_ms = []()
        {
            struct timeval val;
            gettimeofday(&val, NULL);
            return val.tv_sec * 1000 + val.tv_usec / 1000;
        };
        auto ms = get_ms();
        while (get_ms() - ms < 15)
            input();
    }

    volatile int hang = 1;
    while(hang) {}

    fclose(keyboard);
    return 0;
}

void init_draw()
{
    // Only need to draw all the tiles at the start of the game :)
    for (int y = 0; y < tiles_per_side; ++y)
    {
        for (int x = 0; x < tiles_per_side; ++x)
        {
            draw_panel(
                window.framebuffer,
                screen_size.x,
                { x * tile_size, y * tile_size },
                { tile_size, tile_size }
            );
        }
    }

    // Food
    draw_panel(
        window.framebuffer,
        screen_size.x,
        food_position * tile_size,
        { tile_size, tile_size },
        true,
        food_colour
    );
}

void draw()
{
    // Old tail
    draw_panel(
        window.framebuffer,
        screen_size.x,
        tail_position * tile_size,
        { tile_size, tile_size },
        true,
        did_grow ? snake_colour : panel_background_colour
    );

    // New head
    draw_panel(
        window.framebuffer,
        screen_size.x,
        *body[body.size()-1] * tile_size,
        { tile_size, tile_size },
        true,
        snake_colour
    );

    // Food
    if (did_grow)
        draw_panel(
            window.framebuffer,
            screen_size.x,
            food_position * tile_size,
            { tile_size, tile_size },
            true,
            food_colour
        );
}

void input()
{
    uint8_t scancodes[128];
    read(fileno(keyboard), &scancodes, 128);
    if (scancodes[17]) direction = Direction::Up;    // W
    if (scancodes[30]) direction = Direction::Left;  // A
    if (scancodes[31]) direction = Direction::Down;  // S
    if (scancodes[32]) direction = Direction::Right; // D
}

void advance()
{
    const auto head_position = *body[body.size()-1];

    // Expand head
    switch (direction)
    {
        case Direction::Left:  body.push(new Position(head_position.x - 1, head_position.y)); break;
        case Direction::Right: body.push(new Position(head_position.x + 1, head_position.y)); break;
        case Direction::Up:    body.push(new Position(head_position.x, head_position.y - 1)); break;
        case Direction::Down:  body.push(new Position(head_position.x, head_position.y + 1)); break;
    }

    // Check for food
    if (*body[body.size()-1] == food_position)
    {
        did_grow = true;
        tail_position = food_position;
        spawn_food();
    }
    else
    {
        // Remove tail
        tail_position = *body[0];
        did_grow = false;
        body.pop(0);
    }
}

void spawn_food()
{
    while (food_position == *body[body.size()-1])
        food_position = {
            (uint32_t)(rand() % tiles_per_side),
            (uint32_t)(rand() % tiles_per_side)
        };
}

bool game_over()
{
    const auto head_position = *body[body.size()-1];

    // Check for self-collision
    for (size_t i = 0; i < body.size()-1; ++i)
        if (body[i]->x == head_position.x && body[i]->y == head_position.y)
            return true;

    // Check for edges of map
    switch (direction)
    {
        case Direction::Left:  return head_position.x == 0;
        case Direction::Right: return head_position.x == tiles_per_side-1;
        case Direction::Up:    return head_position.y == 0;
        case Direction::Down:  return head_position.y == tiles_per_side-1;
    }

    return false;
}
