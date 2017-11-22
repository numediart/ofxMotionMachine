#ifdef SQLSupport

#include "mmSqlArchiver.h"
#include <windows.h>
#include <ctime>
#include "time.h"
#include <iostream>
#include <sstream>

template<typename T>
std::string toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
class sqlArchiver:: DataBuf : public std::streambuf
{
public:
	DataBuf(char * d, size_t s) {
		setg(d, d, d + s);
	}
};

sqlArchiver::sqlArchiver() {
	con = 0;
	driver = get_driver_instance();
	mFrameCpt = 0;
}


sqlArchiver::~sqlArchiver() {
}

void sqlArchiver::createDataBase(std::string ipAdress, std::string port) {
	try {
		sql::Statement *stmt;
		//MYSQL *mysql;
		SYSTEMTIME st1, st2;
		GetSystemTime(&st1);
		driver = get_driver_instance();
		std::string serverAdress = "tcp://" + ipAdress + ":" + port;
		if (con) {
			delete con;
			con = 0;
		}
		con = driver->connect(serverAdress.c_str(), "user", "user");
		//mysql_real_connect(mysql, "tcp://127.0.0.1:3306", "root", "Marcel666", NULL, 0, NULL, 0);

		stmt = con->createStatement();

		stmt->execute("DROP DATABASE IF EXISTS atvb");
		stmt->execute("CREATE DATABASE atvb CHARACTER SET 'utf8'");
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
	}
}
void sqlArchiver::createIdentityTable(std::string ipAdress, std::string port) {
	try {
		sql::Statement *stmt;
		//MYSQL *mysql;
		SYSTEMTIME st1, st2;
		GetSystemTime(&st1);
		driver = get_driver_instance();
		std::string serverAdress = "tcp://" + ipAdress + ":" + port;
		if (con) {
			delete con;
			con = 0;
		}
		con = driver->connect(serverAdress.c_str(), "user", "user");
		//mysql_real_connect(mysql, "tcp://127.0.0.1:3306", "root", "Marcel666", NULL, 0, NULL, 0);

		stmt = con->createStatement();

		stmt->execute("USE atvb");
		std::string queryString("");
		queryString += "CREATE TABLE Identity (";

		queryString += " id int NOT NULL AUTO_INCREMENT,";
		queryString += " username VARCHAR(200) NOT NULL,";
		queryString += " name VARCHAR(200) NOT NULL,";
		queryString += " firstName VARCHAR(60) NOT NULL,";
		queryString += " email VARCHAR(100) NULL,";
		queryString += " PRIMARY KEY (id)";
		queryString += " ) ";
		queryString += "ENGINE=MyISAM;";
		stmt->execute(queryString.c_str());

		delete stmt;

	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
	}
}
void sqlArchiver::createframesDataTable(std::string ipAdress, std::string port) {
	try {
		sql::Statement *stmt;
		//MYSQL *mysql;
		SYSTEMTIME st1, st2;
		GetSystemTime(&st1);
		std::string serverAdress = "tcp://" + ipAdress + ":" + port;
		if (con) {
			delete con;
			con = 0;
		}

		con = driver->connect(serverAdress.c_str(), "user", "user");
		//mysql_real_connect(mysql, "tcp://127.0.0.1:3306", "root", "Marcel666", NULL, 0, NULL, 0);

		stmt = con->createStatement();

		stmt->execute("USE atvb");
		stmt->execute("DROP TABLE IF EXISTS frames");

		std::string queryString = std::string("");
		queryString += "CREATE TABLE frames (";

		queryString += " id int NOT NULL AUTO_INCREMENT,";
		queryString += " framecounter int DEFAULT NULL,";
		queryString += " timestamp double DEFAULT NULL,";
		queryString += " positionSize int DEFAULT NULL,";
		queryString += " position blob DEFAULT NULL,";
		queryString += " rotationSize int DEFAULT NULL,";
		queryString += " rotation blob DEFAULT NULL,";
		queryString += " PRIMARY KEY (id)";
		queryString += " ) ";
		queryString += "ENGINE=MyISAM;";

		stmt->execute(queryString.c_str());
		delete stmt;

	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
	}
}

void sqlArchiver::createSkeletonDataTable(std::string ipAdress, std::string port) {
	try {
		sql::Statement *stmt;
		//MYSQL *mysql;
		SYSTEMTIME st1, st2;
		GetSystemTime(&st1);
		std::string serverAdress = "tcp://" + ipAdress + ":" + port;
		if (con) {
			delete con;
			con = 0;
		}

		con = driver->connect(serverAdress.c_str(), "user", "user");
		//mysql_real_connect(mysql, "tcp://127.0.0.1:3306", "root", "Marcel666", NULL, 0, NULL, 0);

		stmt = con->createStatement();

		stmt->execute("USE atvb");
		stmt->execute("DROP TABLE IF EXISTS skeletons");

		std::string queryString = std::string("");
		queryString += "CREATE TABLE frames (";

		//queryString += " id int NOT NULL AUTO_INCREMENT,";
		queryString += " id int NOT NULL,";
		//queryString += " userId int NOT NULL";
		//queryString += " sessionId int NOT NULL";
		queryString += " framecounter int DEFAULT NULL,";
		queryString += " timestamp double DEFAULT NULL,";
		queryString += " positionSize int DEFAULT NULL,";
		queryString += " position blob DEFAULT NULL,";
		queryString += " rotationSize int DEFAULT NULL,";
		queryString += " rotation blob DEFAULT NULL,";
		queryString += " AUTO_INCREMENT(id),";
		queryString += " PRIMARY KEY (id)";
		queryString += " ) ";
		queryString += "ENGINE=MyISAM;";

		stmt->execute(queryString.c_str());
		delete stmt;

	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
	}
}
void sqlArchiver::connect(std::string ipAdress, std::string port) {
	if (con) {
		delete con;
		con = 0;
	}
	std::string serverAdress = "tcp://" + ipAdress + ":" + port;

	con = driver->connect(serverAdress.c_str(), "user", "user");
}


int sqlArchiver::identify() {
	return 0;
}

void sqlArchiver::clearDataBase() {
}

void sqlArchiver::pushFrameData(double* data0, int dataSize0, double* data1, int dataSize1,int frameId,double frameRate) {
	try {
		sql::Statement*  stmt = con->createStatement();

		stmt->execute("USE atvb");
		delete stmt;
		sql::PreparedStatement *prep_stmt;
		DataBuf dataBuffer0((char*)data0, dataSize0 * 8);// (&mTrack.back()->position.getRefData().slice(getAppIndex()).at(0)), mTrack.back()->nOfNodes() * 3 * 8);
		std::istream dataStream0(&dataBuffer0);
		DataBuf dataBuffer1((char*)data1, dataSize1 * 8);// (&mTrack.back()->position.getRefData().slice(getAppIndex()).at(0)), mTrack.back()->nOfNodes() * 3 * 8);
		std::istream dataStream1(&dataBuffer1);

		//prep_stmt = con->prepareStatement("INSERT INTO frames VALUES (?, ?,?, ?,?, ?,?)");
		prep_stmt = con->prepareStatement("INSERT INTO frames( framecounter,timestamp, positionSize,position, rotationSize,rotation) VALUES (?,?, ?,?, ?,?)");

		//prep_stmt->setInt(1, ++mFrameCpt);
		prep_stmt->setInt(1, frameId);
		prep_stmt->setDouble(2, 1.0*(frameId + 1) / frameRate);
		prep_stmt->setInt(3, dataSize0);
		prep_stmt->setBlob(4, &dataStream0);
		prep_stmt->setInt(5, dataSize1);
		prep_stmt->setBlob(6, &dataStream1);

		prep_stmt->executeUpdate();
		delete prep_stmt;

	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
	}
}
void sqlArchiver::getFrameData(double** data0, int &dataSize0, double** data1, int &dataSize1, int framecounter) {
	try
	{
		sql::Statement*  stmt = con->createStatement();
		stmt->execute("USE atvb");
		std::string queryString("");
		queryString += "SELECT * FROM frames where framecounter = ";
		queryString += toString(framecounter);
		sql::PreparedStatement *prep_stmt;

		prep_stmt = con->prepareStatement(queryString.c_str());
		sql::ResultSet *res = prep_stmt->executeQuery();
		int cpt = 0;
		while (res->next()) {
			cpt++;
			int i = res->getInt("id") - 1;
			int frameId = res->getInt("framecounter");
			//std::cout << id << std::endl;
			dataSize0 = res->getInt("positionSize");
			char *s;
			std::streamsize blobSize;
			std::istream *PosStream = res->getBlob("position");

			PosStream->seekg(0, std::ios::end);
			blobSize = PosStream->tellg();
			PosStream->seekg(0, std::ios::beg);
			s = new char[blobSize];
			PosStream->read((char*)s, blobSize);
			(*data0) = (double*)s;
			dataSize1 = res->getInt("rotationSize");
			std::istream *RotStream = res->getBlob("rotation");
			RotStream->seekg(0, std::ios::end);
			blobSize = RotStream->tellg();
			RotStream->seekg(0, std::ios::beg);
			s = new char[blobSize];
			RotStream->read((char*)s, blobSize);
			(*data1) = (double*)s;
			delete RotStream;
			delete PosStream;
		}
//		std::cout << cpt << std::endl;


	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
	}
}

void sqlArchiver::getFrameData(double** data0, int &dataSize0, double** data1, int &dataSize1, double frameTime) {
	try
	{
		sql::Statement*  stmt = con->createStatement();
		stmt->execute("USE atvb");
		std::string queryString("");
		queryString += "SELECT * FROM frames ORDER BY ABS(";
		queryString += toString(frameTime);
		queryString += " - timestamp) ASC ";
		sql::PreparedStatement *prep_stmt;

		prep_stmt = con->prepareStatement(queryString.c_str());
		sql::ResultSet *res = prep_stmt->executeQuery();
		int cpt = 0;
		res->next(); 
		{
			cpt++;
			int i = res->getInt("id") - 1;
			int frameId = res->getInt("framecounter");
			//std::cout << id << std::endl;
			dataSize0 = res->getInt("positionSize");
			char *s;
			std::streamsize blobSize;
			std::istream *PosStream = res->getBlob("position");

			PosStream->seekg(0, std::ios::end);
			blobSize = PosStream->tellg();
			PosStream->seekg(0, std::ios::beg);
			s = new char[blobSize];
			PosStream->read((char*)s, blobSize);
			(*data0) = (double*)s;
			dataSize1 = res->getInt("rotationSize");
			std::istream *RotStream = res->getBlob("rotation");
			RotStream->seekg(0, std::ios::end);
			blobSize = RotStream->tellg();
			RotStream->seekg(0, std::ios::beg);
			s = new char[blobSize];
			RotStream->read((char*)s, blobSize);
			(*data1) = (double*)s;
			delete RotStream;
			delete PosStream;
		}
		//		std::cout << cpt << std::endl;


	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode() << ")" << std::endl;
	}
}
void sqlArchiver::destroyDataBase() {
}
#endif