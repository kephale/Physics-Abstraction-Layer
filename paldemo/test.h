#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include "../pal/pal.h"
#include "../pal/palFactory.h"
#include "../example/graphics.h"

class Test : public myFactoryObject {
public:
	Test() {
		pTerrain = NULL;
	}
	virtual ~Test() {};

	virtual void Init(int terrain_type) = 0;

	virtual void Input(SDL_Event E) = 0;
	virtual void Update() = 0;
	virtual void AdditionalRender() {};

	virtual std::string GetName() = 0;
	virtual std::string GetDescription() = 0;

protected:

	palTerrain *pTerrain;

	palBodyBase *CreateBody(const char *paltype,float x, float y, float z, float dimx, float dimy, float dimz, float mass, bool graphics = true);


	virtual void CreateTerrain(int type, float size = 10.0f);
	
	virtual void CreatePool(float height,float length_down,float length_up,float width_down,float width_up);
	virtual void CreateHeightmap(int size, float stretch = 10.0f);
float ufrand() {
	return rand()/(float)RAND_MAX;
}
float sfrand() {
	return (ufrand()-0.5f)*2.0f;
}
	
};

#endif