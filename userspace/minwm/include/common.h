#pragma once
#include <stdint.h>

typedef uint32_t Unit;
typedef uint32_t Colour;

template <typename T>
struct Vector2
{
    T x = 0;
    T y = 0;

    Vector2 operator+(const Vector2& rhs) const { return { x + rhs.x, y + rhs.y }; }
    Vector2 operator-(const Vector2& rhs) const { return { x - rhs.x, y - rhs.y }; }
    Vector2 operator*(const Vector2& rhs) const { return { x * rhs.x, y * rhs.y }; }
    Vector2 operator/(const Vector2& rhs) const { return { x / rhs.x, y / rhs.y }; }

    Vector2 operator+(const Unit& rhs) const { return { x + rhs, y + rhs }; }
    Vector2 operator-(const Unit& rhs) const { return { x - rhs, y - rhs }; }
    Vector2 operator*(const Unit& rhs) const { return { x * rhs, y * rhs }; }
    Vector2 operator/(const Unit& rhs) const { return { x / rhs, y / rhs }; }

    void operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; }
    void operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y; }
    void operator*=(const Vector2& rhs) { x *= rhs.x; y *= rhs.y; }
    void operator/=(const Vector2& rhs) { x /= rhs.x; y /= rhs.y; }

};

typedef Vector2<Unit> Position;
typedef Vector2<Unit> Size;

constexpr Colour to_colour(const uint8_t r, const uint8_t g, const uint8_t b)
{
    return 0xff000000 | Colour(r << 16) | Colour(g << 8) | Colour(b);
}

Colour* framebuffer = (Colour*) 0x30000000;
