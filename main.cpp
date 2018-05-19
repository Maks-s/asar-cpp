#include <iostream>
#include <string>
#include "asar.h"

std::string argGet( char *argv[], int argc, const char *arg ) {
	for ( int i = 1; i < argc; ++i ) {
		if ( strcmp(argv[i], arg) == 0 ) {
			if ( argc == i+1 ) // out of bounds
				return "";
			return argv[i+1];
		}
	}
	return "";
}

bool argExist( char *argv[], int argc, const char *arg ) {
	for ( int i = 1; i < argc; ++i ) {
		if ( strcmp(argv[i], arg) == 0 )
			return true;
	}
	return false;
}

// unpack when it's drag-&-drop'd
int wildUnpack( char *path ) {
#ifdef _WIN32
	const char DIR_SEPARATOR = '\\';
#else
	const char DIR_SEPARATOR = '/';
#endif // _WIN32

	uint iPosParent;
	for ( uint i=0; i < strlen(path); ++i ) {
		if ( path[i] == DIR_SEPARATOR )
			iPosParent = i;
	}

	std::string sOutPath (path, iPosParent+1);

	asarArchive archive;
	archive.unpack(path, sOutPath);

	return 0;
}

int main( int argc, char *argv[] ) {
	if ( argc == 2 ) {
		uint iLength = strlen(argv[1]);
		if ( iLength < 5 )
			return 1;

		// if string doesn't ends with .asar
		if ( 	argv[1][iLength-5] != '.' ||
				argv[1][iLength-4] != 'a' ||
				argv[1][iLength-3] != 's' ||
				argv[1][iLength-2] != 'a' ||
				argv[1][iLength-1] != 'r' ) {
			return 1;
		}

		return wildUnpack(argv[1]);

	} else if ( argc < 3 || ( !argExist(argv, argc, "--pack") && !argExist(argv, argc, "--unpack") ) ) {
		std::cout << "asar arguments :\n\tasar --unpack [-o out] archive..." << std::endl;
		return 1;
	}

	asarArchive archive;
	bool bOutpath = argExist(argv, argc, "-o");
	std::string sOutpath;
	if ( bOutpath )
		sOutpath = argGet(argv, argc, "-o");

	if ( argExist(argv, argc, "--pack") ) {
		std::cout << "--pack isn't implemented yet, use --unpack" << std::endl;
		/*
			archive.pack( (std::string)argv[argc-1], sOutpath );
		*/
	} else {
		if ( bOutpath ) {
			for ( int i = 4; i < argc; ++i ) {
				archive.unpack( argv[i], sOutpath );
			}
		} else {
			for ( int i = 2; i < argc; ++i ) {
				archive.unpack( argv[i] );
			}
		}
	}

	return 0;
}

