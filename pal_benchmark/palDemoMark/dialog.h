#ifndef DIALOG_BOX_H
#define DIALOG_BOX_H

/*
 *  dialog.h
 *  PALBenchmark
 *
 *  Created by Brock Woolf on 16/01/10.
 *  Copyright 2010 Blastcube. All rights reserved.
 *
 */

#ifdef __APPLE__
	#include <Carbon/Carbon.h>
#elif defined WIN32
	#include <windows.h>
#endif

class GUIFeedback
{
public:
	GUIFeedback() { ; }
	
	void DisplayErrorDialogue(const char *cmsg)
	{
	#ifdef __APPLE__
		Str255 msg;
		c2pstrcpy(msg, cmsg);
		StandardAlert(kAlertStopAlert, "\pError", (ConstStr255Param)msg, NULL, NULL);
	#elif defined WIN32
		MessageBox(NULL, cmsg, "Error", MB_ICONERROR | MB_OK);
	#else
		printf("ERROR: %s\n", cmsg);
	#endif
	}
};
 
#endif