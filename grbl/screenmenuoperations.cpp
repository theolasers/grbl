#include "screenmenuoperations.h"
#include "protocol_parser_intf.h"
#include "screen.h"
#include "card.h"
#include "SdFile.h"
#include "print.h"
#include "serial.h"
#include "settings.h"
#include "lcd7920.h"
#include "system.h"
#include "protocol_parser_intf.h"
#include "screenutil.h"

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

extern SdFile SDroot;
extern Lcd7920 *getLCDLib();

static void display_return() {
	Lcd7920 *lcd = getLCDLib();
	lcd->printPgmString(PSTR("Return"));
	lcd->printPgmString(PSTR(" \x80"));
}

ScreenMenuItemDisplayerSettingsValue::ScreenMenuItemDisplayerSettingsValue(int s_index) :
	s_index_(s_index) {

	get_settings_info(s_index_, &settings_info_);
	current_value_ = settings_fetch_global_setting(settings_info_.index);
}

int ScreenMenuItemDisplayerSettingsValue::getCount() {
	return ((settings_info_.max - settings_info_.min) / settings_info_.step) + 2;
}

void ScreenMenuItemDisplayerSettingsValue::itemSelected(ScreenMenu *menu, int index) {
	if (index != 0) {
		settings_info_t settings_info;
		get_settings_info(s_index_, &settings_info);

		index--;
		float value = settings_info_.min + (index * settings_info_.step);
		settings_store_global_setting(settings_info.index, value);
	}

	menu->popScreenMenuItemDisplayerFactory();
}

int ScreenMenuItemDisplayerSettingsValue::getStartItem() {
	int index = 0;
	for (index=0;;index++) {
		float value = settings_info_.min + (index * settings_info_.step);
		if (value >= current_value_) {
			return index + 1;
		}
		if (value > settings_info_.max) {
			return 0;
		}
	}
	return 0;
}

void ScreenMenuItemDisplayerSettingsValue::displayItem(int index) {
	if (index == 0) {
		display_return();
	} else {
		index--;
		float value = settings_info_.min + (index * settings_info_.step);
		int iValue = trunc(value);
		Lcd7920 *lcd = getLCDLib();
		if (current_value_ == value) {
			lcd->printPgmString(PSTR("="));
		}

		switch (settings_info_.type) {
		case SETTINGS_TYPE_INT_BASE2:
			lcd->print(iValue, 10);
			lcd->printPgmString(PSTR(" ("));
			lcd->print_unsigned_int8(iValue, 2, 8);
			lcd->printPgmString(PSTR(")"));
			break;
		case SETTINGS_TYPE_INT_BASE10:
		case SETTINGS_TYPE_BOOL:
			lcd->print(iValue, 10);
			break;
		case SETTINGS_TYPE_FLOAT:
			lcd->print(value, 3);
			break;
		}
	}
}

ScreenMenuItemDisplayerFactorySettingsValue::ScreenMenuItemDisplayerFactorySettingsValue(int s_index) :
	ScreenMenuItemDisplayerFactory(getSettingName(s_index)),
	s_index_(s_index) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactorySettingsValue::createDisplayer() {
	ScreenMenuItemDisplayer *displayer = new ScreenMenuItemDisplayerSettingsValue(s_index_);
	return displayer;
}

ScreenMenuString ScreenMenuItemDisplayerFactorySettingsValue::getSettingName(int s_index) {
	settings_info_t settings_info;
	get_settings_info(s_index, &settings_info);
	return ScreenMenuString(settings_info.description, ScreenMenuStringBuffer::PSTRString);
}

int ScreenMenuItemDisplayerSettings::getCount() {
	return 1 + get_settings_info_count();
}

void ScreenMenuItemDisplayerSettings::itemSelected(ScreenMenu *menu, int index) {
	if (index == 0) {
		menu->popScreenMenuItemDisplayerFactory();
	} else {
		index--;
		menu->pushScreenMenuItemDisplayerFactory(new ScreenMenuItemDisplayerFactorySettingsValue(index));
	}
}

void ScreenMenuItemDisplayerSettings::displayItem(int index) {
	Lcd7920 *lcd = getLCDLib();

	if (index == 0) {
		display_return();
	} else {
		index--;
		settings_info_t settings_info;
		get_settings_info(index, &settings_info);
		lcd->printPgmString(settings_info.description);
		lcd->printPgmString(PSTR(" \x81"));
	}
}

void ScreenMenuOperationSettingsItem::addSettingToList(ScreenMenuItemDisplayerList &list, uint8_t setting_index) {
	uint8_t s_index = get_settings_info_index(setting_index);
	settings_info_t settings_info;
	get_settings_info(s_index, &settings_info);
	ScreenMenuString description = ScreenMenuString(settings_info.description, ScreenMenuStringBuffer::PSTRString);
	addSettingToList(list, setting_index, description);
}

void ScreenMenuOperationSettingsItem::addSettingToList(ScreenMenuItemDisplayerList &list, uint8_t setting_index, const ScreenMenuString &description) {
	list.getList().addScreenMenuItem(new ScreenMenuItem(description,
			SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuOperationSettingsItem(setting_index)));
}

void ScreenMenuOperationSettingsItem::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	uint8_t s_index = get_settings_info_index(setting_index_);
	menu->pushScreenMenuItemDisplayerFactory(new ScreenMenuItemDisplayerFactorySettingsValue(s_index));
}

ScreenMenuItemDisplayerFactorySettingsChange::ScreenMenuItemDisplayerFactorySettingsChange() :
	ScreenMenuItemDisplayerFactory(SCREEN_MENU_PSTR("Change Settings")) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactorySettingsChange::createDisplayer() {
	return new ScreenMenuItemDisplayerSettings();
}

void ScreenMenuOperationSettingsReset::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	settings_restore(SETTINGS_RESTORE_ALL);
	menu->popScreenMenuItemDisplayerFactory();
}

ScreenMenuItemDisplayerFactorySettingsReset::ScreenMenuItemDisplayerFactorySettingsReset() :
	ScreenMenuItemDisplayerFactory(SCREEN_MENU_PSTR("Reset Settings")) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactorySettingsReset::createDisplayer() {
	ScreenMenuItemDisplayerList *newMenuItems = new ScreenMenuItemDisplayerList();
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Return"), SCREEN_MENU_ITEM_UP_ARROW, new ScreenMenuItemOperationReturn()));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Reset"), 0, new ScreenMenuOperationSettingsReset()));
	return newMenuItems;
}

ScreenMenuItemDisplayerFactorySettings::ScreenMenuItemDisplayerFactorySettings() :
	ScreenMenuItemDisplayerFactory(SCREEN_MENU_PSTR("Settings")) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactorySettings::createDisplayer() {
	ScreenMenuItemDisplayerList *newMenuItems = new ScreenMenuItemDisplayerList();
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Return"), SCREEN_MENU_ITEM_UP_ARROW, new ScreenMenuItemOperationReturn()));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Change Settings"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_SETTINGS_CHANGE)));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Reset Settings"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_SETTINGS_RESET)));
	return newMenuItems;
}

ScreenMenuItemDisplayerFactoryCalibrate::ScreenMenuItemDisplayerFactoryCalibrate() :
	ScreenMenuItemDisplayerFactory(SCREEN_MENU_PSTR("Calibrate")) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactoryCalibrate::createDisplayer() {
	ScreenMenuItemDisplayerList *newMenuItems = new ScreenMenuItemDisplayerList();
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Return"), SCREEN_MENU_ITEM_UP_ARROW, new ScreenMenuItemOperationReturn()));
	if (sys.state == STATE_IDLE) {
		newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Engrave Scale"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_CALIBRATE_SCALE)));
		newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Engrave S Scale"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_CALIBRATE_SCALE_SML)));
	}
	return newMenuItems;
}

ScreenMenuItemDisplayerFactorySetup::ScreenMenuItemDisplayerFactorySetup() :
	ScreenMenuItemDisplayerFactory(SCREEN_MENU_PSTR("Setup")) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactorySetup::createDisplayer() {
	ScreenMenuItemDisplayerList *newMenuItems = new ScreenMenuItemDisplayerList();
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Return"), SCREEN_MENU_ITEM_UP_ARROW, new ScreenMenuItemOperationReturn()));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Settings"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_SETTINGS)));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Calibrate"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_CALIBRATE)));
	return newMenuItems;
}

ScreenMenuItemDisplayerJogXY::ScreenMenuItemDisplayerJogXY(uint8_t axis) : axis_(axis) {
	spindle_run(SPINDLE_ENABLE_CW, settings.jog_power);
	if (axis_ == X_AXIS) {
		pos_ = sys.position[X_AXIS];
	} else {
		pos_ = sys.position[Y_AXIS];
	}
}

int ScreenMenuItemDisplayerJogXY::getCount() {
	return 401;
}

int ScreenMenuItemDisplayerJogXY::getStartItem() {
	return 200;
}

void ScreenMenuItemDisplayerJogXY::itemSelected(ScreenMenu *menu, int index) {
	spindle_run(SPINDLE_DISABLE, 10);

	ScreenMenuOperationPositionResetOperation resetOperation;
	resetOperation.invokeOperation(menu, 0, 0);
	menu->popScreenMenuItemDisplayerFactory();
	menu->setExited(false);
}

void ScreenMenuItemDisplayerJogXY::itemHighlighted(ScreenMenu *menu, int index) {
	float value = pos_ - 200.0f + (float) index;
	char val[8];
	ftostr31(value, val);
	char buffer[80];
	sprintf(buffer, "G1%s%sF1000\r\n", (axis_==X_AXIS?"X":"Y"), val);
	protocol_parse_set_command_input(buffer);
}

void ScreenMenuItemDisplayerJogXY::displayItem(int index) {
	Lcd7920 *lcd = getLCDLib();

	float value = pos_ - 200.0f + (float) index;
	lcd->print(value, 3);
}

ScreenMenuItemDisplayerFactoryJogXY::ScreenMenuItemDisplayerFactoryJogXY(uint8_t axis) :
	ScreenMenuItemDisplayerFactory(axis==X_AXIS?SCREEN_MENU_PSTR("Jog X-Axis"):SCREEN_MENU_PSTR("Jog Y-Axis")),
	axis_(axis) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactoryJogXY::createDisplayer() {
	ScreenMenuItemDisplayerJogXY *newMenuItems = new ScreenMenuItemDisplayerJogXY(axis_);
	return newMenuItems;
}

ScreenMenuItemDisplayerFactoryJog::ScreenMenuItemDisplayerFactoryJog() :
	ScreenMenuItemDisplayerFactory(SCREEN_MENU_PSTR("Jog")) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactoryJog::createDisplayer() {
	ScreenMenuItemDisplayerList *newMenuItems = new ScreenMenuItemDisplayerList();
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Return"), SCREEN_MENU_ITEM_UP_ARROW, new ScreenMenuItemOperationReturn()));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Jog X-Axis"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_JOG_X)));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Jog Y-Axis"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_JOG_Y)));
	ScreenMenuOperationSettingsItem::addSettingToList(*newMenuItems, 54);
	return newMenuItems;
}

void ScreenMenuItemOperationExit::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	menu->setExited(true);
}

ScreenMenuOperationOpenFile::ScreenMenuOperationOpenFile(const ScreenMenuString &directory, const ScreenMenuString &file, bool showBoundsOnly) :
	directory_(directory), file_(file), showBoundsOnly_(showBoundsOnly) {
}

void ScreenMenuOperationOpenFile::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	protocol_parse_set_file_input(directory_.getString(), file_.getString(), showBoundsOnly_);
	if (!showBoundsOnly_) {
		menu->setExited(true);
	} else {
		screen_enterwait(PSTR("Bounds..."));
		while (is_protocol_parsing_file_input()) {
			protocol_parse_input();
			protocol_execute_realtime();
		}
		screen_leavewait();
	}
}

ScreenMenuItemDisplayerFactoryFile::ScreenMenuItemDisplayerFactoryFile(const ScreenMenuString &directory, const ScreenMenuString &file) :
	ScreenMenuItemDisplayerFactory(file), directory_(directory) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactoryFile::createDisplayer() {
	ScreenMenuItemDisplayerList *newMenuItems = new ScreenMenuItemDisplayerList();
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Return"), SCREEN_MENU_ITEM_UP_ARROW, new ScreenMenuItemOperationReturn()));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Run"), 0, new ScreenMenuOperationOpenFile(directory_, getName(), false)));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Jog"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_JOG)));
	if (is_protocol_parsing_engrave(getName().getString())) {
		newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Show Bounds"), 0, new ScreenMenuOperationOpenFile(directory_, getName(), true)));
		ScreenMenuOperationSettingsItem::addSettingToList(*newMenuItems, 57, SCREEN_MENU_PSTR("Low power"));
		ScreenMenuOperationSettingsItem::addSettingToList(*newMenuItems, 58, SCREEN_MENU_PSTR("High power"));
		ScreenMenuOperationSettingsItem::addSettingToList(*newMenuItems, 55, SCREEN_MENU_PSTR("Engrave rate"));
		ScreenMenuOperationSettingsItem::addSettingToList(*newMenuItems, 56, SCREEN_MENU_PSTR("Position rate"));
	}
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("All Settings"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_SETTINGS_CHANGE)));
	return newMenuItems;
}

ScreenMenuItemDisplayerDirectory::ScreenMenuItemDisplayerDirectory(const ScreenMenuString &directory, const ScreenMenuString &file) :
	directory_(directory), file_(file), sdFile_(0), cachedCount_(1), cachedIndex_(0) {

	// Open Card
	if (!card_init()) return;

	// Figure out directory to open (if any)
	if (file_.getString()[0]) {
		if (!directory_.getString()[0]) {
			sdFile_ = new SdBaseFile();
			sdFile_->open(SDroot, file_.getString(), O_READ);
		} else {
			char path[256];
			strcpy(path, directory_.getString());
			strcat(path, "/");
			strcat(path, file_.getString());
			sdFile_ = new SdBaseFile();
			sdFile_->open(SDroot, path, O_READ);
		}
	}

	// Reset file
	SdBaseFile *localFile = sdFile_;
	if (!localFile) localFile = &SDroot;
	localFile->rewind();

	// Count entries
	dir_t dir;
	bool finished = false;
	while (!finished) {
		while (1) {
			if (localFile->read(&dir, sizeof(dir)) != sizeof(dir)) { finished = true; break; }
			if (dir.name[0] == DIR_NAME_FREE) { finished = true; break; }

			// skip deleted entry and entries for . and  ..
			if (dir.name[0] != DIR_NAME_DELETED && dir.name[0] != '.'
					&& DIR_IS_FILE_OR_SUBDIR(&dir)) break;
		}
		if (!finished) cachedCount_++;
	}
}

ScreenMenuItemDisplayerDirectory::~ScreenMenuItemDisplayerDirectory() {
	delete sdFile_;
}


bool ScreenMenuItemDisplayerDirectory::getItem(int index, void *dir_res, char *nameBuffer) {
	dir_t *dir = (dir_t *) dir_res;

	SdBaseFile *file = sdFile_;
	if (!file) file = &SDroot;

	if (cachedIndex_ == 0 ||
		cachedIndex_ > index) {
		cachedIndex_ = 1;
		file->rewind();
	}

	while (true) {
		while (1) {
			if (file->read(dir, sizeof(dir_t)) != sizeof(dir_t)) return false;
			if (dir->name[0] == DIR_NAME_FREE) return false;

			// skip deleted entry and entries for . and  ..
			if (dir->name[0] != DIR_NAME_DELETED && dir->name[0] != '.'
					&& DIR_IS_FILE_OR_SUBDIR(dir)) break;
		}

		if (index == cachedIndex_) {
			int j=0;
			for (uint8_t i = 0; i < 11; i++) {
				if (dir->name[i] == ' ')continue;
				if (i == 8) {
					nameBuffer[j++] = '.';
				}
				nameBuffer[j++] = dir->name[i];
			}
			nameBuffer[j] = '\0';

			cachedIndex_++;
			return true;
		}

		cachedIndex_++;
	}
	return false;
}

void ScreenMenuItemDisplayerDirectory::displayItem(int index) {
	if (index == 0) {
		display_return();
		return;
	}

	char name[15];
	dir_t dir;
	if (getItem(index, &dir, name)) {
		Lcd7920 *lcd = getLCDLib();
		lcd->print(name);
	}
}

void ScreenMenuItemDisplayerDirectory::itemSelected(ScreenMenu *menu, int index) {
	if (index == 0) {
		menu->popScreenMenuItemDisplayerFactory();
		return;
	}

	char name[15];
	dir_t dir;
	if (getItem(index, &dir, name)) {
		ScreenMenuString fileName(name, ScreenMenuStringBuffer::CopyString);
		if (DIR_IS_SUBDIR(&dir)) {
			ScreenMenuItemDisplayerFactoryDirectory *result = 0;
			if (file_.getString()[0]) {
				if (!directory_.getString()[0]) {
					result = new ScreenMenuItemDisplayerFactoryDirectory(file_, fileName);
				} else {
					char path[256];
					strcpy(path, directory_.getString());
					strcat(path, "/");
					strcat(path, file_.getString());
					ScreenMenuString newDirectory(path, ScreenMenuStringBuffer::CopyString);
					result = new ScreenMenuItemDisplayerFactoryDirectory(newDirectory, fileName);
				}
			} else {
				// Root
				ScreenMenuString fileName(name, ScreenMenuStringBuffer::CopyString);
				result = new ScreenMenuItemDisplayerFactoryDirectory("", fileName);
			}
			menu->pushScreenMenuItemDisplayerFactory(result);
		} else {
			char path[256];
			strcpy(path, directory_.getString());
			strcat(path, "/");
			strcat(path, file_.getString());
			ScreenMenuString newDirectory(path, ScreenMenuStringBuffer::CopyString);
			menu->pushScreenMenuItemDisplayerFactory(new ScreenMenuItemDisplayerFactoryFile(newDirectory, fileName));
		}
	}
}

ScreenMenuItemDisplayerFactoryDirectory::ScreenMenuItemDisplayerFactoryDirectory(const ScreenMenuString &directory, const ScreenMenuString &file) :
	ScreenMenuItemDisplayerFactory(file.getString()[0]?ScreenMenuString(file.getString(), ScreenMenuStringBuffer::CopyString):SCREEN_MENU_PSTR("SD Root")),
	directory_(directory), file_(file) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactoryDirectory::createDisplayer() {
	return new ScreenMenuItemDisplayerDirectory(directory_, file_);
}

void ScreenMenuOperationUnlockOperation::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	menu->setExited(true);
	system_execute_unlock();
}

void ScreenMenuOperationStartOperation::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	menu->setExited(true);
	bit_true(sys_rt_exec_state, EXEC_CYCLE_START);
}


void ScreenMenuOperationPositionResetOperation::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	menu->setExited(true);
	clear_vector(sys.position);
	clear_vector(gc_state.coord_offset);
	gc_sync_position();
	plan_sync_position();
}

void ScreenMenuOperationPositionHomeOperation::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	screen_enterwait(PSTR("Homing..."));
	system_execute_home();
	screen_leavewait();
	menu->setExited(true);
}

ScreenMenuItemDisplayerFactoryPosition::ScreenMenuItemDisplayerFactoryPosition() :
	ScreenMenuItemDisplayerFactory(SCREEN_MENU_PSTR("Position")) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactoryPosition::createDisplayer() {
	ScreenMenuItemDisplayerList *newMenuItems = new ScreenMenuItemDisplayerList();
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Return"), SCREEN_MENU_ITEM_UP_ARROW, new ScreenMenuItemOperationReturn()));
	if (sys.state == STATE_IDLE) {
		newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Jog"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_JOG)));
	}
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Reset Position"), 0, new ScreenMenuOperationPositionResetOperation()));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Home Position"), 0, new ScreenMenuOperationPositionHomeOperation()));
	return newMenuItems;
}

void ScreenMenuItemOperationNewMenu::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	ScreenMenuItemDisplayerFactory *factory = 0;
	switch (menuNumber_) {
	case MENU_NUMBER_POSITION:
		factory = new ScreenMenuItemDisplayerFactoryPosition();
		break;
	case MENU_NUMBER_JOG:
		factory = new ScreenMenuItemDisplayerFactoryJog();
		break;
	case MENU_NUMBER_JOG_X:
		factory = new ScreenMenuItemDisplayerFactoryJogXY(X_AXIS);
		break;
	case MENU_NUMBER_JOG_Y:
		factory = new ScreenMenuItemDisplayerFactoryJogXY(Y_AXIS);
		break;
	case MENU_NUMBER_SETUP:
		factory = new ScreenMenuItemDisplayerFactorySetup();
		break;
	case MENU_NUMBER_SETTINGS:
		factory = new ScreenMenuItemDisplayerFactorySettings();
		break;
	case MENU_NUMBER_SETTINGS_RESET:
		factory = new ScreenMenuItemDisplayerFactorySettingsReset();
		break;
	case MENU_NUMBER_SETTINGS_CHANGE:
		factory = new ScreenMenuItemDisplayerFactorySettingsChange();
		break;
	case MENU_NUMBER_SD_CARD:
		factory = new ScreenMenuItemDisplayerFactoryDirectory("", "");
		break;
	case MENU_NUMBER_CALIBRATE:
		factory = new ScreenMenuItemDisplayerFactoryCalibrate();
		break;
	case MENU_NUMBER_CALIBRATE_SCALE:
		factory = new ScreenMenuItemDisplayerFactoryFile(SCREEN_MENU_PSTR(""), SCREEN_MENU_PSTR("__SCALE__"));
		break;
	case MENU_NUMBER_CALIBRATE_SCALE_SML:
		factory = new ScreenMenuItemDisplayerFactoryFile(SCREEN_MENU_PSTR(""), SCREEN_MENU_PSTR("__SSCALE__"));
		break;
	case MENU_NUMBER_LAST_FILE:
		factory = new ScreenMenuItemDisplayerFactoryFile(ScreenMenuString(protocol_parse_get_directory_last_used(), ScreenMenuStringBuffer::CopyString),
				ScreenMenuString(protocol_parse_get_file_last_used(), ScreenMenuStringBuffer::CopyString));
		break;
	}
	if (factory) menu->pushScreenMenuItemDisplayerFactory(factory);
}

void ScreenMenuItemOperationReturn::invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) {
	menu->popScreenMenuItemDisplayerFactory();
}

ScreenMenuItemDisplayerFactoryRoot::ScreenMenuItemDisplayerFactoryRoot() :
	ScreenMenuItemDisplayerFactory(SCREEN_MENU_PSTR("Main Menu")) {
}

ScreenMenuItemDisplayer *ScreenMenuItemDisplayerFactoryRoot::createDisplayer() {
	ScreenMenuItemDisplayerList *newMenuItems = new ScreenMenuItemDisplayerList();
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Return"), SCREEN_MENU_ITEM_UP_ARROW, new ScreenMenuItemOperationExit()));
	newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Setup"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_SETUP)));
	if (sys.state == STATE_IDLE || sys.state == STATE_ALARM) {
		newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Position"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_POSITION)));
	}
	if (sys.state == STATE_IDLE) {
		newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Load SD Card"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_SD_CARD)));
		if (protocol_parse_has_file_last_used()) {
			newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Run Last Job"), SCREEN_MENU_ITEM_RIGHT_ARROW, new ScreenMenuItemOperationNewMenu(MENU_NUMBER_LAST_FILE)));
		}
	} else if (sys.state == STATE_ALARM) {
		newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Unlock"), 0, new ScreenMenuOperationUnlockOperation()));
	} else if ((sys.state & STATE_HOLD) || (sys.state & STATE_TEMPERATURE)) {
		newMenuItems->getList().addScreenMenuItem(new ScreenMenuItem(SCREEN_MENU_PSTR("Cycle Start"), 0, new ScreenMenuOperationStartOperation()));
	}
	return newMenuItems;
}
