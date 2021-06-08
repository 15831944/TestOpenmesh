#include "NCAction.h"



//=============== NCPosition ================================
void NCPosition::clear()
{
    m_pos.clear();
}

void NCPosition::setPos(NCAxis axis, double value)
{
    setPos(NCAxes[axis], value);
}

void NCPosition::setPos(char key, double value)
{
    size_t index = npos;
    if((index = find(key)) != npos)
    {
        m_pos[index].second = value;
    }
}

double NCPosition::value(char key)
{
    size_t index = npos;
    if((index = find(key)) != npos)
    {
        return m_pos[index].second;
    }
    else return NAN;
}

double NCPosition::value(NCAxis axis)
{
    return value(NCAxes[axis]);
}

double &NCPosition::operator[](char key)
{
    size_t index = npos;
    if((index = find(key)) == npos)
    {
        NCAxisPos pair = make_pair(key, 0);
        m_pos.push_back(pair);
    }
    else
    {
        return m_pos[index].second;
    }

    return m_pos.back().second;
}

size_t NCPosition::find(char key)
{
    for(size_t i = 0; i < m_pos.size(); ++i)
    {
        if(m_pos[i].first == key)
        {
            return i;
        }
    }
    return npos;
}

size_t NCPosition::find(NCAxis axis)
{
    return find(NCAxes[axis]);
}
