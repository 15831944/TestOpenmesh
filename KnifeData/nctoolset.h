#ifndef NCKNIFESET_H
#define NCKNIFESET_H

#include "tooldata.h"

#include <QSharedPointer>


class NCToolSet
{

public:
    NCToolSet();

    size_t Size();
    void Add(ToolDataSPtr pKnife);
    void Remove(ToolDataSPtr pKnife);
    void Set(size_t index, ToolDataSPtr pKnife);
    ToolDataSPtr Remove(int index);

    ToolDataSPtr GetKnife(size_t index);


    //TODO 导入刀具信息
    void LoadFromBinary(std::ifstream &inf);
    //TODO 保存刀具集合信息
    void SaveToBinary(std::ofstream &outf);

private:
    std::vector<ToolDataSPtr> m_toolSet;
};

using NCKnifeSetSPtr = QSharedPointer<NCToolSet>;
#endif // NCKNIFESET_H
