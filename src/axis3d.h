#ifndef AXIS_H
#define AXIS_H
#include<OpenMesh/Core/Geometry/VectorT.hh>
#include<iostream>
using namespace std;
using namespace OpenMesh;
class Axis3D
{
    //pair represents <originPoint, Vector>
    pair< Vec3f,Vec3f > axisX;
    pair< Vec3f,Vec3f > axisY;
    pair< Vec3f,Vec3f > axisZ;

public:
    Axis3D(pair< Vec3f,Vec3f > paxisX= pair< Vec3f,Vec3f >( Vec3f(0,0,0),Vec3f(0,0,0) ),
         pair< Vec3f,Vec3f > paxisY= pair< Vec3f,Vec3f >( Vec3f(0,0,0),Vec3f(0,0,0) ),
         pair< Vec3f,Vec3f > paxisZ = pair< Vec3f,Vec3f >( Vec3f(0,0,0),Vec3f(0,0,0) ) );

    Axis3D(const Axis3D & a ){
        axisX=a.axisX;
        axisY=a.axisY;
        axisZ=a.axisZ;
    }

   pair< Vec3f,Vec3f > getAxisX() const;
   pair< Vec3f,Vec3f > getAxisY() const;
   pair< Vec3f,Vec3f > getAxisZ() const;
};

#endif // AXIS_H
