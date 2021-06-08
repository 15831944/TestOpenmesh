#ifndef KNIFEDATA_H
#define KNIFEDATA_H

#include <QString>
#include <vector>
#include <QDataStream>

class ToolData
{
public:
    ToolData(const QString &knifeName, size_t dataNum);
    virtual ~ToolData();

    virtual QString GetDes(size_t i) = 0;
    virtual void GetDataDes(std::vector<QString> &dataDes) = 0;

    QString GetKnifeName();

    virtual void Draw() = 0;

    bool isVisiable() {return m_bVisiable;}
    void SetVisiable(bool isVisiable) { m_bVisiable = isVisiable;}

    size_t Size();
    QString GetValue(size_t i);
    void SetValue(size_t i, const QString &value);

    void GetData(std::vector<QString> &data);
    void SetData(std::vector<QString> &data);

    //TODO 按照二进制保存至文件中
    void SaveToBinary(std::ofstream &outf);
    //TODO 二进制文件读入
    void LoadFromBinary(std::ifstream &inf);

protected:
    void SetKnifeName(const QString& knifeName);

protected:
    QString m_knifeName;
    std::vector<QString> m_data;
    bool m_bVisiable = false;
};

using ToolDataSPtr = QSharedPointer<ToolData>;

#endif // KNIFEDATA_H
