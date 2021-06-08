#ifndef ENDMILLCUTTER_H
#define ENDMILLCUTTER_H

#include "tooldata.h"

class EndCutter : public ToolData
{
public:
    enum EndCutData
    {
        ENDCUT_BEGIN,
        ENDCUT_LENGTH = ENDCUT_BEGIN,
        ENDCUT_DIA,
        ENDCUT_HANDLE_LEN,
        ENDCUT_HANDLE_DIA,
        ENDCUT_HANDLE_DIS,
        ENDCUT_END
    };

public:
    EndCutter(const QString& name);

    QString GetDes(size_t i) override;
    void GetDataDes(std::vector<QString> &dataDes) override;

private:
    /*
     * 初始化默认参数
     **/
    void Initial();
};

#endif // ENDMILLCUTTER_H
