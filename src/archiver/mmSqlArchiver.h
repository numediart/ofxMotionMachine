#ifdef SQLSupport
#ifndef __sqlArchiver__
#define __sqlArchiver__

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class sqlArchiver
{
public:
	sqlArchiver();
	~sqlArchiver();

	void createDataBase(std::string ipAdress = "127.0.0.1", std::string port = "3306");
	void createIdentityTable(std::string ipAdress = "127.0.0.1", std::string port = "3306");
	void createframesDataTable(std::string ipAdress = "127.0.0.1", std::string port = "3306");
	void createSkeletonDataTable(std::string ipAdress = "127.0.0.1", std::string port = "3306");
	void connect(std::string ipAdress = "127.0.0.1", std::string port = "3306");

	int identify();
	void clearDataBase();
	void pushFrameData(double* data0, int dataSize0, double* data1, int dataSize1, int frameId, double frameRate);
	void getFrameData(double** data0, int &dataSize0, double** data1, int &dataSize1, int id);
	void getFrameData(double** data0, int &dataSize0, double** data1, int &dataSize1, double frameTime);
	void destroyDataBase();
protected:

	class DataBuf;
	sql::Driver *driver;
	sql::Connection *con;
	int mFrameCpt;
};

#endif
#endif
