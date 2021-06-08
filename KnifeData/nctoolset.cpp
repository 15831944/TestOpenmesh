#include "nctoolset.h"
#include "assert.h"

NCToolSet::NCToolSet()
{

}

size_t NCToolSet::Size()
{
    return m_toolSet.size();
}

void NCToolSet::Add(ToolDataSPtr pKnife)
{
    if(pKnife)
    {
        m_toolSet.push_back(pKnife);
    }
}

void NCToolSet::Remove(ToolDataSPtr pKnife)
{
    for(auto it = m_toolSet.begin(); it != m_toolSet.end(); ++it)
    {
        if(*it == pKnife)
        {
            m_toolSet.erase(it);
        }
    }
}

void NCToolSet::Set(size_t index, ToolDataSPtr pKnife)
{
    assert(index < m_toolSet.size());
    m_toolSet[index] = pKnife;
}

ToolDataSPtr NCToolSet::Remove(int index)
{
    auto it = m_toolSet.erase(m_toolSet.begin() + index);
    return *it;
}

ToolDataSPtr NCToolSet::GetKnife(size_t index)
{
    assert(index < m_toolSet.size());
    return m_toolSet[index];
}
