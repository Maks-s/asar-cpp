#ifndef ASAR_H_INCLUDED
#define ASAR_H_INCLUDED

#include <rapidjson/document.h>
#include <dirent.h>
#include <string>
#include <fstream>
#include <vector>


class asarArchive {

private:
	std::ifstream m_ifsInputFile;
	size_t m_headerSize = 0;
	size_t m_szOffset = 0;
	bool extract = true;
	void unpackFiles( rapidjson::Value& object, std::string sPath );
	void packFiles( std::string sPath, std::string &sFiles, std::vector<char> &vBinFile );
	size_t numSubfile( DIR* dir );

public:
	bool unpack( std::string sArchivePath, std::string sExtractPath = "" );
	bool pack( std::string sPath, std::string sFinalName );
	bool list( std::string sArchivePath );

};

#endif // ASAR_H_INCLUDED
