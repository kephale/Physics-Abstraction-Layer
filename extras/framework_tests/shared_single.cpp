#include "abstract.h"
#include "framework.h"

//#define DLL_IMPLEMENTATION
//#define INTERNAL_DEBUG

class SingleConcreteHello : public AbstractHello {
public:
	SingleConcreteHello() {}; //default constructor
	virtual void SayHello() {
		printf("Hello from Shared - Single!\n");
	}
	FACTORY_CLASS(SingleConcreteHello,AbstractHello,*,9);
};

FACTORY_CLASS_IMPLEMENTATION(SingleConcreteHello);