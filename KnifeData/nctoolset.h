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


    //TODO ���뵶����Ϣ
    void LoadFromBinary(std::ifstream &inf);
    //TODO ���浶�߼�����Ϣ
    void SaveToBinary(std::ofstream &outf);

private:
    std::vector<ToolDataSPtr> m_toolSet;
};

using NCKnifeSetSPtr = QSharedPointer<NCToolSet>;
#endif // NCKNIFESET_H
