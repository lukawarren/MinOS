#pragma once
#ifndef MINLIB_VECTOR_H
#define MINLIB_VECTOR_H

#include <stdlib.h>
#include <stddef.h>

template<typename T>
class Vector
{
public:
    Vector(const size_t nElements = 0) : m_nElements(nElements)
    {
        // Pick size
        if (nElements > 0) m_nMaxElements = m_nElements;
        else m_nMaxElements = 2;
        
        // Allocate memory
        m_pData = (T**) malloc(sizeof(T*) * m_nMaxElements);
    }
    
    void Push(const T* value)
    {
        // If memory remains, copy over into free space
        if (m_nElements < m_nMaxElements)
        {
            memcpy(m_pData + m_nElements, &value, sizeof(T*));
            m_nElements++;
        }
        
        else
        {
            // Else allocate new memory (doubling the size)
            T** newData = (T**) malloc(sizeof(T*) * m_nElements * 2);
            m_nMaxElements = m_nElements * 2;
            
            // Copy everything over
            memcpy(newData, m_pData, sizeof(T*) * m_nElements);
            
            // Clean up old memory
            free(m_pData);
            m_pData = newData;
            
            // Add new data
            Push(value);
        }
    }
    
    T* operator[](unsigned int index)
    {
        return m_pData[index];
    }  
    
    constexpr size_t Length() const { return m_nElements; }
    
    ~Vector()
    {
        free(m_pData);
    }
    
private:
    T** m_pData = nullptr;
    size_t m_nElements;
    size_t m_nMaxElements;
};

#endif
