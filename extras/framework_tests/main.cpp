#include <stdio.h>
#include "abstract.h"
#include "framework.h"

//#define INTERNAL_DEBUG

int main(int argc, char *argv[]) {
	printf("Hello from main!\n");

	printf("Contents before DLL load:\n");
	printf("-----------------\n");
	myFactory::GetInstance()->DisplayAllObjects();
	printf("-----------------\n");

	printf("Loading DLL\n");
	myFactory::GetInstance()->LoadObjects(0,myFactory::GetInstance(),&myFactory::GetInstance()->sInfo());

	printf("Rebuilding Registry\n");
	myFactory::GetInstance()->RebuildRegistry();

	printf("Contents after DLL load:\n");
	printf("-----------------\n");
	myFactory::GetInstance()->DisplayAllObjects();
	printf("-----------------\n");
	printf("If these are both the same (== empty) then you might not be running from the right directory.\n");

	myFactoryObject *pmfo = myFactory::GetInstance()->Construct("AbstractHello");
	if (!pmfo) {
		printf("Could not create AbstractHello! - no registry entry!\n");
		return 0;
	}
	AbstractHello *pah = dynamic_cast<AbstractHello *>(pmfo);
	if (!pah) {
		printf("Could not cast to AbstractHello! - bad registry entry!\n");
		return 0;
	}
	pah->SayHello();


	myFactoryObject *pmfo2 = myFactory::GetInstance()->Construct("AbstractGoodbye");
	if (!pmfo2) {
		printf("Could not create AbstractGoodbye! - no registry entry!\n");
		return 0;
	}
	AbstractGoodbye *pag = dynamic_cast<AbstractGoodbye *>(pmfo2);
	if (!pag) {
		printf("Could not cast to AbstractGoodbye! - bad registry entry!\n");
		return 0;
	}
	pag->SayGoodbye();

	return 0;
}
