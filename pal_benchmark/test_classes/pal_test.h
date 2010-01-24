#ifndef PAL_TEST_H
#define PAL_TEST_H

#include "pal/pal.h"

#ifdef _WIN32
	class palPhysics;
#endif

class PALTest {
public:
	PALTest() {
		pp = 0;
		max_time = 10.0f;
		step_size = 0.01f;
	}
	
	virtual bool Done() {
		if (!pp)
			return false;
		if (pp->GetTime()>max_time)
			return true;
		return false;
	}
	virtual void Reset() {};
	virtual void SetMaxSimTime(float time) {
		max_time = time;
	}
	virtual void SetStepSize(float stepsize) {
		step_size = stepsize;
	}
	virtual int CreatePhysics() {
		return doCreatePhysics();
	}
	virtual void Update() {
		if (!pp)
			return;
		pp->Update(step_size);
		//do inner loop
		doInnerUpdateLoop();
		SaveData();
	}
protected:
	virtual void SaveData() {};
	virtual void doInnerUpdateLoop() = 0;
	virtual int  doCreatePhysics() = 0;
	
	float ufrand() { return rand()/(float)RAND_MAX; }
	float sfrand() { return (ufrand()-0.5f)*2.0f; }
	
	float max_time;
	float step_size;
	palPhysics *pp;
};
#endif