#include "mmBone.h"

/**
*
*  @file mmBone.cpp
*  @brief MotionMachine source file for Bone class.
*  @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

using namespace std;
using namespace arma;

MoMa::Bone::Bone(void) {

	position.resize(3);
	rotation.resize(4);
	position(X) = datum::nan;
	position(Y) = datum::nan;
	position(Z) = datum::nan;

	rotation( qX ) = 0.0f;
	rotation( qY ) = 0.0f;
	rotation( qZ ) = 0.0f;
	rotation( qW ) = 1.0f;
	rotationOffset.resize(1);
	rotationOffset[0].resize(4);
	rotationOffset[0]( qX ) = 0.0f;
	rotationOffset[0]( qY ) = 0.0f;
	rotationOffset[0]( qZ ) = 0.0f;
	rotationOffset[0]( qW ) = 1.0f;

	_hasRotation = false;
	_hasTime = false;
	_time = 0.0f;
	_name = "";
}

MoMa::Bone::Bone(float x, float y, float z) {

	position.resize(3);
	rotation.resize( 4 );
	rotationOffset.resize( 4 );

	position(X) = x;
	position(Y) = y;
	position(Z) = z;

	rotation( qX ) = 0.0f;
	rotation( qY ) = 0.0f;
	rotation( qZ ) = 0.0f;
	rotation( qW ) = 1.0f;

	rotationOffset.resize(1);
	rotationOffset[0].resize(4);
	rotationOffset[0](qX) = 0.0f;
	rotationOffset[0](qY) = 0.0f;
	rotationOffset[0](qZ) = 0.0f;
	rotationOffset[0](qW) = 1.0f;

	_hasRotation = false;
	_hasTime = false;
	_time = 0.0f;
	_name = "";

}
MoMa::Bone::Bone( float x, float y, float z, float qx, float qy, float qz, float qw ) {

position.resize( 3 );
rotation.resize( 4 );
rotationOffset.resize( 4 );

position( X ) = x;
position( Y ) = y;
position( Z ) = z;

rotation( qX ) = qx;
rotation( qY ) = qy;
rotation( qZ ) = qz;
rotation( qW ) = qw;

rotationOffset.resize(1);
rotationOffset[0].resize(4);
rotationOffset[0]( qX ) = 0.0f;
rotationOffset[0]( qY ) = 0.0f;
rotationOffset[0]( qZ ) = 0.0f;
rotationOffset[0]( qW ) = 1.0f;

_hasRotation = true;
_hasTime = false;
_time = 0.0f;
_name = "";

}
MoMa::Bone::Bone( arma::vec coord, arma::vec rot, arma::vec offsetRot,double length) {

position.resize( 3 );
rotation.resize( 4 );
rotationOffset.resize( 4 );

position( X ) = coord(0);
position( Y ) = coord(1);
position( Z ) = coord(2);

rotation( X ) = rot(0);
rotation( Y ) = rot(1);
rotation( Z ) = rot(2);
rotation( 3 ) = rot(3);

rotationOffset.resize(1);
rotationOffset[0].resize(4);
rotationOffset[0]( X ) = offsetRot(0);
rotationOffset[0]( Y ) = offsetRot(1);
rotationOffset[0]( Z ) = offsetRot(2);
rotationOffset[0]( 3 ) = offsetRot(3);

boneLength.resize(1);
boneLength[0] = length;

_hasRotation = true;
_hasTime = false;
_time = 0.0f;
_name = "";

}
MoMa::Bone::Bone(arma::vec coord, arma::vec rot, std::vector< arma::vec> offsetRot, std::vector< double> lengths) {

	if (coord.size() != 3)
		throw std::runtime_error("MoMa::Bone position vector has not the good size");
	if (rot.size() != 4)
		throw std::runtime_error("MoMa::Bone rotation vector has not the good size");
	if (lengths.size() != offsetRot.size())
		throw std::runtime_error("MoMa::Bone offset data and lengths data must have the same size");
	position.resize(3);
	rotation.resize(4);
//	rotationOffset.resize(4);

	position(X) = coord(0);
	position(Y) = coord(1);
	position(Z) = coord(2);

	rotation(X) = rot(0);
	rotation(Y) = rot(1);
	rotation(Z) = rot(2);
	rotation(3) = rot(3);

	rotationOffset = offsetRot;
	boneLength = lengths;

	_hasRotation = true;
	_hasTime = false;
	_time = 0.0f;
	_name = "";

}

void MoMa::Bone::setPosition(float x, float y, float z) {

	position(X) = x;
	position(Y) = y;
	position(Z) = z;
}

void MoMa::Bone::setRotation( float x, float y, float z, float w ) {

	_hasRotation = true;

	if( x*x+y*y+z*z+w*w != 0 ) {

		rotation( qX ) = x;
		rotation( qY ) = y;
		rotation( qZ ) = z;
		rotation( qW ) = w;
	}
}

void MoMa::Bone::setPosition(arma::vec pos) {
	if (pos.size() != 3)
		throw std::runtime_error("MoMa::Bone position vector has not the good size");

	position = pos;
}

void MoMa::Bone::setRotation( arma::vec rot ) {

	if( arma::norm(rot) != 0 ) {
		_hasRotation = true;
		rotation=rot;
	}
}

void MoMa::Bone::setBoneData(std::vector<arma::vec> off, std::vector< double> lengths){
	if (lengths.size()!=off.size())
		throw std::runtime_error("MoMa::Bone offset data and lengths data must have the same size");

	for (int i = 0; i < off.size(); i++)
		if (arma::norm(off[i]) == 0)
			throw std::runtime_error("MoMa::Bone::setBoneData theses quaternion must be non null ");
	rotationOffset = off;
	boneLength = lengths;
}
