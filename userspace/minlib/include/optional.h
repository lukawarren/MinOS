/*
    Copied from implementation in klib.h
*/

template<typename T>
struct Optional
{
    T data;
    bool contains_data;

    Optional(T _data) : data(_data), contains_data(true) {}
    Optional() : data(), contains_data(false) {}

    T operator*() const
    {
        assert(contains_data);
        return data;
    }

    const T* operator->() const
    {
        assert(contains_data);
        return &data;
    }

    bool operator!() const
    {
        return !contains_data;
    }

    T value_or(T or_value) const
    {
        if (contains_data) return data;
        return or_value;
    }
};