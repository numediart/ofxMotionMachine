#include "mmParser.h"
#include "mmXmlArchiver.h"

using namespace std;
using namespace MoMa;


Parser::Parser(string const &fName, Track *tr) {

    fileName = fName;

    checkFilePath(fileName);

    size_t sep = fileName.find_last_of("\\/");
    size_t dot = fileName.find_last_of(".");

    extension = fileName.substr(dot + 1);
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    tr->hasGlobalCoordinate = true;//value by default
	tr->setPathFileName(fName);
    if (extension == "bones") {

        tr->clearData();
        tr->bones(fileName);
        if (tr->hasBoneList == true)
            tr->boneList->updateBoneChildrenName();
        tr->setJointOffsetRotation();
    }

    else if (extension == "nodes") {

        tr->clearData();
        tr->nodes(fileName);
        if (tr->hasBoneList == true)
            tr->boneList->updateBoneChildrenName();
        tr->setJointOffsetRotation();
    }

    else if (extension == "txt") {

        tr->clearData();
        tr->type = FLAT;
        tr->setFileName(fileName.substr(sep + 1, dot - sep - 1));

        FlatParser flatParser(fileName, tr);

        if (!tr->hasNodeList) {

            cout << "Warning (flat txt file parsing): you should load a nodelist before the data." << endl;
        }

        if (!tr->hasBoneList) {

            cout << "Warning (flat txt file parsing): you should load a bonelist before the data." << endl;
        }

        if (tr->hasNodeList && tr->nodeList->size() != tr->nOfNodes()) {

            cout << "Warning (FlatParser): the number of nodes in the data does not correponds with the number of nodes in the nodelist. You should load a corresponding nodelist before the data." << endl;
        }

        tr->position.SetValidParam();
        if (tr->hasRotation)
            tr->rotation.SetValidParam();
    }

    else if (extension == "v3d") {
        tr->clear();
        tr->init();
        tr->type = V3D;
        tr->setFileName(fileName.substr(sep + 1, dot - sep - 1));

        V3dParser v3dParser(fileName, tr);
        tr->position.SetValidParam();
        if (tr->hasRotation)
            tr->rotation.SetValidParam();

        tr->boneList->updateBoneChildrenName();
        tr->setJointOffsetRotation();
    }

    else if (extension == "c3d") {

        tr->clearData();
        tr->type = C3D;
        tr->setFileName(fileName.substr(sep + 1, dot - sep - 1));

        C3dParser c3dParser(fileName, tr);

        if (!tr->hasBoneList) {

            cout << "Warning (c3d file parsing): you should load a bonelist before the data." << endl;
        }

        tr->position.SetValidParam();
        if (tr->hasRotation)
            tr->rotation.SetValidParam();
    }

    else if (extension == "tsv") {

        tr->clearData();
        tr->type = TSV;
        tr->setFileName(fileName.substr(sep + 1, dot - sep - 1));

        TsvParser tsvParser(fileName, tr);

        if (!tr->hasBoneList) {

            cout << "Warning (tsv file parsing): you should load a bonelist before the data." << endl;
        }

        tr->position.SetValidParam();
        if (tr->hasRotation)
            tr->rotation.SetValidParam();
    }

    else if (extension == "bvh") {
        tr->clear();
        tr->init();
        tr->type = BVH;
        tr->setFileName(fileName.substr(sep + 1, dot - sep - 1));

        BvhParser::load(fileName, tr, true, false);
        tr->position.SetValidParam();
        if (tr->hasRotation)
            tr->rotation.SetValidParam();
        tr->boneList->updateBoneChildrenName();
        tr->setJointOffsetRotation();

    }
    else if (extension == "cmp") {

        tr->type = CMP;
        tr->setFileName(fileName.substr(sep + 1, dot - sep - 1));

        CmpParser Parser;
        Parser.load(fileName, tr);
        tr->position.SetValidParam();
        if (tr->hasRotation)
            tr->rotation.SetValidParam();
        //        tr->setJointOffsetRotation(true);

    }
    else if (extension == "kin") {

        tr->clearData();
        tr->type = KIN;
        tr->setFileName(fileName.substr(sep + 1, dot - sep - 1));
        KinParser kinParser(fileName, tr);
        
        if (!tr->hasNodeList) {

            cout << "Warning (flat txt file parsing): you should load a nodelist before the data." << endl;
        }

        if (!tr->hasBoneList) {

            cout << "Warning (flat txt file parsing): you should load a bonelist before the data." << endl;
        }

        if (tr->hasNodeList && tr->nodeList->size() != tr->nOfNodes()) {

            cout << "Warning (FlatParser): the number of nodes in the data does not correponds with the number of nodes in the nodelist. You should load a corresponding nodelist before the data." << endl;
        }

        tr->position.SetValidParam();
        if (tr->hasRotation)
            tr->rotation.SetValidParam();        
        if (tr->hasBoneList == true)
            tr->boneList->updateBoneChildrenName();
        tr->setJointOffsetRotation();
        tr->setFrameRate(30);
    }

    else if (extension == "xml") {

        tr->type = XML;
        tr->setFileName(fileName.substr(sep + 1, dot - sep - 1));
		XmlArchiver myArchiver;
		myArchiver.load(fileName);

        //XmlParser xmlParser(fileName, tr);
        
		myArchiver.getTrack(*tr);
		tr->position.SetValidParam();
        if (tr->hasRotation) {
            tr->rotation.SetValidParam();
            tr->setJointOffsetRotation();
        }
    }    

    else cout << "Invalid file format" << endl;
}


