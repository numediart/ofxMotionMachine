#include "mmQuaternion.h"

namespace MoMa {

//--------------------------------------------------
double mmRadToDeg(double radians) {
    return radians * 180.0/arma::datum::pi;
}

//--------------------------------------------------
double mmDegToRad(double degrees) {
    return degrees * arma::datum::pi/180.0;
}

void quaternion::set(const arma::mat& _matrix) {
    //ofQuaternion q;
    if ((_matrix.n_cols!=4||_matrix.n_rows!=4)&&(_matrix.n_cols!=3||_matrix.n_rows!=3)){
        std::cerr<<"MoMa::quaternion::set: Input Matrix has bad dimension"<<std::endl;
        return;
    }
    double QW,QX,QY,QZ;
    double s;
    double tq[4];
    int    i, j;

    // Use tq to store the largest trace
    tq[0] = 1 + _matrix(0,0)+_matrix(1,1)+_matrix(2,2);
    tq[1] = 1 + _matrix(0,0)-_matrix(1,1)-_matrix(2,2);
    tq[2] = 1 - _matrix(0,0)+_matrix(1,1)-_matrix(2,2);
    tq[3] = 1 - _matrix(0,0)-_matrix(1,1)+_matrix(2,2);

    // Find the maximum (could also use stacked if's later)
    j = 0;
    for(i=1;i<4;i++) j = (tq[i]>tq[j])? i : j;

    // check the diagonal
    if (j==0)
    {
        /* perform instant calculation */
        QW = tq[0];
        QX = _matrix(2,1)-_matrix(1,2);
        QY = _matrix(0,2)-_matrix(2,0);
        QZ = _matrix(1,0)-_matrix(0,1);
    }
    else if (j==1)
    {
        QW = _matrix(2,1)-_matrix(1,2);
        QX = tq[1];
        QY = _matrix(1,0)+_matrix(0,1);
        QZ = _matrix(0,2)+_matrix(2,0);
    }
    else if (j==2)
    {
        QW = _matrix(0,2)-_matrix(2,0);
        QX = _matrix(1,0)+_matrix(0,1);
        QY = tq[2];
        QZ = _matrix(2,1)+_matrix(1,2);
    }
    else /* if (j==3) */
    {
        QW = _matrix(1,0)-_matrix(0,1);
        QX = _matrix(0,2)+_matrix(2,0);
        QY = _matrix(2,1)+_matrix(1,2);
        QZ = tq[3];
    }

    s = sqrt(0.25/tq[j]);
    QW *= s;
    QX *= s;
    QY *= s;
    QZ *= s;

    (*this)<<QX<<QY<<QZ<<QW;
//  *this = matrix.getRotate();
}

void quaternion::get(arma::mat& _matrix) const {
	arma::mat44 temp;
	this->get(temp);
	_matrix = temp;
}
void quaternion::get(arma::mat44& _matrix) const {
	arma::mat33 tempMatrix;
	this->get(tempMatrix);

	_matrix = arma::eye(4, 4);
	_matrix.submat(0, 0, 2, 2)=tempMatrix;
}
void quaternion::get(arma::mat33& _matrix) const {
    _matrix=arma::eye(3,3);
    double QX=(*this)(0);
    double QY=(*this)(1);
    double QZ=(*this)(2);
    double QW=(*this)(3);
    double length2 = this->length2();
    if (fabs(length2) > std::numeric_limits<double>::min())
    {
        double rlength2;
        // normalize quat if required.
        // We can avoid the expensive sqrt in this case since all 'coefficients' below are products of two q components.
        // That is a square of a square root, so it is possible to avoid that
        if (length2 != 1.0)
        {
            rlength2 = 2.0/length2;
        }
        else
        {
            rlength2 = 2.0;
        }

        // Source: Gamasutra, Rotating Objects Using Quaternions
        //
        //http://www.gamasutra.com/features/19980703/quaternions_01.htm

        double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

        // calculate coefficients
        x2 = rlength2*QX;
        y2 = rlength2*QY;
        z2 = rlength2*QZ;

        xx = QX * x2;
        xy = QX * y2;
        xz = QX * z2;

        yy = QY * y2;
        yz = QY * z2;
        zz = QZ * z2;

        wx = QW * x2;
        wy = QW * y2;
        wz = QW * z2;

        // Note.  Gamasutra gets the matrix assignments inverted, resulting
        // in left-handed rotations, which is contrary to OpenGL and OSG's
        // methodology.  The matrix assignment has been altered in the next
        // few lines of code to do the right thing.
        // Don Burns - Oct 13, 2001
        _matrix(0,0) = 1.0 - (yy + zz);
        _matrix(0,1) = xy - wz;
        _matrix(0,2) = xz + wy;


        _matrix(1,0) = xy + wz;
        _matrix(1,1) = 1.0 - (xx + zz);
        _matrix(1,2) = yz - wx;

        _matrix(2,0) = xz - wy;
        _matrix(2,1) = yz + wx;
        _matrix(2,2) = 1.0 - (xx + yy);
    }
    //matrix.makeRotationMatrix(*this);
}


/// Set the elements of the Quat to represent a rotation of angle
/// (degrees) around the axis (x,y,z)
void quaternion::makeRotate( double angle, double x, double y, double z ) {
    angle = mmDegToRad(angle);
    
    const double epsilon = 0.0000001f;

    double length = sqrtf( x * x + y * y + z * z );
    if (length < epsilon) {
        // ~zero length axis, so reset rotation to zero.
        *this = quaternion();
        return;
    }

    double inversenorm  = 1.0f / length;
    double coshalfangle = cosf( 0.5f * angle );
    double sinhalfangle = sinf( 0.5f * angle );

    (*this)(0) = x * sinhalfangle * inversenorm;
    (*this)(1) = y * sinhalfangle * inversenorm;
    (*this)(2) = z * sinhalfangle * inversenorm;
    (*this)(3) = coshalfangle;
}


void quaternion::makeRotate( double angle, const arma::vec& _vec ) {
    if (_vec.n_elem==3||_vec.n_elem==4)
        makeRotate( angle, _vec(0), _vec(1), _vec(2) );
    else
        std::cerr<<"MoMa::quaternion::makeRotate: bad size for input vector"<<std::endl;

}


void quaternion::makeRotate ( double angle1, const arma::vec& axis1,
                              double angle2, const arma::vec& axis2,
                              double angle3, const arma::vec& axis3) {
    quaternion q1; q1.makeRotate(angle1,axis1);
    quaternion q2; q2.makeRotate(angle2,axis2);
    quaternion q3; q3.makeRotate(angle3,axis3);

    *this = q3*q2*q1;
}

/** Make a rotation Quat which will rotate vec1 to vec2

 This routine uses only fast geometric transforms, without costly acos/sin computations.
 It's exact, fast, and with less degenerate cases than the acos/sin method.

 For an explanation of the math used, you may see for example:
 http://logiciels.cnes.fr/MARMOTTES/marmottes-mathematique.pdf

 @note This is the rotation with shortest angle, which is the one equivalent to the
 acos/sin transform method. Other rotations exists, for example to additionally keep
 a local horizontal attitude.

 @author Nicolas Brodu
 */
void quaternion::makeRotate( const arma::vec& from, const arma::vec& to ) {

    // This routine takes any vector as argument but normalized
    // vectors are necessary, if only for computing the dot product.
    // Too bad the API is that generic, it leads to performance loss.
    // Even in the case the 2 vectors are not normalized but same length,
    // the sqrt could be shared, but we have no way to know beforehand
    // at this point, while the caller may know.
    // So, we have to test... in the hope of saving at least a sqrt
    arma::vec sourceVector = from;
    if (sourceVector.n_rows==1)
        sourceVector=sourceVector.t();
    arma::vec targetVector = to;
    if (targetVector.n_rows==1)
        targetVector=targetVector.t();
    double n=arma::norm(from);
    double fromLen2 = n*n;
    double fromLen;
    // normalize only when necessary, epsilon test
    if ((fromLen2 < 1.0 - 1e-7) || (fromLen2 > 1.0 + 1e-7)) {
        fromLen = sqrt(fromLen2);
        sourceVector /= fromLen;
    } else fromLen = 1.0;
    n=arma::norm(to);
    double toLen2 = n*n;
    // normalize only when necessary, epsilon test
    if ((toLen2 < 1.0 - 1e-7) || (toLen2 > 1.0 + 1e-7)) {
        double toLen;
        // re-use fromLen for case of mapping 2 vectors of the same length
        if ((toLen2 > fromLen2 - 1e-7) && (toLen2 < fromLen2 + 1e-7)) {
            toLen = fromLen;
        } else toLen = sqrt(toLen2);
        targetVector /= toLen;
    }

    // Now let's get into the real stuff
    // Use "dot product plus one" as test as it can be re-used later on
    double dotProdPlus1 = 1.0 + arma::dot(sourceVector,targetVector);

    // Check for degenerate case of full u-turn. Use epsilon for detection
    if (dotProdPlus1 < 1e-7) {

        // Get an orthogonal vector of the given vector
        // in a plane with maximum vector coordinates.
        // Then use it as quaternion axis with pi angle
        // Trick is to realize one value at least is >0.6 for a normalized vector.
        if (fabs(sourceVector(0)) < 0.6) {
            const double norm = sqrt(1.0 - sourceVector(0) * sourceVector(0));
            (*this)(0) = 0.0;
            (*this)(1) = sourceVector(2) / norm;
            (*this)(2) = -sourceVector(1) / norm;
            (*this)(3) = 0.0;
        } else if (fabs(sourceVector(1)) < 0.6) {
            const double norm = sqrt(1.0 - sourceVector(1) * sourceVector(1));
            (*this)(0) = -sourceVector(2) / norm;
            (*this)(1) = 0.0;
            (*this)(2) = sourceVector(0) / norm;
            (*this)(3) = 0.0;
        } else {
            const double norm = sqrt(1.0 - sourceVector(2) * sourceVector(2));
            (*this)(0) = sourceVector(1) / norm;
            (*this)(1) = -sourceVector(0) / norm;
            (*this)(2) = 0.0;
            (*this)(3) = 0.0;
        }
    }

    else {
        // Find the shortest angle quaternion that transforms normalized vectors
        // into one other. Formula is still valid when vectors are colinear
        const double s = sqrt(0.5 * dotProdPlus1);
        const arma::vec tmp = arma::cross(sourceVector,targetVector) / (2.0 * s);
        (*this)(0) = tmp(0);
        (*this)(1) = tmp(1);
        (*this)(2) = tmp(2);
        (*this)(3) = s;
    }
}


// Make a rotation Quat which will rotate vec1 to vec2
// Generally take adot product to get the angle between these
// and then use a cross product to get the rotation axis
// Watch out for the two special cases of when the vectors
// are co-incident or opposite in direction.
void quaternion::makeRotate_original( const arma::vec& from, const arma::vec& to ) {
    const double epsilon = 0.0000001f;

    double length1  = from.n_elem;
    double length2  = to.n_elem;

    // dot product vec1*vec2
    double cosangle = arma::dot(from,to) / (length1 * length2);

    if ( fabs(cosangle - 1) < epsilon ) {
        //osg::notify(osg::INFO)<<"*** Quat::makeRotate(from,to) with near co-linear vectors, epsilon= "<<fabs(cosangle-1)<<std::endl;

        // cosangle is close to 1, so the vectors are close to being coincident
        // Need to generate an angle of zero with any vector we like
        // We'll choose (1,0,0)
        makeRotate( 0.0, 0.0, 0.0, 1.0 );
    } else {
        if ( fabs(cosangle + 1.0) < epsilon ) {
            // vectors are close to being opposite, so will need to find a
            // vector orthongonal to from to rotate about.
            arma::vec tmp(3);
            if (fabs(from(0)) < fabs(from(1)))
                if (fabs(from(0)) < fabs(from(2))) tmp<<1.0<<0.0<<0.0; // use x axis.
                else tmp<<0.0<<0.0<<1.0;
            else if (fabs(from(1)) < fabs(from(2))) tmp<<0.0<<1.0<<0.0;
            else tmp<<0.0<<0.0<<1.0;

            arma::vec fromd(3);
            fromd<<from(0)<< from(1)<< from(2);

            // find orthogonal axis.
            arma::vec axis=arma::cross(fromd,tmp);
            arma::normalise(axis);

            (*this)(0) = axis[0]; // sin of half angle of PI is 1.0.
            (*this)(1) = axis[1]; // sin of half angle of PI is 1.0.
            (*this)(2) = axis[2]; // sin of half angle of PI is 1.0.
            (*this)(3) = 0; // cos of half angle of PI is zero.

        } else {
            // This is the usual situation - take a cross-product of vec1 and vec2
            // and that is the axis around which to rotate.
            arma::vec axis=arma::cross(from,to);
            double angle = acos( cosangle );
            makeRotate( angle, axis );
        }
    }
}

void quaternion::getRotate( double& angle, arma::vec& _vec ) const {
    if (_vec.n_elem!=3)
        _vec=arma::vec(3);
    double x, y, z;
    getRotate(angle, x, y, z);
    _vec(0) = x;
    _vec(1) = y;
    _vec(2) = z;
}
// Get the angle of rotation and axis of this Quat object.
// Won't give very meaningful results if the Quat is not associated
// with a rotation!
void quaternion::getRotate( double& angle, double& x, double& y, double& z ) const {
    double sinhalfangle = sqrt( (*this)(0) * (*this)(0) + (*this)(1) * (*this)(1) + (*this)(2) * (*this)(2) );

    angle = 2.0 * atan2( sinhalfangle, (*this)(3) );
    if (sinhalfangle) {
        x = (*this)(0) / sinhalfangle;
        y = (*this)(1) / sinhalfangle;
        z = (*this)(2) / sinhalfangle;
    } else {
        x = 0.0;
        y = 0.0;
        z = 1.0;
    }
    
    angle = mmRadToDeg(angle);
}


/// Spherical Linear Interpolation
/// As t goes from 0 to 1, the Quat object goes from "from" to "to"
/// Reference: Shoemake at SIGGRAPH 89
/// See also
/// http://www.gamasutra.com/features/programming/19980703/quaternions_01.htm
void quaternion::slerp( double t, const quaternion& from, const quaternion& to ) {
    const double epsilon = 0.00001;
    double omega, cosomega, sinomega, scale_from, scale_to ;

    quaternion quatTo(to);
    // this is a dot product

    cosomega =arma::dot((arma::vec)from,(arma::vec)to);

    if ( cosomega < 0.0 ) {
        cosomega = -cosomega;
        quatTo = -to;
    }

    if ( (1.0 - cosomega) > epsilon ) {
        omega = acos(cosomega) ; // 0 <= omega <= Pi (see man acos)
        sinomega = sin(omega) ;  // this sinomega should always be +ve so
        // could try sinomega=sqrt(1-cosomega*cosomega) to avoid a sin()?
        scale_from = sin((1.0 - t) * omega) / sinomega ;
        scale_to = sin(t * omega) / sinomega ;
    } else {
        /* --------------------------------------------------
        The ends of the vectors are very close
        we can use simple linear interpolation - no need
        to worry about the "spherical" interpolation
        -------------------------------------------------- */
        scale_from = 1.0 - t ;
        scale_to = t ;
    }

    *this = (from * scale_from) + (quatTo * scale_to);

    // so that we get a Vec4
}


// ref at http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
arma::vec quaternion::getEuler( rotationOrder order) const
{
    double e; 
    double X=x();
    double Y=y();
    double Z=z();
    double W=w();
    double p0,p1,p2,p3;
    if(order==EULER_XYZ) {p0=W;p1=X;p2=Y;p3=Z;e=-1.0f;}
    else if(order==EULER_YZX) {p0=W;p1=Y;p2=Z;p3=X;e=-1.0f;}
    else if(order==EULER_ZXY) {p0=W;p1=Z;p2=X;p3=Y;e=-1.0f;}
    else if(order==EULER_ZYX) {p0=W;p1=Z;p2=Y;p3=X;e=1.0f;}
    else if(order==EULER_YXZ) {p0=W;p1=Y;p2=X;p3=Z;e=1.0f;}
    else if(order==EULER_XZY) {p0=W;p1=X;p2=Z;p3=Y;e=1.0f;}
    else {p0=W;p1=X;p2=Y;p3=Z;e=-1.0f;} //XYZ by default

    double test = p0*p2+e*p1*p3;
    double a1;
    double a2;
    double a3;
    if (test - 0.5 > -DBL_EPSILON  ) { // singularity at north pole
        a1 = arma::datum::nan;
        a2 = arma::datum::pi/2;
        a3 = arma::datum::nan;
        std::cout << "Singularity at north pole! (gimbal lock)" << std::endl;
    } else if (test + 0.5 < DBL_EPSILON ) { // singularity at south pole
        a1 = arma::datum::nan;
        a2 = - arma::datum::pi/2;
        a3 = arma::datum::nan;
        std::cout << "Singularity at south pole! (gimbal lock)" << std::endl;
    } else {
        double sqp1 = p1 * p1;
        double sqp2 = p2 * p2;
        double sqp3 = p3 * p3;

        a1 = atan2(2.0f*(p0*p1 - e*p2*p3), 1.0f - 2.0f*(sqp1 + sqp2));
        a2 = asin(2*test);
        a3 = atan2(2.0f*(p3*p0 - e*p2*p1), 1.0f - 2.0f*(sqp2 + sqp3));
    }
    arma::vec ret(3);
    
#ifndef M_PI
    double M_PI=arma::datum::pi;
#endif
    
    if(order==EULER_XYZ) ret<<a1*180/M_PI<<a2*180/M_PI<<a3*180/M_PI;
    else if(order==EULER_YZX) ret<<a3*180/M_PI<<a1*180/M_PI<<a2*180/M_PI;
    else if(order==EULER_ZXY) ret<<a2*180/M_PI<<a3*180/M_PI<<a1*180/M_PI;
    else if(order==EULER_ZYX) ret<<a3*180/M_PI<<a2*180/M_PI<<a1*180/M_PI;
    else if(order==EULER_YXZ) ret<<a2*180/M_PI<<a1*180/M_PI<<a3*180/M_PI;
    else if(order==EULER_XZY) ret<<a1*180/M_PI<<a3*180/M_PI<<a2*180/M_PI;
    return ret;
}

double quaternion::getRoll() const {

    double test=w()*y()-x()*z();

    if (test<0.499&&test>-0.499)
        return (atan2(2*(this->at(3)*this->at(0) + this->at(1)*this->at(2)), 1-2*(this->at(0)*this->at(0) + this->at(1)*this->at(1))));
    else
        return 0.0;
}
//-----------------------------------------------------------------------
double quaternion::getPitch() const {   // internal version
    double test=w()*y()-x()*z();

    if (test<0.499&&test>-0.499)
        return (asin(-2*(this->at(0)*this->at(2) - this->at(3)*this->at(1))));
    else {
        if (test>0.499){
            return arma::datum::pi/2;
        }
        else{
            return -arma::datum::pi/2;
        }
    }
}
//-----------------------------------------------------------------------
double quaternion::getYaw() const {
    double test=w()*y()-x()*z();
    if (test<0.499&&test>-0.499)
        return (atan2(2*(this->at(3)*this->at(2) + this->at(0)*this->at(1)),1 - 2*(this->at(1)*this->at(1) + this->at(2)*this->at(2))));
    else            
        return atan2(x(),w());
}

} // namespace
