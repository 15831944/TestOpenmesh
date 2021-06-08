#include "cylinder.h"

#include "ArcBall/Quaternion.h"

Cylinder::Cylinder(double radius, double height, const CVector3d &center, const CVector3d &dir)
    : m_radius(radius)
    , m_height(height)
    , m_center(center)
    , m_dir(dir)

{
    m_dir.Normalize();
    computerDate();
}

Cylinder::~Cylinder()
{
    if(m_stripPoints)
        delete m_stripPoints;

    if(m_stripNormals)
        delete m_stripNormals;

}

void Cylinder::draw()
{
    GLfloat mat_a[] = {0.1f, 0.1f, 0.1f, 0.7f};
    GLfloat mat_d[] = {0.5f, 0.7f, 0.5f, 0.7f};
    GLfloat mat_s[] = {1.0f, 1.0f, 1.0f, 0.7f};
    GLfloat shine[] = {120.0f};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, stripPoints());
    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, stripNormals());
    glDrawArrays( GL_TRIANGLE_STRIP, 0, stripSize());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
}

void Cylinder::Set(double radius, double height, const CVector3d &center, const CVector3d &dir)
{
    m_radius = radius;
    m_height = height;
    m_center = center;
    m_dir = dir;
    m_dir.Normalize();
    computerDate();
}

void Cylinder::computerDate()
{
    if(m_stripPoints)
    {
        delete m_stripPoints;
        m_stripPoints = nullptr;
    }

    struct Vec3f {
        float x, y, z;
    };
    CQuaternion qua(ZDIR, m_dir);
    auto  mat = qua.GetMatrix();

    KWPolyline poly1;
    poly1.fromCircle(m_radius, 0.1, CVector3d(0,0,0));

    auto center1 = m_center;
    CVector3d center2 = m_center + (m_dir * m_height);
    KWPolyline poly2;
    for(size_t i = 0; i < poly1.size(); ++i)
    {
        poly1[i] = mat*poly1[i] + m_center;
        poly2.push_back(poly1[i]+m_dir*m_height);
    }

    m_size = poly1.size()*4 + 6;

    auto points = new Vec3f[m_size];
    auto normals = new Vec3f[m_size];
    int index = 0;


    size_t i = 0, j = poly1.size()-1;
    for(; i <= j; ++i, --j)
    {
        normals[index].x = static_cast<float>(-m_dir.x());
        normals[index].y = static_cast<float>(-m_dir.y());
        normals[index].z = static_cast<float>(-m_dir.z());

        points[index].x = static_cast<float>(poly1[i].x());
        points[index].y = static_cast<float>(poly1[i].y());
        points[index].z = static_cast<float>(poly1[i].z());

        ++index;

        if(j != i)
        {
            normals[index].x = static_cast<float>(-m_dir.x());
            normals[index].y = static_cast<float>(-m_dir.y());
            normals[index].z = static_cast<float>(-m_dir.z());

            points[index].x = static_cast<float>(poly1[j].x());
            points[index].y = static_cast<float>(poly1[j].y());
            points[index].z = static_cast<float>(poly1[j].z());

            ++index;
        }
        else
        {
            auto nor = poly1[j] - center1;
            nor.Normalize();
            normals[index].x = static_cast<float>(nor.x());
            normals[index].y = static_cast<float>(nor.y());
            normals[index].z = static_cast<float>(nor.z());

            points[index].x = static_cast<float>(poly1[j].x());
            points[index].y = static_cast<float>(poly1[j].y());
            points[index].z = static_cast<float>(poly1[j].z());

            ++index;
            break;
        }
    }

    auto endJ = j;
    auto nSize = poly1.size();
    while( j < nSize || (j-nSize)  <= (endJ) )
    {
        auto nor = poly1[j%nSize] - center1;
        nor.Normalize();
        normals[index].x = static_cast<float>(nor.x());
        normals[index].y = static_cast<float>(nor.y());
        normals[index].z = static_cast<float>(nor.z());

        points[index].x = static_cast<float>(poly1[j%nSize].x());
        points[index].y = static_cast<float>(poly1[j%nSize].y());
        points[index].z = static_cast<float>(poly1[j%nSize].z());

        ++index;

        normals[index].x = static_cast<float>(nor.x());
        normals[index].y = static_cast<float>(nor.y());
        normals[index].z = static_cast<float>(nor.z());

        points[index].x = static_cast<float>(poly2[j%nSize].x());
        points[index].y = static_cast<float>(poly2[j%nSize].y());
        points[index].z = static_cast<float>(poly2[j%nSize].z());

        ++j;
        ++index;
    }


    i = 0;
    while( endJ + i < nSize && endJ - i >= 0)
    {

        if(endJ + i < nSize)
        {
            normals[index].x = static_cast<float>(m_dir.x());
            normals[index].y = static_cast<float>(m_dir.y());
            normals[index].z = static_cast<float>(m_dir.z());

            points[index].x = static_cast<float>(poly2[endJ+i].x());
            points[index].y = static_cast<float>(poly2[endJ+i].y());
            points[index].z = static_cast<float>(poly2[endJ+i].z());
            ++index;

        }


        if(endJ - i >= 0)
        {
            normals[index].x = static_cast<float>(m_dir.x());
            normals[index].y = static_cast<float>(m_dir.y());
            normals[index].z = static_cast<float>(m_dir.z());

            points[index].x = static_cast<float>(poly2[endJ-i].x());
            points[index].y = static_cast<float>(poly2[endJ-i].y());
            points[index].z = static_cast<float>(poly2[endJ-i].z());
            ++index;
        }


        ++i;
    }





    m_size = index;
//    glBegin(GL_TRIANGLE_FAN);           //扇形连续填充三角形串
//    glNormal3f(-dir.x(), -dir.y(), -dir.z());
//    glVertex3f(center.x(), center.y(), center.z());
//    for(size_t i = 0; i < poly1.size(); ++i)
//    {
//        glVertex3f(poly1[i].x(), poly1[i].y(), poly1[i].z());
//    }
//    glEnd();

//    glBegin(GL_TRIANGLE_FAN);
//    glNormal3f(dir.x(), dir.y(), dir.z());
//    glVertex3f(center2.x(), center2.y(), center2.z());
//    for(size_t i = 0; i < poly2.size(); ++i)
//    {
//        glVertex3f(poly2[i].x(), poly2[i].y(), poly2[i].z());
//    }
//    glEnd();

//    glBegin(GL_TRIANGLE_STRIP);
//    for(size_t i = 0; i < poly1.size(); ++i)
//    {
//        auto n1 = poly1[i] - center;
//        glNormal3f(n1.x(), n1.y(), n1.z());
//        glVertex3f(poly1[i].x(), poly1[i].y(), poly1[i].z());

//        auto n2 = poly2[i] - center;
//        glNormal3f(n2.x(), n2.y(), n2.z());
//        glVertex3f(poly2[i].x(), poly2[i].y(), poly2[i].z());
//    }
//    glEnd();

    m_stripPoints = reinterpret_cast<float*>(points);
    m_stripNormals = reinterpret_cast<float*>(normals);

}
