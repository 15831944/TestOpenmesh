#ifndef BLACKBOX_H
#define BLACKBOX_H

#include "tooldata.h"
#include "Geometry/nccube.h"
#include "Geometry/nctriamesh.h"

class BlankBox : public ToolData
{
public:
    enum BoxData
    {
        BOX_BEGIN,
        BOX_XMIN = BOX_BEGIN,
        BOX_YMIN,
        BOX_ZMIN,
        BOX_XMAX,
        BOX_YMAX,
        BOX_ZMAX,
        BOX_END,
    };
public:
    BlankBox(const QString &BlankName = "BlankBox");

    QString GetDes(size_t i) override;
    void GetDataDes(std::vector<QString> &dataDes) override;
    void Draw() override;

private:

};

#endif // BLACKBOX_H
