#include <iostream>
#include <string>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include "asar.h"

#ifdef _WIN32

#include <direct.h>
#define MKDIR(a) mkdir(a)
#define DIR_SEPARATOR '\\'

#else

#include <sys/stat.h>
#define MKDIR(a) mkdir(a,0777)
#define DIR_SEPARATOR '/'

#endif // _WIN32



// Return number of files in a folder
size_t asarArchive::numSubfile( DIR* dir ) {
	size_t uFiles = 0;
	struct dirent* file;

	long iDirPos = telldir(dir);
	rewinddir(dir);

	while ( (file = readdir(dir)) )
		uFiles++;

	seekdir(dir,iDirPos);
	return uFiles-2; // remove '.' and '..' dirs
}

// Pack files, not working at the moment
void asarArchive::packFiles( std::string sPath, std::string &sFiles, std::vector<char> &vBinFile ) {
/*
	DIR* dir = opendir( sPath.c_str() );

	if ( !dir )
		return;

	struct dirent* file;
	size_t uFolderSize = numSubfile( dir );
	size_t uFileNum = 0;

	while ( (file = readdir(dir)) ) {
		if ( !strcmp(file->d_name, ".") || !strcmp(file->d_name, "..") )
			continue;

		std::string sLocalPath = (sPath + file->d_name);
		const char* cPath = sLocalPath.c_str();

		DIR* isDir = opendir( cPath );

		if ( isDir ) {
			// WHO NEEDS A JSON LIB WHEN WE CAN DO THE DIRTY WORK OURSELVES
			sFiles += (std::string)"\"" + file->d_name + "\":{\"files\":{";
			closedir( isDir );
			packFiles( sLocalPath + DIR_SEPARATOR, sFiles, vBinFile );
			sFiles.push_back('}');
			sFiles.push_back('}');
		} else {
			std::ifstream ifsFile( sLocalPath, std::ios::binary | std::ios::ate );
			size_t szFile = ifsFile.tellg();
			std::vector<char> fileBuf ( szFile );
			ifsFile.seekg(0, std::ios::beg);
			ifsFile.read( fileBuf.data(), szFile );
			for ( auto bChar : fileBuf )
				vBinFile.push_back(bChar);

			ifsFile.close();

			sFiles += (std::string)"\"" + file->d_name + "\":{\"size\":" + std::to_string(szFile) + ",\"offset\":\"" + std::to_string(m_szOffset) + "\"}";
			m_szOffset += szFile;
		}
		if ( ++uFileNum < uFolderSize )
			sFiles.push_back(',');
	}

	closedir(dir);
*/
}

void asarArchive::unpackFiles( rapidjson::Value& object, const std::string &sPath ) {
	if ( !object.IsObject() ) // how ?
		return;

	if ( m_extract && !sPath.empty() )
		MKDIR( sPath.c_str() );

	for ( auto itr = object.MemberBegin(); itr != object.MemberEnd(); ++itr ) {
		std::string sFilePath = sPath + itr->name.GetString();
		rapidjson::Value& vMember = itr->value;
		if ( vMember.IsObject() ) {
			if ( vMember.HasMember("files") ) {
				if ( m_extract )
					MKDIR( sFilePath.c_str() );

				unpackFiles( vMember["files"], sFilePath + DIR_SEPARATOR );
			} else {
				if ( !( vMember.HasMember("size") && vMember.HasMember("offset") && vMember["size"].IsInt() && vMember["offset"].IsString() ) )
					continue;

				if ( !m_extract ) {
					std::cout << '\t' << sFilePath << std::endl;
					continue;
				}

				size_t uSize = vMember["size"].GetUint();
				int uOffset = std::stoi( vMember["offset"].GetString() );

				char fileBuf[uSize];
				m_ifsInputFile.seekg(m_headerSize + uOffset);
				m_ifsInputFile.read(fileBuf, uSize);
				std::ofstream ofsOutputFile( sFilePath, std::ios::trunc | std::ios::binary );

				if ( !ofsOutputFile ) {
					std::cerr << "Error when writing to file " << sFilePath << std::endl;
					continue;
				}
				ofsOutputFile.write( fileBuf, uSize );
				ofsOutputFile.close();
			}
		}
	}
}

// Unpack archive to a specific location
bool asarArchive::unpack( const std::string &sArchivePath, std::string sExtractPath ) {
	m_ifsInputFile.open( sArchivePath, std::ios::binary );
	if ( !m_ifsInputFile ) {
		perror("");
		return false;
	}

	char sizeBuf[8];
	m_ifsInputFile.read( sizeBuf, 8 );
	uint32_t uSize = *(uint32_t*)(sizeBuf + 4) - 8;

	m_headerSize = uSize + 16;
	char headerBuf[uSize + 1];
	m_ifsInputFile.seekg(16); // skip header
	m_ifsInputFile.read(headerBuf, uSize);
	headerBuf[uSize] = 0; // append nul byte to end

	rapidjson::Document json;
	rapidjson::ParseResult res = json.Parse( headerBuf );
	if ( !res ) {
		std::cout << rapidjson::GetParseError_En(res.Code()) << std::endl;
		return false;
	}

	if ( !sExtractPath.empty() && sExtractPath.back() != DIR_SEPARATOR )
		sExtractPath.push_back(DIR_SEPARATOR);

	unpackFiles( json["files"], sExtractPath );
	m_szOffset = 0;

	m_ifsInputFile.close();

	return true;
}

// Pack archive, not working at the moment
bool asarArchive::pack( std::string sPath, std::string sFinalName ) {
/*
	std::string sFiles = "{\"files\":{";
	std::vector<char> vBinFile;

	packFiles( sPath, sFiles, vBinFile );
	sFiles.push_back('}');
	sFiles.push_back('}');

	std::ofstream ofsOutputFile( sFinalName, std::ios::binary | std::ios::trunc );
	ofsOutputFile.write( sFiles.c_str(), sFiles.length() );
	ofsOutputFile.write( &vBinFile[0], vBinFile.size() );
	ofsOutputFile.close();
*/
	return true;
}

// List archive content
bool asarArchive::list( const std::string &sArchivePath ) {
	m_extract = false;
	std::cout << sArchivePath << ":" << std::endl;
	bool ret = unpack( sArchivePath );
	m_extract = true;

	return ret;
}
