#ifndef __mmSignal__
#define __mmSignal__

#include <cmath>
#include <armadillo>

namespace MoMa {
namespace Signal {
        
float mean( arma::vec feature );
    
float min( arma::vec feature );
    
float max( arma::vec feature );
    
arma::vec thresh( arma::vec feature, float high, float low, float min=0.0f, float max=1.0f );
    
arma::vec peaks( arma::vec feature, float width );
        
}
}

#endif
