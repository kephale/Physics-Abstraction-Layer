#include "statusobject.h"
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		Status object for message/error passing, without exceptions
	Author: 
		Adrian Boeing
	Revision History:
		Version 1.1 : 04/08/03
	TODO:
		-Allow integration of status tracker, or similar, to enable multiple path error recovery.
*/

#ifndef NULL
#define NULL 0
#endif

StatusObject::StatusObject() {
	m_pParent=NULL;
	m_StatusCode = SOK;
}

void StatusObject::SetStatus(StatusCode Statuscode) {
	if (m_StatusCode & TEST_ERROR) 
		return; //dont overwrite existing errors
	m_StatusCode = Statuscode;
	if (m_pParent!=NULL) {
		if (!(m_pParent->m_StatusCode & TEST_ERROR) ) {
			m_pParent->SetStatus(Statuscode);
			//dont overwrite existing errors
		}	
	}
}

void StatusObject::ClearStatus(StatusCode Statuscode) {
	m_StatusCode = Statuscode;
	if (m_pParent!=NULL)	
		m_pParent->ClearStatus(Statuscode);
}

void StatusObject::SetParent(StatusObject *pParent) {
	m_pParent=pParent;
}

StatusObject *StatusObject::GetParent() {
	return m_pParent;
}
