#ifndef SCREENScreenMenu_H_
#define SCREENScreenMenu_H_

#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#define SCREEN_MENU_PSTR(x) ScreenMenuString(PSTR(x), ScreenMenuStringBuffer::PSTRString)

class ScreenMenuStringBuffer {
public:
	enum ScreenMenuStringType {
		NoCopyString = false,
		CopyString = true,
		PSTRString = 3
	};

	ScreenMenuStringBuffer(const char *str, ScreenMenuStringType copy) : refCount_(0) {
		switch (copy) {
		case NoCopyString:
			allocated_ = false;
			str_ = (char *) str;
			break;
		case CopyString:
			allocated_ = true;
			str_ = new char[strlen(str)+1];
			strcpy(str_, str);
			break;
		case PSTRString:
			allocated_ = true;
			str_ = new char[strlen_P(str)+1];
			char *pos = str_;
			char c;
			while ((c = pgm_read_byte_near(str++))) *(pos++) = c;
			*(pos++) = c;
			break;
		}
	}

	~ScreenMenuStringBuffer() {
		if (allocated_) {
			delete [] str_;
		}
	}

	void incrementReference() {
		++refCount_;
	}

	void decrementReference() {
		if (--refCount_<=0) {
			delete this;
		}
	}

	const char *getString() { return str_; }

private:
	char *str_;
	bool allocated_;
	int refCount_;
};

class ScreenMenuString {
public:
	ScreenMenuString(const char *str = "", ScreenMenuStringBuffer::ScreenMenuStringType copy = ScreenMenuStringBuffer::NoCopyString);
	ScreenMenuString(const ScreenMenuString &other);
	~ScreenMenuString();

	const ScreenMenuString &operator=(const ScreenMenuString &other);

	const char *getString() { return buffer_->getString(); }
	const char *getString() const { return buffer_->getString(); }

private:
	ScreenMenuStringBuffer *buffer_;
};

class ScreenMenuItem;
class ScreenMenuItemList {
public:
	ScreenMenuItemList();
	~ScreenMenuItemList();

	ScreenMenuItem *getFirstItem() {
		return firstItem_;
	}

	void clearMenuItems();
	void addScreenMenuItem(ScreenMenuItem *item);
	void replaceFromOther(ScreenMenuItemList &other);
	int getCount() { return count_; }

private:
	ScreenMenuItem *firstItem_;
	int count_;
};

class ScreenMenu;
class ScreenMenuItemOperation {
public:
	virtual ~ScreenMenuItemOperation();

	virtual void invokeOperation(ScreenMenu *menu, ScreenMenuItem *item, const char *name) = 0;
};

#define SCREEN_MENU_ITEM_UP_ARROW 1
#define SCREEN_MENU_ITEM_RIGHT_ARROW 2

class ScreenMenuItem {
public:
	ScreenMenuItem(const ScreenMenuString &name, int menuItemType, ScreenMenuItemOperation *operation = 0);
	~ScreenMenuItem();

	void itemSelected(ScreenMenu *menu);

	void setNextItem(ScreenMenuItem *item);
	ScreenMenuItem *getNextItem();

	const char *getName() { return name_.getString(); }
	const char *getExtraName();

private:
	int menuItemType_;
	ScreenMenuString name_;
	ScreenMenuItem *nextItem_;
	ScreenMenuItemOperation *operation_;
};

class ScreenMenu;
class ScreenMenuItemDisplayer {
public:
	ScreenMenuItemDisplayer() {}
	virtual ~ScreenMenuItemDisplayer() {}

	virtual int getCount() = 0;
	virtual void itemSelected(ScreenMenu *menu, int index) = 0;
	virtual void itemHighlighted(ScreenMenu *menu, int index) {}
	virtual void displayItem(int index) = 0;
	virtual int getStartItem() { return -1; }
	virtual bool getSmallFonts() { return false; }
	virtual bool getFastScroll() { return false; }
};

class ScreenMenuItemDisplayerFactory {
public:
	ScreenMenuItemDisplayerFactory(const ScreenMenuString &name) :
		startItem_(-1), name_(name), next_(0), previous_(0) {}
	virtual ~ScreenMenuItemDisplayerFactory() {
		delete next_;
		next_ = 0;
		previous_ = 0;
	}

	virtual ScreenMenuItemDisplayer *createDisplayer() = 0;

	ScreenMenuString &getName() { return name_; }
	ScreenMenuItemDisplayerFactory *getPrevious() { return previous_; }

	void setNext(ScreenMenuItemDisplayerFactory *next) { next_ = next; }
	void setPrevious(ScreenMenuItemDisplayerFactory *previous) { previous_ = previous; }
	void setStartItem(int startItem) { startItem_ = startItem; }
	int getStartItem() { return startItem_; }

private:
	int startItem_;
	ScreenMenuString name_;
	ScreenMenuItemDisplayerFactory *next_, *previous_;
};

class ScreenMenuItemDisplayerList : public ScreenMenuItemDisplayer {
public:
	ScreenMenuItemDisplayerList() {}
	virtual ~ScreenMenuItemDisplayerList() {}

	virtual int getCount() { return itemList_.getCount(); }
	virtual void itemSelected(ScreenMenu *menu, int index);
	virtual void displayItem(int index);

	ScreenMenuItemList &getList() { return itemList_; }

private:
	ScreenMenuItemList itemList_;
};

class ScreenMenu {
public:
	ScreenMenu();
	~ScreenMenu();

	void drawScreenMenu();

	void pushScreenMenuItemDisplayerFactory(ScreenMenuItemDisplayerFactory *factory);
	void popScreenMenuItemDisplayerFactory();

	void cursorSelected();
	void cursorUp();
	void cursorDown();

	bool getExited();
	void setExited(bool exited);

private:
	ScreenMenuItemDisplayer *itemDisplayer_;
	ScreenMenuItemDisplayerFactory *topFactory_, *currentFactory_;
	int selectionIndex_, displayIndex_;
	bool exited_;

	int getPageSize();
	void createScreenMenuItemDisplayer();
};

#endif /* SCREENScreenMenu_H_ */
