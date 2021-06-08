#ifndef NCTRIAMESH_H
#define NCTRIAMESH_H

#include "Vector3d.h"
#include "NCPath.h"

typedef CVector3d KWLine[2];
typedef NCPath KWPolyline;

struct KWBox{

    KWBox():
        xmin(0), xmax(0),
        ymin(0), ymax(0),
        zmin(0), zmax(0){}
    KWBox(const KWBox &box):
        xmin(box.xmin), xmax(box.xmax),
        ymin(box.ymin), ymax(box.ymax),
        zmin(box.zmin), zmax(box.zmax){}

    inline double Width()   {return fabs(xmax-xmin);}
    inline double Height()  {return fabs(ymax-ymin);}
    inline double Depth()   {return fabs(zmax-zmin);}

    inline bool isEmpty()
    {
        return Width() < DBL_EPSILON || Height() < DBL_EPSILON || Depth() < DBL_EPSILON;
    }

    //∪
    void operator |= (const KWBox &box)
    {
        if(this->xmin > box.xmin)
            this->xmin = box.xmin;
        else if(this->xmax < box.xmax)
            this->xmax = box.xmax;
        if(this->ymin > box.ymin)
            this->ymin = box.ymin;
        else if(this->ymax < box.ymax)
            this->ymax = box.ymax;
        if(this->zmin > box.zmin)
            this->zmin = box.zmin;
        else if(this->zmax < box.zmax)
            this->zmax = box.zmax;
    }

    void operator |= (const CVector3d &pt)
    {
        if(this->xmin > pt.x())
            this->xmin = pt.x();
        else if(this->xmax < pt.x())
            this->xmax = pt.x();
        if(this->ymin > pt.y())
            this->ymin = pt.y();
        else if(this->ymax < pt.y())
            this->ymax = pt.y();
        if(this->zmin > pt.z())
            this->zmin = pt.z();
        if(this->zmax < pt.z())
            this->zmax = pt.z();
    }


    KWBox operator |(const KWBox &theBox)
    {
        struct KWBox box;
        if(box.xmin > theBox.xmin)
            box.xmin = theBox.xmin;
        if(box.xmax < theBox.xmax)
            box.xmax = theBox.xmax;
        if(box.ymin > theBox.ymin)
            box.ymin = theBox.ymin;
        if(box.ymax < theBox.ymax)
            box.ymax = theBox.ymax;
        if(box.zmin > theBox.zmin)
            box.zmin = theBox.zmin;
        if(box.zmax < theBox.zmax)
            box.zmax = theBox.zmax;
        return box;
    }

    void operator = (const KWBox &box)
    {
        this->xmin = box.xmin;
        this->xmax = box.xmax;
        this->ymin = box.ymin;
        this->ymax = box.ymax;
        this->zmin = box.zmin;
        this->zmax = box.zmax;
    }

    //∩
    void operator &= (const KWBox &box)
    {
        if(this->xmin < box.xmin)
            this->xmin = box.xmin;
        if(this->xmax > box.xmax)
            this->xmax = box.xmax;
        if(this->ymin < box.ymin)
            this->ymin = box.ymin;
        if(this->ymax > box.ymax)
            this->ymax = box.ymax;
        if(this->zmin < box.zmin)
            this->zmin = box.zmin;
        if(this->zmax > box.zmax)
            this->zmax = box.zmax;
    }

    KWBox operator & (const KWBox &theBox)
    {
        struct KWBox box;
        if(box.xmin < theBox.xmin)
            box.xmin = theBox.xmin;
        if(box.xmax > theBox.xmax)
            box.xmax = theBox.xmax;
        if(box.ymin < theBox.ymin)
            box.ymin = theBox.ymin;
        if(box.ymax > theBox.ymax)
            box.ymax = theBox.ymax;
        if(box.zmin < theBox.zmin)
            box.zmin = theBox.zmin;
        if(box.zmax > theBox.zmax)
            box.zmax = theBox.zmax;
        return box;
    }

    bool Intersect(const KWBox &box)
    {
        if(this->xmax < box.xmin
                ||this->xmin > box.xmax
                ||this->ymax < box.ymin
                ||this->ymin > box.ymax
                ||this->zmax < box.zmin
                ||this->zmin > box.zmax
                )
        {
            return false;
        }
        return true;

    }

    CVector3d center() const
    {
        CVector3d c;
        c[0] = 0.5*(xmin + xmax);
        c[1] = 0.5*(ymin + ymax);
        c[2] = 0.5*(zmin + zmax);
        return c;
    }

    double xmin, xmax;
    double ymin, ymax;
    double zmin, zmax;

};


///下标从1开始, 解三元一次方程
template<class T>
vector<T> calc(T matrix[4][5])
{
    T     base_D = matrix[1][1]*matrix[2][2]*matrix[3][3] + matrix[2][1]*matrix[3][2]*matrix[1][3] + matrix[3][1]*matrix[1][2]*matrix[2][3];//计算行列式
    base_D = base_D-(matrix[1][3]*matrix[2][2]*matrix[3][1] + matrix[1][1]*matrix[2][3]*matrix[3][2] + matrix[1][2]*matrix[2][1]*matrix[3][3]);

    vector<T> res;
    res.assign(3, NAN);
    if(base_D != 0)
    {
        T     x_D = matrix[1][4]*matrix[2][2]*matrix[3][3] + matrix[2][4]*matrix[3][2]*matrix[1][3] + matrix[3][4]*matrix[1][2]*matrix[2][3];
        x_D = x_D-(matrix[1][3]*matrix[2][2]*matrix[3][4] + matrix[1][4]*matrix[2][3]*matrix[3][2] + matrix[1][2]*matrix[2][4]*matrix[3][3]);
        T     y_D = matrix[1][1]*matrix[2][4]*matrix[3][3] + matrix[2][1]*matrix[3][4]*matrix[1][3] + matrix[3][1]*matrix[1][4]*matrix[2][3];
        y_D = y_D-(matrix[1][3]*matrix[2][4]*matrix[3][1] + matrix[1][1]*matrix[2][3]*matrix[3][4] + matrix[1][4]*matrix[2][1]*matrix[3][3]);
        T     z_D = matrix[1][1]*matrix[2][2]*matrix[3][4] + matrix[2][1]*matrix[3][2]*matrix[1][4] + matrix[3][1]*matrix[1][2]*matrix[2][4];
        z_D = z_D-(matrix[1][4]*matrix[2][2]*matrix[3][1] + matrix[1][1]*matrix[2][4]*matrix[3][2] + matrix[1][2]*matrix[2][1]*matrix[3][4]);

        T x =  x_D/base_D;
        T y =  y_D/base_D;
        T z =  z_D/base_D;
        res[0] = x;
        res[1] = y;
        res[2] = z;
    }else{
    }

    return res;
}

class NCTriaMesh
{
public:
    NCTriaMesh();
    virtual ~NCTriaMesh();

    virtual bool LoadFile(const std::string& fileName) = 0;
    virtual bool SaveFile(const std::string& fileName) = 0;

    virtual void ComputerBox() = 0;
    KWBox &GetBouningBox() {return m_box;}

    virtual vector<KWPolyline> ProcessPolyLine(double offset) = 0;

    virtual std::string GetMeshName() {return m_fileName;}
    std::string GetFileName() {return m_fileName;}

    bool IsVisiable() {return m_bVisiable;}
    void SetVisiable(bool bVisiable){m_bVisiable = bVisiable;}

    virtual void Output() = 0;

protected:
    KWBox m_box;
    std::string m_fileName;
    unsigned int m_modelList;
    bool m_bVisiable;
};

#endif // NCTRIAMESH_H
