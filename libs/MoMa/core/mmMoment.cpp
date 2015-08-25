#include "mmMoment.h"

using namespace MoMa;

Moment::Moment( void ) {

    _frameRate = 0.0f;
    _time = 0.0f;
    _index = 0u;
}

Moment::Moment( double time, double frameRate ) {

    setTime( time, frameRate );
}

Moment::Moment( unsigned int idx, double frameRate ) {

    setIndex( idx, frameRate );
}

void Moment::setFrameRate( double frameRate ) {

    _frameRate = frameRate;
}

void Moment::setTime( double time, double frameRate ) {
    
    _index = (unsigned int)( time * frameRate );
    _frameRate = frameRate;
    _time = time;
}

void Moment::setIndex( unsigned int idx, double frameRate ) {

    _time = (double)idx / frameRate;
    _frameRate = frameRate;
    _index = idx;
}

void Moment::setIndex( int idx, double frameRate ) {

    _time = (double)idx / frameRate;
    _index = (unsigned int)idx;
    _frameRate = frameRate;
}

void Moment::setTime( double time ) {

    setTime( time, _frameRate );
}

void Moment::setIndex( unsigned int idx ) {

    setIndex( idx, _frameRate );
}

void Moment::setIndex( int idx ) {

    setIndex( idx, _frameRate );
}
