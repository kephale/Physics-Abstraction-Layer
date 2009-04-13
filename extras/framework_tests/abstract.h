#ifndef ABSTRACT_H
#define ABSTRACT_H

#include "../../framework/factoryconfig.h"
#include "../../framework/errorlog.h"

class AbstractHello : public myFactoryObject {
public:
	virtual void SayHello() = 0;
};

class AbstractGoodbye : public myFactoryObject {
public:
	virtual void SayGoodbye() = 0;
};

#endif