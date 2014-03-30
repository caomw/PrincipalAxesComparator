
#include "LinalgTools.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;

Vector3f LinalgTools::projectPointOntoPlane (Vector3f point, Vector3f planePoint, Vector3f planeNormal) {
    //q_proj = q - dot(q - p, n) * n
    Vector3f projPoint;
    projPoint = point.array() - ((point-planePoint).dot(planeNormal) * planeNormal.array());
    
    //cout << "dot product: " << (point - planePoint).dot(planeNormal) << endl;
    //cout << "From point " << point.transpose() << " whitd planePint " << planePoint.transpose() << " and plane normal " << planeNormal.transpose() << " we get " << projPoint.transpose() << endl;
    return projPoint;
    
}

float LinalgTools::distPointToLine (Vector3f o, Vector3f n, Vector3f p) {
    
    Vector3f vec;
    float dist;
    
    vec = (o.array()-p.array()) - (Vector3f(o.array()-p.array()).dot(n))*n.array();
    //vec = vec.array().square();
    
    //dist = vec.sum();
    //dist = sqrt(dist);
    dist = vec.norm();
    
    
    return dist;
}

Vector3f LinalgTools::getMeanPoint (MatrixXf points) {
    
    Vector3f ret;
    
        ret <<  points.row(0).sum(),
                points.row(1).sum(),
                points.row(2).sum();
    
        //cout << points << endl;
        //cout << ret.transpose()<< endl;
    ret /= points.cols();
    //cout << "origin: " << endl;
    //cout << ret.transpose() << endl;
    //getchar() ;
    
    return ret;
    
}


float LinalgTools::getMedian (VectorXf v) {
    
    
    float ret;
    int size = max(v.rows(), v.cols());
    
    VectorXi sortedIdx;
    VectorXf sorted = sortVector(v, sortedIdx);
    //std::sort(v.data(), v.data() + size);
    
    //cout << "size sorted: " << sorted.size() << endl;
    //cout << sorted << endl;
    if (size%2 == 0) 
        ret = (sorted(size/2 - 1) + sorted(size/2))/2;
    else 
        ret = sorted(size/2);
    
    //cout << "ordered distances:" << endl;
    //cout << sorted.transpose() << endl;
    //cout << "ret " << ret << endl;
    
    //getchar();
    return ret;
}

VectorXf LinalgTools::sortVector (VectorXf v, VectorXi & sortedIndexes) {
    
    
        typedef std::pair <float,unsigned int> myPair;
        typedef std::vector <myPair> PermutationIndices;
        
        // allocate space for the sorted vector
        VectorXf sortedVector = VectorXf(v.size());
        sortedIndexes = VectorXi (sortedVector.size());
        
        // copy vector to a std::vector with pairs with the index
        // this way we get the the intex of each element in the original vector
        PermutationIndices pi;
        for (unsigned int i = 0 ; i < v.size(); i++)
                pi.push_back(std::make_pair(v(i), i));

        std::sort(pi.begin(), pi.end());
        
        // copy sorted vector and indexes from std::vector to VectorX
        for (unsigned int i = 0 ; i < v.size(); i++) {
            sortedVector(i) = pi[i].first;
            sortedIndexes(i) = pi[i].second;
        }
        
        return sortedVector;
}

Vector3f LinalgTools::computePca (MatrixXf DataPoints) {
  double mean; VectorXf meanVector;
  Vector3f ret;

  typedef std::pair <double, int> myPair;
  typedef std::vector <myPair> PermutationIndices;
  
  
  
  unsigned int m = DataPoints.rows();   // dimension of each point
  unsigned int n = DataPoints.cols();

  //
  // for each point
  //   center the poin with the mean among all the coordinates
  //
  for (int i = 0; i < DataPoints.rows(); i++){
	   mean = (DataPoints.row(i).sum())/n;		 //compute mean
	   meanVector  = VectorXf::Constant(n,mean); // create a vector with constant value = mean
	   DataPoints.row(i) -= meanVector;
	  // std::cout << meanVector.transpose() << "\n" << DataPoints.col(i).transpose() << "\n\n";
  }

  // get the covariance matrix
  MatrixXf Covariance = MatrixXf::Zero(m, m);
  Covariance = (1 / (float) n) * DataPoints * DataPoints.transpose();
  //std::cout << "covariance: " << std::endl;
  //std::cout << Covariance << std::endl;
  

  // compute the eigenvalue on the Cov Matrix
  EigenSolver<MatrixXf> m_solve(Covariance);
//  std::cout << "Done" << std::endl;
  VectorXf eigenvalues = VectorXf::Zero(m);
  eigenvalues = m_solve.eigenvalues().real();
  
  MatrixXf eigenVectors = MatrixXf::Zero(n, m);  // matrix (n x m) (points, dims)
  eigenVectors = m_solve.eigenvectors().real();	

  
  //std::cout << "eigenvalues: " << std::endl;
  //std::cout << eigenvalues << std::endl;
  //std::cout << "eigenvectors: " << std::endl;
  //std::cout << eigenVectors << std::endl;
  
          
  // sort and get the permutation indices
  PermutationIndices pi;
  for (int i = 0 ; i < m; i++)
	  pi.push_back(std::make_pair(eigenvalues(i), i));

  std::sort(pi.begin(), pi.end());

  //for (unsigned int i = 0; i < pi.size() ; i++)
  //	  std::cout << "eigen=" << pi[i].first << " pi=" << pi[i].second << std::endl;
 
  
  //std::cout << eigenVectors << std::endl;
  
  int idx = pi[m-1].second;
  ret = eigenVectors.col(idx);

  if (ret(2) < 0)
      ret = ret*-1;
  
  return ret;
  
}

MatrixXf LinalgTools::computeRotationMatrix (Vector3f X1, Vector3f Y1, Vector3f Z1, \
                                Vector3f X2, Vector3f Y2, Vector3f Z2) {
    
    MatrixXf A (9,9), AInv (9,9);
    MatrixXf b (9,1);
    
    MatrixXf x;
    MatrixXf R (3,3);
    
    A.fill(0);
    
    A.block(0,0,1,3) = X1.transpose();
    A.block(1,3,1,3) = X1.transpose();
    A.block(2,6,1,3) = X1.transpose();
    
    A.block(3,0,1,3) = Y1.transpose();
    A.block(4,3,1,3) = Y1.transpose();
    A.block(5,6,1,3) = Y1.transpose();
    
    A.block(6,0,1,3) = Z1.transpose();
    A.block(7,3,1,3) = Z1.transpose();
    A.block(8,6,1,3) = Z1.transpose();
    
    //cout << "A: " << endl << A << endl;
    
    
    b << X2 , Y2 , Z2;
    //cout << "b: " << endl << b << endl; 
    
    
    AInv = A.inverse();
    //cout << "Ainv: " << endl << AInv << endl;
    
    
    x = AInv * b;
    R  = Map<MatrixXf> (x.data(),3,3).transpose();
    
    return R;
}

Vector3f LinalgTools::translatePoint (Vector3f P, Vector3f X1, Vector3f Y1, Vector3f Z1, Vector3f O1, \
                                                  Vector3f X2, Vector3f Y2, Vector3f Z2, Vector3f O2) {
    MatrixXf R;
    Vector3f tP, RtP;
    
    R = computeRotationMatrix(X1, Y1, Z1, X2, Y2, Z2);
    
    // Get back to the real origin
    tP = P - O1;
    
    // Apply translation and get to the new origin
    RtP = R*tP + O2;
    
    return RtP;
    
}
