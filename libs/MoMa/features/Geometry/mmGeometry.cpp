#include "mmGeometry.h"

using namespace arma;
using namespace MoMa;


float Geometry::degToRad(float deg) {

    return(deg * M_PI / 180.0);
}

float Geometry::radToDeg(float rad) {

    return(rad * 180.0 / M_PI);
}

float Geometry::distance(const arma::vec &a, const arma::vec &b) {

    float dist;

    dist = sqrt(pow(a(X) - b(X), 2) + pow(a(Y) - b(Y), 2) + pow(a(Z) - b(Z), 2));

    return(dist);
}

vec Geometry::distance(const mat &a, const mat &b) {

    return sqrt(sum(pow(a - b, 2))).t();
}

TimedVec Geometry::distance(const TimedMat &a, const TimedMat &b) {

    vec tmp = sqrt(sum(pow(a.getData() - b.getData(), 2)).t());
    TimedVec ret;
    ret.setData(a.frameRate(), tmp, a.initialTime());
    return ret;
}

//Distance of point d to plane defined by points a, b and c
float Geometry::distanceToPlane(const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d) {

    float dist;

    vec ab, cb, n, dc;

    ab = a - b;
    cb = c - b;

    n = cross(ab, cb);

    n = normalise(n);

    dc = d - c;
    dist = dot(dc, n);

    return(dist);
}

//Distance between d and plane fixed in c and normal to vector a->b
float Geometry::distanceToNPlane(const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d) {

    float dist;

    vec n, dc;

    n = b - a;
    n = normalise(n);

    dc = d - c;
    dist = dot(dc, n);

    return(dist);

}

float Geometry::velocityToVect(const arma::vec &a, const arma::vec &b, const arma::vec &c1, const arma::vec &c2, const arma::vec &d1, const arma::vec &d2) {
    //Speed of d relative to c in direction determined by a->b
    float velo;

    vec vRel, n;

    vRel = (d2 - c2) - (d1 - c1);

    n = b - a;
    n = normalise(n);

    velo = dot(vRel, n);

    return(velo);
}

float Geometry::velocityToNPlane(const arma::vec &a1, const arma::vec &a2, const arma::vec &b, const arma::vec &c, const arma::vec &d1, const arma::vec &d2) {

    float velo;

    vec a2b, cb, n, vRel;

    a2b = a2 - b;
    cb = c - b;

    n = cross(cb, a2b);
    n = normalise(n);

    vRel = (d2 - a2) - (d1 - a1);

    velo = dot(vRel, n);

    return(velo);
}

float Geometry::angleBtwVectors(const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d) {

    vec v1, v2;
    float angle;

    v1 = b - a;
    v2 = d - c;

    angle = atan2(norm(cross(v1, v2)), arma::dot(v1, v2));

    return(angle);
}

float Geometry::angleBtwVectors2D(const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d) {

    vec v1, v2;
    float angle;

    v1 = b - a;
    v2 = d - c;
    //float tmp = arma::dot(v1,v2);
    //tmp=norm(v1);
    //tmp = norm(v2);
    //tmp = arma::dot(v1,v2)/(norm(v1)*norm(v2));
    angle = acos(arma::dot(v1, v2) / (norm(v1)*norm(v2)));

    return(angle);
}

mat Geometry::projection(const mat &a, const mat &b, const mat &c, const mat &p, bool invert)
{
    mat v1, v2, v3, proj;

    //Compute coordinate system
    v1 = b - a;
    v1 = normalise(v1);
    v2.copy_size(v1);
    v3.copy_size(v1);
    proj.copy_size(v1);

    for (int i = 0; i < v3.n_cols; ++i) {

        v3.col(i) = cross(v1.col(i), c.col(i) - a.col(i)); //v3 normal to plan
        v3.col(i) /= norm(v3.col(i));
        v2.col(i) = cross(v3.col(i), v1.col(i));

        //Compute projection matrix
        mat projmat(3, 3);
        projmat.col(0) = v1.col(i); projmat.col(1) = v2.col(i); projmat.col(2) = v3.col(i);

        if (invert == 0)
            //Compute projection
            proj.col(i) = ((p.col(i) - a.col(i)).t()*projmat).t();

        else {

            if (projmat.is_finite()) {

                //Compute inverse projection matrix
                mat invprojmat = pinv(projmat);

                //Compute inverse projection
                proj.col(i) = (p.col(i).t()*invprojmat).t() + a.col(i);
            }
            else {

                proj.col(i).fill(datum::nan);
            }
        }
    }

    return proj;
}

TimedMat Geometry::projection(const MoMa::TimedMat &a, const MoMa::TimedMat &b, const MoMa::TimedMat &c, const MoMa::TimedMat &p, bool invert)
{
    mat proj = projection(a.getData(), b.getData(), c.getData(), p.getData(), invert);
    if (!p.isTimed())
        return TimedMat(p.frameRate(), proj, p.initialTime());
    else
        return TimedMat(p.getTimeVec(), proj);
}

MoMa::Trace Geometry::projection(const MoMa::Trace &t1, const MoMa::Trace &t2, const MoMa::Trace &t3, const MoMa::Trace &p, bool invert)
{
    TimedMat proj = projection(t1.position, t2.position, t3.position, p.position, invert);

    Trace ret;
    ret.setTimeFlag(p.hasTime());
    ret.position = proj;
    return ret;
}

void MoMa::Geometry::translate(TimedCube &pos, double x, double y, double z) {

    //Works only on the 3D position TimedCube of the track
    vec translation;
    translation << x << y << z;

    cube translateCube(1, pos.nOfRows()*pos.nOfCols(), pos.nOfFrames()); //We use first the shape of a matrix so we can use the repmat function
    translateCube.tube(span(0), span::all) = repmat(translation, pos.nOfCols(), pos.nOfFrames()); //Replicate the vector to make a cube with the dimensions of the track
    translateCube.reshape(pos.nOfRows(), pos.nOfCols(), pos.nOfFrames()); //Reshape to the initial dimension

    //Todo : use each_col/row/slice methods of the arma::cube (not available in this version)

    pos.getRefData() += translateCube;
}

void MoMa::Geometry::translate(TimedMat &pos, double x, double y, double z) {

    //Works only on the 3D position TimedCube of the track
    vec translation;
    translation << x << y << z;

    mat translateMat = repmat(translation, 1, pos.nOfFrames()); //Replicate the vector to make a cube with the dimensions of the track

    //Todo : use each_col/row/slice methods of the arma::cube (not available in this version)

    pos.getRefData() += translateMat;
}

void MoMa::Geometry::translate(MoMa::Track & tr, double x, double y, double z)
{
    bool isLocal = false;
    if (!tr.hasGlobalCoordinate) {

        isLocal = true;
        tr.localToGlobal();
    }

    translate(tr.position, x, y, z);

    if (isLocal)
        tr.globalToLocal();
}


