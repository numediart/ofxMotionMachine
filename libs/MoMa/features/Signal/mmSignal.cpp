#include "mmSignal.h"

using namespace arma;
using namespace MoMa;


vec MoMa::thresh( vec feature, float high, float low, float min, float max ) {
    
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

vec MoMa::peaks( vec feature, float width ) {
    
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

arma::vec MoMa::hann(size_t s)
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


double MoMa::nanmean(arma::vec v) {

    arma::vec tmp = v.elem(find_finite(v));
    return arma::mean(tmp);
}

double MoMa::nanstd(arma::vec v) {

    arma::vec tmp = v.elem(find_finite(v));
    return arma::stddev(tmp);
}

arma::vec MoMa::nanmean(arma::mat v, int dim) {

    //arma::mat tmp = v.elem(find_finite(v)); // treats v as a long vector
    
    if (dim == 0) {
        arma::vec tmpvec = sum(v, 1);
        arma::mat tmp = v.rows(find_finite(tmpvec));
        if (tmp.n_rows <= 0) return arma::datum::nan*ones(v.n_cols);
        return arma::mean(tmp, 0);
    }
    else {
        arma::rowvec tmpvec = sum(v);
        arma::mat tmp = v.cols(find_finite(tmpvec));
        if (tmp.n_cols <= 0) return arma::datum::nan*ones(v.n_rows);
        return arma::mean(tmp, 1);
    }
}

arma::vec MoMa::nanstd(arma::mat v, int dim) {

    //arma::mat tmp = v.elem(find_finite(v)); // treats v as a long vector
    if (dim == 0) {
        arma::vec tmpvec = sum(v, 1);
        arma::mat tmp = v.rows(find_finite(tmpvec));
        if (tmp.n_rows <= 0) return arma::datum::nan*ones(v.n_cols);
        return arma::stddev(tmp, 0, 0);
    }
    else {
        arma::rowvec tmpvec = sum(v);
        arma::mat tmp = v.cols(find_finite(tmpvec));
        if (tmp.n_cols <= 0) return arma::datum::nan*ones(v.n_rows);
        return arma::stddev(tmp, 0, 1);
    }
}


arma::vec MoMa::medfilter(const arma::vec & v, int windowSize)
{
    vec ret(v.size());
    int n = windowSize;

    if (windowSize < 2) return ret;

    int n_left = floor(n / 2);
    int n_right = n - n_left - 1;
    int left, right;
    for (int i = 0; i < v.n_elem; i++) {

        left = std::max(0, i - n_left);
        right = std::min(i + n_right, (int)v.n_elem - 1);
        ret(i) = median(v(span(left, right)));
    }

    return ret;
}

mat MoMa::medfilter(const mat &m, int windowSize) {

    mat ret = m.t();

    for (int i = 0; i < ret.n_cols; i++) {

        ret.col(i) = medfilter((vec)ret.col(i), windowSize);
    }
    return ret.t();
}

TimedVec MoMa::medfilter(const TimedVec &V, int windowSize) {

    vec data = V.getData();

    vec filtered = medfilter(data, windowSize);

    if (!V.isTimed())
        return TimedVec(V.frameRate(), filtered, V.initialTime());
    else
        return TimedVec(V.getTimeVec(), filtered);
}

TimedMat MoMa::medfilter(const TimedMat &M, int windowSize) {

    mat data = M.getData();

    mat filtered = medfilter(data, windowSize);

    if (!M.isTimed())
        return TimedMat(M.frameRate(), filtered, M.initialTime());
    else
        return TimedMat(M.getTimeVec(), filtered);
}

MoMa::Track MoMa::medfilter(const MoMa::Track & tr, int windowSize, bool filterRotation)
{
    Track ret = tr;

    for (int i = 0; i < tr.nOfNodes(); i++) {
        for (int j = 0; j < 3; j++) {

            ret.position.getRefData().tube(j, i) = medfilter((vec)ret.position.getRefData().tube(j, i), windowSize);
        }
    }

    if (filterRotation) {

        for (int i = 0; i < tr.rotation.nOfRows(); i++) {
            for (int j = 0; j < tr.rotation.nOfCols(); j++) {

                ret.rotation.getRefData().tube(j, i) = medfilter((vec)ret.rotation.getRefData().tube(j, i), windowSize);
            }
        }
    }

    return ret;
}


arma::vec MoMa::meanfilter(const arma::vec & v, int windowSize)
{
    vec ret = v;
    int n = windowSize;

    if (windowSize < 2) return ret;

    int n_left = floor(n / 2);
    int n_right = n - n_left - 1;
    int left, right;

    //Avoid beginning and ending (as it change a lot derivative)
    /*for (int i = n_left; i < v.n_elem - n_right; i++) {

    ret(i) = mean(v(span(i - n_left, i + n_right)));
    }*/


    //Complete filtering
    for (int i = 0; i < v.n_elem; i++) {

        left = std::max(0, i - n_left);
        right = std::min(i + n_right, (int)v.n_elem - 1);
        ret(i) = nanmean((vec)v(span(left, right)));
    }

    return ret;
}


mat MoMa::meanfilter(const mat &m, int windowSize) {

    mat ret = m.t();

    for (int i = 0; i < ret.n_cols; i++) {

        ret.col(i) = meanfilter((vec)ret.col(i), windowSize);
    }
    return ret.t();
}

TimedVec MoMa::meanfilter(const TimedVec &V, int windowSize) {

    vec data = V.getData();

    vec filtered = meanfilter(data, windowSize);

    if (!V.isTimed())
        return TimedVec(V.frameRate(), filtered, V.initialTime());
    else
        return TimedVec(V.getTimeVec(), filtered);
}

TimedMat MoMa::meanfilter(const TimedMat &M, int windowSize) {

    mat data = M.getData();

    mat filtered = meanfilter(data, windowSize);

    if (!M.isTimed())
        return TimedMat(M.frameRate(), filtered, M.initialTime());
    else
        return TimedMat(M.getTimeVec(), filtered);
}

MoMa::Track MoMa::meanfilter(const MoMa::Track & tr, int windowSize, bool filterRotation)
{
    Track ret = tr;

    for (int i = 0; i < tr.nOfNodes(); i++) {
        for (int j = 0; j < 3; j++) {

            ret.position.getRefData().tube(j, i) = meanfilter((vec)ret.position.getRefData().tube(j, i), windowSize);            
        }
    }

    if (filterRotation) {

        for (int i = 0; i < tr.rotation.nOfCols(); i++) {
            for (int j = 0; j < tr.rotation.nOfRows(); j++) {

                ret.rotation.getRefData().tube(j, i) = meanfilter((vec)ret.rotation.getRefData().tube(j, i), windowSize);
            }
        }
    }


    return ret;
}