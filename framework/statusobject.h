#ifndef STATUSOBJECT_H
#define STATUSOBJECT_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/**
	Abstract:
		Status object for message/error passing, without exceptions
	Author: 
		Adrian Boeing
	Revision History:
		Version 1.1 : 04/08/03
		Version 1.0 : 18/11/03
	TODO:
		-Allow integration of status tracker, or similar, to enable multiple path error recovery.
*/

#include "statuscode.h"

class StatusObject {
public:
	StatusObject();
	StatusObject(const StatusObject& so);
	virtual ~StatusObject();
	virtual void SetStatus(StatusCode Statuscode);
	virtual void ClearStatus(StatusCode Statuscode);
	void SetParent(StatusObject *pParent);
	StatusObject *GetParent();
protected:
	StatusObject *m_pParent;
	StatusCode m_StatusCode;
};

#define OWN(x) {if ((x)) (x)->SetParent(this);}
#define CERROR (m_StatusCode & TEST_ERROR)
#define PERROR(x) ((x) && ((x)->m_StatusCode & TEST_ERROR))

#endif


