#include "mmSignal.h"

using namespace arma;
using namespace MoMa;

float Signal::mean( vec feature ) {
    
    float mean = 0.0f;
    
    for( int t=0; t<feature.n_rows; t++ ) {
        
        mean += feature(t);
    }
    
    mean /= (float)( feature.n_rows );
    
    return( mean );
}

float Signal::min( vec feature ) {
    
    float min = 1e9;
    
    for( int t=0; t<feature.n_rows; t++ ) {
        
        if( feature(t) < min ) min = feature(t);
    }
    
    return( min );
}

float Signal::max( vec feature ) {
    
    float max = -1e9;
    
    for( int t=0; t<feature.n_rows; t++ ) {
        
        if( feature(t) > max ) max = feature(t);
    }
    
    return( max );
}

vec Signal::thresh( vec feature, float high, float low, float min, float max ) {
    
    vec thresh;
    
    thresh.resize( feature.n_rows, 1 );
    
    // Schmitt trigger: when signal has passed a high thresh, it looks if signal
    // is passing a low thresh before commuting back; when signal has passed a low
    // thresh, it looks if signal is passing a high thresh before commuting back.
    
    if( feature(0) <= high ) thresh(0) = min;
    if( feature(0) > high ) thresh(0) = max;
    
    for( int t=1; t<feature.n_rows; t++ ) {
        
        if( thresh(t-1) == max ) {
        
            if( feature(t) > high ) thresh(t) = max;
            else if( feature(t) <= high && feature(t) > low ) thresh(t) = max;
            else if( feature(t) <= low ) thresh(t) = min;
            
        } else if( thresh(t-1) == min ) {
        
            if( feature(t) > high ) thresh(t) = max;
            else if( feature(t) <= high && feature(t) > low ) thresh(t) = min;
            else if( feature(t) <= low ) thresh(t) = min;
        }
    }
    
    return( thresh );
}

vec Signal::peaks( vec feature, float width ) {
    
    vec dx = feature.rows( 1, feature.n_rows-1 ) - feature.rows( 0, feature.n_rows-2 );
    
    uvec ur = find( dx > 0.0f ); uvec uf = find( dx < 0.0f ); // Rise and falls
    vec r; r.resize( ur.n_rows, 1 ); for( int k=0; k<r.n_rows; k++ ) r(k) = ur(k);
    vec f; f.resize( uf.n_rows, 1 ); for( int k=0; k<f.n_rows; k++ ) f(k) = uf(k);
    
    vec dr = r; dr.rows( 1, r.n_rows-1 ) = r.rows( 1, r.n_rows-1 ) - r.rows( 0, r.n_rows-2 );
    vec rc = ones( feature.n_rows, 1 );
    rc( ur+1 ) = 1-dr; rc(0) = 0;
    vec rs = cumsum( rc );
    
    vec df = f; df.rows( 1, f.n_rows-1 ) = f.rows( 1, f.n_rows-1 ) - f.rows( 0, f.n_rows-2 );
    vec fc = ones( feature.n_rows, 1 );
    fc( uf+1 ) = 1-df; fc(0) = 0;
    vec fs = cumsum( fc );
    
    vec rp = -ones( feature.n_rows, 1 );
    uvec rz; rz.zeros(1,1); uvec irp = join_vert( rz, ur+1 );
    rp( irp ) = join_vert( dr-1, (feature.n_rows-r( r.n_rows-1 )-2)*ones(1,1) );
    vec rq = cumsum( rp );
    
    vec fp = -ones( feature.n_rows, 1 );
    uvec fz; fz.zeros(1,1); uvec ifp = join_vert( fz, uf+1 );
    fp( ifp ) = join_vert( df-1, (feature.n_rows-f( f.n_rows-1 )-2)*ones(1,1) );
    vec fq = cumsum( fp );
    
    uvec k = find( (rs<fs) && (fq<rq) && ( floor((fq-rs)/2)==-1 ) );
    vec v = feature( k ); // Final term centres peaks within a plateau
    
    uvec j = find( (k.rows(1,k.n_rows-1)-k.rows(0,k.n_rows-2)) <= width );
    
    while( j.n_rows > 0 ) {
        
        uvec new_k; vec new_v;
    
        j += ( v(j) >= v(j+1) );
        
        for( int m=0; m<k.n_rows; m++ ) {
        
            bool found = false;
            
            for( int n=0; n<j.n_rows; n++ ) {
            
                if( j(n) == m ) found = true;
            }
            
            if( !found ) {
            
                new_k.resize( new_k.n_rows+1, 1 );
                new_k( new_k.n_rows-1 ) = k(m);
                
                new_v.resize( new_v.n_rows+1, 1 );
                new_v( new_v.n_rows-1 ) = v(m);
            }
        }
        
        k = new_k; v = new_v;
        
        j = find( (k.rows(1,k.n_rows-1)-k.rows(0,k.n_rows-2)) <= width );
    }
    
    vec kk; kk.resize( k.n_rows, 1 );
    for( int i=0; i<k.n_rows; i++ ) kk(i) = k(i);
    
    return( kk );
}

arma::vec Signal::hann(size_t s)
{
    int k, N, M, half;

    arma::vec win;

    // handle special cases first
    switch (s) {
        case 1:
            win = arma::vec(1,1.0);
        case 0:
            return win;
    }

    win.set_size(s);

    // even-length window should have only one 1.0 value nonetheless
    if (s % 2 == 0) {
        N = s;
    } else {
        N = s-1;
    }

    half = (N)/2+1;
    M = s/2-1;

    for(k = 0; k < half; k++) {
        win(k) = 0.5 * ( 1 - cos( M_2PI*(k)/(N) ) );
    }
    for(k = half; k < s; k++, M--) {
        win(k) = win(M);
    }

    return win;
}
