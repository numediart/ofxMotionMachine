#ifndef __mmMuller__
#define __mmMuller__

#include <armadillo>
#include <stdio.h>

#include "mmTrack.h"
#include "mmGeometry.h"
#include "mmBoundingBox.h"
#include "mmSignal.h"
#include <vector>


namespace MoMa {
namespace Muller {
    
    enum{ RESCALE, BINARY };
    
    const int nOfMullerFeatures = 40;
    
    float meanHumerusLength( Track &track );
    float meanShoulderWidth( Track &track );
    float meanHipWidth( Track &track );
    
    float f22( const arma::vec &ar1, const arma::vec &ar2, const arma::vec
    &al1, const arma::vec &al2, const arma::vec &lh, const arma::vec &rh );
    
    arma::mat continuous( Track &track ); // Function continuous() is called directely is user wants to plot continuous features from mat
    
    arma::mat binary( arma::mat &cont, float meanHl, float meanSw, float meanHw ); // Function binary() with mat argument takes continuous matrix and returns binary matrix from continuous
    
    arma::mat rescale( arma::mat &cont, arma::mat &bina ); // Function rescale() takes continuous & binary matrices and returns the rescaled binary
    
    arma::mat binary( Track &track, int type = RESCALE ); // Bundle of all (with flag for rescaling) if we just need binary in computation
    
    // arma::mat compute( Track &track, vector<int> mIdx ); // TODO Be able to compute a subset of features
    // arma::mat compute( Track &track, int mIdx ); // TODO Be able to compute one single feature
    
    class ThresholdNames : public std::vector<std::string> {
        
      public:
        
        ThresholdNames( void );
    };
}
}

#endif
