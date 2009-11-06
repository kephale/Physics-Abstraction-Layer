#include "errorlog.h"
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		ErrorLog		-	PAL version
	
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.2: 05/11/09 Fixed a bug where it was passing the wrong length to snprintf
		Version 0.1: 19/08/04
	
	TODO:
*/

ErrorLog *ErrorLog::m_pInstance;

ErrorLog::ErrorLog() {
#ifdef NDEBUG
		m_DebugAlertLevel = 0;
#else
		m_DebugAlertLevel = 1;
#endif
		m_DebugLevel = 1;
		error = false;
		char buf[4096];
		OS_snprintf(buf,4096,"Error Log Initialized. Compiled:%s %s\n",__TIME__,__DATE__);
		WriteLog("----------------------------------------\n");
		WriteLog(buf);
		WriteLog("---------------------------------------:\n");
	}
	void ErrorLog::SetInfo(const char *FileName, const long Line, StatusObject *pObject, const char *Type) {
		m_infoFileName = FileName;
		m_infoLine = Line;
		m_infopObject = pObject;
		m_infoType = Type;
	}
	void ErrorLog::CriticalError(const char *Message, ...) {
		char buf[4096];
		va_list ap;
		va_start(ap, Message);
		OS_vsnprintf(buf, 4096, Message, ap);
		va_end(ap);
		DoLog(buf);
		StatusObject *pSO = (StatusObject *)m_infopObject;
		pSO->SetStatus(SCRITICALERROR);
		error = true;
		throw std::runtime_error(buf);
	}
	
	void ErrorLog::Error(const char *Message, ...) {
		char buf[4096];
		va_list ap;
		va_start(ap, Message);
		OS_vsnprintf(buf, 4096, Message, ap);
		va_end(ap);
		DoLog(buf);
		StatusObject *pSO = (StatusObject *)m_infopObject;
		if (pSO)
			pSO->SetStatus(SERROR);
		error = true;
	}
	void ErrorLog::Warning(const char *Message, ...) {
		char buf[4096];
		va_list ap;
		va_start(ap, Message);
		OS_vsnprintf(buf, 4096, Message, ap);
		va_end(ap);
		DoLog(buf);
		StatusObject *pSO = (StatusObject *)m_infopObject;
		pSO->SetStatus(SWARNING);
	}
	void ErrorLog::Debug(const char *Message, ...) {
		if (m_DebugLevel>m_DebugAlertLevel) return;
		char buf0[4096];
		char buf1[4096];
		va_list ap;
		va_start(ap, Message);
		OS_vsnprintf(buf0, 4096, Message, ap);
		va_end(ap);
		
		OS_snprintf(buf1,4096,"l(%d):%s",m_DebugLevel,buf0);
		DoLog(buf1);
	}
	int  ErrorLog::GetDebugLevel() {
		return m_DebugLevel;
	}
	void ErrorLog::SetDebugLevel(int Level) {
		m_DebugLevel=Level;
	}
	int  ErrorLog::GetDebugAlertLevel() {
		return m_DebugAlertLevel;
	}
	void ErrorLog::SetDebugAlertLevel(int Level) {
		m_DebugAlertLevel=Level;
	}
	ErrorLog *ErrorLog::GetInstance() {
		if (m_pInstance == NULL) 
			m_pInstance = new ErrorLog;
		return m_pInstance;
	}
	void ErrorLog::DoLog(const char *Message) {
		char sz[8192];
		OS_snprintf(sz,8192,"%s:%ld: Object:(0%.8p) : (%s): %s\n",m_infoFileName.c_str(),m_infoLine,m_infopObject,m_infoType.c_str(),Message);
		WriteLog(sz);
	}
	void ErrorLog::WriteLog(const char *sz) {
		FILE *fout=fopen("log.txt","a");
		if (fout) {
			fprintf(fout,"%s",sz);
			fclose(fout);
		}
		//else : we are on a CD? -- who cares now?
	}
	void ErrorLog::ClearError() {
		error=false;
	}
	bool ErrorLog::GetError() {
		return error;
	}
