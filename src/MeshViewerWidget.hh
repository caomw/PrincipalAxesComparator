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

#ifndef OPENMESHAPPS_VIEWERWIDGET_HH
#define OPENMESHAPPS_VIEWERWIDGET_HH

//== INCLUDES =================================================================

#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <OpenMesh/Tools/Utils/getopt.h>
#include <OpenMesh/Tools/Utils/Timer.hh>
#include <MeshViewerWidgetT.hh>

#include "mesh.h"

//== CLASS DEFINITION =========================================================

using namespace OpenMesh;  
using namespace OpenMesh::Attributes;




//== CLASS DEFINITION =========================================================

class MeshViewerWidget : public MeshViewerWidgetT<MyMesh>
{
    Q_OBJECT
signals:
    void mouseMoved(QMouseEvent*);
    void wheelMoved(QWheelEvent*);
    void mousePressed(QMouseEvent*);
    void mouseReleased(QMouseEvent*);
public:

    virtual void mouseMoveEvent( QMouseEvent* event ){

        QGLViewerWidget::mouseMoveEvent(event);
        emit mouseMoved(event);

    }

    virtual void mousePressEvent( QMouseEvent*event ){
        QGLViewerWidget::mousePressEvent(event);
        if( event->button() !=Qt::RightButton){

            emit mousePressed(event);
        }

    }

    virtual void mouseReleaseEvent( QMouseEvent*event ){
        QGLViewerWidget::mouseReleaseEvent(event);
        emit mouseReleased(event);
    }

    virtual void wheelEvent( QWheelEvent* event ){
        QGLViewerWidget::wheelEvent(event);
        emit wheelMoved(event);
    }

    /// default constructor
    MeshViewerWidget(QWidget* parent=0) : MeshViewerWidgetT<MyMesh>(parent)
    {}

    OpenMesh::IO::Options& options() { return _options; }
    const OpenMesh::IO::Options& options() const { return _options; }
    void setOptions(const OpenMesh::IO::Options& opts) { _options = opts; }

    void open_mesh_gui(QString fname)
    {
        OpenMesh::Utils::Timer t;
        t.start();
        if ( fname.isEmpty() || !open_mesh(fname.toLocal8Bit(), _options) )
        {
            QString msg = "Cannot read mesh from file:\n '";
            msg += fname;
            msg += "'";
            QMessageBox::critical( NULL, windowTitle(), msg);
        }
        t.stop();
        std::cout << "Loaded mesh in ~" << t.as_string() << std::endl;


    }


public slots:
    void wheelMoveSlot(QWheelEvent* event){
        QGLViewerWidget::wheelEvent(event);
    }

    void mouseMoveSlot(QMouseEvent* event){
        QGLViewerWidget::mouseMoveEvent(event);
    }

    void mousePresSlot(QMouseEvent* event){
        QGLViewerWidget::mousePressEvent(event);
    }

    void mouseReleaseSlot(QMouseEvent* event){
        QGLViewerWidget::mouseReleaseEvent(event);
    }

private:

    OpenMesh::IO::Options _options;
};


#endif
