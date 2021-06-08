#ifndef BALLENDCUTTER_H
#define BALLENDCUTTER_H

#include "tooldata.h"

class BallEndCutter : public ToolData
{
public:
    enum BallEndCutData
    {
        BALLCUT_BEGIN,
        BALLCUT_LENGTH = BALLCUT_BEGIN,
        BALLCUT_DIA,
        BALLCUT_HANDLE_LEN,
        BALLCUT_HANDLE_DIA,
        BALLCUT_HANDLE_DIS,
        BALLCUT_END
    };

public:
    BallEndCutter(const QString& name);

    QString GetDes(size_t i) override;
    void GetDataDes(std::vector<QString> &dataDes) override;

private:
    /*
     * 初始化默认参数
     **/
    void Initial();
};

#endif // BALLENDCUTTER_H
