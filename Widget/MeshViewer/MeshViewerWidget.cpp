/* ========================================================================= *
 *                                                                           *
 *                               OpenMesh                                    *
 *           Copyright (c) 2001-2015, RWTH-Aachen University                 *
 *           Department of Computer Graphics and Multimedia                  *
 *                          All rights reserved.                             *
 *                            www.openmesh.org                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of OpenMesh.                                            *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
 *                                                                           *
 * ========================================================================= */

#include "Geometry/cylinder.h"
#include "Geometry/ncball.h"
//#include "gl/freeglut.h"
#define OPENMESHAPPS_MESHVIEWERWIDGET_CC

//== INCLUDES =================================================================

#include <Widget/MeshViewer/MeshViewerWidget.h>

//== QT============================
#include <QAction>
#include <QTimer>

//== IMPLEMENTATION ==========================================================

//== DEFINES ========================================
#define INTERVAL_SIMU 150

/// default constructor
MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    : MeshViewerWidgetT<MyMesh>(parent)
{
    QAction* a = add_draw_mode(ACT_SELECT_TRIAS);
    a->setCheckable(false);
    a = add_draw_mode(ACT_SELECT_POINTS);
    a->setCheckable(false);
}

void MeshViewerWidget::open_mesh_gui(QString fname)
{
    OpenMesh::Utils::Timer t;
    t.start();
    if (fname.isEmpty() || !open_mesh(fname.toLocal8Bit(), _options)) {
        QString msg = "Cannot read mesh from file:\n '";
        msg += fname;
        msg += "'";
        QMessageBox::critical(nullptr, windowTitle(), msg);
    }
    t.stop();
    std::cout << "Loaded mesh in ~" << t.as_string() << std::endl;
}

void MeshViewerWidget::set_mesh_gui(const MeshViewerWidgetT::Mesh& mesh)
{
    if (mesh.faces_empty()) {
        return;
    }
    set_mesh(mesh);
    polys.clear();
}

void MeshViewerWidget::set_path(KWPolyline& path)
{
    polys = path;
    m_dRadius = path.Offset();
}

void MeshViewerWidget::open_texture_gui(QString fname)
{
    if (fname.isEmpty() || !open_texture(fname.toLocal8Bit())) {
        QString msg = "Cannot load texture image from file:\n '";
        msg += fname;
        msg += "'\n\nPossible reasons:\n";
        msg += "- Mesh file didn't provide texture coordinates\n";
        msg += "- Texture file does not exist\n";
        msg += "- Texture file is not accessible.\n";
        QMessageBox::warning(nullptr, windowTitle(), msg);
    }
}

void MeshViewerWidget::start_simu()
{
    m_tmSimu.start(INTERVAL_SIMU, this);
}

void MeshViewerWidget::stop_simu()
{
    m_tmSimu.stop();
}

void MeshViewerWidget::draw_scene(const string& _draw_mode)
{
    GLfloat mat_a[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat mat_d[] = { 1.0f, .5f, 0.5f, 1.0f };
    GLfloat mat_s[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shine[] = { 120.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_a);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shine);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < m_selectTrias.size(); ++i) {
        glNormal(m_selectTrias[i]);

        auto iterV = mesh_.cfv_iter(m_selectTrias[i]);
        glVertex(mesh_.point(*iterV));
        ++iterV;

        glVertex(mesh_.point(*iterV));
        ++iterV;

        glVertex(mesh_.point(*iterV));
        ++iterV;
    }
    glEnd();
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    setDefaultMaterial();

    if (g_pMesh->IsVisiable())
        MeshViewerWidgetT::draw_scene(_draw_mode);

    if (g_pBlank && g_pBlank->isVisiable()) {
        glEnable(GL_BLEND);
        g_pBlank->Draw();
        glDisable(GL_BLEND);
        setDefaultMaterial();
    }

    if (!polys.empty()) {
        if (m_tmSimu.isActive()) {
            NCBall b(m_ptCenter, m_dRadius);
            b.draw();
        }
        setDefaultMaterial();
    }

    glDisable(GL_LIGHTING);
    glColor4f(0.7f, 0.5f, 0.7f, 1.0);
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    if (polys.IsVisiable()) {
        for (size_t i = 0; i < polys.size(); ++i) {
            glVertex3d(polys[i].x(), polys[i].y(), polys[i].z());
        }
        glEnd();
    }
    glLineWidth(1.0);
}

void MeshViewerWidget::timerEvent(QTimerEvent* evt)
{
    if (m_tmSimu.timerId() == evt->timerId()) {
        simuball();
    } else {
        return QGLViewerWidget::timerEvent(evt);
    }
}

void MeshViewerWidget::slotDrawModeAct(QAction* _mode)
{
    if (ACT_SELECT_TRIAS == _mode->text()) {
        m_nSelectMode = SELECT_TRIAS;
    } else if (ACT_SELECT_TRIAS == _mode->text()) {
        m_nSelectMode = SELECT_POINTS;
    } else {
        m_nSelectMode = SELECT_NONE;
        return QGLViewerWidget::slotDrawModeAct(_mode);
    }
}

void MeshViewerWidget::mousePressEvent(QMouseEvent* evt)
{
    switch (m_nSelectMode) {
    case SELECT_TRIAS:
        if (Qt::LeftButton == evt->button()) {
            m_prePoint2d = QPoint(evt->pos().x(), height() - evt->pos().y());
            Vec3f l1(m_prePoint2d.x(), m_prePoint2d.y(), 0);
            Vec3f l2(m_prePoint2d.x(), m_prePoint2d.y(), 1.0);
            Vec3f p1, p2;
            unproject(l1, p1);
            unproject(l2, p2);

            auto res = g_pMesh->SelectFace(p1, p2);
            if (res.is_valid())
                m_selectTrias.push_back(res);
            updateGL();
        }
        return;
    case SELECT_POINTS:
        return;
    default:
        break;
    }

    return QGLViewerWidget::mousePressEvent(evt);
}

void MeshViewerWidget::mouseMoveEvent(QMouseEvent* evt)
{
    switch (m_nSelectMode) {
    case SELECT_TRIAS:
        if (Qt::LeftButton == evt->buttons()) {
            QPoint pointWin(evt->pos().x(), height() - evt->pos().y());

            Vec3f l11(m_prePoint2d.x(), m_prePoint2d.y(), 0);
            Vec3f l12(m_prePoint2d.x(), m_prePoint2d.y(), 1.0);

            Vec3f l21(pointWin.x(), pointWin.y(), 0);
            Vec3f l22(pointWin.x(), pointWin.y(), 1.0);
            Vec3f line2[2], line1[2];
            unproject(l11, line1[0]);
            unproject(l12, line1[1]);
            unproject(l21, line2[0]);
            unproject(l22, line2[1]);

            auto res = g_pMesh->SelectFace(line1, line2);
            for (size_t i = 0; i < res.size(); ++i) {
                if (res[i].is_valid())
                    m_selectTrias.push_back(res[i]);
            }

            updateGL();
        }
        return;
    case SELECT_POINTS:
        return;
    default:
        break;
    }

    return QGLViewerWidget::mouseMoveEvent(evt);
}

void MeshViewerWidget::query_open_mesh_file()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open mesh file"),
        tr(""),
        tr("OBJ Files (*.obj);;"
           "OFF Files (*.off);;"
           "STL Files (*.stl);;"
           "All Files (*)"));
    if (!fileName.isEmpty())
        open_mesh_gui(fileName);
}

void MeshViewerWidget::query_open_texture_file()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open texture file"),
        tr(""),
        tr("PNG Files (*.png);;"
           "BMP Files (*.bmp);;"
           "GIF Files (*.gif);;"
           "JPEG Files (*.jpg);;"
           "TIFF Files (*.tif);;"
           "All Files (*)"));
    if (!fileName.isEmpty())
        open_texture_gui(fileName);
}

void MeshViewerWidget::simuball()
{
    if (polys.empty()) {
        m_tmSimu.stop();
        return;
    }
    m_dCurDis += (INTERVAL_SIMU / 1000.0 * 5000) / 60.0;
    double d = m_dCurDis;
    size_t index = -1;
    CVector3d dir;
    for (size_t i = 1; i < polys.size(); i++) {
        dir = (polys[i] - polys[i - 1]);
        d -= dir.Normalize();
        if (d < 0) {
            index = i;
            break;
        }
    }

    if (index != -1) {
        m_ptCenter = polys[index] + d * dir;
    }
    updateGL();
}
//=============================================================================
