/* RecentFoldersBListView.h
 * Copyright 2012 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#ifndef EINSTEINIUM_LAUNCHER_RECFOLDERSBLISTVIEW_H
#define EINSTEINIUM_LAUNCHER_RECFOLDERSBLISTVIEW_H

#include "RecentDocsBListView.h"


class RecentFoldersBListView : public RecentDocsBListView {
public:
						RecentFoldersBListView(BRect size);
		virtual	void		BuildList(AppSettings *settings, bool force=false);
		virtual	void		SettingsChanged(uint32 what, AppSettings settings);

private:

	entry_ref			fLastRecentFolderRef, fLastRecentQueryRef;
	SuperTypeListItem	*fFolderSuperListItem, *fQuerySuperListItem;
	const char			*fQueryTypes[];
};

#endif
