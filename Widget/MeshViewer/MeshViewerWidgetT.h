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




#pragma once


//== INCLUDES =================================================================

#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/Utils/GenProg.hh>
#include <OpenMesh/Core/Utils/color_cast.hh>
#include <OpenMesh/Core/Mesh/Attributes.hh>
#include <OpenMesh/Tools/Utils/StripifierT.hh>
#include <OpenMesh/Tools/Utils/Timer.hh>
#include <Widget/MeshViewer/QGLViewerWidget.h>


//== FORWARDS =================================================================

class QImage;

//== DEFINE ==================================================================
#define ACT_STRIP_VA "Strips'n VertexArrays"
#define ACT_STRIP "Show Strips"
#define ACT_COLOR_V "Colored Vertices"
#define ACT_COLOR_F "Solid Colored Faces"
#define ACT_SMOOTH_COLOR_F "Smooth Colored Faces"
//== CLASS DEFINITION =========================================================


template <typename M>
class MeshViewerWidgetT : public QGLViewerWidget
{

public:

    typedef M                             Mesh;
    typedef OpenMesh::StripifierT<Mesh>   MyStripifier;
public:

    /// default constructor
    explicit MeshViewerWidgetT(QWidget* _parent=0)
        : QGLViewerWidget(_parent),
          f_strips_(false),
          tex_id_(0),
          tex_mode_(GL_MODULATE),
          flat_list_(-1),
          smooth_list_(-1),
          line_list_(-1),
          strips_(mesh_),
          use_color_(true),
          show_vnormals_(false),
          show_fnormals_(false),
          normal_scale_(1.0)
    {
        add_draw_mode("Points");
        add_draw_mode("Hidden-Line");
#if defined(OM_USE_OSG) && OM_USE_OSG
        add_draw_mode("OpenSG Indices");
#endif
    }

    /// destructor
    ~MeshViewerWidgetT() {}

public:

    /// open mesh
    virtual bool open_mesh(const char* _filename, OpenMesh::IO::Options _opt);

    /// load texture
    virtual bool open_texture( const char *_filename );
    bool set_texture( QImage& _texsrc );

    virtual void set_mesh(const Mesh &mesh);

    void enable_strips();
    void disable_strips();


    Mesh& mesh() { return mesh_; }
    const Mesh& mesh() const { return mesh_; }

protected:

    /// inherited drawing method
    virtual void draw_scene(const std::string& _draw_mode) override;

protected:

    /// draw the mesh
    virtual void draw_openmesh(const std::string& _drawmode);


    void glVertex( const typename Mesh::VertexHandle _vh )
    { glVertex3fv( &mesh_.point( _vh )[0] ); }

    void glVertex( const typename Mesh::Point& _p )
    { glVertex3fv( &_p[0] ); }

    void glNormal( const typename Mesh::VertexHandle _vh )
    { glNormal3fv( &mesh_.normal( _vh )[0] ); }

    void glTexCoord( const typename Mesh::VertexHandle _vh )
    { glTexCoord2fv( &mesh_.texcoord(_vh)[0] ); }

    void glColor( const typename Mesh::VertexHandle _vh )
    { glColor3ubv( &mesh_.color(_vh)[0] ); }

    // face properties

    void glNormal( const typename Mesh::FaceHandle _fh )
    { glNormal3fv( &mesh_.normal( _fh )[0] ); }

    void glColor( const typename Mesh::FaceHandle _fh )
    { glColor3ubv( &mesh_.color(_fh)[0] ); }

    void glMaterial( const typename Mesh::FaceHandle _fh,
                     int _f=GL_FRONT_AND_BACK, int _m=GL_DIFFUSE )
    {
        OpenMesh::Vec3f c=OpenMesh::color_cast<OpenMesh::Vec3f>(mesh_.color(_fh));
        OpenMesh::Vec4f m( c[0], c[1], c[2], 1.0f );

        glMaterialfv(_f, _m, &m[0]);
    }


protected: // Strip support

    void compute_strips(void)
    {
        if (f_strips_)
        {
            strips_.clear();
            strips_.stripify();
        }
    }

    void updateFaceNormalPoints()
    {
        mesh_.add_property( fp_normal_base_ );
        typename M::FaceIter f_it = mesh_.faces_begin();
        typename M::FaceVertexIter fv_it;
        for (;f_it != mesh_.faces_end(); ++f_it)
        {
            typename Mesh::Point v(0,0,0);
            for( fv_it=mesh_.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
                v += OpenMesh::vector_cast<typename Mesh::Normal>(mesh_.point(*fv_it));
            v *= 1.0f/3.0f;
            mesh_.property( fp_normal_base_, *f_it ) = v;
        }
    }

    void updateFlatList()
    {
        if(flat_list_ != -1)
        {
            glDeleteLists(flat_list_, 1);
        }
        else
        {
            flat_list_ = glGenLists(1);
        }

        typename Mesh::ConstFaceIter    fIt(mesh_.faces_begin()),
                fEnd(mesh_.faces_end());

        typename Mesh::ConstFaceVertexIter fvIt;

        glNewList(flat_list_, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (; fIt!=fEnd; ++fIt)
        {
            glNormal3fv( &mesh_.normal(*fIt)[0] );

            fvIt = mesh_.cfv_iter(*fIt);
            glVertex3fv( &mesh_.point(*fvIt)[0] );
            ++fvIt;
            glVertex3fv( &mesh_.point(*fvIt)[0] );
            ++fvIt;
            glVertex3fv( &mesh_.point(*fvIt)[0] );
        }
        glEnd();
        glEndList();
    }

    void UpdateSmoothList()
    {
        if(smooth_list_ != -1)
        {
            glDeleteLists(smooth_list_, 1);
        }
        else
        {
            smooth_list_ = glGenLists(1);
        }

        typename Mesh::ConstFaceIter    fIt(mesh_.faces_begin()),
                fEnd(mesh_.faces_end());

        typename Mesh::ConstFaceVertexIter fvIt;

        glNewList(smooth_list_, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (; fIt!=fEnd; ++fIt)
        {
            fvIt = mesh_.cfv_iter(*fIt);
            glNormal3fv( &mesh_.normal(*fvIt)[0] );
            glVertex3fv( &mesh_.point(*fvIt)[0] );
            ++fvIt;
            glNormal3fv( &mesh_.normal(*fvIt)[0] );
            glVertex3fv( &mesh_.point(*fvIt)[0] );
            ++fvIt;
            glNormal3fv( &mesh_.normal(*fvIt)[0] );
            glVertex3fv( &mesh_.point(*fvIt)[0] );
        }
        glEnd();
        glEndList();
    }

    void updateLineList()
    {
        if(line_list_ != -1)
        {
            glDeleteLists(line_list_, 1);
        }
        else
        {
            line_list_ = glGenLists(1);
        }

        typename Mesh::ConstFaceIter    fIt(mesh_.faces_begin()),
                fEnd(mesh_.faces_end());

        typename Mesh::ConstFaceVertexIter fvIt;

        glNewList(line_list_, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (; fIt!=fEnd; ++fIt)
        {
            fvIt = mesh_.cfv_iter(*fIt);
            glVertex3fv( &mesh_.point(*fvIt)[0] );
            ++fvIt;
            glVertex3fv( &mesh_.point(*fvIt)[0] );
            ++fvIt;
            glVertex3fv( &mesh_.point(*fvIt)[0] );
        }
        glEnd();
        glEndList();
    }


protected: // inherited

    virtual void keyPressEvent( QKeyEvent* _event) override;

protected:

    bool                   f_strips_; // enable/disable strip usage
    GLuint                 tex_id_;
    GLint                  tex_mode_;
    GLuint                 flat_list_, smooth_list_, line_list_;
    OpenMesh::IO::Options  opt_; // mesh file contained texcoords?

    Mesh                   mesh_;
    MyStripifier           strips_;
    bool                   use_color_;
    bool                   show_vnormals_;
    bool                   show_fnormals_;
    float                  normal_scale_;
    OpenMesh::FPropHandleT< typename Mesh::Point > fp_normal_base_;
};


//=============================================================================
#if defined(OM_INCLUDE_TEMPLATES)
#  define OPENMESH_MESHVIEWERWIDGET_TEMPLATES
#  include "MeshViewerWidgetT_impl.h"
#endif
//=============================================================================



