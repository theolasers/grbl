/*
 * sdcardreader.h
 *
 *  Created on: 21 Mar 2016
 *      Author: gcamp
 */

#ifndef SDCARDREADER_H_
#define SDCARDREADER_H_

#include "grbl.h"
#include "Sd2Card.h"
#include "SdVolume.h"
#include "SdFile.h"

class SDCardReader {
public:
	SDCardReader();
	~SDCardReader();

	bool openFile(const char *fileName);
	SdFile &getCurrentFile() { return currentFile_; }

private:
	Sd2Card SDcard_;
	SdVolume SDvolume_;
	SdFile SDroot_;
	SdFile currentFile_;
};

#endif /* SDCARDREADER_H_ */
