
#include "MeshTranslator.h"

#include "LinalgTools.h"


MeshTranslator::MeshTranslator() {
}

MeshTranslator::MeshTranslator(const MeshTranslator& orig) {
}

MeshTranslator::~MeshTranslator() {
}

MyMesh MeshTranslator::translateMesh ( const MyMesh & mesh, Axis3D a1, Axis3D a2) {
    
    cout<<"axis x " <<a2.getAxisX().second<<endl;
    MyMesh rmesh(mesh);
    
    Vector3f p, pt;
    
    Vector3f X1, Y1, Z1, O1, X2, Y2, Z2, O2;

    X1 = Vector3f(a1.getAxisX().second[0],a1.getAxisX().second[1],a1.getAxisX().second[2]);

    Y1 = Vector3f ( a1.getAxisY().second[0],a1.getAxisY().second[1],a1.getAxisY().second[2] );
    Z1 = Vector3f ( a1.getAxisZ().second[0],a1.getAxisZ().second[1],a1.getAxisZ().second[2] );
    O1 = Vector3f ( a1.getAxisX().first[0],a1.getAxisX().first[1],a1.getAxisX().first[2] );

    X2 = Vector3f ( a2.getAxisX().second[0],a2.getAxisX().second[1],a2.getAxisX().second[2] );
    Y2 = Vector3f ( a2.getAxisY().second[0],a2.getAxisY().second[1],a2.getAxisY().second[2] );
    Z2 = Vector3f ( a2.getAxisZ().second[0],a2.getAxisZ().second[1],a2.getAxisZ().second[2] );
    O2 = Vector3f ( a2.getAxisX().first[0],a2.getAxisX().first[1],a2.getAxisX().first[2] );

    cout << "X1: " << endl << X1 << endl;
    
    MyMesh::VertexIter v_it, v_end(rmesh.vertices_end());
    
    for (v_it=rmesh.vertices_begin(); v_it!=v_end; ++v_it) {
        p <<      rmesh.point( v_it )[0], 
                  rmesh.point( v_it )[1], 
                  rmesh.point( v_it )[2];
        
        pt = LinalgTools::translatePoint(p, X1, Y1, Z1, O1, X2, Y2, Z2, O2);
        
        rmesh.point( v_it )[0] = pt(0);
        rmesh.point( v_it )[1] = pt(1);
        rmesh.point( v_it )[2] = pt(2);
        
    }
    
    return rmesh;
    
}
