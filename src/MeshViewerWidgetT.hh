/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/ 

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 362 $                                                         *
 *   $Date: 2011-01-26 10:21:12 +0100 (Mi, 26 Jan 2011) $                   *
 *                                                                           *
\*===========================================================================*/


#ifndef OPENMESHAPPS_MESHVIEWERWIDGETT_HH
#define OPENMESHAPPS_MESHVIEWERWIDGETT_HH


//== INCLUDES =================================================================
#include <vector>
#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/Utils/GenProg.hh>
#include <OpenMesh/Core/Utils/color_cast.hh>
#include <OpenMesh/Core/Mesh/Attributes.hh>
#include <OpenMesh/Tools/Utils/StripifierT.hh>
#include <OpenMesh/Tools/Utils/Timer.hh>
#include <QGLViewerWidget.hh>
#include <QAction>
#include "colormap.h"
#include "axis3d.h"

//== FORWARDS =================================================================

class QImage;


//== CLASS DEFINITION =========================================================


template <typename M>
class MeshViewerWidgetT : public QGLViewerWidget
{

public:

    typedef M                             Mesh;
    //typedef OpenMesh::StripifierT<Mesh>   MyStripifier;

protected:

    //bool                   f_strips_; // enable/disable strip usage
    GLuint                 tex_id_;
    GLint                  tex_mode_;
    OpenMesh::IO::Options  opt_; // mesh file contained texcoords?

    std::vector<Mesh>     meshVect_;
    std::vector< Axis3D > axisVect_;
    std::vector<bool>     showVect_;

    bool                   use_color_;// true when using file color data, false using colormap to set colors
    bool                   show_axis_;

    ColorMap              colormap_;
    OpenMesh::Vec3f       bbMin_;
    OpenMesh::Vec3f       bbMax_;

private:


public:

    /// default constructor
    MeshViewerWidgetT(QWidget* _parent=0)
        : QGLViewerWidget(_parent)

    {
        show_axis_=true;
        use_color_=true;
        add_draw_mode("Points");
        add_draw_mode("Hidden-Line");

#if defined(OM_USE_OSG) && OM_USE_OSG
        add_draw_mode("OpenSG Indices");
#endif

        del_draw_mode("Solid Smooth");
        removeAction("Solid Smooth");
        del_draw_mode("Solid Flat");
        findAction("Wireframe")->setShortcut(QKeySequence ());
    }

    /// destructor
    ~MeshViewerWidgetT() {}

public:
    void switchAxis();

    int getNumberOfMeshes(){
        return meshVect_.size();
    }

    const float * getMeshColor(int index){
        return colormap_.getColor(index);
    }

    bool getShowAxis(){
        return show_axis_;
    }
    Axis3D getAxis( int index ){
        return axisVect_.at( index );
    }

    bool isMeshVisible( int index ){
        return showVect_.at(index);
    }

    void setMeshVisible( int index ){
        if( index>=0 && index<showVect_.size() ){
            showVect_[index]=!showVect_[index];
            updateGL();
        }
    }

    void setMeshVisiblility( int index, bool visible ){
        if( index>=0 && index<showVect_.size() ){
            showVect_[index]=visible;
            updateGL();
        }
    }

    void resetScenePos(){
        initScene();
        set_scene_pos( (bbMin_+bbMax_)*0.5, (bbMin_-bbMax_).norm()*0.5 );
        updateGL();
    }

    vector< Axis3D > getAxisVector(){
        return axisVect_;
    }




    /// open mesh
    virtual bool open_mesh(const char* _filename, OpenMesh::IO::Options _opt, Axis3D axis=Axis3D() );
    void insertAxis(int index, Axis3D axis);

    //bool insert_mesh( Mesh * mesh, std::pair<OpenMesh::Vec3f,OpenMesh::Vec3f>  axis= std::pair<OpenMesh::Vec3f,OpenMesh::Vec3f>( OpenMesh::Vec3f(0,0,0),OpenMesh::Vec3f(0,0,0) ) );
    bool insert_mesh( const Mesh & mesh, Axis3D axis=Axis3D() );



    //Mesh& mesh() { return meshVect_.front(); }
    const Mesh& getMesh( int index ) const { return meshVect_.at(index); }

    void deleteMeshes(){

        meshVect_.clear();
        axisVect_.clear();
        showVect_.clear();
    }

protected:

    /// inherited drawing method
    virtual void draw_scene(const std::string& _draw_mode);

protected:

    /// draw the mesh
    virtual void draw_openmesh(const std::string& _drawmode, bool fillColor=false);

protected: // inherited

    virtual void keyPressEvent( QKeyEvent* _event){
        //disabling key events not needed
    }


};


//=============================================================================
#if defined(OM_INCLUDE_TEMPLATES) && !defined(OPENMESHAPPS_MESHVIEWERWIDGET_CC)
#  define OPENMESH_MESHVIEWERWIDGET_TEMPLATES
#  include "MeshViewerWidgetT.cc"
#endif
//=============================================================================
#endif // OPENMESHAPPS_MESHVIEWERWIDGETT_HH defined
//=============================================================================

