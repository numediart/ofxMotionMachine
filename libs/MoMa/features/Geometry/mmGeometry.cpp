#include "mmGeometry.h"
#include "mmKinematics.h"
#include "mmSignal.h"
using namespace arma;
using namespace MoMa;


float Geometry::degToRad(float deg) {

	return(deg * M_PI / 180.0);
}

float Geometry::radToDeg(float rad) {

	return(rad * 180.0 / M_PI);
}

float Geometry::distance(const arma::vec &a, const arma::vec &b) {

	float dist;

	dist = sqrt(pow(a(X) - b(X), 2) + pow(a(Y) - b(Y), 2) + pow(a(Z) - b(Z), 2));

	return(dist);
}

vec Geometry::distance(const mat &a, const mat &b) {

	return sqrt(sum(pow(a - b, 2))).t();
}

TimedVec Geometry::distance(const TimedMat &a, const TimedMat &b) {

	vec tmp = sqrt(sum(pow(a.getData() - b.getData(), 2)).t());
	TimedVec ret;
	ret.setData(a.frameRate(), tmp, a.initialTime());
	return ret;
}

//Distance of point d to plane defined by points a, b and c
float Geometry::distanceToPlane(const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d) {

	float dist;

	vec ab, cb, n, dc;

	ab = a - b;
	cb = c - b;

	n = cross(ab, cb);

	n = normalise(n);

	dc = d - c;
	dist = dot(dc, n);

	return(dist);
}

//Distance between d and plane fixed in c and normal to vector a->b
float Geometry::distanceToNPlane(const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d) {

	float dist;

	vec n, dc;

	n = b - a;
	n = normalise(n);

	dc = d - c;
	dist = dot(dc, n);

	return(dist);

}

float Geometry::velocityToVect(const arma::vec &a, const arma::vec &b, const arma::vec &c1, const arma::vec &c2, const arma::vec &d1, const arma::vec &d2) {
	//Speed of d relative to c in direction determined by a->b
	float velo;

	vec vRel, n;

	vRel = (d2 - c2) - (d1 - c1);

	n = b - a;
	n = normalise(n);

	velo = dot(vRel, n);

	return(velo);
}

float Geometry::velocityToNPlane(const arma::vec &a1, const arma::vec &a2, const arma::vec &b, const arma::vec &c, const arma::vec &d1, const arma::vec &d2) {

	float velo;

	vec a2b, cb, n, vRel;

	a2b = a2 - b;
	cb = c - b;

	n = cross(cb, a2b);
	n = normalise(n);

	vRel = (d2 - a2) - (d1 - a1);

	velo = dot(vRel, n);

	return(velo);
}

float Geometry::angleBtwVectors(const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d) {

	vec v1, v2;
	float angle;

	v1 = b - a;
	v2 = d - c;

	angle = atan2(norm(cross(v1, v2)), arma::dot(v1, v2));

	return(angle);
}

float Geometry::angleBtwVectors2D(const arma::vec &a, const arma::vec &b, const arma::vec &c, const arma::vec &d) {

	vec v1, v2;
	float angle;

	v1 = b - a;
	v2 = d - c;
	//float tmp = arma::dot(v1,v2);
	//tmp=norm(v1);
	//tmp = norm(v2);
	//tmp = arma::dot(v1,v2)/(norm(v1)*norm(v2));
	angle = acos(arma::dot(v1, v2) / (norm(v1)*norm(v2)));

	return(angle);
}

mat Geometry::projection(const mat &a, const mat &b, const mat &c, const mat &p, bool invert)
{
	mat v1, v2, v3, proj;

	//Compute coordinate system
	v1 = b - a;
	v1 = normalise(v1);
	v2.copy_size(v1);
	v3.copy_size(v1);
	proj.copy_size(v1);

	for (int i = 0; i < v3.n_cols; ++i) {

		v3.col(i) = cross(v1.col(i), c.col(i) - a.col(i)); //v3 normal to plan
		v3.col(i) /= norm(v3.col(i));
		v2.col(i) = cross(v3.col(i), v1.col(i));

		//Compute projection matrix
		mat projmat(3, 3);
		projmat.col(0) = v1.col(i); projmat.col(1) = v2.col(i); projmat.col(2) = v3.col(i);

		if (invert == 0)
			//Compute projection
			proj.col(i) = ((p.col(i) - a.col(i)).t()*projmat).t();

		else {

			if (projmat.is_finite()) {

				//Compute inverse projection matrix
				mat invprojmat = pinv(projmat);

				//Compute inverse projection
				proj.col(i) = (p.col(i).t()*invprojmat).t() + a.col(i);
			}
			else {

				proj.col(i).fill(datum::nan);
			}
		}
	}

	return proj;
}

arma::mat MoMa::Geometry::quatprojection(const arma::mat & a, const arma::mat & b, const arma::mat & c, const arma::mat & q, bool invert)
{
	mat v1, v2, v3, proj;

	//Compute coordinate system
	v1 = b - a;
	v1 = normalise(v1);
	v2.copy_size(v1);
	v3.copy_size(v1);
	proj.set_size(4, v1.n_cols);

	for (int i = 0; i < v3.n_cols; ++i) {

		v3.col(i) = cross(v1.col(i), c.col(i) - a.col(i)); //v3 normal to plan
		v3.col(i) /= norm(v3.col(i));
		v2.col(i) = cross(v3.col(i), v1.col(i));

		//Compute projection matrix
		mat projmat(4, 4);
		vec vec1, vec2, vec3, vec4;
		vec1 << v1.col(i)(0) << v1.col(i)(1) << v1.col(i)(2) << 0;
		vec2 << v2.col(i)(0) << v2.col(i)(1) << v2.col(i)(2) << 0;
		vec3 << v3.col(i)(0) << v3.col(i)(1) << v3.col(i)(2) << 0;
		vec4 << 0 << 0 << 0 << 1;
		projmat.col(0) = vec1; projmat.col(1) = vec2; projmat.col(2) = vec3; projmat.col(3) = vec4;

		//Compute quaternion matrix
		quaternion quat = (vec)q.col(i);
		mat matrix(4, 4);
		quat.get(matrix);

		if (invert == 0) {

			//Compute projection
			//mat projectedmatrix = matrix*projmat;
			mat projectedmatrix = (matrix.t()*projmat).t(); //this one seems to work! Seems that in matrix, the three axes vectors are horitonzal
			//mat projectedmatrix = projmat*matrix*pinv(projmat);
			quaternion projectedquat;
			projectedquat.set(projectedmatrix);

			proj.col(i) = (vec)projectedquat;
		}
		else {

			if (projmat.is_finite()) {

				//Compute inverse projection matrix
				mat invprojmat = pinv(projmat);

				//Compute inverse projection
				//mat projectedmatrix = matrix*invprojmat;
				mat projectedmatrix = (matrix.t()*invprojmat).t();
				//mat projectedmatrix = invprojmat*matrix*projmat;
				quaternion projectedquat;
				projectedquat.set(projectedmatrix);

				proj.col(i) = (vec)projectedquat;
			}
			else {

				proj.col(i).fill(datum::nan);
			}
		}



	}

	return proj;
}

TimedMat Geometry::projection(const MoMa::TimedMat &a, const MoMa::TimedMat &b, const MoMa::TimedMat &c, const MoMa::TimedMat &p, bool invert)
{
	mat proj = projection(a.getData(), b.getData(), c.getData(), p.getData(), invert);
	if (!p.isTimed())
		return TimedMat(p.frameRate(), proj, p.initialTime());
	else
		return TimedMat(p.getTimeVec(), proj);
}

TimedMat Geometry::quatprojection(const MoMa::TimedMat & a, const MoMa::TimedMat & b, const MoMa::TimedMat & c, const MoMa::TimedMat & q, bool invert)
{
	mat proj = quatprojection(a.getData(), b.getData(), c.getData(), q.getData(), invert);
	if (!q.isTimed())
		return TimedMat(q.frameRate(), proj, q.initialTime());
	else
		return TimedMat(q.getTimeVec(), proj);
}

MoMa::Trace Geometry::projection(const MoMa::Trace &t1, const MoMa::Trace &t2, const MoMa::Trace &t3, const MoMa::Trace &p, bool invert)
{
	TimedMat proj = projection(t1.position, t2.position, t3.position, p.position, invert);

	Trace ret;
	ret.setTimeFlag(p.hasTime());
	ret.position = proj;
	return ret;
}

MoMa::Track MoMa::Geometry::projection(const MoMa::Trace & a, const MoMa::Trace & b, const MoMa::Trace & c, const MoMa::Track & tr, bool invert)
{
	Track ret = tr;
	for (int n = 0; n < ret.nOfNodes(); n++) {
		TimedMat nodetracen = ret.position.col(n);
		nodetracen = projection(a.position, b.position, c.position, nodetracen);
		ret.position.getRefData()(span::all, span(n), span::all) = nodetracen.getData();

	}
	if (ret.hasRotation) {

		for (int n = 0; n < ret.rotation.nOfCols(); n++) {
			TimedMat bonetracen = ret.rotation.col(n);
			bonetracen = quatprojection(a.position, b.position, c.position, bonetracen);
			//
			ret.rotation.getRefData()(span::all, span(n), span::all) = bonetracen.getData();
		}
	}

	return ret;
}

void MoMa::Geometry::translate(TimedCube &pos, double x, double y, double z) {

	//Works only on the 3D position TimedCube of the track
	vec translation;
	translation << x << y << z;

	cube translateCube(1, pos.nOfRows()*pos.nOfCols(), pos.nOfFrames()); //We use first the shape of a matrix so we can use the repmat function
	translateCube.tube(span(0), span::all) = repmat(translation, pos.nOfCols(), pos.nOfFrames()); //Replicate the vector to make a cube with the dimensions of the track
	translateCube.reshape(pos.nOfRows(), pos.nOfCols(), pos.nOfFrames()); //Reshape to the initial dimension

	//Todo : use each_col/row/slice methods of the arma::cube (not available in this version)

	pos.getRefData() += translateCube;
}

void MoMa::Geometry::translate(TimedMat &pos, double x, double y, double z) {

	//Works only on the 3D position TimedCube of the track
	vec translation;
	translation << x << y << z;

	mat translateMat = repmat(translation, 1, pos.nOfFrames()); //Replicate the vector to make a cube with the dimensions of the track

	//Todo : use each_col/row/slice methods of the arma::cube (not available in this version)

	pos.getRefData() += translateMat;
}

void MoMa::Geometry::translate(MoMa::Track & tr, double x, double y, double z)
{
	bool isLocal = false;
	if (!tr.hasGlobalCoordinate) {

		isLocal = true;
		tr.localToGlobal();
	}

	translate(tr.position, x, y, z);

	if (isLocal)
		tr.globalToLocal();
}

void MoMa::Geometry::translate(MoMa::Track & tr, arma::mat translation)
{
	for (uint i = 0; i < tr.nOfNodes(); i++)
	{
		tr.position.getRefData()(arma::span::all, arma::span(i), arma::span::all) -= translation;
	}
}

void MoMa::Geometry::placeOnOrigin(MoMa::Track & tr, std::string Pelvis, std::string LHip, std::string RHip)
{
	bool isLocal = false;
	if (!tr.hasGlobalCoordinate) {

		isLocal = true;
		tr.localToGlobal();
	}

	//Move coordinate system to initial (i.e. first frame) frontal plane, origin on LHip
	Trace a;
	vec tmp = tr(RHip)[0u].position;
	a.setPosition(repmat(tmp, 1, tr.nOfFrames()), tr.frameRate(), tr.position.initialTime());
	Trace b;
	tmp = tr(RHip)[0u].position;
	tmp(2) = tmp(2) + 1000;
	b.setPosition(repmat(tmp, 1, tr.nOfFrames()), tr.frameRate(), tr.position.initialTime());
	Trace c;
	c.setPosition(repmat(tr(LHip)[0u].position, 1, tr.nOfFrames()), tr.frameRate(), tr.position.initialTime());

	tr = Geometry::projection(a, b, c, tr);

	//Reorient global axes correctly
	tmp << 0 << 0 << 0;
	a.setPosition(repmat(tmp, 1, tr.nOfFrames()), tr.frameRate(), tr.position.initialTime());
	tmp << 0 << 0 << -1;
	b.setPosition(repmat(tmp, 1, tr.nOfFrames()), tr.frameRate(), tr.position.initialTime());
	tmp << 0 << 1 << 0;
	c.setPosition(repmat(tmp, 1, tr.nOfFrames()), tr.frameRate(), tr.position.initialTime());
	tr = Geometry::projection(a, b, c, tr);

	//Translate (translate track to place pelvis at first frame to 0,0,0)
	Geometry::translate(tr, -tr(Pelvis)[0u].position(0), -tr(Pelvis)[0u].position(1), -tr(Pelvis)[0u].position(2));

	if (isLocal)
		tr.globalToLocal();
}

arma::mat MoMa::Geometry::centerOfMassmat(MoMa::Track & tr) {

	bool isLocal = false;
	if (!tr.hasGlobalCoordinate) {

		isLocal = true;
		tr.localToGlobal();
	}

	mat center;
	center.zeros(3, tr.nOfFrames());

	//avoid nans on each frame
	for (uint f = 0; f < tr.nOfFrames(); f++) {

		center.col(f) = nanmean(tr.framePosition(f), 1);
	}

	if (isLocal)
		tr.globalToLocal();

	return center;
}

MoMa::TimedMat MoMa::Geometry::centerOfMass(MoMa::Track & tr) {

	mat center = centerOfMassmat(tr);
	if (!tr.position.isTimed())
		return TimedMat(tr.position.frameRate(), center, tr.position.initialTime());
	else
		return TimedMat(tr.position.getTimeVec(), center);
}

MoMa::Trace MoMa::Geometry::centerOfMassTrace(MoMa::Track & tr) {
	
	Trace ret;
	ret.setTimeFlag(tr.position.isTimed());
	ret.position = centerOfMass(tr);
	return ret;
}

void MoMa::Geometry::COMToOrigin(MoMa::Track & tr)
{

	bool isLocal = false;
	if (!tr.hasGlobalCoordinate) {

		isLocal = true;
		tr.localToGlobal();
	}

	mat center = centerOfMassmat(tr);
	translate(tr, center);

	if (isLocal)
		tr.globalToLocal();
}

void  MoMa::Geometry::stickToOrigin(MoMa::Track & tr, std::string LHip, std::string RHip)
{
	bool isLocal = false;
	if (!tr.hasGlobalCoordinate) {

		isLocal = true;
		tr.localToGlobal();
	}

	//Move coordinate system to frontal plane, origin between LHip and RHip
	Trace a = tr(RHip);
	Trace c = tr(LHip);
	a.position.getRefData() = (a.position.getData() + c.position.getData()) / 2; //Origin between LHip and RHip
	Trace b = a;
	b.position.getRefData().row(2) += 1000; //Frontal plane!
	tr = Geometry::projection(a, b, c, tr);

	//Reorient global axes correctly
	vec tmp;
	tmp << 0 << 0 << 0;
	a.setPosition(repmat(tmp, 1, tr.nOfFrames()), tr.frameRate(), tr.position.initialTime());
	tmp << 0 << 0 << -1;
	b.setPosition(repmat(tmp, 1, tr.nOfFrames()), tr.frameRate(), tr.position.initialTime());
	tmp << 0 << 1 << 0;
	c.setPosition(repmat(tmp, 1, tr.nOfFrames()), tr.frameRate(), tr.position.initialTime());
	tr = Geometry::projection(a, b, c, tr);

	if (isLocal)
		tr.globalToLocal();
}

void  MoMa::Geometry::stickToOriginLoose(MoMa::Track & tr, std::string rootnodename)
{
	//placeOnOrigin(tr, Pelvis, LHip, RHip);

	bool isLocal = false;
	if (!tr.hasGlobalCoordinate) {

		isLocal = true;
		tr.localToGlobal();
	}

	mat origin = tr.position.getRefData()(arma::span::all, arma::span(tr.nodeList->index(rootnodename)), arma::span::all);
	translate(tr, origin);

	if (isLocal)
		tr.globalToLocal();
}

void  MoMa::Geometry::scaleSkeleton(MoMa::Track & tr, float newsize)
{

	float meansize = meanSize(tr);
	tr.position.getRefData() /= meansize;
	tr.position.getRefData() *= newsize;

}
