#include "ncball.h"
#include "qgl.h"

NCBall::NCBall(const CVector3d &center, double radius):
    m_size(0),
    m_radius(radius),
    m_center(center)
{
    computerDate();
}

NCBall::~NCBall()
{
    if(m_stripPoints)
    {
        delete m_stripPoints;
        m_stripPoints = nullptr;
    }
    if(m_stripNormals)
    {
        delete m_stripNormals;
        m_stripNormals = nullptr;
    }
}

float *NCBall::stripPoints()
{
    return m_stripPoints;
}

float *NCBall::stripNormals()
{
    return m_stripNormals;
}

void NCBall::draw()
{
    GLfloat mat_d[] = {0.5f, 0.5f, 0.5f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_d);
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

void NCBall::computerDate()
{
    if(m_stripPoints)
    {
        delete m_stripPoints;
        m_stripPoints = nullptr;
    }

    struct Vec3f {
        float x, y, z;
    };

    size_t kRow = 20;
    size_t kCol = 20;

    // GLfloat delta = 2 * M_PI / 100;
    m_size = 2 * kRow * kCol;
//    m_stripData = (float *) malloc(m_size* sizeof(Vec3f));
    auto points = new Vec3f[m_size];
    auto normals = new Vec3f[m_size];

    float step_z = M_PI / kRow ;  //z方向每次步进的角度(上下，纬度)
    float step_xy = 2*M_PI/ kCol;  //x,y平面每次步进的角度（经度）
    float x[2],y[2],z[2];  //用来存坐标

    float angle_z = 0; //起始角度
    float angle_xy = 0;

    size_t i=0, j=0;
    size_t index = 0;
    // 90 -->  -90
    for(i = 0; i < kRow; i++)
    {
        angle_z = i * step_z;  //每次步进step_z

        for(j = 0; j < kCol ; j++)
        {
            // j++;
            angle_xy = j * step_xy;  //每次步进step_xy
            //一层一层的画出来
            x[0] = m_radius * sin(angle_z) * cos(angle_xy);  //第一个小平面的第一个顶点坐标
            y[0] = m_radius * sin(angle_z) * sin(angle_xy);
            z[0] = m_radius * cos(angle_z);

            x[1] = m_radius * sin(angle_z + step_z) * cos(angle_xy);  //第一个小平面的第二个顶点坐标，下面类似
            y[1] = m_radius * sin(angle_z + step_z) * sin(angle_xy);
            z[1] = m_radius * cos(angle_z + step_z);

            //至此得到一个平面的两个顶点
            for(int k=0; k<2; k++)
            {
                points[index].x = x[k] + m_center.x();
                points[index].y = y[k] + m_center.y();
                points[index].z = z[k] + m_center.z();
                double len = sqrt(x[k]*x[k] + y[k]*y[k] + z[k]*z[k]);
                normals[index].x = x[k]/len;
                normals[index].y = y[k]/len;
                normals[index].z = z[k]/len;
                ++index;
            }
        } //循环画出这一层的平面，组成一个环
    }  //z轴++，画出剩余层

    m_stripPoints = reinterpret_cast<float *>(points);
    m_stripNormals = reinterpret_cast<float *>(normals);
    m_size = index;
}
