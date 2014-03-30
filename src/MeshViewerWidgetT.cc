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

#define OPENMESHAPPS_MESHVIEWERWIDGET_CC

//== INCLUDES =================================================================

#ifdef _MSC_VER
//#  pragma warning(disable: 4267 4311)
#endif

//
#include <iostream>
#include <fstream>

// --------------------
#include <QImage>
#include <QFileInfo>
#include <QKeyEvent>
// --------------------
#include <OpenMesh/Core/Utils/vector_cast.hh>
#include <OpenMesh/Tools/Utils/Timer.hh>
#include <MeshViewerWidgetT.hh>

using namespace OpenMesh;
using namespace Qt;

#if defined(_MSC_VER)
#  undef min
#  undef max
#endif

using namespace Qt;
//== IMPLEMENTATION ========================================================== 
template <typename M>
void MeshViewerWidgetT<M>::insertAxis(int index, Axis3D axis){

cout<<"axis insertAxis "<<axis.getAxisX().second<<endl;
    axisVect_[index]=axis;
    cout<<"axis insertAxis2 "<<axisVect_[index].getAxisX().second<<endl;
}

template <typename M>
bool
MeshViewerWidgetT<M>::insert_mesh( const Mesh & pmesh , Axis3D axis){
    // bounding box

    meshVect_.push_back( pmesh );
    axisVect_.push_back(axis);
    showVect_.push_back( true );

    Mesh *mesh=&meshVect_.back();
    mesh->request_face_normals();
    mesh->request_face_colors();
    mesh->request_vertex_normals();
    mesh->request_vertex_colors();
    mesh->request_vertex_texcoords2D();


    // update face and vertex normals
    if ( ! opt_.check( IO::Options::FaceNormal ) )
        mesh->update_face_normals();
    else
        std::cout << "File provides face normals\n";

    if ( ! opt_.check( IO::Options::VertexNormal ) )
        mesh->update_vertex_normals();
    else
        std::cout << "File provides vertex normals\n";


    // check for possible color information
    if ( opt_.check( IO::Options::VertexColor ) )
    {
        std::cout << "File provides vertex colors\n";
        add_draw_mode("Colored Vertices");
    }
    else
        mesh->release_vertex_colors();

    if ( opt_.check( IO::Options::FaceColor ) )
    {
        std::cout << "File provides face colors\n";
        add_draw_mode("Solid Colored Faces");
        add_draw_mode("Smooth Colored Faces");
    }
    else
        mesh->release_face_colors();

    if ( opt_.check( IO::Options::VertexTexCoord ) )
        std::cout << "File provides texture coordinates\n";

    //bounding box
    typename Mesh::ConstVertexIter vIt(mesh->vertices_begin());
    typename Mesh::ConstVertexIter vEnd(mesh->vertices_end());

    typedef typename Mesh::Point Point;
    using OpenMesh::Vec3f;

    Vec3f bbMin, bbMax;

    bbMin = bbMax = OpenMesh::vector_cast<Vec3f>(mesh->point(vIt));

    for (size_t count=0; vIt!=vEnd; ++vIt, ++count)
    {
        bbMin.minimize( OpenMesh::vector_cast<Vec3f>(mesh->point(vIt)));
        bbMax.maximize( OpenMesh::vector_cast<Vec3f>(mesh->point(vIt)));
    }

    if( meshVect_.empty() ){
        bbMax_=bbMax;
        bbMin_=bbMin;
    }else{
        for(int i=0;i<3;i++){
            bbMax_[i]=std::max(bbMax_[i],bbMax[i]);
            bbMin_[i]=std::min(bbMin_[i],bbMin[i]);
        }
    }

    // set center and radius
    set_scene_pos( (bbMin_+bbMax_)*0.5, (bbMin_-bbMax_).norm()*0.5 );


    // info
    std::cout <<"Inserting mesh..."<<std::endl;
    std::clog << mesh->n_vertices() << " vertices, "
              << mesh->n_edges()    << " edge, "
              << mesh->n_faces()    << " faces\n";
#if defined(WIN32)
    updateGL();
#endif



    // loading done
    return true;

}

template <typename M>
bool 
MeshViewerWidgetT<M>::open_mesh(const char* _filename, IO::Options _opt, Axis3D axis )
{
    // load mesh
    // calculate normals
    // set scene center and radius
    Mesh mesh_;

    mesh_.request_face_normals();
    mesh_.request_face_colors();
    mesh_.request_vertex_normals();
    mesh_.request_vertex_colors();
    mesh_.request_vertex_texcoords2D();

    std::cout << "Loading from file '" << _filename << "'\n";
    if ( IO::read_mesh(mesh_, _filename, _opt ))
    {
        // store read option
        opt_ = _opt;


        // update face and vertex normals
        if ( ! opt_.check( IO::Options::FaceNormal ) )
            mesh_.update_face_normals();
        else
            std::cout << "File provides face normals\n";

        if ( ! opt_.check( IO::Options::VertexNormal ) )
            mesh_.update_vertex_normals();
        else
            std::cout << "File provides vertex normals\n";


        // check for possible color information
        if ( opt_.check( IO::Options::VertexColor ) )
        {
            std::cout << "File provides vertex colors\n";
            add_draw_mode("Colored Vertices");
        }
        else
            mesh_.release_vertex_colors();

        if ( _opt.check( IO::Options::FaceColor ) )
        {
            std::cout << "File provides face colors\n";
            add_draw_mode("Solid Colored Faces");
            add_draw_mode("Smooth Colored Faces");
        }
        else
            mesh_.release_face_colors();

        if ( _opt.check( IO::Options::VertexTexCoord ) )
            std::cout << "File provides texture coordinates\n";


        // bounding box
        typename Mesh::ConstVertexIter vIt(mesh_.vertices_begin());
        typename Mesh::ConstVertexIter vEnd(mesh_.vertices_end());

        typedef typename Mesh::Point Point;
        using OpenMesh::Vec3f;

        Vec3f bbMin, bbMax;

        bbMin = bbMax = OpenMesh::vector_cast<Vec3f>(mesh_.point(vIt));

        for (size_t count=0; vIt!=vEnd; ++vIt, ++count)
        {
            bbMin.minimize( OpenMesh::vector_cast<Vec3f>(mesh_.point(vIt)));
            bbMax.maximize( OpenMesh::vector_cast<Vec3f>(mesh_.point(vIt)));
        }

        if( meshVect_.empty() ){
            bbMax_=bbMax;
            bbMin_=bbMin;
        }else{
            for(int i=0;i<3;i++){
                bbMax_[i]=std::max(bbMax_[i],bbMax[i]);
                bbMin_[i]=std::min(bbMin_[i],bbMin[i]);
            }
        }

        // set center and radius
        set_scene_pos( (bbMin_+bbMax_)*0.5, (bbMin_-bbMax_).norm()*0.5 );


        // info
        std::clog << mesh_.n_vertices() << " vertices, "
                  << mesh_.n_edges()    << " edge, "
                  << mesh_.n_faces()    << " faces\n";

#if defined(WIN32)
        updateGL();
#endif

        setWindowTitle(QFileInfo(_filename).fileName());

        meshVect_.push_back( mesh_ );
        axisVect_.push_back( axis );
        showVect_.push_back( true );
        // loading done
        return true;
    }
    return false;
}


template <typename M>
void
MeshViewerWidgetT<M>::switchAxis(){
    show_axis_=!show_axis_;
    updateGL();
}

template <typename M>
void
MeshViewerWidgetT<M>::draw_openmesh(const std::string& _draw_mode ,bool fillColor )
{


    //Mesh mesh_=meshVect_.front();
    typename std::vector<Mesh>::iterator it;
    Axis3D axis;
    int xScale,yScale,zScale,xScaleN,yScaleN,zScaleN;

    xScale=80;
    yScale=80;
    zScale=80;

    /*
    xScale=(bbMax_[0]-bbMin_[0])/4;
    yScale=(bbMax_[1]-bbMin_[1])/4;
    zScale=(bbMax_[2]-bbMin_[2])/4;
    */
    xScaleN=0;
    yScaleN=0;
    zScaleN=0;

    int itColor=0;
    for( it=meshVect_.begin(); it<meshVect_.end(); it++ ){
        if( showVect_.at(itColor)){
            if( show_axis_){
                axis=axisVect_.at(itColor);



                glBegin(GL_LINES);
                //glColor3f( colormap_.getColor(itColor)[0],colormap_.getColor(itColor)[1] , colormap_.getColor(itColor)[2] );
                glColor3f(1,0,0);
                glVertex3f(axis.getAxisX().first[0]-axis.getAxisX().second[0]*xScaleN,
                           axis.getAxisX().first[1]-axis.getAxisX().second[1]*xScaleN,
                           axis.getAxisX().first[2]-axis.getAxisX().second[2]*xScaleN);
                glVertex3f(axis.getAxisX().first[0]+axis.getAxisX().second[0]*xScale,
                           axis.getAxisX().first[1]+axis.getAxisX().second[1]*xScale,
                           axis.getAxisX().first[2]+axis.getAxisX().second[2]*xScale);
                glEnd();

                glBegin(GL_LINES);
                //glColor3f( colormap_.getColor(itColor)[0],colormap_.getColor(itColor)[1] , colormap_.getColor(itColor)[2] );
                glColor3f(0,1,0);
                glVertex3f(axis.getAxisY().first[0]-axis.getAxisY().second[0]*yScaleN,
                            axis.getAxisY().first[1]-axis.getAxisY().second[1]*yScaleN,
                            axis.getAxisY().first[2]-axis.getAxisY().second[2]*yScaleN);
                 glVertex3f(axis.getAxisY().first[0]+axis.getAxisY().second[0]*yScale,
                            axis.getAxisY().first[1]+axis.getAxisY().second[1]*yScale,
                            axis.getAxisY().first[2]+axis.getAxisY().second[2]*yScale);
                glEnd();

                glBegin(GL_LINES);
                //glColor3f( colormap_.getColor(itColor)[0],colormap_.getColor(itColor)[1] , colormap_.getColor(itColor)[2] );
                glColor3f(0,0,1);
                glVertex3f(axis.getAxisZ().first[0]-axis.getAxisZ().second[0]*zScaleN,
                           axis.getAxisZ().first[1]-axis.getAxisZ().second[1]*zScaleN,
                           axis.getAxisZ().first[2]-axis.getAxisZ().second[2]*zScaleN);
                glVertex3f(axis.getAxisZ().first[0]+axis.getAxisZ().second[0]*zScale,
                           axis.getAxisZ().first[1]+axis.getAxisZ().second[1]*zScale,
                           axis.getAxisZ().first[2]+axis.getAxisZ().second[2]*zScale);
                glEnd();

                glColor3f( 0.0, 0.0, 0.0 );
            }

            typename Mesh::ConstFaceIter    fIt((*it).faces_begin()),
                    fEnd((*it).faces_end());

            typename Mesh::ConstFaceVertexIter fvIt;

#if defined(OM_USE_OSG) && OM_USE_OSG
            if (_draw_mode == "OpenSG Indices") // --------------------------------------
            {
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(3, GL_FLOAT, 0, (*it).points());

                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT, 0, (*it).vertex_normals());

                if ( tex_id_ && (*it).has_vertex_texcoords2D() )
                {
                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer(2, GL_FLOAT, 0, (*it).texcoords2D());
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, tex_id_);
                    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_mode_);
                }

                glDrawElements(GL_TRIANGLES,
                               (*it).osg_indices()->size(),
                               GL_UNSIGNED_INT,
                               &(*it).osg_indices()->getField()[0] );

                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_NORMAL_ARRAY);
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
            else
#endif

                if (_draw_mode == "Wireframe") // -------------------------------------------
                {
                    glBegin(GL_TRIANGLES);
                    if(!fillColor){
                        glColor3f( colormap_.getColor(itColor)[0],colormap_.getColor(itColor)[1] , colormap_.getColor(itColor)[2] );
                    }
                    for (; fIt!=fEnd; ++fIt)
                    {
                        fvIt = (*it).cfv_iter(fIt.handle());
                        glVertex3fv( &(*it).point(fvIt)[0] );
                        ++fvIt;
                        glVertex3fv( &(*it).point(fvIt)[0] );
                        ++fvIt;
                        glVertex3fv( &(*it).point(fvIt)[0] );
                    }
                    glEnd();
                }

                else if (_draw_mode == "Solid Flat") // -------------------------------------
                {
                    glBegin(GL_TRIANGLES);
                    for (; fIt!=fEnd; ++fIt)
                    {
                        glNormal3fv( &(*it).normal(fIt)[0] );

                        fvIt = (*it).cfv_iter(fIt.handle());
                        glVertex3fv( &(*it).point(fvIt)[0] );
                        ++fvIt;
                        glVertex3fv( &(*it).point(fvIt)[0] );
                        ++fvIt;
                        glVertex3fv( &(*it).point(fvIt)[0] );
                    }
                    glEnd();

                }


                else if (_draw_mode == "Solid Smooth") // -----------------------------------
                {
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(3, GL_FLOAT, 0, (*it).points());

                    glEnableClientState(GL_NORMAL_ARRAY);
                    glNormalPointer(GL_FLOAT, 0, (*it).vertex_normals());

                    if ( tex_id_ && (*it).has_vertex_texcoords2D() )
                    {
                        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                        glTexCoordPointer(2, GL_FLOAT, 0, (*it).texcoords2D());
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, tex_id_);
                        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_mode_);
                    }

                    glBegin(GL_TRIANGLES);
                    for (; fIt!=fEnd; ++fIt)
                    {
                        fvIt = (*it).cfv_iter(fIt.handle());
                        glArrayElement(fvIt.handle().idx());
                        ++fvIt;
                        glArrayElement(fvIt.handle().idx());
                        ++fvIt;
                        glArrayElement(fvIt.handle().idx());
                    }
                    glEnd();

                    glDisableClientState(GL_VERTEX_ARRAY);
                    glDisableClientState(GL_NORMAL_ARRAY);
                    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

                    if ( tex_id_ && (*it).has_vertex_texcoords2D() )
                    {
                        glDisable(GL_TEXTURE_2D);
                    }
                }

                else if (_draw_mode == "Colored Vertices") // --------------------------------
                {
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(3, GL_FLOAT, 0, (*it).points());

                    glEnableClientState(GL_NORMAL_ARRAY);
                    glNormalPointer(GL_FLOAT, 0, (*it).vertex_normals());

                    if ( (*it).has_vertex_colors() )
                    {
                        glEnableClientState( GL_COLOR_ARRAY );
                        glColorPointer(3, GL_UNSIGNED_BYTE, 0,(*it).vertex_colors());
                    }

                    glBegin(GL_TRIANGLES);
                    for (; fIt!=fEnd; ++fIt)
                    {
                        fvIt = (*it).cfv_iter(fIt.handle());
                        glArrayElement(fvIt.handle().idx());
                        ++fvIt;
                        glArrayElement(fvIt.handle().idx());
                        ++fvIt;
                        glArrayElement(fvIt.handle().idx());
                    }
                    glEnd();

                    glDisableClientState(GL_VERTEX_ARRAY);
                    glDisableClientState(GL_NORMAL_ARRAY);
                    glDisableClientState(GL_COLOR_ARRAY);
                }


                else if (_draw_mode == "Solid Colored Faces") // -----------------------------
                {
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(3, GL_FLOAT, 0, (*it).points());

                    glEnableClientState(GL_NORMAL_ARRAY);
                    glNormalPointer(GL_FLOAT, 0, (*it).vertex_normals());

                    glBegin(GL_TRIANGLES);
                    for (; fIt!=fEnd; ++fIt)
                    {
                        //glColor( fIt.handle() );
                        glColor3ubv( &(*it).color( fIt.handle() )[0] );

                        fvIt = (*it).cfv_iter(fIt.handle());
                        glArrayElement(fvIt.handle().idx());
                        ++fvIt;
                        glArrayElement(fvIt.handle().idx());
                        ++fvIt;
                        glArrayElement(fvIt.handle().idx());
                    }
                    glEnd();

                    glDisableClientState(GL_VERTEX_ARRAY);
                    glDisableClientState(GL_NORMAL_ARRAY);
                }


                else if (_draw_mode == "Smooth Colored Faces") // ---------------------------
                {
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(3, GL_FLOAT, 0, (*it).points());

                    glEnableClientState(GL_NORMAL_ARRAY);
                    glNormalPointer(GL_FLOAT, 0, (*it).vertex_normals());

                    glBegin(GL_TRIANGLES);
                    for (; fIt!=fEnd; ++fIt)
                    {
                        //glMaterial( fIt.handle() );
                        int _f=GL_FRONT_AND_BACK; int _m=GL_DIFFUSE;
                        OpenMesh::Vec3f c=OpenMesh::color_cast<OpenMesh::Vec3f>((*it).color(fIt.handle()));
                        OpenMesh::Vec4f m( c[0], c[1], c[2], 1.0f );
                        glMaterialfv(_f, _m, &m[0]);


                        fvIt = (*it).cfv_iter(fIt.handle());
                        glArrayElement(fvIt.handle().idx());
                        ++fvIt;
                        glArrayElement(fvIt.handle().idx());
                        ++fvIt;
                        glArrayElement(fvIt.handle().idx());
                    }
                    glEnd();

                    glDisableClientState(GL_VERTEX_ARRAY);
                    glDisableClientState(GL_NORMAL_ARRAY);
                }
                else if( _draw_mode == "Points" ) // -----------------------------------------
                {
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(3, GL_FLOAT, 0, (*it).points());

                    if ( (*it).has_vertex_colors() && use_color_)
                    {
                        glEnableClientState(GL_COLOR_ARRAY);
                        glColorPointer(3, GL_UNSIGNED_BYTE, 0, (*it).vertex_colors());
                    }else{

                        glColor3f( colormap_.getColor(itColor)[0],colormap_.getColor(itColor)[1] , colormap_.getColor(itColor)[2] );

                    }

                    glDrawArrays( GL_POINTS, 0, (*it).n_vertices() );
                    glDisableClientState(GL_VERTEX_ARRAY);
                    glDisableClientState(GL_COLOR_ARRAY);
                }
        }
        itColor+=1;

    }
}


//-----------------------------------------------------------------------------
template <typename M>
void 
MeshViewerWidgetT<M>::draw_scene(const std::string& _draw_mode)
{

    if ( meshVect_.empty() )
        return;

#if defined(OM_USE_OSG) && OM_USE_OSG
    else if ( _draw_mode == "OpenSG Indices")
    {
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        draw_openmesh( _draw_mode );
    }
    else
#endif
        if ( _draw_mode == "Points" )
        {
            glDisable(GL_LIGHTING);
            draw_openmesh(_draw_mode);
        }
        else if (_draw_mode == "Wireframe")
        {
            glDisable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            draw_openmesh(_draw_mode);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        else //if ( _draw_mode == "Hidden-Line" )
        {
            //Hidden-Line draw mode
            glDisable(GL_LIGHTING);
            glShadeModel(GL_FLAT);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
            glDepthRange(0.01, 1.0);
            draw_openmesh( "Wireframe", true );

            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
            //glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
            glDepthRange( 0.0, 1.0 );
            draw_openmesh( "Wireframe" );

            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
        }


}

