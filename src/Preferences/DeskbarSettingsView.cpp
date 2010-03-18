/*DeskbarSettingsView.cpp

*/
#include "DeskbarSettingsView.h"

DeskbarSettingsView::DeskbarSettingsView(BRect size)
	:BView(size, "Deskbar Settings", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{	SetViewColor(bg_color);
	BRect viewRect;
	deskbarBox = new BBox("Deskbar");
	deskbarBox->SetLabel("Deskbar Settings");
	showDeskbarCB = new BCheckBox(viewRect, "Show Deskbar",
								"Show a list of ranked applications in the Deskbar",
								new BMessage(EE_DESKBAR_CHANGED));
	itemCountTC = new BTextControl("Show this many applications:", "",
								new BMessage(EE_DESKBAR_CHANGED));
	//itemCountTC->SetDivider(be_plain_font->StringWidth(itemCountTC->Label()) + 4);
	itemCountTC->SetExplicitMaxSize(BSize(be_plain_font->StringWidth(itemCountTC->Label())
				+ be_plain_font->StringWidth("00000000"), B_SIZE_UNSET));
	long i;
	for (i = 0; i < 256; i++) itemCountTC->TextView()->DisallowChar(i);
	for (i = '0'; i <= '9'; i++) itemCountTC->TextView()->AllowChar(i);
	itemCountTC->TextView()->AllowChar(B_BACKSPACE);

	deskbarBox->AddChild(BGroupLayoutBuilder(B_VERTICAL, 5)
		.Add(showDeskbarCB)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, 5)
			.Add(itemCountTC)
			.AddGlue()
			.SetInsets(15, 0, 0, 0)
		)
		.SetInsets(5, 5, 5, 5)
	);

	SetLayout(new BGroupLayout(B_HORIZONTAL));
	AddChild(BGroupLayoutBuilder(B_VERTICAL, 10)
		.Add(deskbarBox)
		.AddGlue()
	);
}

/*DeskbarSettingsView::~DeskbarSettingsView()
{

}
*/

void DeskbarSettingsView::MessageReceived(BMessage* msg)
{	switch(msg->what)
	{
		case EE_DESKBAR_CHANGED: {
			bool showDeskbar = showDeskbarCB->Value();
			int16 count = strtol(itemCountTC->Text(), NULL, 0);
			// TODO update settings file
			EESettingsFile *settings = new EESettingsFile();
			settings->SaveDeskbarSettings(showDeskbar, count);

			app_info info;
			status_t result = be_roster->GetAppInfo(e_engine_sig, &info);
			if(result==B_OK)
			{
				// Send message to update the deskbar menu
				BMessenger messenger(e_engine_sig);
				BMessage msg(E_UPDATE_SHELFVIEW_SETTINGS);
				msg.AddBool("show", showDeskbar);
				msg.AddInt16("count", count);
				messenger.SendMessage(&msg);
			}

			itemCountTC->SetEnabled(showDeskbar);
			break;
		}
	}
}

void DeskbarSettingsView::SetDeskbarValues(bool show, int count)
{
	showDeskbarCB->SetValue(show);
	char *number = new char[16];
	sprintf(number, "%i", count);
	itemCountTC->SetText(number);
	itemCountTC->SetEnabled(show);
	delete[] number;
}
