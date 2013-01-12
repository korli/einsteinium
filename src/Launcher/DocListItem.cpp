/* DocListItem.cpp
 * Copyright 2012 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#include "DocListItem.h"


DocListItem::DocListItem(entry_ref *entry, AppSettings *settings, int level=0)
	:
	BListItem(level),
//	fDrawTwoLines(false),
	fInitStatus(B_ERROR)
{
	// Draw Settings
//	fDrawTwoLines = settings->drawTwoLines;
	fIconSize = settings->docIconSize;
	fIcon = NULL;
	fShadowIcon = NULL;

/*	if(!entry.Exists())
		return;

	BPath path;
	entry.GetPath(&path);
	fName.SetTo(path.Leaf());
//	fSignature.SetTo(sig);
	entry.GetRef(&fEntryRef);*/
	fEntryRef = *entry;
	fName.SetTo(entry->name);

	// Get super type
	BNode node;
	if (node.SetTo(entry) == B_OK)
	{
		BNodeInfo nodeInfo(&node);
		char mimeString[B_MIME_TYPE_LENGTH];
		if(nodeInfo.GetType(mimeString) == B_OK)
		{
			fMimeType.SetTo(mimeString);
			// TODO: recurse to be sure we have the final super type?
			fMimeType.GetSupertype(&fSuperMimeType);
		}
	}

	_GetIcon();
	fInitStatus = B_OK;
}


DocListItem::~DocListItem()
{
	delete fIcon;
	delete fShadowIcon;
}


void
DocListItem::ProcessMessage(BMessage* msg)
{
	switch(msg->what)
	{
		case EL_START_SERVICE:
		{
			status_t rc = _OpenDoc();
			break;
		}
	/*	case EL_STOP_SERVICE:
		{
			status_t rc = _StopService();
			break;
		}*/
	}
}


/*
void
DocListItem::SetDrawTwoLines(bool value)
{
	fDrawTwoLines = value;
}*/


void
DocListItem::SetIconSize(int value)
{
	fIconSize = value;
	_GetIcon();
}

/*
void
DocListItem::SetIconSize(int minIconSize, int maxIconSize, int totalCount, int index)
{
	int iconSize = int( maxIconSize - (index*(maxIconSize-minIconSize)/(totalCount-1)) );
	SetIconSize(iconSize);
}*/



const char*
DocListItem::GetSuperTypeName()
{
	if(fSuperMimeType.IsValid())
		return fSuperMimeType.Type();
	else
		return EL_UNKNOWN_SUPERTYPE;
}

const char*
DocListItem::GetTypeName()
{
	return fMimeType.Type();
}


status_t
DocListItem::Launch()
{
	return _OpenDoc();
}


status_t
DocListItem::ShowInTracker()
{
	BEntry folderEntry;
	BEntry fileEntry(&fEntryRef);
	fileEntry.GetParent(&folderEntry);
	entry_ref folderRef;
	folderEntry.GetRef(&folderRef);
	BMessenger tracker("application/x-vnd.Be-TRAK");
	BMessage message(B_REFS_RECEIVED);
	message.AddRef("refs", &folderRef);
	status_t rc = tracker.SendMessage(&message);
	return rc;
}


BString
DocListItem::GetPath()
{
	BPath docPath(&fEntryRef);
	BString pathStr(docPath.Path());
	return pathStr;
}


void
DocListItem::_GetIcon()
{
	delete fIcon;
	delete fShadowIcon;
	fIcon = NULL;
	fShadowIcon = NULL;
	if(fIconSize == 0)
		return;

	BNode node;
	status_t result = B_ERROR;
	if (node.SetTo(&fEntryRef) == B_OK) {
		BRect iconRect(0, 0, fIconSize - 1, fIconSize - 1);
		fIcon = new BBitmap(iconRect, 0, B_RGBA32);
		BNodeInfo nodeInfo(&node);
		result = nodeInfo.GetTrackerIcon(fIcon, icon_size(fIconSize));
		if(result!=B_OK)
		{
			// Get icon from mime type
			char mimeString[B_MIME_TYPE_LENGTH];
			uint8* data;
			size_t size;
			BMimeType nodeType;
			if(nodeInfo.GetType(mimeString) == B_OK)
			{
				nodeType.SetTo(mimeString);
				nodeType.GetIcon(&data, &size);
				result = BIconUtils::GetVectorIcon(data, size, fIcon);
			}
			// Get super type icon
			if(result!=B_OK)
			{
				BMimeType superType;
				if(nodeType.GetSupertype(&superType)==B_OK)
				{
					superType.GetIcon(&data, &size);
					result = BIconUtils::GetVectorIcon(data, size, fIcon);
				}

			}
		}

	/*	if(result==B_OK)
		{
			iconRect.Set(0, 0, fIconSize + 2*kIconMargin - 1, fIconSize + 2*kIconMargin - 1);
			BBitmap tempIcon = new BBitmap(iconRect, 0, B_RGBA32);
			result = BIconUtils::GetVectorIcon(&node, "BEOS:ICON", &tempIcon);
			if(result==B_OK)
				fShadowIcon = _ConvertToGrayscale(&tempIcon);

		}*/
	}
	if(result!=B_OK)
	{
		delete fIcon;
		fIcon = NULL;
	}
}

/*
BBitmap*
AppListItem::_ConvertToGrayscale(const BBitmap* bitmap) const
{
    BBitmap* convertedBitmap = new BBitmap(bitmap->Bounds(),
            B_BITMAP_ACCEPTS_VIEWS, B_GRAY8);

    if (convertedBitmap && convertedBitmap->IsValid()) {
        memset(convertedBitmap->Bits(), 0, convertedBitmap->BitsLength());
        BView* helper = new BView(bitmap->Bounds(), "helper", B_FOLLOW_NONE, B_WILL_DRAW);
        if (convertedBitmap->Lock()) {
            convertedBitmap->AddChild(helper);
            helper->SetDrawingMode(B_OP_OVER);
            helper->DrawBitmap(bitmap, BPoint(0.0, 0.0));
            helper->Sync();
            convertedBitmap->Unlock();
        }
    } else {
        delete convertedBitmap;
        convertedBitmap = NULL;
    }

    return convertedBitmap;
}*/

status_t
DocListItem::_OpenDoc()
{
	status_t rc = be_roster->Launch(&fEntryRef);
	return rc;
}


void
DocListItem::DrawItem(BView *owner, BRect item_rect, bool complete = false)
{
	float offset_width = 0, offset_height = fFontAscent;
	float listItemHeight = Height();
	rgb_color backgroundColor;

	//background
	if(IsSelected()) {
		backgroundColor = ui_color(B_MENU_SELECTED_BACKGROUND_COLOR);
	}
	else {
		backgroundColor = ui_color(B_DOCUMENT_BACKGROUND_COLOR);
	}
	owner->SetHighColor(backgroundColor);
	owner->SetLowColor(backgroundColor);
	owner->FillRect(item_rect);

	//icon
/*	if (IsSelected() && fShadowIcon) {
		float offsetMarginHeight = floor( (listItemHeight - fShadowIcon->Bounds().Height())/2);
		float offsetMarginWidth = floor( (fIconSize - fShadowIcon->Bounds().Width())/2 ) + kIconMargin;
		owner->SetDrawingMode(B_OP_OVER);
		owner->DrawBitmap(fShadowIcon, BPoint(item_rect.left + offsetMarginWidth,
							item_rect.top + offsetMarginHeight));
		owner->SetDrawingMode(B_OP_COPY);
//		offset_width += fIconSize + 2*kIconMargin;
	}*/

	if (fIcon) {
		float offsetMarginHeight = floor( (listItemHeight - fIconSize)/2);
		float offsetMarginWidth = kIconMargin;

		//owner->SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
		owner->SetDrawingMode(B_OP_ALPHA);
		if(fIcon->Bounds().IntegerWidth()+1 != fIconSize)
		{
//			printf("Bounds width=%i, iconSize=%i\n", fIcon->Bounds().IntegerWidth()+1,fIconSize);
			BRect destRect(item_rect.left + offsetMarginWidth,
							item_rect.top + offsetMarginHeight,
							item_rect.left + offsetMarginWidth + fIconSize,
							item_rect.top + offsetMarginHeight + fIconSize);
			owner->DrawBitmap(fIcon, destRect);
		}
		else
		{
			owner->DrawBitmap(fIcon, BPoint(item_rect.left + offsetMarginWidth,
							item_rect.top + offsetMarginHeight));
		}
		owner->SetDrawingMode(B_OP_COPY);
		offset_width += fIconSize + 2*kIconMargin;
	}
	// no icon, give the text some margin space
	else
	{
		offset_width += kTextMargin;
	}

	//text
	if(listItemHeight > fTextOnlyHeight)
		offset_height += floor( (listItemHeight - fTextOnlyHeight)/2 );
			// center the text vertically
	BPoint cursor(item_rect.left + offset_width, item_rect.top + offset_height + kTextMargin);
	owner->SetHighColor(ui_color(B_CONTROL_TEXT_COLOR));
	owner->MovePenTo(cursor.x, cursor.y);
	owner->DrawString(fName);
}



void
DocListItem::Update(BView *owner, const BFont *font)
{
	BListItem::Update(owner, font);
	_UpdateHeight(font);
}


void
DocListItem::_UpdateHeight(const BFont *font)
{
	font_height fontHeight;
	font->GetHeight(&fontHeight);
	fFontHeight = fontHeight.ascent + fontHeight.descent + fontHeight.leading;
	fFontAscent = fontHeight.ascent;
//	if(fDrawTwoLines)
//		fTextOnlyHeight = 2*fFontHeight + 3*kTextMargin;
//	else
		fTextOnlyHeight = fFontHeight + 2*kTextMargin;

	if(fIcon) {
		float iconSpacing = fIconSize + 2*kIconMargin;
		if(iconSpacing > fTextOnlyHeight)
			SetHeight(iconSpacing);
		else
			SetHeight(fTextOnlyHeight);
	}
	else
		SetHeight(fTextOnlyHeight);
}
