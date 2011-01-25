/*
 * palException.hh
 *
 *  Created on: Jul 27, 2010
 *      Author: Chris Long
 *  Copyright (C) 2010 SET Corporation
 */

#ifndef PALEXCEPTION_HH_
#define PALEXCEPTION_HH_

#include <stdexcept>

class palException : public std::exception {
public:
	palException() throw();
	palException(const char* message, const std::exception* cause = 0) throw();
	virtual ~palException() throw();
	virtual const char* what() const throw();
	virtual void InitMessage(const char* message);
	virtual void InitCause(std::exception* cause);
	virtual const std::exception* GetCause() const;
protected:
	const char* m_message;
	const std::exception* m_cause;
};

class palIllegalStateException : public palException {
public:
	palIllegalStateException() throw();
	palIllegalStateException(const char* message, std::exception* cause = 0) throw() :
		palException(message, cause) {}
};

#endif /* PALEXCEPTION_HH_ */
