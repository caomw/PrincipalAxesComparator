
#ifndef LINALGTOOLS_H
#define	LINALGTOOLS_H

#include <Eigen/Core>
#include <Eigen/Eigen>

using namespace Eigen;

namespace LinalgTools {
    
    Vector3f projectPointOntoPlane (Vector3f point, Vector3f planePoint, Vector3f planeNormal);
    float distPointToLine (Vector3f o, Vector3f n, Vector3f p);
    Vector3f getMeanPoint (MatrixXf points);
    float getMedian (VectorXf v);
    VectorXf sortVector (VectorXf v, VectorXi & sortedIndexes);
    Vector3f computePca (MatrixXf DataPoints);
    
    MatrixXf computeRotationMatrix (Vector3f X1, Vector3f Y1, Vector3f Z1, \
                                    Vector3f X2, Vector3f Y2, Vector3f Z2);
    Vector3f translatePoint (Vector3f P, Vector3f X1, Vector3f Y1, Vector3f Z1, Vector3f O1, \
                                         Vector3f X2, Vector3f Y2, Vector3f Z2, Vector3f O2);

};

#endif	/* LINALGTOOLS_H */

