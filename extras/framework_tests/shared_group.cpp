#include "abstract.h"
#include "framework.h"

//#define DLL_GROUP_IMPLEMENTATION
//#define INTERNAL_DEBUG

class ConcreteHello : public AbstractHello {
public:
	ConcreteHello() {}; //default constructor
	virtual void SayHello() {
		printf("Hello from Shared - Group!\n");
	}
	FACTORY_CLASS(ConcreteHello,AbstractHello,*,3);
};



class ConcreteGoodbye: public AbstractGoodbye {
public:
	ConcreteGoodbye() {}; //default constructor
	virtual void SayGoodbye() {
		printf("Goodbye from Shared - Group!\n");
	}
	FACTORY_CLASS(ConcreteGoodbye,AbstractGoodbye,*,3);
};

FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP
FACTORY_CLASS_IMPLEMENTATION(ConcreteHello)
FACTORY_CLASS_IMPLEMENTATION(ConcreteGoodbye)
FACTORY_CLASS_IMPLEMENTATION_END_GROUP