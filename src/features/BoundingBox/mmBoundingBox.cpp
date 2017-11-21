#include "mmBoundingBox.h"

using namespace std;
using namespace arma;
using namespace MoMa;

float BoundingBox::width( const arma::mat &frame ) {
    
    float dist = 0.0f;
    float maxdist = 0.0f;
    
    for( int n=0; n<frame.n_cols; n++ ) {
        
        for( int m=0; m<frame.n_cols; m++ ) {
            
            dist = sqrt( pow( frame(X,n)-frame(X,m), 2 )
            + pow(frame(Y,n)-frame(Y,m), 2 ) );
            
            if( dist > maxdist ) maxdist = dist;
        }
    }
    
    return( maxdist );
}

float BoundingBox::height( const arma::mat &frame ) {
    
    float diff = highest( frame )( Z )
    - lowest( frame )( Z );
    
    return( diff );
}

float BoundingBox::width( MoMa::Frame &frame ) {
    
    mat frameMtx = frame.getPosition();
    return( width( frameMtx ) );
}

float BoundingBox::height( MoMa::Frame &frame ) {
    
    mat frameMtx = frame.getPosition();
    return( height( frameMtx ) );
}

arma::vec BoundingBox::lowest( const arma::mat &frame ) {
    
    arma::vec low;
    low << 0.0f << 0.0f << 1e12 ;
    
    for( int n=0; n<frame.n_cols; n++ ) {
        
        if( frame(Z,n) < low( Z ) ) {
            
            low = frame.col( n );
        }
    }
    
    return( low );
}

arma::vec BoundingBox::highest( const arma::mat &frame ) {
    
    arma::vec high;
    high << 0.0f << 0.0f << -1e12 ;
    
    for( int n=0; n<frame.n_cols; n++ ) {
        
        if( frame(Z, n )> high( Z ) ) {
            
            high = frame.col( n );
        }
    }
    
    return( high );
}

arma::vec BoundingBox::center( const arma::mat &frame ) {
    
    int nMax, mMax;
    float dist = 0.0f;
    float maxdist = 0.0f;
    arma::vec center;
    
    for( int n=0; n<frame.n_cols; n++ ) {
        
        for( int m=0; m<frame.n_cols; m++ ) {
            
            dist = sqrt( pow( frame(X,n)-frame(X,m), 2 )
            + pow( frame(Y,n)-frame(Y,m), 2 ) );
            
            if( dist > maxdist ) {
                
                nMax = n; mMax = m;
                maxdist = dist;
            }
        }
    }
    
    if( frame.n_cols > 0 ) {
    
        center( X ) = 0.5 * ( frame(X, nMax ) + frame(X, mMax ));
        center( Y ) = 0.5 * ( frame(Y, nMax )+ frame(Y, mMax ));
        center( Z ) = 0.5 * ( highest( frame )( Z ) + lowest( frame )( Z ) );
    }
        
    return( center );
}

MoMa::Node BoundingBox::lowest( MoMa::Frame &frame ) {

    Node low( 0.0f, 0.0f, 1e12 );
    
    mat frameMtx = frame.getPosition();
    mat lowMtx = lowest( frameMtx );
    low.setPosition( lowMtx );
    
    return( low );
}

MoMa::Node BoundingBox::highest( MoMa::Frame &frame ) {

    Node high( 0.0f, 0.0f, -1e12 );
    
    mat frameMtx = frame.getPosition();
    mat highMtx = highest( frameMtx );
    high.setPosition( highMtx );
    
    return( high );
}

MoMa::Node BoundingBox::center( MoMa::Frame &frame ) {
    
    Node ctr( 0.0f, 0.0f, 0.0f );
    
    mat frameMtx = frame.getPosition();
    mat centerMtx = center( frameMtx );
    ctr.setPosition( centerMtx );
    
    return( ctr );
}

arma::vec BoundingBox::lowest( const arma::mat &frame, int index ) {
    
    arma::vec low(3);
    low << 0.0f << 0.0f << 1e12 ;
    
    for( int n=0; n<frame.n_cols; n++ ) {
        
        if ( n == index ) continue;
        
        else {
        
            if( frame(Z, n ) < low( Z )) {
                
                low = frame.col( n );
            }
        }
    }
    
    return( low );
}

arma::vec BoundingBox::highest(const arma::mat &frame, int index ) {
    
    arma::vec high(3);
    high << 0.0f << 0.0f << -1e12 ;
    
    for( int n=0; n<frame.n_cols; n++ ) {
        
        if( n == index ) continue;
        
        else {
            
            if( frame(Z, n ) > high( Z )) {
                
                high = frame.col( n );
            }
        }
    }
    
    return( high );
}

Node BoundingBox::lowest( Frame frame, string name ) {
    
    Node low( 0.0f, 0.0f, 1e12 );
    
    for( int n=0; n<frame.nOfNodes(); n++ ) {
        
        if( frame.node( n ).position( Z ) < low.position( Z )
        && name != frame.nodeList->name( n ) ) {
            
            low = frame.node( n );
        }
    }
    
    return( low );
}

Node BoundingBox::highest( Frame frame, string name ) {
    
    Node high( 0.0f, 0.0f, -1e12 );
    
    for( int n=0; n<frame.nOfNodes(); n++ ) {
        
        if( frame.node( n ).position( Z ) > high.position( Z )
        && name != frame.nodeList->name( n ) ) {
            
            high = frame.node( n );
        }
    }
    
    return( high );
}
