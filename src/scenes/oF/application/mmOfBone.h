//
//  mmOfBone.h
//  MotionMachine
//
//  Created by Thierry Ravet on 20/04/15.
//  Copyright (c) 2015 YCAMInterlab. All rights reserved.
//

#ifndef __mmOfBone__
#define __mmOfBone__

#include <stdio.h>
#include "ofMain.h"

namespace MoMa{
    class ofBone: public of3dPrimitive{
    public:
        ofBone();
		void setDefaultColor();
	protected:
		std::vector<ofFloatColor> defaultColor;
        
    };
}

#endif
