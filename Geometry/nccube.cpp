#include "nccube.h"

NCCube::NCCube(const KWBox &box)
{
    Set(box);
}

void NCCube::draw()
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

    glBegin(GL_QUADS);

    glNormal3d(-1.0, 0, 0);
    glVertex3d(m_box.xmin, m_box.ymin, m_box.zmin);
    glVertex3d(m_box.xmin, m_box.ymin, m_box.zmax);
    glVertex3d(m_box.xmin, m_box.ymax, m_box.zmax);
    glVertex3d(m_box.xmin, m_box.ymax, m_box.zmin);

    glNormal3d(1.0, 0, 0);
    glVertex3d(m_box.xmax, m_box.ymin, m_box.zmin);
    glVertex3d(m_box.xmax, m_box.ymin, m_box.zmax);
    glVertex3d(m_box.xmax, m_box.ymax, m_box.zmax);
    glVertex3d(m_box.xmax, m_box.ymax, m_box.zmin);

    glNormal3d(0.0, -1.0, 0);
    glVertex3d(m_box.xmin, m_box.ymax, m_box.zmin);
    glVertex3d(m_box.xmax, m_box.ymax, m_box.zmin);
    glVertex3d(m_box.xmax, m_box.ymax, m_box.zmax);
    glVertex3d(m_box.xmin, m_box.ymax, m_box.zmax);

    glNormal3d(0.0, 1.0, 0);
    glVertex3d(m_box.xmin, m_box.ymin, m_box.zmin);
    glVertex3d(m_box.xmax, m_box.ymin, m_box.zmin);
    glVertex3d(m_box.xmax, m_box.ymin, m_box.zmax);
    glVertex3d(m_box.xmin, m_box.ymin, m_box.zmax);

    glNormal3d(0.0, 0, -1.0);
    glVertex3d(m_box.xmin, m_box.ymin, m_box.zmin);
    glVertex3d(m_box.xmax, m_box.ymin, m_box.zmin);
    glVertex3d(m_box.xmax, m_box.ymax, m_box.zmin);
    glVertex3d(m_box.xmin, m_box.ymax, m_box.zmin);

    glNormal3d(0.0, 0, 1.0);
    glVertex3d(m_box.xmin, m_box.ymin, m_box.zmax);
    glVertex3d(m_box.xmax, m_box.ymin, m_box.zmax);
    glVertex3d(m_box.xmax, m_box.ymax, m_box.zmax);
    glVertex3d(m_box.xmin, m_box.ymax, m_box.zmax);

    glEnd();


    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);
}

void NCCube::Set(const KWBox &box)
{
    m_box = box;
}
