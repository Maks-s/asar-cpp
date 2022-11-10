#include <iostream>
#include <string>
#include "asar.h"


void printHelp(const char *argv0) {
	std::cout << argv0 << " arguments :\n\t"
		<< argv0 << " unpack [-o out] archive...\n\t"
		<< argv0 << " list archive..." << std::endl;
}

int main( int argc, char *argv[] ) {
	if ( argc == 2 ) {
		uint iLength = strlen(argv[1]);
		if ( iLength < 5 )
			return 1;

		// if string doesn't end with .asar
		if ( strcmp(argv[1] + iLength-5, ".asar") != 0 ) {
			return 1;
		}

		// unpack into the archive's directory when it's drag-&-dropped (Windows)
		// or the current working directory if run from command line
		asarArchive archive;
		return archive.unpack( argv[1] );

	} else if ( argc == 1 ) {
		printHelp(argv[0]);
		return 1;
	}

	if ( strcmp(argv[1], "pack") == 0 ) {
		std::cout << "'pack' isn't implemented yet, use 'unpack' or 'list'" << std::endl;
	} else if ( strcmp(argv[1], "list") == 0 ) {
		asarArchive archive;

		for ( int i = 2; i < argc; ++i ) {
			archive.list( argv[i] );
		}
	} else if ( strcmp(argv[1], "unpack") == 0 ) {
		asarArchive archive;

		if ( argc > 4 && strcmp(argv[2], "-o") == 0 ) {
			for ( int i = 4; i < argc; ++i ) {
				archive.unpack( argv[i], argv[3] );
			}
		} else {
			for ( int i = 2; i < argc; ++i ) {
				archive.unpack( argv[i] );
			}
		}
	} else {
	  printHelp(argv[0]);
		return 1;
  }

	return 0;
}

