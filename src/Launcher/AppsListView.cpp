/* AppsListView.cpp
 * Copyright 2012 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#include "AppsListView.h"

AppsListView::AppsListView(BRect size)
	:
	BListView(size, "Launcher List", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES),
	fMenu(NULL),
	isShowing(false)
{
/*	fMenu = new LPopUpMenu(B_EMPTY_STRING);
	fStartStopMI = new BMenuItem("Start", new BMessage());
	fMenu->AddItem(fStartStopMI);
	fTrackerMI = new BMenuItem("Show in Tracker", new BMessage(SHOW_IN_TRACKER));
	fMenu->AddItem(fTrackerMI);
	fRemoveMI = new BMenuItem("Remove from Launcher", new BMessage(ADD_APP_EXCLUSION));
	fMenu->AddItem(fRemoveMI);
	fMenu->AddSeparatorItem();
	fSettingsMI = new BMenuItem("Settings" B_UTF8_ELLIPSIS, new BMessage(SHOW_SETTINGS));
	fMenu->AddItem(fSettingsMI);*/
//	_InitPopUpMenu(-1);

}

/*
void
AppsListView::AttachedToWindow()
{
	BListView::AttachedToWindow();

	fMenu->SetTargetForItems(this);
	fSettingsMI->SetTarget(be_app);
}*/


void
AppsListView::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case EL_START_SERVICE:
		case EL_STOP_SERVICE:
		{
			int32 selectedIndex = CurrentSelection();
			if(selectedIndex < 0)
				return;
			uint32 modifier = modifiers();
			AppListItem *selectedItem = (AppListItem*)ItemAt(selectedIndex);
			selectedItem->ProcessMessage(msg);
			_HideApp(modifier);
			break;
		}
		case B_MOUSE_WHEEL_CHANGED:
		{
		//	printf("Mouse wheel\n");
			// Prevent scrolling while the menu is showing
			if(fMenu && fMenu->IsShowing())
				break;
			// If this list view isn't currently selected redirect message
			// to the main view to handle (fixes bug in Haiku R1A3)
			if(!isShowing)
			{
				msg->what = EL_REDIRECTED_MOUSE_WHEEL_CHANGED;
				Parent()->MessageReceived(msg);
				break;
			}
			HandleMouseWheelChanged(msg);
			break;
		}
		case EL_SHOW_IN_TRACKER:
		{
			int32 selectedIndex = CurrentSelection();
			if(selectedIndex < 0)
				break;
			uint32 modifier = modifiers();
			AppListItem *selectedItem = (AppListItem*)ItemAt(selectedIndex);
			selectedItem->ShowInTracker();
			_HideApp(modifier);
			break;
		}
		default:
			BListView::MessageReceived(msg);
	}
}

/*
void
AppsListView::MouseDown(BPoint pos)
{
	int32 button=0;
	Looper()->CurrentMessage()->FindInt32("buttons", &button);
	if ( button & B_PRIMARY_MOUSE_BUTTON )
	{
		// Select list item under mouse pointer, then launch item
		int32 index = IndexOf(pos);
		if(index>=0)
		{
			Select(index);
			_InvokeSelectedItem();
		}
	}
	else if ( button & B_TERTIARY_MOUSE_BUTTON )
	{
		// Launch the currently selected item
		_InvokeSelectedItem();
	}
	else if (button & B_SECONDARY_MOUSE_BUTTON)
	{
		// Select list item under mouse pointer and show menu
		int32 index = IndexOf(pos);
		if(index < 0)
			return;
		Select(index);
		int32 selectedIndex = CurrentSelection();
		if(selectedIndex < 0)
			return;

		_InitPopUpMenu(selectedIndex);

		ConvertToScreen(&pos);
		//fMenu->Go(pos, false, false, true);
		fMenu->Go(pos, true, true, BRect(pos.x - 2, pos.y - 2,
			pos.x + 2, pos.y + 2), true);
	}
}
*/


void
AppsListView::MouseDown(BPoint pos)
{
	int32 button=0;
	Looper()->CurrentMessage()->FindInt32("buttons", &button);
	if ( button & B_PRIMARY_MOUSE_BUTTON )
	{
		// Select list item under mouse pointer, then launch item
		int32 index = IndexOf(pos);
		if(index>=0)
		{
			Select(index);
			_InvokeSelectedItem();
		}
	}
	else if ( button & B_TERTIARY_MOUSE_BUTTON )
	{
		// Launch the currently selected item
		_InvokeSelectedItem();
	}
	else if (button & B_SECONDARY_MOUSE_BUTTON)
	{
		// Select list item under mouse pointer and show menu
		int32 index = IndexOf(pos);
		if(index < 0)
			return;
		Select(index);
		int32 selectedIndex = CurrentSelection();
		if(selectedIndex < 0)
			return;
	/*	AppListItem *selectedItem = (AppListItem*)ItemAt(selectedIndex);
		bool isRunning = selectedItem->IsRunning();
		if(isRunning)
		{
			fStartStopMI->SetLabel("Quit");
			fStartStopMI->Message()->what = STOP_SERVICE;
		}
		else
		{
			fStartStopMI->SetLabel("Launch");
			fStartStopMI->Message()->what = START_SERVICE;
		}
		BMessage *message = fRemoveMI->Message();
		message->MakeEmpty();
		message->AddString(EL_EXCLUDE_SIGNATURE, selectedItem->GetSignature());
		message->AddString(EL_EXCLUDE_NAME, selectedItem->GetName());
	*/
		_InitPopUpMenu(selectedIndex);

		ConvertToScreen(&pos);
		//fMenu->Go(pos, false, false, true);
		fMenu->Go(pos, true, true, BRect(pos.x - 2, pos.y - 2,
			pos.x + 2, pos.y + 2), true);
	}
}


void
AppsListView::KeyDown(const char* bytes, int32 numbytes)
{
	if(numbytes == 1) {
		switch(bytes[0]) {
			case B_RETURN:
			case B_SPACE:
			{
				_InvokeSelectedItem();
				break;
			}
			case B_DOWN_ARROW:
			{
				int32 currentSelection = CurrentSelection();
				if(currentSelection==(CountItems()-1))
				{
					Select(0);
					ScrollToSelection();
				}
				else
					BListView::KeyDown(bytes, numbytes);
				break;
			}
			case B_UP_ARROW:
			{
				int32 currentSelection = CurrentSelection();
				if(currentSelection==0)
				{
					Select(CountItems()-1);
					ScrollToSelection();
				}
				else
					BListView::KeyDown(bytes, numbytes);
				break;
			}
			default:
				BListView::KeyDown(bytes, numbytes);
		}
	}
	else
		BListView::KeyDown(bytes, numbytes);
}



void
AppsListView::HandleMouseWheelChanged(BMessage *msg)
{
	if(msg->what!=B_MOUSE_WHEEL_CHANGED
		&& msg->what!=EL_REDIRECTED_MOUSE_WHEEL_CHANGED)
		return;

	float deltaY=0;
//	printf("Mouse wheel changed\n");
	status_t result = msg->FindFloat("be:wheel_delta_y", &deltaY);
	if(result!=B_OK)
		return;
	if(deltaY>0)
	{
		int32 currentSelection = CurrentSelection();
		if(currentSelection==(CountItems()-1))
			Select(0);
		else
			Select(currentSelection+1);
		ScrollToSelection();
	}
	else
	{
		int32 currentSelection = CurrentSelection();
		if(currentSelection==0)
			Select(CountItems()-1);
		else
			Select(currentSelection-1);
		ScrollToSelection();
	}
}



void
AppsListView::SettingsChanged(uint32 what, AppSettings settings)
{
	Window()->Lock();
	int32 currentSelection = CurrentSelection();
	switch(what)
	{
		case EL_APP_ICON_OPTION_CHANGED:
		{
			int count = CountItems();
			for(int i=0; i<count; i++)
			{
				AppListItem* item = (AppListItem*)(ItemAt(0));
				item->SetIconSize(settings.minIconSize, settings.maxIconSize, count, i);
				RemoveItem(item);
				AddItem(item);
			}
			Select(currentSelection);
			ScrollToSelection();
			break;
		}
		case EL_FONT_OPTION_CHANGED:
		{
			SetFontSizeForValue(settings.fontSize);
			int count = CountItems();
			for(int i=0; i<count; i++)
			{
				AppListItem* item = (AppListItem*)(ItemAt(0));
				RemoveItem(item);
				AddItem(item);
			}
			Select(currentSelection);
			ScrollToSelection();
			break;
		}
	}
	Window()->Unlock();
}


void
AppsListView::SetFontSizeForValue(float fontSize)
{
	if(fontSize==0)
		fontSize = be_plain_font->Size();
	BListView::SetFontSize(fontSize);
}

status_t
AppsListView::_InvokeSelectedItem()
{
	status_t result = B_OK;
	int32 selectedIndex = CurrentSelection();
	if(selectedIndex >= 0)
	{
		uint32 modifier = modifiers();
		if(modifier & kRemoveAppModifier)
		{
			AppListItem *selectedItem = (AppListItem*)ItemAt(selectedIndex);
			BMessage message(EL_ADD_APP_EXCLUSION);
			message.AddString(EL_EXCLUDE_SIGNATURE, selectedItem->GetSignature());
			message.AddString(EL_EXCLUDE_NAME, selectedItem->GetName());
			Window()->PostMessage(&message);
		}
		else
		{
			AppListItem *selectedItem = (AppListItem*)ItemAt(selectedIndex);
			result = selectedItem->Launch();
			_HideApp(modifier);
		}
	}
	return result;
}


void
AppsListView::_InitPopUpMenu(int32 selectedIndex)
{
	if(fMenu==NULL)
	{
		fMenu = new LPopUpMenu(B_EMPTY_STRING);
		fStartStopMI = new BMenuItem("Start", new BMessage());
		fMenu->AddItem(fStartStopMI);
		fTrackerMI = new BMenuItem("Show in Tracker", new BMessage(EL_SHOW_IN_TRACKER));
		fMenu->AddItem(fTrackerMI);
		fRemoveMI = new BMenuItem("Remove from Launcher", new BMessage(EL_ADD_APP_EXCLUSION));
		fMenu->AddItem(fRemoveMI);
		fMenu->AddSeparatorItem();
		fSettingsMI = new BMenuItem("Settings" B_UTF8_ELLIPSIS, new BMessage(EL_SHOW_SETTINGS));
		fMenu->AddItem(fSettingsMI);
	//	fMenu->SetTargetForItems(this);
		fStartStopMI->SetTarget(this);
		fTrackerMI->SetTarget(this);
		fRemoveMI->SetTarget(be_app);
		fSettingsMI->SetTarget(be_app);
	}
	if(selectedIndex>=0)
	{
		AppListItem *selectedItem = (AppListItem*)ItemAt(selectedIndex);
		bool isRunning = selectedItem->IsRunning();
		if(isRunning)
		{
			fStartStopMI->SetLabel("Quit");
			fStartStopMI->Message()->what = EL_STOP_SERVICE;
		}
		else
		{
			fStartStopMI->SetLabel("Launch");
			fStartStopMI->Message()->what = EL_START_SERVICE;
		}
		BMessage *message = fRemoveMI->Message();
		message->MakeEmpty();
		message->AddString(EL_EXCLUDE_SIGNATURE, selectedItem->GetSignature());
		message->AddString(EL_EXCLUDE_NAME, selectedItem->GetName());
	}
}



void
AppsListView::_HideApp(uint32 modifier=0)
{
	if(!(modifier & kPreventHideModifier) )
		be_app->PostMessage(EL_HIDE_APP);
}