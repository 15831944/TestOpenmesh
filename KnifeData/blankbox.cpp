#include "blankbox.h"

BlankBox::BlankBox(const QString &str)
    :ToolData(str, BOX_END)
{

}

QString BlankBox::GetDes(size_t i)
{
    QString desStr;
    switch (i)
    {
    case BlankBox::BOX_XMIN:
        desStr = QObject::tr("Box Xmin");
        break;
    case BlankBox::BOX_YMIN:
        desStr = QObject::tr("Box Ymin");
        break;
    case BlankBox::BOX_ZMIN:
        desStr = QObject::tr("Box Zmin");
        break;
    case BlankBox::BOX_XMAX:
        desStr = QObject::tr("Box Xmax");
        break;
    case BlankBox::BOX_YMAX:
        desStr = QObject::tr("Box Ymax");
        break;
    case BlankBox::BOX_ZMAX:
        desStr = QObject::tr("Box Zmax");
        break;
    default:
        break;
    }
    return desStr;
}

void BlankBox::GetDataDes(std::vector<QString> &dataDes)
{
    dataDes.resize(BOX_END);
    for(size_t i = 0; i < BOX_END; i++)
    {
        dataDes[i] = GetDes(i);
    }
}

void BlankBox::Draw()
{
    KWBox box;
    box.xmin = GetValue(BOX_XMIN).toDouble();
    box.xmax = GetValue(BOX_XMAX).toDouble();
    box.ymin = GetValue(BOX_YMIN).toDouble();
    box.ymax = GetValue(BOX_YMAX).toDouble();
    box.zmin = GetValue(BOX_ZMIN).toDouble();
    box.zmax = GetValue(BOX_ZMAX).toDouble();
    NCCube cube(box);
    cube.draw();
}
