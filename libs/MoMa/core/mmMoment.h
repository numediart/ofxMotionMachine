#ifndef __mmMoment__
#define __mmMoment__

#include <iostream>

namespace MoMa {
    
    class Moment {
    
      public:
        
        Moment( void );
        Moment( double time, double frameRate );
        Moment( unsigned int idx, double frameRate );
        Moment( double time , unsigned int idx );
        
        inline double time( void ) const { return( _time ); }
        inline unsigned int index( void ) const { return( _index ); }
        
        double frameRate( void ) const { return( _frameRate ); }
        void setFrameRate( double frameRate );
        
        void setTime( double time, double frameRate );
        void setIndex( int idx, double frameRate );
        
        void setTime( double time );
        void setIndex( unsigned int idx );
        void setIndex( int idx );
		Moment & operator++();
		Moment & operator--();
        
      protected:
        
        double _frameRate;
        unsigned int _index;
        double _time;
    };
}

#endif
