
#ifndef MESHTRANSLATOR_H
#define	MESHTRANSLATOR_H

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <Eigen/Core>
#include <Eigen/Eigen>

#include "axis3d.h"
#include "mesh.h"

using namespace std;
using namespace Eigen;


class MeshTranslator {
public:
    MeshTranslator();
    MeshTranslator(const MeshTranslator& orig);
    virtual ~MeshTranslator();
    
    MyMesh translateMesh (const MyMesh & mesh, Axis3D a1, Axis3D a2);
private:

};

#endif	/* MESHTRANSLATOR_H */

