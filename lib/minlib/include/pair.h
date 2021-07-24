#pragma once
#ifndef MINLIB_PAIR_H
#define MINLIB_PAIR_H

#include <stdlib.h>
#include <stddef.h>

#include "pair.h"

template<typename A, typename B>
class Pair
{
public:
    Pair() {}
    ~Pair() {}
    
    Pair(A a, B b)
    {
        m_first = a;
        m_second = b;
    }
    
    A m_first;
    B m_second;
};

#endif
