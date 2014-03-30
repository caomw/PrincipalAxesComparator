
#include "PrincipalAxisGenerator.h"

#include "LinalgTools.h"
#include <time.h>

using namespace LinalgTools;

PrincipalAxisGenerator::PrincipalAxisGenerator(Method m) {
    usedMethod = m;
    srand( time(NULL) );
}

PrincipalAxisGenerator::PrincipalAxisGenerator(const PrincipalAxisGenerator& orig) {
    vertexes = orig.vertexes;
    axis1 = orig.axis1;
    axis2 = orig.axis2;
    axis3 = orig.axis3;
    center = orig.center;
    
    usedMethod = orig.usedMethod;
}

PrincipalAxisGenerator::PrincipalAxisGenerator(const MyMesh mesh, Method m) {
    setMesh(mesh);
    usedMethod = m;
    srand( time(NULL) );
}

PrincipalAxisGenerator::~PrincipalAxisGenerator() {
}


void PrincipalAxisGenerator::setMesh (const MyMesh mesh) {
    vertexes = meshToMatrixXf(mesh);
}

Axis3D PrincipalAxisGenerator::getAxis () {
    
    Axis3D finalAxis;
    Vec3f c, a1, a2, a3;

    c = Vec3f(center(0), center(1), center(2));
    a1 = Vec3f(axis1(0), axis1(1), axis1(2));
    a2 = Vec3f(axis2.data());
    a3 = Vec3f(axis3.data());

    pair <Vec3f, Vec3f> axisX (c, a1);
    pair <Vec3f, Vec3f> axisY (c, a2);
    pair <Vec3f, Vec3f> axisZ (c, a3);

    finalAxis = Axis3D(axisX,axisY,axisZ);

    return finalAxis;

}

void PrincipalAxisGenerator::setMethod (Method m) {
    usedMethod = m;
}

void PrincipalAxisGenerator::compute () {
    
    switch (usedMethod) {
        case JUST_PCA:
            computeWithJustPCA();
            break;
        case ROBUS_DETERMINATION:
            computeWithRobustAxis();
            break;
    }

    /*
    for (int i = 0; i < 3; i++) {
        if (axis1(i) < 0)
            axis1(i) = axis1(i)*-1;
        if (axis2(i) < 0)
            axis2(i) = axis2(i)*-1;
        if (axis3(i) < 0)
            axis3(i) = axis3(i)*-1;
    }
*/

    cout << "final origin" << endl;
    cout << center << endl;
    cout << "final axis" << endl;
    cout << axis1 << endl;
    cout << "final second axis:" <<endl;
    cout << axis2 << endl;
    cout << "final third axis:" <<endl;
    cout << axis3 << endl;
}

void PrincipalAxisGenerator::computeWithJustPCA () {
    
    MatrixXf projectedVertex;
    
    axis1 = computePca(vertexes);
    center = getMeanPoint(vertexes);
    
    // Compute second axis with the points projected
    projectedVertex = projectAllPointsOntoPlane(vertexes, center, axis1);
    
    axis2 = computePca(projectedVertex);
    
    // Compute the third axis as the dot product of aixs1 and axis2
    axis3 = axis1.cross(axis2);
    
    
}

void PrincipalAxisGenerator::computeWithRobustAxis () {
    
    MatrixXf subSet, principalRegion, projectedVertex;
    VectorXf mask;
    
    subSet = getSubsetRANSAC(vertexes, 7, mask, 4000);
    principalRegion = forwardSearch(subSet, vertexes, mask);
    
    
    axis1 = computePca(principalRegion);
    center = getMeanPoint(principalRegion);
    
    
    // Compute second axis with the points projected
    projectedVertex = projectAllPointsOntoPlane(vertexes, center, axis1);
    
    subSet = getSubsetRANSAC(projectedVertex, 7, mask, 4000);
    principalRegion = forwardSearch(subSet, projectedVertex, mask);
    axis2 = computePca(principalRegion);
    
    
    // Compute the third axis as the dot product of aixs1 and axis2
    axis3 = axis1.cross(axis2);
    
    
}

MatrixXf PrincipalAxisGenerator::forwardSearch (MatrixXf subSet, MatrixXf &vertex, VectorXf &vertexMask) {
    
    float r, rmax, landa = 0.45;
    bool stop;
    unsigned int numAdded, m = 1, totalPoints;
    
    
    VectorXf origin, principalAxis, dist;
    VectorXf subSetMask;
    VectorXi originalIdx;
    
    // initialize principal region
    MatrixXf principalRegion (subSet.rows(), vertex.cols());
    // copy the initial subset to the final regions;
    principalRegion.block(0,0, subSet.rows(), subSet.cols()) = subSet;
    totalPoints = subSet.cols();
    
    cout << "fist subset: " << endl;
    cout << subSet << endl;
    
    principalAxis = computePca(subSet);
    cout << "first principal axe" << endl;
    cout << principalAxis << endl;
    
    origin = getMeanPoint(subSet);
    cout << "first origin" << endl;
    cout << origin << endl;
    
    //compute max distance from all points of the subset to the principal axis
    subSetMask = VectorXf::Zero(subSet.cols());
    cout << subSet << endl;
    dist = getDistances(subSet, subSetMask, origin, principalAxis, originalIdx);
        cout << "distances from the first subset to the fist axe" << endl;
        cout << dist << endl;
    
    // comute max distance to stop adding points
    r = dist.maxCoeff();
    rmax = landa*r;
    
    stop = false;
    // we stop when the min distance from the points to the fist axe is less
    // than rmax or if we already added all points (should not happen)
    while (!stop && totalPoints < vertex.cols()) {
        
        
        // get distance from all points to the fisrtaxe
        dist = getDistances(vertex, vertexMask, origin, principalAxis, originalIdx);
        
        VectorXi sortedIdx;
        VectorXf sortedDist = sortVector(dist, sortedIdx);
       // cout << "sorted dists" << endl;
       // cout << sortedDist <<endl;
       // getchar();
        
        //cout << "Min and max distances " << sortedDist(0) << " " << sortedDist(sortedDist.size() - 1) << endl;
        
        // add points with less distance
        numAdded = 0;
        while (numAdded < m && !stop) {
            
            //cout << "comparing: " << sortedDist(numAdded) << " and " << rmax << endl;
            //cout << "amount in maks :" << vertexMask.sum() << endl;
            
            
            // if the current distance from the point to the axe line
            // is smaller than the threshold we copy it to the principal region
            if (sortedDist(numAdded) < rmax) {
                //cout << "index to add " << originalIdx(sortedIdx(numAdded)) << endl;
                principalRegion.col(totalPoints) = vertex.col(originalIdx(sortedIdx(numAdded)));
                
                // mark vertex as part of the principal region
                // this way we wont compute its distance to the axes
                vertexMask(originalIdx(sortedIdx(numAdded))) = 1;
                totalPoints++;
                numAdded++;
            } else {
                // case were we are over the threshold so we need
                // to stop adding points
                stop = true;
            }
            
            subSet = principalRegion.block(0,0, principalRegion.rows(), totalPoints);
            principalAxis = computePca(subSet);
            origin = getMeanPoint(subSet);
            
            //cout << "num cols after addin: " << totalPoints << endl;
            //cout << "subset: " << endl;
            //cout << subSet << endl;
            //cout << "principal axis" << endl;
            //cout << principalAxis << endl;
            
            //getchar();
            //stop = true;
        }
        
        
        
    }
    
    
    return subSet;
    
    
}

MatrixXf PrincipalAxisGenerator::getSubsetRANSAC (MatrixXf &points, int sizeSubset, VectorXf &finalMask, int numIt) {
    
    float minMedian = numeric_limits<float>::max(), median;
    MatrixXf bestSubset, tempSubset;
    VectorXf origin, mAxe, dists;
    
    for (int i = 0; i < numIt; ++i) {
        VectorXf mask;
        mask = VectorXf::Zero(points.cols());
        //cout << "mask size & points cols" << mask.size() << points.cols() << endl;
        tempSubset = getRandomSubset(sizeSubset, points, mask);
        
        mAxe = computePca(tempSubset);
        origin = getMeanPoint(tempSubset);
        
        VectorXi originalIdx;
        dists = getDistances(points, mask, origin, mAxe, originalIdx);
        median = getMedian(dists);
        
        //cout << dists <<endl;
        //cout << tempSubset << endl;
        //cout << "median distance: " << endl;
        //cout << median << endl <<endl;
        //getchar();
        
        if (median < minMedian) {
            minMedian = median;
            bestSubset = tempSubset;
            finalMask = mask;
            //cout << mask.transpose () << endl;
            cout << "median Dist: " << minMedian << endl; 
        }
        
    }
    
    return bestSubset;
    
}

MatrixXf PrincipalAxisGenerator::projectAllPointsOntoPlane (MatrixXf points, Vector3f planePoint, Vector3f planeNormal) {
    
    MatrixXf projectedPonts (points.rows(), points.cols());
    
    //cout << "points. cols = " << points.cols() << endl;
    for (int i = 0; i < points.cols(); i++) {
         projectedPonts.col(i) = projectPointOntoPlane(Vector3f(points.col(i)), planePoint, planeNormal);
    }
    
    return projectedPonts;
}

VectorXf PrincipalAxisGenerator::getDistances (MatrixXf &points, VectorXf &mask, Vector3f origin, Vector3f mvec, VectorXi &originalIndex) {
    
    VectorXf dists(points.cols());
    VectorXi originalIdx_tmp (points.cols());
    
    int count = 0;
    for (int i = 0; i < points.cols(); ++i) {
        
        if (mask(i) <= 0) {
            //cout << squareDistPintToLine(origin, mvec, points.col(i)) << " ";
            dists(count) = distPointToLine(origin, mvec, points.col(i));
            originalIdx_tmp(count) = i;
            count++;
        }
           
    }
    
    originalIndex = originalIdx_tmp.head(count);
    return dists.head(count);
}

MatrixXf PrincipalAxisGenerator::getRandomSubset (int n, const MatrixXf &vertexs, VectorXf &mask) {
    
    // seed random with time
    //srand ( time(NULL) );
    
    //cout << "size mask: " << mask.size() << endl;
    
    MatrixXf choosen (3,n);
    for (int i = 0; i < n; i++) {
        
        unsigned int idx;
        do {
                idx = rand() % (vertexs.cols()-1);
        
        }while (mask(idx) == 1);
        //cout << idx << " ";
        choosen.col(i) = vertexs.col(idx);
        //cout << vertexs.col(idx).rows();
        mask(idx) =  1;
    }
    
    
    //cout << endl << mask.sum() << endl;
    //getchar();
    
    return choosen;
}

MatrixXf PrincipalAxisGenerator::meshToMatrixXf (const MyMesh  mesh) {
    
    MatrixXf vertex;
    
    vertex = MatrixXf(3, mesh.n_vertices());
    MyMesh::VertexIter v_it, v_end(mesh.vertices_end());
    
    int i = 0;
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it) {
        vertex.col(i++) <<      mesh.point( v_it )[0], 
                                mesh.point( v_it )[1], 
                                mesh.point( v_it )[2];
    }
    
    return vertex;
    
}
