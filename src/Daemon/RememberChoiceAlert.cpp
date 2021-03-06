/* RememberChoiceAlert.cpp
 * Copyright 2010 Brian Hill
 * All rights reserved. Distributed under the terms of the BSD License.
 */
#include "RememberChoiceAlert.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "RememberChoiceAlert.cpp"

RememberChoiceAlert::RememberChoiceAlert(const char* title, const char* text,
								const char* button0Label, const char* button1Label,
								const char* button2Label,
								button_width widthStyle,
								alert_type type)
	:
	BAlert(title, text, button0Label, button1Label, button2Label, widthStyle, type)
{
	fInvoker = NULL;
	// Use the text view to add a check box to the alert
	fRememberCB = new BCheckBox("Save Settings", B_TRANSLATE_COMMENT("Always use the following choice:", "Alert window checkbox label"), NULL);
	BTextView *textView = TextView();
	BLayoutBuilder::Group<>(textView, B_VERTICAL, 0)
		.AddGlue()
		.Add(fRememberCB);
	BSize viewSize=textView->MinSize();
	viewSize.height+=75;
	textView->SetExplicitMinSize(viewSize);
}


void
RememberChoiceAlert::MessageReceived(BMessage* msg)
{
	// Insert the check box value into the message
	if (msg->what == 'ALTB')
	{
		if (fInvoker)
		{
			BMessage* out = fInvoker->Message();
			bool value = fRememberCB->Value()? true : false;
			out->AddBool(ED_ALERT_REMEMBER, value);
		}
	}
	BAlert::MessageReceived(msg);
}


status_t
RememberChoiceAlert::Go(BInvoker *invoker)
{
	fInvoker = invoker;
	return BAlert::Go(invoker);
}
