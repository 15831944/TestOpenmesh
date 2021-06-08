#ifndef TAPERBALLCUTTER_H
#define TAPERBALLCUTTER_H

#include "tooldata.h"

class TaperBallCutter : public ToolData
{
public:
    enum TaperBallData
    {
        TBC_BEGIN,
        TBC_LENGTH = TBC_BEGIN,
        TBC_DIA,
        TBC_HANDLE_LEN,
        TBC_HANDLE_DIA,
        TBC_HANDLE_DIS,
        TBC_ANGLE,
        TBC_TIP_RADIUS,
        TBC_END
    };

public:
    TaperBallCutter(const QString& name);

    QString GetDes(size_t i) override;
    void GetDataDes(std::vector<QString> &dataDes) override;

private:
    /*
     * ��ʼ��Ĭ�ϲ���
     **/
    void Initial();
};

#endif // TAPERBALLCUTTER_H
