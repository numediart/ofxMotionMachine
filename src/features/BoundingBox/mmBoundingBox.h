#ifndef __mmBoundingBox__
#define __mmBoundingBox__

#include <cmath>
#include <armadillo>

#include "mmFrame.h"

namespace MoMa {
namespace BoundingBox {
    
    float width( const arma::mat &frame ); // Get width of the bounding cylinder
    float height( const arma::mat &frame ); // Get height of the bounding cylinder
    
    float width( MoMa::Frame &frame ); // Get width of the bounding cylinder
    float height( MoMa::Frame &frame ); // Get height of the bounding cylinder
    
    arma::vec lowest( const arma::mat &frame ); // Get lowest point of the frame
    arma::vec highest( const arma::mat &frame ); // Get highest point of the frame
    arma::vec center( const arma::mat &frame ); // Get center of gravity of the frame
    
    MoMa::Node lowest( MoMa::Frame &frame ); // Get lowest point of the frame
    MoMa::Node highest( MoMa::Frame &frame ); // Get highest point of the frame
    MoMa::Node center( MoMa::Frame &frame ); // Get center of gravity of the frame
    
    arma::vec lowest( const arma::mat &frame, int index ); // Get lowest point - node @ index
    arma::vec highest( const arma::mat &frame, int index ); // Get highest point - node @ index
    
    Node lowest( MoMa::Frame frame, std::string name ); // Get lowest point - node @ name
    Node highest( MoMa::Frame frame, std::string name ); // Get highest point - node @ name
}
}

#endif
