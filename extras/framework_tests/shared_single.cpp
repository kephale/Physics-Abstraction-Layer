#include "abstract.h"
#include "framework.h"

//#define DLL_IMPLEMENTATION
//#define INTERNAL_DEBUG

class ConcreteHello : public AbstractHello {
public:
	ConcreteHello() {}; //default constructor
	virtual void SayHello() {
		printf("Hello from Shared - Single!\n");
	}
	FACTORY_CLASS(ConcreteHello,AbstractHello,*,9);
};

FACTORY_CLASS_IMPLEMENTATION(ConcreteHello);