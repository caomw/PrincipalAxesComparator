#include "axis3d.h"

Axis3D::Axis3D(pair<Vec3f, Vec3f> paxisX, pair<Vec3f, Vec3f> paxisY, pair<Vec3f, Vec3f> paxisZ)
{

    axisX=paxisX;
    axisY=paxisY;
    axisZ=paxisZ;
}

pair< Vec3f,Vec3f > Axis3D::getAxisX() const{
    return axisX;
}

pair< Vec3f,Vec3f > Axis3D::getAxisY() const{
    return axisY;
}

pair< Vec3f,Vec3f > Axis3D::getAxisZ() const{
    return axisZ;
}
