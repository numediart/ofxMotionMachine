#ifndef __mmGeometry__
#define __mmGeometry__

#define _USE_MATH_DEFINES // for Visual Studio compatibility of M_PI
#include <cmath>
#include <armadillo>
#include "mmNode.h"
#include "MoMa.h"

namespace MoMa {
namespace Geometry {

float degToRad( float deg );

float radToDeg( float rad );
    
float distance( const arma::vec &a, const arma::vec &b );
arma::vec distance(const arma::mat &a, const arma::mat &b);
MoMa::TimedVec distance(const MoMa::TimedMat &a, const MoMa::TimedMat &b);

float distanceToPlane( const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d );

float distanceToNPlane( const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d );
    
float velocityToVect( const arma::vec &a, const arma::vec &b, const arma::vec &c1, const arma::vec &c2, const arma::vec &d1, const arma::vec &d2 );
    
float velocityToNPlane( const arma::vec &a1, const arma::vec &a2, const arma::vec &b, const arma::vec &c, const arma::vec &d1, const arma::vec &d2 );

float angleBtwVectors( const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d );

float angleBtwVectors2D( const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d );

arma::mat projection(const arma::mat & a, const arma::mat & b, const arma::mat & c, const arma::mat & p, bool invert = 0);
arma::mat quatprojection(const arma::mat & a, const arma::mat & b, const arma::mat & c, const arma::mat & q, bool invert = 0);
MoMa::TimedMat projection(const MoMa::TimedMat & a, const MoMa::TimedMat & b, const MoMa::TimedMat & c, const MoMa::TimedMat & p, bool invert = 0);
MoMa::TimedMat quatprojection(const MoMa::TimedMat & a, const MoMa::TimedMat & b, const MoMa::TimedMat & c, const MoMa::TimedMat & q, bool invert = 0);
//Project only positions (TODO on orientations)
MoMa::Trace projection(const MoMa::Trace & a, const MoMa::Trace & b, const MoMa::Trace & c, const MoMa::Trace & p, bool invert = 0);

//Project only positions (TODO on orientations)
MoMa::Track projection(const MoMa::Trace & a, const MoMa::Trace & b, const MoMa::Trace & c, const MoMa::Track & tr, bool invert = 0);

void translate(MoMa::TimedCube & pos, double x, double y, double z);
void translate(MoMa::TimedMat & pos, double x, double y, double z);
void translate(MoMa::Track &tr, double x, double y, double z);

//Translate at each frame (i.e. define new origin)
void translate(MoMa::Track &tr, arma::mat translation);

//Experimental

//Move coordinate system to initial frontal plane (i.e. first frame of the track), origin on LHip
void placeOnOrigin(MoMa::Track & tr, std::string Pelvis = "Pelvis", std::string LHip = "LHip", std::string RHip = "RHip");

arma::mat centerOfMassmat(MoMa::Track & tr);

MoMa::TimedMat centerOfMass(MoMa::Track & tr);

MoMa::Trace centerOfMassTrace(MoMa::Track & tr);

//Move origin to COM (at each frame)
void COMToOrigin(MoMa::Track & tr);

//Move coordinate system to frontal plane, origin between LHip and RHip
void stickToOrigin(MoMa::Track & tr, std::string LHip = "LHip", std::string RHip = "RHip");

//Move origin to Pelvis (at each frame), but do not change orientation of coordinate system
void stickToOriginLoose(MoMa::Track & tr, std::string rootnodename = "Pelvis");

//Scale track data (resize skeleton)
void scaleSkeleton(MoMa::Track & tr, float newsize = 1710); //1710mm = Belgian mean size (we are patriot and are proud of our chocolate)
}
}

#endif
