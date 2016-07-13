#include "mmGeometry.h"

using namespace arma;
using namespace MoMa;


float Geometry::degToRad( float deg ) {
    
    return( deg * M_PI / 180.0 );
}

float Geometry::radToDeg( float rad ) {
    
    return( rad * 180.0 / M_PI );
}

float Geometry::distance( const arma::vec &a, const arma::vec &b ) {
    
    float dist;
    
    dist = sqrt( pow( a(X)-b(X), 2 ) + pow( a(Y)-b(Y), 2 ) + pow( a(Z)-b(Z), 2 ) );
    
    return( dist );
}

vec Geometry::distance(const mat &a, const mat &b) {

    return sqrt(sum(pow(a - b, 2)));
}

TimedVec Geometry::distance(const TimedMat &a, const TimedMat &b) {

    vec tmp = sqrt(sum(pow(a.getData() - b.getData(), 2)).t());
    TimedVec ret;
    ret.setData(a.frameRate(), tmp);
    return ret;
}

float Geometry::distanceToPlane( const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d ) {
    
    float dist;
    
    vec ab, cb, n, dc;
    
    ab = a - b;
    cb = c - b;
    
    n = cross( ab, cb );
    
    n = normalise( n );
    
    dc = d - c;
    dist = dot( dc, n );
    
    return( dist );
}

float Geometry::distanceToNPlane( const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d ) {
    
    float dist;
    
    vec n, dc;
    
    n = b-a;
    n = normalise( n );
    
    dc = d-c;
    dist = dot( dc, n );
    
    return( dist );
    
}

float Geometry::velocityToVect( const arma::vec &a, const arma::vec &b, const arma::vec &c1, const arma::vec &c2, const arma::vec &d1, const arma::vec &d2 ) {
    //Speed of d relative to c in direction determined by a->b
    float velo;
    
    vec vRel, n;
    
    vRel = ( d2-c2 ) - ( d1-c1 );
    
    n = b-a;
    n = normalise( n );
    
    velo = dot( vRel, n );
    
    return( velo );
}

float Geometry::velocityToNPlane( const arma::vec &a1, const arma::vec &a2, const arma::vec &b, const arma::vec &c, const arma::vec &d1, const arma::vec &d2 ) {
    
    float velo;
    
    vec a2b, cb, n, vRel;
    
    a2b = a2-b;
    cb = c-b;
    
    n = cross( cb, a2b );
    n = normalise( n );
    
    vRel = ( d2-a2 ) - ( d1-a1 );
    
    velo = dot( vRel, n );
    
    return( velo );
}

float Geometry::angleBtwVectors( const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d ) {
    
    vec v1, v2;
    float angle;
    
    v1 = b - a;
    v2 = d - c;
    
    angle = atan2( norm( cross( v1, v2 ) ), arma::dot( v1, v2 ) );
    
    return( angle );
}

float Geometry::angleBtwVectors2D( const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d ) {
    
    vec v1, v2;
    float angle;
    
    v1 = b - a;
    v2 = d - c;
    float tmp = arma::dot(v1,v2);
    tmp=norm(v1);
    tmp = norm(v2);
    tmp = arma::dot(v1,v2)/(norm(v1)*norm(v2));
    angle = acos(arma::dot(v1,v2)/(norm(v1)*norm(v2)));
    
    return( angle );
}