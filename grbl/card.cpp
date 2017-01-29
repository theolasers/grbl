#include "grbl.h"
#include "Sd2Card.h"
#include "SdVolume.h"
#include "SdFile.h"
#include "card.h"

static Sd2Card SDcard;
static SdVolume SDvolume;
SdFile SDroot;

bool card_init() {
	if (SDroot.isOpen()) SDroot.close();
	if (!SDcard.init(SPI_EIGHTH_SPEED)) {
		printString("Failed to initialize SD card.\r\n");
		return false;
	}
	if (!SDvolume.init(&SDcard)) {
		printString("Failed to open SD card volume.\r\n");
		return false;
	}
	if (!SDroot.openRoot(&SDvolume)) {
		printString("Failed to open SD card root file system.\r\n");
		return false;
	}
	return true;
}

void card_ls_root() {
	if (!card_init()) return;
	SDroot.ls(LS_R, 2);
}

void card_cat_file(const char *file) {
	if (!card_init()) return;

	SdFile file2;
	if (!file2.open(SDroot, file, O_READ)) {
		printString("Failed to open file:");
		printString(file);
		printString("\r\n");
		return;
	}

	char buff[128];
	int read = 0;
	while ((read = file2.read(buff, 128)) > 0) {
		for (int i=0; i<read; i++) {
			serial_write(buff[i]);
		}
	}
	printString("\r\n");
}
