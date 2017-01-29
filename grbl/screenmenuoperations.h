#ifndef SCREENMENUOPERATIONS_H_
#define SCREENMENUOPERATIONS_H_

#include "screenmenu.h"
#include "settings.h"

class ScreenMenuItemDisplayerSettingsValue : public ScreenMenuItemDisplayer {
public:
	ScreenMenuItemDisplayerSettingsValue(int s_index);
	virtual ~ScreenMenuItemDisplayerSettingsValue() {}

	virtual int getCount();
	virtual void itemSelected(ScreenMenu *menu, int index);
	virtual void displayItem(int index);
	virtual int getStartItem();
	virtual bool getSmallFonts() { return true; }
	virtual bool getFastScroll() { return true; }

private:
	int s_index_;
	float current_value_;
	settings_info_t settings_info_;
};

class ScreenMenuItemDisplayerFactorySettingsValue : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactorySettingsValue(int s_index);
	virtual ~ScreenMenuItemDisplayerFactorySettingsValue() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
private:
	int s_index_;

	static ScreenMenuString getSettingName(int s_index);
};

class ScreenMenuItemDisplayerSettings : public ScreenMenuItemDisplayer {
public:
	ScreenMenuItemDisplayerSettings() {}
	virtual ~ScreenMenuItemDisplayerSettings() {}

	virtual int getCount();
	virtual void itemSelected(ScreenMenu *menu, int index);
	virtual void displayItem(int index);
	virtual bool getSmallFonts() { return true; }
};

class ScreenMenuOperationSettingsItem : public ScreenMenuItemOperation {
public:
	ScreenMenuOperationSettingsItem(uint8_t setting_index) : setting_index_(setting_index) {}
	virtual ~ScreenMenuOperationSettingsItem() { }
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);

	static void addSettingToList(ScreenMenuItemDisplayerList &list, uint8_t setting_index);
	static void addSettingToList(ScreenMenuItemDisplayerList &list, uint8_t setting_index, const ScreenMenuString &description);

private:
	uint8_t setting_index_;
};

class ScreenMenuItemDisplayerFactorySettingsChange : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactorySettingsChange();
	virtual ~ScreenMenuItemDisplayerFactorySettingsChange() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
};

class ScreenMenuOperationSettingsReset : public ScreenMenuItemOperation {
public:
	ScreenMenuOperationSettingsReset() {}
	virtual ~ScreenMenuOperationSettingsReset() {}
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);
};

class ScreenMenuItemDisplayerFactorySettingsReset : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactorySettingsReset();
	virtual ~ScreenMenuItemDisplayerFactorySettingsReset() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
};

class ScreenMenuItemDisplayerFactorySettings : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactorySettings();
	virtual ~ScreenMenuItemDisplayerFactorySettings() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
};

class ScreenMenuItemDisplayerFactoryCalibrate : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactoryCalibrate();
	virtual ~ScreenMenuItemDisplayerFactoryCalibrate() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
};

class ScreenMenuItemDisplayerFactorySetup : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactorySetup();
	virtual ~ScreenMenuItemDisplayerFactorySetup() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
};

class ScreenMenuItemDisplayerJogXY : public ScreenMenuItemDisplayer {
public:
	ScreenMenuItemDisplayerJogXY(uint8_t axis);
	virtual ~ScreenMenuItemDisplayerJogXY() {}

	virtual int getCount();
	virtual void itemSelected(ScreenMenu *menu, int index);
	virtual void itemHighlighted(ScreenMenu *menu, int index);
	virtual void displayItem(int index);
	virtual int getStartItem();
	virtual bool getFastScroll() { return true; }

private:
	uint8_t axis_;
	float pos_;
};

class ScreenMenuItemDisplayerFactoryJogXY : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactoryJogXY(uint8_t axis);
	virtual ~ScreenMenuItemDisplayerFactoryJogXY() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
private:
	uint8_t axis_;
};

class ScreenMenuItemDisplayerFactoryJog : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactoryJog();
	virtual ~ScreenMenuItemDisplayerFactoryJog() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
};

class ScreenMenuItemOperationExit : public ScreenMenuItemOperation {
public:
	virtual ~ScreenMenuItemOperationExit() {}
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);
};

class ScreenMenuOperationOpenFile : public ScreenMenuItemOperation {
public:
	ScreenMenuOperationOpenFile(const ScreenMenuString &directory, const ScreenMenuString &file, bool showBoundsOnly);
	virtual ~ScreenMenuOperationOpenFile() {}
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);
private:
	ScreenMenuString directory_, file_;
	bool showBoundsOnly_;
};

class ScreenMenuItemDisplayerFactoryFile : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactoryFile(const ScreenMenuString &directory, const ScreenMenuString &file);
	virtual ~ScreenMenuItemDisplayerFactoryFile() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();

private:
	ScreenMenuString directory_;
};

class SdBaseFile;
class ScreenMenuItemDisplayerDirectory : public ScreenMenuItemDisplayer {
public:
	ScreenMenuItemDisplayerDirectory(const ScreenMenuString &directory, const ScreenMenuString &file);
	virtual ~ScreenMenuItemDisplayerDirectory();

	virtual int getCount() { return cachedCount_; }
	virtual void itemSelected(ScreenMenu *menu, int index);
	virtual void displayItem(int index);
	virtual bool getSmallFonts() { return false; }

private:
	int cachedCount_, cachedIndex_;
	ScreenMenuString directory_, file_;
	SdBaseFile *sdFile_;

	bool getItem(int index, void *dir, char *nameBuffer);
};

class ScreenMenuItemDisplayerFactoryDirectory : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactoryDirectory(const ScreenMenuString &directory, const ScreenMenuString &file);
	virtual ~ScreenMenuItemDisplayerFactoryDirectory() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();

private:
	ScreenMenuString directory_, file_;
};

class ScreenMenuOperationUnlockOperation : public ScreenMenuItemOperation {
public:
	virtual ~ScreenMenuOperationUnlockOperation() {}
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);
};

class ScreenMenuOperationStartOperation : public ScreenMenuItemOperation {
public:
	virtual ~ScreenMenuOperationStartOperation() {}
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);
};

class ScreenMenuOperationPositionResetOperation : public ScreenMenuItemOperation {
public:
	virtual ~ScreenMenuOperationPositionResetOperation() {}
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);
};

class ScreenMenuOperationPositionHomeOperation : public ScreenMenuItemOperation {
public:
	virtual ~ScreenMenuOperationPositionHomeOperation() {}
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);
};

class ScreenMenuItemDisplayerFactoryPosition : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactoryPosition();
	virtual ~ScreenMenuItemDisplayerFactoryPosition() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
};

#define MENU_NUMBER_POSITION 0
#define MENU_NUMBER_JOG 1
#define MENU_NUMBER_JOG_X 2
#define MENU_NUMBER_JOG_Y 3
#define MENU_NUMBER_SETTINGS 4
#define MENU_NUMBER_SETTINGS_RESET 5
#define MENU_NUMBER_SETTINGS_CHANGE 6
#define MENU_NUMBER_SD_CARD 7
#define MENU_NUMBER_SETUP 8
#define MENU_NUMBER_CALIBRATE 9
#define MENU_NUMBER_CALIBRATE_SCALE 10
#define MENU_NUMBER_CALIBRATE_SCALE_SML 11
#define MENU_NUMBER_LAST_FILE 12

class ScreenMenuItemOperationNewMenu : public ScreenMenuItemOperation {
public:
	ScreenMenuItemOperationNewMenu(uint8_t menuNumber) : menuNumber_(menuNumber) { }
	virtual ~ScreenMenuItemOperationNewMenu() {}
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);

private:
	uint8_t menuNumber_;
};

class ScreenMenuItemOperationReturn : public ScreenMenuItemOperation {
public:
	virtual ~ScreenMenuItemOperationReturn() {}
	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name);
};

class ScreenMenuItemDisplayerFactoryRoot : public ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactoryRoot();
	virtual ~ScreenMenuItemDisplayerFactoryRoot() {}

	virtual ScreenMenuItemDisplayer *createDisplayer();
};

#endif /* SCREENMENUOPERATIONS_H_ */
