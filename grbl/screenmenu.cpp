#include "grbl.h"
#include "screen.h"
#include "screenmenu.h"
#include "lcd7920.h"
#include "fastio.h"
#include "nuts_bolts.h"

extern Lcd7920 *getLCDLib();
extern PROGMEM LcdFont font7x8fb;    // in glcd10x10.cpp
extern PROGMEM LcdFont font7x8f;    // in glcd10x10.cpp
extern PROGMEM LcdFont font5x5f;    // in glcd10x10.cpp

ScreenMenuString::ScreenMenuString(const char *str, ScreenMenuStringBuffer::ScreenMenuStringType copy) {
	buffer_ = new ScreenMenuStringBuffer(str, copy);
	buffer_->incrementReference();
}

ScreenMenuString::ScreenMenuString(const ScreenMenuString &other) {
	buffer_ = other.buffer_;
	buffer_->incrementReference();
}

ScreenMenuString::~ScreenMenuString(){
	buffer_->decrementReference();
}

const ScreenMenuString &ScreenMenuString::operator=(const ScreenMenuString &other)
{
	if (buffer_) buffer_->decrementReference();

	buffer_ = other.buffer_;
	buffer_->incrementReference();
	return *this;
}

ScreenMenuItemList::ScreenMenuItemList() : firstItem_(0), count_(0) {
}

ScreenMenuItemList::~ScreenMenuItemList() {
	clearMenuItems();
}

void ScreenMenuItemList::clearMenuItems() {
	for (ScreenMenuItem *item = firstItem_; item; item=item->getNextItem()) {
		delete item;
	}
	firstItem_ = 0;
	count_ = 0;
}

void ScreenMenuItemList::replaceFromOther(ScreenMenuItemList &other) {
	clearMenuItems();
	firstItem_ = other.firstItem_;
	count_ = other.count_;
	other.firstItem_ = 0;
	other.count_ = 0;
}

void ScreenMenuItemList::addScreenMenuItem(ScreenMenuItem *newItem) {
	if (!firstItem_) firstItem_ = newItem;
	else {
		ScreenMenuItem *item = firstItem_;
		while (item->getNextItem()) {
			item = item->getNextItem();
		}
		item->setNextItem(newItem);
	}
	count_++;
}

ScreenMenuItemOperation::~ScreenMenuItemOperation() {

}

ScreenMenuItem::ScreenMenuItem(const ScreenMenuString &name, int menuItemType, ScreenMenuItemOperation *operation) :
	menuItemType_(menuItemType), name_(name), nextItem_(0), operation_(operation)  {
}

ScreenMenuItem::~ScreenMenuItem() {
	delete operation_;
}

void ScreenMenuItem::itemSelected(ScreenMenu *menu) {
	if (operation_) {
		operation_->invokeOperation(menu, this, getName());
	}
}

const char *ScreenMenuItem::getExtraName() {
	switch (menuItemType_) {
	default:
		return "";
	case SCREEN_MENU_ITEM_UP_ARROW:
		return " \x80";
	case SCREEN_MENU_ITEM_RIGHT_ARROW:
		return " \x81";
	}
}

void ScreenMenuItem::setNextItem(ScreenMenuItem *item) {
	nextItem_ = item;
}

ScreenMenuItem *ScreenMenuItem::getNextItem() {
	return nextItem_;
}

void ScreenMenuItemDisplayerList::itemSelected(ScreenMenu *menu, int selectionIndex) {
	int index = 0;
	for (ScreenMenuItem *item = itemList_.getFirstItem(); item; item=item->getNextItem()) {
		if (index == selectionIndex) {
			item->itemSelected(menu);
			return;
		}
		index++;
	}
}

void ScreenMenuItemDisplayerList::displayItem(int selectionIndex) {
	Lcd7920 *lcd = getLCDLib();
	int index = 0;
	for (ScreenMenuItem *item = itemList_.getFirstItem(); item; item=item->getNextItem()) {
		if (index == selectionIndex) {
			lcd->print(item->getName());
			lcd->print(item->getExtraName());
			return;
		}
		index++;
	}
}

ScreenMenu::ScreenMenu() : selectionIndex_(0), displayIndex_(0), exited_(false),
		topFactory_(0), currentFactory_(0), itemDisplayer_(0) {

}

ScreenMenu::~ScreenMenu() {
	delete topFactory_;
	delete itemDisplayer_;
}

void ScreenMenu::pushScreenMenuItemDisplayerFactory(ScreenMenuItemDisplayerFactory *factory) {
	if (!topFactory_) topFactory_ = factory;
	factory->setPrevious(currentFactory_);
	if (currentFactory_) currentFactory_->setNext(factory);
	currentFactory_ = factory;

	createScreenMenuItemDisplayer();
}

void ScreenMenu::popScreenMenuItemDisplayerFactory() {
	ScreenMenuItemDisplayerFactory *factory = currentFactory_;
	currentFactory_ = currentFactory_->getPrevious();
	currentFactory_->setNext(0);
	delete factory;
	if (!currentFactory_) topFactory_ = 0;

	createScreenMenuItemDisplayer();
}

void ScreenMenu::createScreenMenuItemDisplayer() {
	delete itemDisplayer_;
	itemDisplayer_ = 0;
	if (currentFactory_) {
		itemDisplayer_ = currentFactory_->createDisplayer();
	}
	if (itemDisplayer_) {
		int startItem = itemDisplayer_->getStartItem();
		if (startItem == -1) startItem = currentFactory_->getStartItem();
		if (startItem == -1) startItem = 0;

		selectionIndex_ = startItem;
		int pageSize = getPageSize();
		displayIndex_ = selectionIndex_ - (pageSize / 2);
		if (displayIndex_ < 0) displayIndex_ = 0;
		exited_ = false;

		drawScreenMenu();
	} else {
		exited_ = true;
	}
}

void ScreenMenu::cursorSelected() {
	currentFactory_->setStartItem(selectionIndex_);
	itemDisplayer_->itemSelected(this, selectionIndex_);
}

bool ScreenMenu::getExited() {
	return exited_;
}

void ScreenMenu::setExited(bool exited) {
	exited_ = exited;
}

int ScreenMenu::getPageSize() {
	int pageSize = 5;
	if (itemDisplayer_->getSmallFonts()) {
		pageSize = 8;
	}
	return pageSize;
}

void ScreenMenu::cursorDown() {
	if (selectionIndex_ == 0) return;

	int pageSize = getPageSize();
	if (selectionIndex_ == displayIndex_ &&
		itemDisplayer_->getFastScroll()) {
		selectionIndex_ -= pageSize;
	} else {
		selectionIndex_--;
	}

	if (selectionIndex_ <= 0) {
		selectionIndex_ = 0;
	}
	itemDisplayer_->itemHighlighted(this, selectionIndex_);

	if (selectionIndex_ < displayIndex_) {
		displayIndex_ = selectionIndex_;
	}
	drawScreenMenu();
}

void ScreenMenu::cursorUp() {
	if (selectionIndex_ >= itemDisplayer_->getCount() - 1) return;

	int pageSize = getPageSize();
	if (selectionIndex_ == displayIndex_ + (pageSize - 1) &&
		itemDisplayer_->getFastScroll()) {
		selectionIndex_ += pageSize;
	} else {
		selectionIndex_++;
	}

	if (selectionIndex_ >= itemDisplayer_->getCount() - 1) {
		selectionIndex_ = itemDisplayer_->getCount() - 1;
	}
	itemDisplayer_->itemHighlighted(this, selectionIndex_);

	if (selectionIndex_ > displayIndex_ + (pageSize - 1)) {
		displayIndex_ = selectionIndex_ - (pageSize - 1);
	}
	drawScreenMenu();
}

void ScreenMenu::drawScreenMenu()
{
	Lcd7920 *lcd = getLCDLib();
	lcd->clear();

	// Draw scroll bar arrows
	if (displayIndex_ > 0) {
		for (int x=119; x<=127; x++) {
			lcd->line(123, 13, x, 19, PixelSet);
		}
	}
	if (displayIndex_ < itemDisplayer_->getCount() - 5) {
		for (int x=119; x<=127; x++) {
			lcd->line(123, 59, x, 53, PixelSet);
		}
	}

	// Draw title
	lcd->setFont(&font7x8fb);
	lcd->setCursor(2, 0);
	lcd->print(currentFactory_->getName().getString());
	lcd->line(0, 9, 128, 9, PixelSet);

	// Draw items
	if (itemDisplayer_->getSmallFonts()) {
		lcd->setFont(&font5x5f);

		int index = 0;
		int pos = 12;
		for (index=0; index<itemDisplayer_->getCount(); index++) {
			if (index >= displayIndex_) {
				lcd->textInvert(index == selectionIndex_);
				if (index == selectionIndex_) {
					for (int y=pos-1; y<pos+6; y++) {
						lcd->line(4, y, 116, y, PixelSet);
					}
				}
				lcd->setCursor(6, pos);
				itemDisplayer_->displayItem(index);

				pos += 6;
				if (pos > 58) break;
			}
		}
	} else {
		lcd->setFont(&font7x8f);

		int index = 0;
		int pos = 12;
		for (index=0; index<itemDisplayer_->getCount(); index++) {
			if (index >= displayIndex_) {
				lcd->textInvert(index == selectionIndex_);
				if (index == selectionIndex_) {
					for (int y=pos-1; y<pos+9; y++) {
						lcd->line(8, y, 116, y, PixelSet);
					}
					lcd->setFont(&font7x8fb);
				}
				lcd->setCursor(10, pos);
				itemDisplayer_->displayItem(index);

				if (index == selectionIndex_) {
					lcd->setFont(&font7x8f);
				}

				pos += 10;
				if (pos > 55) break;
			}
		}
	}
	lcd->flush();
}
