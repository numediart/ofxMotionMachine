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


}