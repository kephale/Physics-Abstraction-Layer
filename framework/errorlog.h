#ifndef ERRORLOG_H
#define ERRORLOG_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/**
	Abstract:
		ErrorLog		-	PAL version
	
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.2:  13/04/09 - Const & string conformance
		Version 0.1.1:12/01/08 - Error flag
		Version 0.1:  19/08/04
	
	TODO:
*/

#include "statusobject.h"
#include "os.h"
#include <stdexcept>
//#define RANGE_ERROR(x) throw std::range_error((x));
//#define ARG_ERROR(x) throw std::invalid_argument((x));

#define SET_CRITICAL_ERROR ErrorLog::GetInstance()->SetInfo(__FILE__,__LINE__,this,"CRITICAL ERROR");ErrorLog::GetInstance()->CriticalError
#define SET_ERROR ErrorLog::GetInstance()->SetInfo(__FILE__,__LINE__,this,"Error");ErrorLog::GetInstance()->Error
#define SET_WARNING ErrorLog::GetInstance()->SetInfo(__FILE__,__LINE__,this,"warning");ErrorLog::GetInstance()->Warning
#define SET_DEBUG ErrorLog::GetInstance()->SetInfo(__FILE__,__LINE__,this,"debug");ErrorLog::GetInstance()->Debug

#define STATIC_SET_ERROR ErrorLog::GetInstance()->SetInfo(__FILE__,__LINE__,0,"(static function) Error");ErrorLog::GetInstance()->Error

//nope, got to use line var.
//#define SET_DEBUG_ALERT_LEVEL(x) DebugAlertLevelControl set_debug_alert_level((x));
//#define SET_DEBUG_LEVEL(x) DebugLevelControl debug_level_control((x));
#define SET_DEBUG_ALERT_LEVEL(x) DebugAlertLevelControl NAMED_LINE_VAR(set_debug_alert_level) ((x));
#define SET_DEBUG_LEVEL(x) DebugLevelControl NAMED_LINE_VAR(debug_level_control) ((x));

class ErrorLog {
public:
	ErrorLog();
        ErrorLog(const ErrorLog& log);
	void SetInfo(const char *FileName, const long Line, StatusObject *pObject, const char *Type);
	void CriticalError(const char *Message, ...);
	void Error(const char *Message, ...);
	void Warning(const char *Message, ...);
	void Debug(const char *Message, ...);
	int  GetDebugLevel();
	void SetDebugLevel(int Level);
	int  GetDebugAlertLevel();
	void SetDebugAlertLevel(int Level);
	void ClearError();
	bool GetError();
	static ErrorLog *GetInstance();
private:
	// change to make a deep copy before making this public
	ErrorLog& operator=(const ErrorLog& log) { return *this; }
protected:
	bool error;
	PAL_STRING m_infoFileName;
	long m_infoLine;
	StatusObject *m_infopObject;
	PAL_STRING m_infoType;
	int m_DebugAlertLevel;
	int m_DebugLevel;
	void DoLog(const char *Message);
	void WriteLog(const char *sz);
        void init();
	static ErrorLog *m_pInstance;
};

//use this class just to set a temporary change in the debug level.
class DebugLevelControl {
public:
	DebugLevelControl(int Level)
          : m_old_level(ErrorLog::GetInstance()->GetDebugLevel()) {
		ErrorLog::GetInstance()->SetDebugLevel(Level);
//		printf("setting debug level to :%d\n",Level);
	}
	~DebugLevelControl() {
		ErrorLog::GetInstance()->SetDebugLevel(m_old_level);
//		printf("setting debug level to :%d\n",m_old_level);
	}
private:
	int m_old_level;
};

class DebugAlertLevelControl {
public:
	DebugAlertLevelControl(int Level)
          : m_old_level(ErrorLog::GetInstance()->GetDebugAlertLevel()) {
		ErrorLog::GetInstance()->SetDebugAlertLevel(Level);
	}
	~DebugAlertLevelControl() {
		ErrorLog::GetInstance()->SetDebugAlertLevel(m_old_level);
	}
private:
	int m_old_level;
};

#endif
