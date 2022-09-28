#pragma once
#include <stdint.h>

typedef uint32_t Unit;
typedef uint32_t Colour;

template <typename T>
struct Vector
{
    T x = 0;
    T y = 0;

    Vector operator+(const Vector& rhs) const { return { x + rhs.x, y + rhs.y }; }
    Vector operator-(const Vector& rhs) const { return { x - rhs.x, y - rhs.y }; }
    Vector operator*(const Vector& rhs) const { return { x * rhs.x, y * rhs.y }; }
    Vector operator/(const Vector& rhs) const { return { x / rhs.x, y / rhs.y }; }

    Vector operator+(const Unit& rhs) const { return { x + rhs, y + rhs }; }
    Vector operator-(const Unit& rhs) const { return { x - rhs, y - rhs }; }
    Vector operator*(const Unit& rhs) const { return { x * rhs, y * rhs }; }
    Vector operator/(const Unit& rhs) const { return { x / rhs, y / rhs }; }

    void operator+=(const Vector& rhs) { x += rhs.x; y += rhs.y; }
    void operator-=(const Vector& rhs) { x -= rhs.x; y -= rhs.y; }
    void operator*=(const Vector& rhs) { x *= rhs.x; y *= rhs.y; }
    void operator/=(const Vector& rhs) { x /= rhs.x; y /= rhs.y; }

};

typedef Vector<Unit> Position;
typedef Vector<Unit> Size;

constexpr Colour to_colour(const uint8_t r, const uint8_t g, const uint8_t b)
{
    return 0xff000000 | Colour(r << 16) | Colour(g << 8) | Colour(b);
}

Colour* framebuffer = (Colour*) 0x30000000;
