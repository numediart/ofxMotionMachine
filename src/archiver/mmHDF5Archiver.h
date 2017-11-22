#ifdef HDF5Support

#ifndef mmHDF5Archiver_H
#include "mmTrack.h"

namespace MoMa{
	class HDF5Archiver
	{
	public:
		HDF5Archiver(std::string fileName, bool newFile=true);
		bool saveTrack(std::string groupName,const MoMa::Track &track);
		bool loadTrack(std::string groupName, MoMa::Track &track);
		~HDF5Archiver();
	protected:
		std::string mFileName;
	}; 
};

#endif//mmHDF5Archiver_H
#ifdef HDF5Support


