/*
 * sdcardreader.cpp
 *
 *  Created on: 21 Mar 2016
 *      Author: gcamp
 */

#include "sdcardreader.h"

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

SDCardReader::SDCardReader() {

}

SDCardReader::~SDCardReader() {
	if (SDroot_.isOpen()) SDroot_.close();
}

bool SDCardReader::openFile(const char *fileName) {
	if (!SDcard_.init(SPI_EIGHTH_SPEED)) {
		printPgmString(PSTR("Failed to initialize SD card.\r\n"));
		return false;
	}
	if (!SDvolume_.init(&SDcard_)) {
		printPgmString(PSTR("Failed to open SD card volume.\r\n"));
		return false;
	}
	if (!SDroot_.openRoot(&SDvolume_)) {
		printPgmString(PSTR("Failed to open SD card root file system.\r\n"));
		return false;
	}

	if (!currentFile_.open(SDroot_, fileName, O_READ)) {
		printPgmString(PSTR("Failed to open file:"));
		printString(fileName);
		printPgmString(PSTR("\r\n"));
		return false;
	}
	return true;
}
