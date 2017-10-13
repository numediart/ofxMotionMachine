//
//  mmArmaUtils.cpp
//  MotionMachine
//
//  Created by Thierry Ravet on 8/05/15.
//  Copyright (c) 2015 YCAMInterlab. All rights reserved.
//

#include "mmArmaUtils.h"

namespace MoMa{

arma::mat coordMat(double a1,double a2,double a3,rotationOrder order){
    arma::mat ret;
    ret.eye(4,4);
    switch (order) {
        case EULER_XYZ:
            ret=rotMat(a1,0)*ret;
            ret=rotMat(a2,1)*ret;
            ret=rotMat(a3,2)*ret;
            break;
        case EULER_XZY:
            ret=rotMat(a1,0)*ret;
            ret=rotMat(a2,2)*ret;
            ret=rotMat(a3,1)*ret;
            break;
        case EULER_YXZ:
            ret=rotMat(a1,1)*ret;
            ret=rotMat(a2,0)*ret;
            ret=rotMat(a3,2)*ret;
            break;
        case EULER_YZX:
            ret=rotMat(a1,1)*ret;
            ret=rotMat(a2,2)*ret;
            ret=rotMat(a3,0)*ret;
            break;
        case EULER_ZXY:
            ret=rotMat(a1,2)*ret;
            ret=rotMat(a2,0)*ret;
            ret=rotMat(a3,1)*ret;
            break;
        case EULER_ZYX:
            ret=rotMat(a1,2)*ret;
            ret=rotMat(a2,1)*ret;
            ret=rotMat(a3,0)*ret;
            break;
            
        default:
            break;
    }
    return ret;
}
    
    arma::mat rotMat(double angle,unsigned int axisId){
        arma::mat ret;
        ret.eye(4,4);
        double c=cos(angle*arma::datum::pi/180),s=sin(angle*arma::datum::pi/180);
        switch (axisId) {
            case 0:{
                ret
                <<1   <<   0<<   0<<   0<<arma::endr
                <<0   <<   c<<-1*s<<   0<<arma::endr
                <<0   <<   s<<   c<<   0<<arma::endr
                <<0   <<   0<<   0<<   1;
                break;}
            case 1:{
                ret
                <<c   <<   0<<   s<<   0<<arma::endr
                <<0   <<   1<<   0<<   0<<arma::endr
                <<-1*s<<   0<<   c<<   0<<arma::endr
                <<0   <<   0<<   0<<   1;
                break;}
            case 2:{
                ret
                <<c   <<-1*s<<   0<<   0<<arma::endr
                <<   s<<   c<<   0<<   0<<arma::endr
                <<0   <<   0<<   1<<   0<<arma::endr
                <<0   <<   0<<   0<<   1;
                break;}
                
            default:
                break;
        }
        return ret;
        
    }

	arma::mat translMat(arma::vec3 pVec) {
		arma::mat  ret(4, 4);
		ret	<< 1 << 0 << 0 << pVec(0) << arma::endr
			<< 0 << 1 << 0 << pVec(1) << arma::endr
			<< 0 << 0 << 1 << pVec(2) << arma::endr
			<< 0 << 0 << 0 << 1;
		return ret;
	}


	arma::mat quat2ExpMap(arma::mat pMat) {
		arma::rowvec sinhalftheta =arma::sqrt(arma::sum(pMat.rows(0, 2) % pMat.rows(0, 2), 0));
		arma::rowvec coshalftheta = pMat.row(3);
		arma::mat temp = (arma::ones(3,1)*sinhalftheta + arma::datum::eps);
		arma::mat r0 = pMat.rows(0, 2) /temp;
		arma::rowvec theta=2 * arma::atan(sinhalftheta/ coshalftheta);
		theta.elem(find(theta > 2 * arma::datum::pi)) -= 2 * arma::datum::pi;
		theta.elem(find(theta <  0)) += 2 * arma::datum::pi;
		r0.cols(find(theta > arma::datum::pi)) = -r0.cols(find(theta > arma::datum::pi));
		theta.elem(find(theta > arma::datum::pi)) = 2 * arma::datum::pi- theta.elem(find(theta > arma::datum::pi));
		return (r0% (arma::ones(3, 1)*theta));
	}

	arma::cube quat2ExpMap(arma::cube pCube) {
		arma::cube ret(3, pCube.n_cols, pCube.n_slices);
		for (int i = 0; i < pCube.n_slices; i++)
			ret.slice(i) = quat2ExpMap(pCube.slice(i));
		return ret;
	}
	arma::mat expMap2quat(arma::mat pMat) {
		arma::mat ret(4, pMat.n_cols);
		arma::rowvec theta= arma::sqrt(arma::sum(pMat % pMat, 0));
		ret.row(3) = cos(theta / 2);
		arma::uvec ind;
		ind << 0 << 1 << 2;
		ret.rows(0, 2) = pMat % (arma::ones(3, 1)*(sin(theta / 2) / theta));
		ret.submat(ind, find(abs(theta) <= arma::datum::eps)).zeros();
		ind.clear();
		ind << 3;
		ret.submat(ind, find(abs(theta) <= arma::datum::eps)).ones();
		return ret;
	}
	arma::cube expMap2quat(arma::cube pCube) {
		arma::cube ret(4, pCube.n_cols, pCube.n_slices);
		for (int i = 0; i < pCube.n_slices; i++)
			ret.slice(i) = expMap2quat(pCube.slice(i));
		return ret;
	}

}