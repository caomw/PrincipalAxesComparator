
#ifndef PRINCIPALAXISGENERATOR_H
#define	PRINCIPALAXISGENERATOR_H

#include <Eigen/Core>
#include <Eigen/Eigen>

#include "axis3d.h"
#include "mesh.h"

using namespace std;
using namespace Eigen;



enum Method {ROBUS_DETERMINATION, JUST_PCA};

class PrincipalAxisGenerator {
    

    
public:
    
     
    
    PrincipalAxisGenerator(Method m = ROBUS_DETERMINATION);
    PrincipalAxisGenerator(const PrincipalAxisGenerator& orig);
    PrincipalAxisGenerator(const MyMesh mesh, Method m = ROBUS_DETERMINATION);
    virtual ~PrincipalAxisGenerator();
    
    void setMesh (const MyMesh mesh);
    Axis3D getAxis ();
    void setMethod (Method m);
    
    void compute ();
    
    
private:
    
    void computeWithRobustAxis ();
    void computeWithJustPCA ();

    MatrixXf getSubsetRANSAC (MatrixXf &points, int sizeSubset, VectorXf &finalMask, int numIt);
    MatrixXf forwardSearch (MatrixXf subSet, MatrixXf &vertex, VectorXf &vertexMask);
    MatrixXf projectAllPointsOntoPlane (MatrixXf points, Vector3f planePoint, Vector3f planeNormal);
    VectorXf getDistances (MatrixXf &points, VectorXf &mask, Vector3f origin, Vector3f mvec, VectorXi &originalIndex);
    MatrixXf getRandomSubset (int n, const MatrixXf &vertexs, VectorXf &mask);
    
    
    MatrixXf meshToMatrixXf (const MyMesh  mesh);
    
    MatrixXf vertexes;
    Vector3f axis1;
    Vector3f axis2;
    Vector3f axis3;
    Vector3f center;
    
    Method usedMethod;
};

#endif	/* PRINCIPAAXISGENERATOR_H */

