#include "tooldata.h"
#include "cassert"

ToolData::ToolData(const QString &knifeName, size_t dataNum)
    : m_knifeName(knifeName)
    , m_data(dataNum)
{
}

ToolData::~ToolData()
{

}

QString ToolData::GetKnifeName()
{
    return m_knifeName;
}

size_t ToolData::Size()
{
    return m_data.size();
}

QString ToolData::GetValue(size_t i)
{
    assert(i < m_data.size());
    return m_data[i];
}

void ToolData::SetValue(size_t i, const QString &value)
{
    assert(i < m_data.size());
    m_data[i] = value;
}

void ToolData::GetData(std::vector<QString> &data)
{
    data = m_data;
}

void ToolData::SetData(std::vector<QString> &data)
{
    m_data = data;
}

void ToolData::SaveToBinary(std::ofstream &outf)
{

}

void ToolData::LoadFromBinary(std::ifstream &inf)
{

}

void ToolData::SetKnifeName(const QString &knifeName)
{
    m_knifeName = knifeName;
}
