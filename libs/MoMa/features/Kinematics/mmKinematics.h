#ifndef __mmKinematics__
#define __mmKinematics__

//#include "mmFeature.h"


/*namespace MoMa {

    class MoMa::Track;
}*/


#include "mmTrack.h"
#include "mmNodeTrace.h"
#include "mmContainer.h"


namespace MoMa {    
    
    arma::mat derivate(const arma::mat &data, double frameRate, int type = 0); //type : 0 for five-point derivation, 1 for asymmetric derivation, 2 for symmetric derivation
    arma::mat derivate(const arma::mat &data, const arma::vec &timeVec, int type = 2); //type : 1 for asymmetric derivation, 2 for symmetric derivation (five-point derivation not available for timed data)
    arma::vec norm_col(const arma::mat &data);
    arma::vec derivate(const arma::vec &data, double frameRate, int type = 0); //type : 0 for five-point derivation, 1 for asymmetric derivation, 2 for symmetric derivation
    arma::vec derivate(const arma::vec &data, const arma::vec &timeVec, int type = 2); //type : 1 for asymmetric derivation, 2 for symmetric derivation (five-point derivation not available for timed data)

    MoMa::TimedMat derivate(const MoMa::TimedMat &data, int type = 0); //type : 0 for five-point derivation, 1 for asymmetric derivation, 2 for symmetric derivation
    MoMa::TimedVec derivate(const MoMa::TimedVec &data, int type = 0); //type : 0 for five-point derivation, 1 for asymmetric derivation, 2 for symmetric derivation
    MoMa::TimedVec norm(const MoMa::TimedMat &data);
    MoMa::TimedVec abs(const MoMa::TimedVec &data);

    //Kinematics on Trace data

    arma::mat speedm(const MoMa::Trace &trace);
    arma::mat accelerationm(const MoMa::Trace &trace);
    arma::vec normSpeedm(const MoMa::Trace &trace);
    arma::vec normAccelerationm(const MoMa::Trace &trace);
    MoMa::TimedMat speed(const MoMa::Trace &trace);
    MoMa::TimedMat acceleration(const MoMa::Trace &trace);
    MoMa::TimedVec normSpeed(const MoMa::Trace &trace);
    MoMa::TimedVec normAcceleration(const MoMa::Trace &trace);

    //Kinematics on TimeMad data

    arma::mat speedm(const MoMa::TimedMat &data);
    arma::mat accelerationm(const MoMa::TimedMat &data);
    arma::vec normSpeedm(const MoMa::TimedMat &data);
    arma::vec normAccelerationm(const MoMa::TimedMat &data);
    MoMa::TimedMat speed(const MoMa::TimedMat &data);
    MoMa::TimedMat acceleration(const MoMa::TimedMat &data);
    MoMa::TimedVec normSpeed(const MoMa::TimedMat &data);
    MoMa::TimedVec normAcceleration(const MoMa::TimedMat &data);
    MoMa::TimedVec speed(const MoMa::TimedVec &data);
    MoMa::TimedVec acceleration(const MoMa::TimedVec &data);
    
    //Scaling values

    float meanFemurLength(const MoMa::Track &tr);
    float meanHumerusLength(const MoMa::Track &tr);
    float meanArmSize(const MoMa::Track &tr);
    float meanLegSize(const MoMa::Track &tr);
    float meanTrunkSize(const MoMa::Track &tr);
    float meanFootSize(const MoMa::Track &tr);
    float meanForearmLength(const MoMa::Track &tr);
    float meanShankLength(const MoMa::Track &tr);
    float meanHeadHeight(const MoMa::Track &tr);
    float meanShoulderWidth(const MoMa::Track &tr);
    float meanHipWidth(const MoMa::Track &tr);
    float meanSizeToArm(const MoMa::Track &tr);
    float meanSize(const MoMa::Track &tr);

}

#endif