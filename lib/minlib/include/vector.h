#pragma once
#ifndef MINLIB_VECTOR_H
#define MINLIB_VECTOR_H

#include <stdlib.h>
#include <stddef.h>

template<typename T>
class Vector
{
public:
    Vector(const size_t nElements = 0) : m_nElements(0)
    {
        // Pick size
        if (nElements > 0) m_nMaxElements = nElements;
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
            
            // Clean up old memory (but not the underlying owned objects)
            free(m_pData);
            m_pData = newData;
            
            // Add new data
            Push(value);
        }
    }
    
    void Pop(T* element)
    {
        auto index = GetIndex(element);
        
        // Free memory
        delete m_pData[index];
        
        // Shift all elements above down by one
        for (size_t i = index; i < Length(); ++i)
            m_pData[i] = m_pData[i+1];
        
        m_nElements--;
    }
    
    void Replace(T* element, T* newElement)
    {
        auto index = GetIndex(element);
        
        // Free memory
        delete m_pData[index];
        
        // Set new elements in its place
        m_pData[index] = newElement;
    }
    
    size_t GetIndex(T* element)
    {
        for (size_t i = 0; i < Length(); ++i)
            if (m_pData[i] == element)
                return i;
        
        assert(false);
        return 0xdeadbeef;
    }
    
    void Clear()
    {
        for (size_t i = 0; i < Length(); ++i) delete m_pData[i];
        m_nElements = 0;
    }
    
    T* operator[](unsigned int index) const
    {
        return m_pData[index];
    }  
    
    constexpr size_t Length() const { return m_nElements; }
    
    ~Vector()
    {
        for (size_t i = 0; i < Length(); ++i) delete m_pData[i];
        free(m_pData);
    }
    
private:
    T** m_pData = nullptr;
    size_t m_nElements;
    size_t m_nMaxElements;
};

#endif
