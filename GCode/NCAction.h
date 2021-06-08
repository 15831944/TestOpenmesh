#ifndef CNCACTION_H
#define CNCACTION_H

const string NCAxes = "XYZABCPQUVW""IJK";

enum ACTION_TYPE
{
    ACTION_NONE,
    ACTION_G,           //�˶���λ��
    ACTION_M,           //
    ACTION_T,           //�����л�
    ACTION_S,           //�ȴ�
    ACTION_F,           //�ٶ�
    ACTION_P,           //�ȴ�
    ACTION_W,           // дO��
    ACTION_PT,          // ������
    ACTION_C,           // Բ��
    ACTION_R,           // �뾶
    ACTION_END
};

enum NCAxis
{
    NC_X,
    NC_Y,
    NC_Z,
    NC_A,
    NC_B,
    NC_C,
    NC_P,
    NC_Q,
    NC_U,
    NC_V,
    NC_W,

    NC_I,
    NC_J,
    NC_K,
};

typedef pair<char, double> NCAxisPos;

struct NCPosition
{
    void clear();

    void setPos(NCAxis axis, double value);

    void setPos(char key, double value);

    double value(char key);

    double value(NCAxis axis);

    double &operator[](char key);

    size_t find(char key);

    size_t find(NCAxis axis);

    const static size_t npos {static_cast<size_t>(-1)};
    vector<NCAxisPos> m_pos;
};


class NCAction
{

public:
    NCAction(int nType = ACTION_NONE):m_nType(nType) {}
    virtual ~NCAction();

    NCAction *next() {return m_pNext;}
    void setNext(NCAction *action) {m_pNext = action;}

    void setValue(int value) { m_value.int_ = value;}
    int value() {return m_value.int_;};

    void setPosition(NCPosition *pos) { m_value.pos_ = pos;}
    NCPosition *position() {return m_value.pos_;}

    virtual std::string toString() = 0;
    virtual int fromString(const std::string &str) = 0;

private:
    int m_nType;

    union ActData
    {
        int int_;
        NCPosition *pos_;
    }m_value {0};

    NCAction *m_pNext {nullptr};
};

class GAction: public NCAction
{

};

class MAction: public NCAction
{

};

class TAction: public NCAction
{

};

class SAction: public NCAction
{

};

class FAction: public NCAction
{

};

class PAction: public NCAction
{

};

class WAction: public NCAction
{

};


class PTAction: public NCAction
{

};


class CAction: public NCAction
{

};


class RAction: public NCAction
{

};

#endif // CNCACTION_H
