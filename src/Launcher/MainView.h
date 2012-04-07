/* MainView.h
 * Copyright 2012 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#ifndef EINSTEINIUM_LAUNCHER_MAIN_VIEW_H
#define EINSTEINIUM_LAUNCHER_MAIN_VIEW_H

#include <InterfaceKit.h>
#include <SupportKit.h>
#include <StorageKit.h>
#include <fs_attr.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <Roster.h>
#include "launcher_constants.h"
#include "LauncherSettingsFile.h"
#include "AppsListView.h"
#include "RecentDocsBListView.h"
#include "AppListItem.h"
#include "EngineSubscriber.h"


class MainView : public BTabView/*, public EngineSubscriber*/ {
public:
						MainView(BRect, AppSettings);
//						~MainView();
	virtual void		AttachedToWindow();
	virtual void		DetachedFromWindow();
	virtual void		MessageReceived(BMessage*);
//	virtual void		MouseDown(BPoint pos);
	virtual void		KeyDown(const char* bytes, int32 numbytes);
	virtual void		Select(int32 tab);
			void		SelectDefaultTab();
			void		SettingsChanged(uint32 what, AppSettings settings);
			void		BuildAppsListView(BMessage *message);
private:
	BTab					*fAppsTab, *fRecentDocsTab;
	BScrollView				*fAppsScrollView, *fDocsScrollView;
	AppsListView			*fAppsListView;
	RecentDocsBListView		*fDocsListView;
	int32					fTabCount;
	BListView				*fSelectedListView;
//	bool					fWatchingRoster;
//	LauncherSettingsFile	*fSettingsFile;
	AppSettings				fCurrentSettings;
	entry_ref				fLastRecentDocRef;
	void					_UpdateSelectedListView();
	status_t				_AddAppListItem(BEntry appEntry, int totalCount, int index);
//	void					_BuildAppsListView(BMessage *message);
//	void					_Subscribe();
	// virtual functions inherited from the EngineSubscriber class
//	virtual void			_SubscribeFailed();
//	virtual void			_SubscribeConfirmed();
//	virtual void			_UpdateReceived(BMessage *message);
	void					_BuildDocsListView(bool force=false);
};

#endif
