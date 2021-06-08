#ifndef NCOFFETPOLYLINE_H
#define NCOFFETPOLYLINE_H

#include "nctriamesh.h"
#include <Vector3d.h>
#include <Quaternion.h>

class NCPath
{
    using iterator = vector<CVector3d>::iterator;
    using const_iterator = vector<CVector3d>::const_iterator;
public:
    NCPath(){}
    NCPath(const vector<CVector3d> &points) : m_points(points){}

    CVector3d & operator[](size_t i);
    void push_back(const CVector3d& point);
    iterator insert(iterator where, iterator begin, iterator end) { return m_points.insert(where, begin, end);}
    size_t size(){return m_points.size();}

    iterator begin() {return m_points.begin();}
    iterator end() {return m_points.end();}

    CVector3d back() {return m_points.back();}
    CVector3d front() {return m_points.front();}


    const_iterator cbegin() const {return m_points.cbegin();}
    const_iterator cend() const {return m_points.cend();}

    bool empty()const {return m_points.empty();}

    void clear() {return m_points.clear();}

    void reverse() {return std::reverse(m_points.begin(), m_points.end());}

    bool IsVisiable() {return m_isVisiable;}
    void SetVisiable(bool isVisiable) { m_isVisiable = isVisiable;}

    std::string GetFileName(){return m_name;}

    double Offset(){return m_offset;}
    void SetOffset(double offset){ m_offset = offset;}

    void fromCircle(const double &radius, const double &eps, const CVector3d &center);

private:
    void GetNextCirclePt(double r,
                         double reps,
                         const CVector3d& p1,
                         const CVector3d& p2);

private:
    vector<CVector3d> m_points;
    bool m_isVisiable = true;
    std::string m_name = "Path1";
    double m_offset = 10;
};



#endif // NCOFFETPOLYLINE_H
