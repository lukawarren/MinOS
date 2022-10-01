#pragma once
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

/*
    This vector is very quick and dirty.
    It should be stressed that elements
    are not necessarily contiguous in memory,
    making this more akin to a linked list!
*/
template<typename T>
class Vector
{
public:
    Vector(const size_t elements = 0) : elements(0)
    {
        // Pick size
        if (elements > 0) max_elements = elements;
        else max_elements = 2;

        // Allocate memory
        data = (T**) malloc(sizeof(T*) * max_elements);
    }

    void push(const T* value)
    {
        // If memory remains, copy over into free space
        if (elements < max_elements)
        {
            memcpy(data + elements, &value, sizeof(T*));
            elements++;
        }

        else
        {
            // Else allocate new memory (doubling the size)
            T** newData = (T**) malloc(sizeof(T*) * elements * 2);
            max_elements = elements * 2;

            // Copy everything over
            memcpy(newData, data, sizeof(T*) * elements);

            // Clean up old memory (but not the underlying owned objects)
            free(data);
            data = newData;

            // Add new data
            push(value);
        }
    }

    void pop(T* element)
    {
        auto index = get_index(element);

        // Free memory
        delete data[index];

        // Shift all elements above down by one
        for (size_t i = index; i < size(); ++i)
            data[i] = data[i+1];

        elements--;
    }

    void replace(T* element, T* newElement)
    {
        auto index = get_index(element);

        // Free memory
        delete data[index];

        // Set new elements in its place
        data[index] = newElement;
    }

    size_t get_index(T* element)
    {
        for (size_t i = 0; i < size(); ++i)
            if (data[i] == element)
                return i;

        assert(false);
        return 0xdeadbeef;
    }

    void clear()
    {
        for (size_t i = 0; i < size(); ++i) delete data[i];
        elements = 0;
    }

    T* operator[](unsigned int index) const
    {
        return data[index];
    }

    constexpr size_t size() const { return elements; }

    void for_each(void (*on_each)(T*))
    {
        for (size_t i = 0; i < size(); ++i)
            on_each(data[i]);
    }

    ~Vector()
    {
        for (size_t i = 0; i < size(); ++i) delete data[i];
        free(data);
    }

private:
    T** data = nullptr;
    size_t elements;
    size_t max_elements;
};
