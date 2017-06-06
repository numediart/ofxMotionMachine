
#ifndef __mmOfMutedSkeleton__
#define __mmOfMutedSkeleton__
#include "mmOfSkeleton.h"
namespace MoMa {
	class ofMutedSkeleton : public ofSkeleton
	{
		/**
		* This class is a way to test the rendering of muted skeleton for ocuslab. 
		* We create a structure mutedBones to store the link between the new muted body and the old normal ofSkeleton
		*/
	public:
		ofMutedSkeleton(const MoMa::Track &pTrack);
		virtual ~ofMutedSkeleton();
		//void init();
		/**
		* Display all the recorded bones.
		*/
		void displayNameListBone();
		/**
		* Create and store a new muted bone
		* @param parentMutedBoneName name of the bone parent for the new bone in the muted structure.
		* @param newChildrenBoneNameRad name of the new muted bone (must be unique).
		* @param boneName normal bone name to copy in the muted structure.
		* @param fullPath copy the whole articulated chain (with automatic name assignement )or just one bone.
		*/
		bool pushBone(std::string  parentMutedBoneName, std::string newChildrenBoneNameRad, std::string boneName, bool fullPath = false, std::string parentJointName=std::string(""));
		void draw(unsigned int index);
		bool setPreTrans(std::string boneName ,arma::vec3 pTrans);
		bool setPreRot(std::string boneName, quaternion pRot);
		bool setPreRot(std::string boneName, double angle, arma::vec3 axis);
		bool setPreScale(std::string boneName, arma::vec3 pScale);
		bool setFrameDelay(std::string boneName, int pDelay);
		bool hideBone(std::string boneName);
		bool hideAll();
		bool showBone(std::string boneName);
		bool showAll();

	protected:
		BoneList mutedBoneList;//We will store the muted bone list in a BoneList with a unique name for each bone
		bool checkBoneName(std::string boneName);
		void boneLocalDraw(int index, const arma::cube &rotation, const arma::cube &position, boneMapType::iterator it);
		 
	};
}
#endif