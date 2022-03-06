#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cstdio>

#include "inputbuf.h"

using namespace std;

bool InputBuffer::EndOfInput()
{
    if (!input_buffer.empty())
        return false;
    else
        return cin.eof();
}

char InputBuffer::UngetChar(char ch)
{
    if (ch != EOF)
        input_buffer.push_back(ch);
    ;
    return ch;
}

void InputBuffer::GetChar(char &ch)
{
    if (!input_buffer.empty())
    {
        ch = input_buffer.back();
        input_buffer.pop_back();
    }
    else
    {
        cin.get(ch);
    }
}

string InputBuffer::UngetString(string s)
{
    for (int i = 0; i < s.size(); i++)
        input_buffer.push_back(s[s.size() - i - 1]);
    return s;
}
