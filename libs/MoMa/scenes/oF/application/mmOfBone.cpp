//
//  mmOfBone.cpp
//  MotionMachine
//
//  Created by Thierry Ravet on 20/04/15.
//  Copyright (c) 2015 YCAMInterlab. All rights reserved.
//

#include "mmOfBone.h"

namespace MoMa{
    ofBone::ofBone(){
        GLfloat vdata[4][3] = {
            {1.0, 0.0, 0.0}, {0.0, 50.0, 0}, {0.0, -50.0, 0}, {0.0, 0.0, 50}
        };
        GLint indices[4][3] = {
            {0,1,3},{0,2,3},{0,1,2},{1,2,3}
        };
        GLfloat vcolor[4][3] = {
            {0.5,0,0},{0,0.3,0},{0,0,0.5},{0.5,0.5,0.5}
        };
        for (int j=0;j<4;j++)
            for (int i=0;i<4;i++){
                mesh->addVertex(ofVec3f(vdata[i][0],vdata[i][1],vdata[i][2]));
                mesh->addColor(ofFloatColor(vcolor[j][0],vcolor[j][1],vcolor[j][2]));
            }
        for (int i = 0; i<4; ++i){
            mesh->addIndex((i*4)+indices[i][0]);
            mesh->addIndex((i*4)+indices[i][1]);
            mesh->addIndex((i*4)+indices[i][2]);
        }

		defaultColor = mesh->getColors();
		mesh->clearColors();
		this->setDefaultColor();
    };
	void ofBone::setDefaultColor() {
		mesh->addColors(&(defaultColor[0]), 16);
	}
};
