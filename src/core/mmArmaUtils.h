#ifndef __mmArmaUtils__
#define __mmArmaUtils__

#include <stdio.h>
#include <armadillo>

namespace MoMa {
    
    enum rotationOrder {
        
        EULER_ZYX = 0,
        EULER_YZX,
        EULER_XZY,
        EULER_ZXY,
        EULER_YXZ,
        EULER_XYZ
    };
    
    arma::mat coordMat( double a1, double a2, double a3, rotationOrder order );
	arma::mat rotMat(double angle, unsigned int axisId);
	arma::mat translMat(arma::vec3 pVec);
	arma::mat quat2ExpMap(arma::mat pMat);
	arma::cube quat2ExpMap(arma::cube pCube);

	arma::mat expMap2quat(arma::mat pMat);
	arma::cube expMap2quat(arma::cube pCube);
}

#endif
