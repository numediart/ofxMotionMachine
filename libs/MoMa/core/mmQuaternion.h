/*
 *  quaternion.h
 *
 *  This class is an adaptation for Armadillo (http://arma.sourceforge.net/) of ofQuaternion class
 *  provided by openframeworks.
 *
 *  ofQuaternion.h
 *  Created by Aaron Meyers on 6/22/09 -- modified by Arturo Castro, Zach Lieberman, Memo Akten
 *  based on code from OSG - 
 *  see OSG license for more details: 
 *  http://www.openscenegraph.org/projects/osg/wiki/Legal
 * 
 */

#ifndef __mmQuaternion__
#define __mmQuaternion__

#include <armadillo>
#include <cfloat> // #define DBL_EPSILON

#include "mmArmaUtils.h"

/*class quaternion : public arma::colvec
{
public:
    quaternion(void);
    quaternion(double x,double y,double z,double w);
    quaternion(arma::mat rotMat);
    ~quaternion(void);
};*/
namespace MoMa{

	double mmRadToDeg(double radians);
	double mmDegToRad(double degrees);
class quaternion : public arma::colvec4 {
public:

    
    inline quaternion();
    inline quaternion(double x, double y, double z, double w);
    inline quaternion(const arma::vec& v);
    inline quaternion(double angle, const arma::vec& axis);
    
    // rotation order is axis3,axis2,axis1
    inline quaternion(double angle1, const arma::vec& axis1, double angle2, const arma::vec& axis2, double angle3, const arma::vec& axis3);
    
    //inline quaternion& operator =(const quaternion& q);
    inline bool operator ==(const quaternion& q) const;
    inline bool operator !=(const quaternion& q) const;
     //Already implemented in arma::colvec
    
    
    inline arma::vec asVec4() const;

    
    inline void set(double x, double y, double z, double w);
    inline void set(const arma::vec& v);
    
    //matrix must give the axis coordinate transform from world to local referential.(Pay attention because there are different conventions
    //to encode Orientation Matrix.
    
    void set(const arma::mat& matrix);
	void get(arma::mat& matrix) const;
	void get(arma::mat44& matrix) const;
	void get(arma::mat33& matrix) const;
    
   // inline double& operator [](int i);
    //inline double operator [](int i) const;
    
    inline double& x();
    inline double& y();
    inline double& z();
    inline double& w();
    
    inline double x() const;
    inline double y() const;
    inline double z() const;
    inline double w() const;
    
    // return true if the Quat represents a zero rotation, and therefore can be ignored in computations.
    inline bool zeroRotation() const;
    
    
    
    // BASIC ARITHMETIC METHODS
    // Implemented in terms of Vec4s. Some Vec4 operators, e.g.
    // operator* are not appropriate for quaternions (as
    // mathematical objects) so they are implemented differently.
    // Also define methods for conjugate and the multiplicative inverse.
    
    inline const quaternion operator *(double rhs) const;                  // Multiply by scalar
    inline quaternion& operator *=(double rhs);                            // Unary multiply by scalar
    inline const quaternion operator*(const quaternion& rhs) const;     // Binary multiply
    inline quaternion& operator*=(const quaternion& rhs);               // Unary multiply
    inline quaternion operator /(double rhs) const;                        // Divide by scalar
    inline quaternion& operator /=(double rhs);                            // Unary divide by scalar
    inline const quaternion operator/(const quaternion& denom) const;   // Binary divide
    inline quaternion& operator/=(const quaternion& denom);             // Unary divide
    inline const quaternion operator +(const quaternion& rhs) const;    // Binary addition
    inline quaternion& operator +=(const quaternion& rhs);              // Unary addition
    inline const quaternion operator -(const quaternion& rhs) const;    // Binary subtraction
    inline quaternion& operator -=(const quaternion& rhs);              // Unary subtraction
    inline const quaternion operator -() const;                           // returns the negative of the quaternion. calls operator -() on the Vec4 */
    inline arma::vec operator*(const arma::vec& v) const;                       // Rotate a vector by this quaternion.
    
    
    // Length of the quaternion = sqrt(vec . vec)
    inline double length() const;
    
    // Length of the quaternion = vec . vec
    inline double length2() const;
    
    // Conjugate
    inline quaternion conj() const;
    
    // Multiplicative inverse method: q^(-1) = q^*/(q.q^*)
    inline const quaternion inverse() const;
    
    
    
    // METHODS RELATED TO ROTATIONS
    // Set a quaternion which will perform a rotation of an
    // angle around the axis given by the vector(x,y,z).
    
    // Define Spherical Linear interpolation method also
    void makeRotate(double angle, double x, double y, double z);
    void makeRotate(double angle, const arma::vec& vec);
    void makeRotate(double angle1, const arma::vec& axis1, double angle2, const arma::vec& axis2, double angle3, const arma::vec& axis3);
    
    
    // Make a rotation Quat which will rotate vec1 to vec2.
    // Generally take a dot product to get the angle between these
    // and then use a cross product to get the rotation axis
    // Watch out for the two special cases when the vectors
    // are co-incident or opposite in direction.
    void makeRotate(const arma::vec& vec1, const arma::vec& vec2);
    
    void makeRotate_original(const arma::vec& vec1, const arma::vec& vec2);
    
    // Return the angle and vector components represented by the quaternion.
    void getRotate(double&angle, double& x, double& y, double& z) const;
    void getRotate(double& angle, arma::vec& vec) const;
    
    // calculate and return the rotation as euler angles
    //the three components of the returned vector are the euler angles for the respective axis X, Y and Z (in this order and not in the rotation order)
    arma::vec getEuler( rotationOrder order) const;
    
    
    // Spherical Linear Interpolation.
    // As t goes from 0 to 1, the Quat object goes from "from" to "to".
    void slerp(double t, const quaternion& from, const quaternion& to);

    inline void normalize();
    double getRoll() const;
    double getPitch() const;
    double getYaw() const;
};


//----------------------------------------
quaternion::quaternion() {
    this->resize(4);
    (*this)<<0.0<<0.0<<0.0<<1.0;
}



//----------------------------------------
quaternion::quaternion(double x, double y, double z, double w) {
    (*this)<<x<<y<<z<<w;
}


//----------------------------------------
quaternion::quaternion(const arma::vec& v) {
    if (v.n_elem!=4)
        std::cout<<"quaternion::quaternion bad size for vector initialization"<<std::endl;
    this->resize(4);
    this->at(0)=v(0);
    this->at(1)=v(1);
    this->at(2)=v(2);
    this->at(3)=v(3);
}


//----------------------------------------
quaternion::quaternion(double angle, const arma::vec& axis) {
    this->resize(4);
    makeRotate(angle, axis);
}


//----------------------------------------
quaternion::quaternion(double angle1, const arma::vec& axis1, double angle2, const arma::vec& axis2, double angle3, const arma::vec& axis3) {
    makeRotate(angle1, axis1, angle2, axis2, angle3, axis3);
}


//----------------------------------------
/*quaternion& quaternion::operator =(const quaternion& q) {
    _v = q;
    return *this;
}*/


//----------------------------------------
bool quaternion::operator ==(const quaternion& q) const {
    return ((this->at(0)==q(0)&&this->at(1)==q(1)&&this->at(2)==q(2)&&this->at(3)==q(3))||(this->at(0)==-q(0)&&this->at(1)==-q(1)&&this->at(2)==-q(2)&&this->at(3)==-q(3)));
}


//----------------------------------------
bool quaternion::operator !=(const quaternion& q) const {
    return !((this->at(0)==q(0)&&this->at(1)==q(1)&&this->at(2)==q(2)&&this->at(3)==q(3))||(this->at(0)==-q(0)&&this->at(1)==-q(1)&&this->at(2)==-q(2)&&this->at(3)==-q(3)));
}


//----------------------------------------
//bool quaternion::operator <(const quaternion& q) const {
//    if((*this)(0) < v.(*this)(0)) return true;
//    else if((*this)(0) > v.(*this)(0)) return false;
//    else if((*this)(1) < v.(*this)(1)) return true;
//    else if((*this)(1) > v.(*this)(1)) return false;
//    else if((*this)(2) < v.(*this)(2)) return true;
//    else if((*this)(2) > v.(*this)(2)) return false;
//    else return ((*this)(3) < v.(*this)(3));
//}



//----------------------------------------
arma::vec quaternion::asVec4() const {
    return arma::vec(*this);
}



//----------------------------------------
void quaternion::set(double x, double y, double z, double w) {
    *this<<x<< y<<z<<w;
}


//----------------------------------------
void quaternion::set(const arma::vec& v) {
    *this = v;
}


//----------------------------------------
/*double& quaternion::operator [](int i) {
    return _v[i];
}



//----------------------------------------
double quaternion::operator [](int i) const {
    return _v[i];
}*/


//----------------------------------------
double& quaternion::x() {
    return (*this)(0);
}


//----------------------------------------
double& quaternion::y() {
    return (*this)(1);
}


//----------------------------------------
double& quaternion::z() {
    return (*this)(2);
}


//----------------------------------------
double& quaternion::w() {
    return (*this)(3);
}


//----------------------------------------
double quaternion::x() const {
    return (*this)(0);
}


//----------------------------------------
double quaternion::y() const {
    return (*this)(1);
}


//----------------------------------------
double quaternion::z() const {
    return (*this)(2);
}


//----------------------------------------
double quaternion::w() const {
    return (*this)(3);
}


//----------------------------------------
bool quaternion::zeroRotation() const {
    return (*this)(0) == 0.0 && (*this)(1) == 0.0 && (*this)(2) == 0.0 && (*this)(3) == 1.0;
}



//----------------------------------------
const quaternion quaternion::operator *(double rhs) const {
    return quaternion((*this)(0)*rhs, (*this)(1)*rhs, (*this)(2)*rhs, (*this)(3)*rhs);
}


//----------------------------------------
quaternion& quaternion::operator *=(double rhs) {
    (*this)(0) *= rhs;
    (*this)(1) *= rhs;
    (*this)(2) *= rhs;
    (*this)(3) *= rhs;
    return *this; // enable nesting
}


//----------------------------------------
const quaternion quaternion::operator*(const quaternion& rhs) const {
    return quaternion(
        rhs(0) * (*this)(3) + rhs(3) * (*this)(0) + rhs(2) * (*this)(1) - rhs(1) * (*this)(2),
        rhs(1) * (*this)(3) - rhs(2) * (*this)(0) + rhs(3) * (*this)(1) + rhs(0) * (*this)(2),
        rhs(2) * (*this)(3) + rhs(1) * (*this)(0) - rhs(0) * (*this)(1) + rhs(3) * (*this)(2),
        rhs(3) * (*this)(3) - rhs(0) * (*this)(0) - rhs(1) * (*this)(1) - rhs(2) * (*this)(2));
}


//----------------------------------------
quaternion& quaternion::operator*=(const quaternion& rhs) {
    double x = rhs(0) * (*this)(3) + rhs(3) * (*this)(0) + rhs(2) * (*this)(1) - rhs(1) * (*this)(2);
    double y = rhs(1) * (*this)(3) - rhs(2) * (*this)(0) + rhs(3) * (*this)(1) + rhs(0) * (*this)(2);
    double z = rhs(2) * (*this)(3) + rhs(1) * (*this)(0) - rhs(0) * (*this)(1) + rhs(3) * (*this)(2);
    double w = rhs(3) * (*this)(3) - rhs(0) * (*this)(0) - rhs(1) * (*this)(1) - rhs(2) * (*this)(2);
    
    (*this)(3) = w;
    (*this)(2) = z;
    (*this)(1) = y;
    (*this)(0) = x;
    return (*this); // enable nesting
}


//----------------------------------------
quaternion quaternion::operator /(double rhs) const {
    double div = 1.0 / rhs;
    return quaternion((*this)(0)*div, (*this)(1)*div, (*this)(2)*div, (*this)(3)*div);
}


//----------------------------------------
quaternion& quaternion::operator /=(double rhs) {
    double div = 1.0 / rhs;
    (*this)(0) *= div;
    (*this)(1) *= div;
    (*this)(2) *= div;
    (*this)(3) *= div;
    return *this;
}


//----------------------------------------
const quaternion quaternion::operator/(const quaternion& denom) const {
    return ((*this) * denom.inverse());
}


//----------------------------------------
quaternion& quaternion::operator/=(const quaternion& denom) {
    (*this) = (*this) * denom.inverse();
    return (*this); // enable nesting
}


//----------------------------------------
const quaternion quaternion::operator +(const quaternion& rhs) const {
    return quaternion((*this)(0) + rhs(0), (*this)(1) + rhs(1),
                        (*this)(2) + rhs(2), (*this)(3) + rhs(3));
}


//----------------------------------------
quaternion& quaternion::operator +=(const quaternion& rhs) {
    (*this)(0) += rhs(0);
    (*this)(1) += rhs(1);
    (*this)(2) += rhs(2);
    (*this)(3) += rhs(3);
    return *this; // enable nesting
}


//----------------------------------------
const quaternion quaternion::operator -(const quaternion& rhs) const {
    return quaternion((*this)(0) - rhs(0), (*this)(1) - rhs(1),
                        (*this)(2) - rhs(2), (*this)(3) - rhs(3));
}


//----------------------------------------
quaternion& quaternion::operator -=(const quaternion& rhs) {
    (*this)(0) -= rhs(0);
    (*this)(1) -= rhs(1);
    (*this)(2) -= rhs(2);
    (*this)(3) -= rhs(3);
    return *this; // enable nesting
}


//----------------------------------------
const quaternion quaternion::operator -() const {
    return quaternion(-(*this)(0), -(*this)(1), -(*this)(2), -(*this)(3));
}


//----------------------------------------
double quaternion::length() const {
    return sqrt((*this)(0)*(*this)(0) + (*this)(1)*(*this)(1) + (*this)(2)*(*this)(2) + (*this)(3)*(*this)(3));
}


//----------------------------------------
double quaternion::length2() const {
    return (*this)(0)*(*this)(0) + (*this)(1)*(*this)(1) + (*this)(2)*(*this)(2) + (*this)(3)*(*this)(3);
}


//----------------------------------------
quaternion quaternion::conj() const {
    return quaternion(-(*this)(0), -(*this)(1), -(*this)(2), (*this)(3));
}


//----------------------------------------
const quaternion quaternion::inverse() const {
    return conj() / length2();
}



//----------------------------------------
arma::vec quaternion::operator*(const arma::vec& v) const {
    // nVidia SDK implementation
    if (v.n_elem==3) {
        arma::vec uv, uuv;
        arma::vec qvec=this->subvec(0,2);//((*this)(0), (*this)(1), (*this)(2));
        uv = arma::cross(qvec,v);// qvec.getCrossed(v);    //uv = qvec ^ v;
        uuv = arma::cross(qvec,uv);//qvec.getCrossed(uv);    //uuv = qvec ^ uv;
        uv *= (2.0f * (*this)(3));
        uuv *= 2.0f;
        return v + uv + uuv;
    }
    else {
        std::cerr<<" quaternion::operator* input vector has a bad size"<<std::endl;
        return v;
    }
}

void quaternion::normalize(){
    double len = (*this)(3)*(*this)(3) + (*this)(0)*(*this)(0) + (*this)(1)*(*this)(1) + (*this)(2)*(*this)(2);
    double factor = 1.0f / sqrt(len);
    (*this)(0) *= factor;
    (*this)(1) *= factor;
    (*this)(2) *= factor;
    (*this)(3) *= factor;
}
}

#endif
