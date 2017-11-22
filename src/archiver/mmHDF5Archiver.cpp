#ifdef HDF5Support

#include "mmHDF5Archiver.h"
#include <chrono>
#include <thread>

#include <string>
#include "H5Cpp.h"
using namespace H5;
using namespace std;

namespace MoMa {
	void write_hdf5Attribute(H5::DataSet str_dataset, const std::vector<std::string>& strings, std::string AttrName)
	{
		//H5::Exception::dontPrint();
		try
		{
			// HDF5 only understands vector of char* :-(
			std::vector<const char*> arr_c_str;
			for (unsigned ii = 0; ii < strings.size(); ++ii)
				arr_c_str.push_back(strings[ii].c_str());
			//  one dimension
			hsize_t     str_dimsf[1]{ arr_c_str.size() };
			H5::DataSpace   dataspace(1, str_dimsf);
			// Variable length string
			H5::StrType datatype(H5::PredType::C_S1, H5T_VARIABLE);
			// Create a dataset attribute. 
			Attribute attribute = str_dataset.createAttribute(AttrName, datatype,
				dataspace);
			// Write the attribute data. 
			attribute.write(datatype, arr_c_str.data());
		}
		catch (H5::Exception& err){
			throw std::runtime_error(string("HDF5 Error in ")
				+ err.getFuncName()
				+ ": "
				+ err.getDetailMsg());
		}
	}
	HDF5Archiver::HDF5Archiver(std::string fileName, bool newFile) {
		mFileName = fileName;
		if (newFile) {
			H5File file(mFileName, H5F_ACC_TRUNC);
			file.close();
		}
	}



	bool HDF5Archiver::saveTrack(std::string groupName, const MoMa::Track &track){
		try{
			// Create a new file using default property lists.
			H5File file(mFileName, H5F_ACC_RDWR);
			Group group(file.createGroup("/"+groupName));

			// Create the data space for the attribute.
			hsize_t scalDims[1] = { 1 };
			DataSpace attr_dataspace = DataSpace(1, scalDims);
			Attribute attribute = group.createAttribute("frameRate", PredType::IEEE_F32LE,
				attr_dataspace);
			float frameRate[1] = { track.frameRate() };
			attribute.write(PredType::NATIVE_FLOAT, frameRate);
			attribute.close();

			hsize_t dims[3];               // dataset dimensions
			dims[0] = track.nOfFrames();
			dims[1] = track.nOfNodes();
			dims[2] = 3;
			DataSpace *dataspace = new DataSpace(3, dims);
			// Create the dataset in group "MyGroup".  Same note as for the
			// dataspace above.
			DataSet *dataset = new DataSet(file.createDataSet("/"+ groupName +"/position",
				PredType::IEEE_F64LE, *dataspace));
			// Write the data to the dataset using default memory space, file
			// space, and transfer properties.
			dataset->write(track.position.getData().mem, PredType::NATIVE_DOUBLE);
			vector<string> strArray;
			if (track.hasNodeList) {
				strArray.resize(track.nOfNodes());
				for (auto it = track.nodeList->begin(); it != track.nodeList->end(); it++){
					strArray[it->second] = it->first;
				}
				write_hdf5Attribute(*dataset, strArray,"nodeNames");
			}
			delete dataset;
			delete dataspace;
			if (track.hasRotation){
				hsize_t dims[3];               // dataset dimensions
				dims[0] = track.nOfFrames();
				dims[1] = track.nOfBones();
				dims[2] = 4;
				DataSpace *dataspace = new DataSpace(3, dims);
				// Create the dataset in group "MyGroup".  Same note as for the
				// dataspace above.
				DataSet *dataset = new DataSet(file.createDataSet("/" + groupName + "/rotation",
					PredType::IEEE_F64LE, *dataspace));
				// Write the data to the dataset using default memory space, file
				// space, and transfer properties.
				dataset->write(track.rotation.getData().mem, PredType::NATIVE_DOUBLE);
				vector<string> strArray;
				if (track.hasBoneList) {
					strArray.resize(track.nOfBones());
					for (auto it = track.boneList->begin(); it != track.boneList->end(); it++){
						strArray[it->second.boneId] = it->first;
					}
					write_hdf5Attribute(*dataset, strArray,"boneNames");
				}
				delete dataset;
				delete dataspace;
			}
			group.close();
			file.close();
		}
		catch (FileIException error){
			error.printError();
			return false;
		}
		return true;
	}
	bool HDF5Archiver::loadTrack(std::string groupName, MoMa::Track &track) {
		track.position.getRefData().clear();
		try {
			if (!H5File::isHdf5(mFileName))
				return false;
			H5File file(mFileName, H5F_ACC_RDWR);
			if (!file.nameExists("/" + groupName))
				return false;
			Group group(file.openGroup("/" + groupName));
			if (!group.nameExists("/" + groupName+"/position"))
				return false;

			float frameRate=1.f;

			if (group.attrExists("frameRate")) {
				Attribute attr = group.openAttribute("frameRate");
				DataType type = attr.getDataType();
				std::string typeStr = attr.getDataType().fromClass();
				attr.read(type, &frameRate);
			}


			DataSet *dataset = new DataSet(group.openDataSet("position"));
			/*if (dataset->attrExists("nodeNames")) {

				Attribute attr = dataset->openAttribute("nodeNames");
				DataType type = attr.getDataType();
				DataSpace attDataSpace = attr.getSpace();
				hsize_t dim = 0;
				attDataSpace.getSimpleExtentDims(&dim); //The number of strings.
				char **rdata = new char*[dim];
				attr.read(type, rdata);
				if (dim > 0) {
					track.nodeList = std::shared_ptr<NodeList>(new NodeList);
					track.hasNodeList = true;
				}
				for (int i = 0; i < dim; i++)
					track.nodeList->insert(pair<string, int>(rdata[i], i));
			}*/
			DataSpace  dataspace = dataset->getSpace();
			hsize_t dims[3];              // dataset dimensions
			int rank = dataspace.getSimpleExtentDims(dims);
			H5T_class_t type_class = dataset->getTypeClass();
			DataSpace mspace1(rank, dims);
			std::cout << dims[0] * dims[1] * dims[2] << std::endl;
			track.position = TimedCube();
			//track.position.getRefData().resize(dims[2], dims[1], dims[0]);
			arma::cube cubeBuffer(dims[2], dims[1], dims[0]);
			dataset->read(cubeBuffer.memptr(), PredType::NATIVE_DOUBLE, mspace1, dataspace);
			track.position.setData(frameRate, cubeBuffer);

			delete dataset;
			group.close();
			file.close();
		}
		catch (FileIException error)
		{
			error.printError();
			return false;
		}
	}


	HDF5Archiver::~HDF5Archiver()
	{
	}
}

#endif