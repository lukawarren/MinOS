#pragma once
#ifndef STRING_H
#define STRING_H

class String
{
public:
    String(char const* text)
    {
        m_nChars = strlen(text);
        m_text = (char*) malloc(sizeof(char) * m_nChars);
        strcpy(m_text, text);
    }
    
    char operator[](unsigned int index) const
    {
        return m_text[index];
    }
    
    size_t Length() const
    {
        return m_nChars;
    }
    
    ~String()
    {
        free(m_text);
    }
    
private:
    char* m_text;
    size_t m_nChars;
};

#endif
