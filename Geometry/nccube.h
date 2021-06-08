#ifndef NCCUBE_H
#define NCCUBE_H


class NCCube
{
public:
    NCCube(const KWBox &box);

    void draw();

    void Set(const KWBox &box);

    KWBox GetBox() {return m_box;}


protected:
    float *m_stripPoints = nullptr;
    float *m_stripNormals = nullptr;
    size_t m_size;

    KWBox m_box;
};

#endif // NCCUBE_H
