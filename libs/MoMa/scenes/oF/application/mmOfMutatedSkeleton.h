
#ifndef __mmOfMutatedSkeleton__
#define __mmOfMutatedSkeleton__
#include "mmOfSkeleton.h"
#include "mmMutatedBoneList.h"
namespace MoMa {
	class ofMutatedSkeleton : public ofSkeleton
	{
		/**
		* This class is a way to test the rendering of mutated skeleton for ocuslab. 
		* We create a structure mutatedBones to store the link between the new mutated body and the old normal ofSkeleton
		*/
	public:
		ofMutatedSkeleton(const MoMa::Track &pTrack);
		virtual ~ofMutatedSkeleton();
		//void init();
		/**
		* Display all the recorded bones.
		*/
		bool addNewTrack(const MoMa::Track &pTrack);
		void displayNameListBone() const;
		int nOfTrack() const { return trackContainer.size(); }
		/**
		* Create and store a new mutated bone
		* @param parentMutatedBoneName name of the bone parent for the new bone in the mutated structure.
		* @param newChildrenBoneNameRad name of the new mutated bone (must be unique).
		* @param boneName normal bone name to copy in the mutated structure.
		* @param fullPath copy the whole articulated chain (with automatic name assignement )or just one bone.
		*/
		bool pushBone(std::string  parentMutatedBoneName, std::string newChildrenBoneNameRad, std::string boneName, bool fullPath = false, int trackId = 0, std::string parentJointName=std::string(""));
		void draw(unsigned int index);
		bool setPreTrans(std::string boneName ,arma::vec3 pTrans);
		bool setPreRot(std::string boneName, quaternion pRot);
		bool setPreRot(std::string boneName, double angle, arma::vec3 axis);
		bool setPreScale(std::string boneName, arma::vec3 pScale);
		bool setFrameDelay(std::string boneName, int pDelay);
		bool setTrackIndex(std::string boneName, int pTrackIndex);
		bool hideBone(std::string boneName);
		bool hideAll();
		bool showBone(std::string boneName);
		bool showAll();
	protected:
		MutatedBoneList mutatedBoneList;//We will store the mutated bone list in a BoneList with a unique name for each bone
		bool checkBoneName(std::string boneName);
		void boneLocalDraw(int index, MutatedBoneMapType::iterator it);
		std::vector<const MoMa::Track*> trackContainer;
		 
	};
}
#endif