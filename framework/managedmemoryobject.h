#ifndef MANAGEDMEMORYOBJECT_H
#define MANAGEDMEMORYOBJECT_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		Object memory management via an object-manager
	Author: 
		Adrian Boeing
	Revision History:	
		Version 1.03: 04/08/04 Virtual free all
		Version 1.02: 12/06/04 Protected list for MOM to allow custom free.
		Version 1.01: 22/01/04 Restored to working state
		Version 1.0 : 28/12/03
	TODO:
		- Improve iterator performance, incase it need not delete
		- Support FreeStoreDetect class or force private MMO's
		- Proper virtual free all
*/

//header:

#include "empty.h"
#include "pal/palStringable.h"
#include <cstdio>

template <typename MemoryBase> class MemoryObjectManager;

template <typename MemoryBase>
class ManagedMemoryObject : public MemoryBase, public palStringable {
public:
//private:
	ManagedMemoryObject();
	ManagedMemoryObject(const ManagedMemoryObject<MemoryBase>& mmo)
		: MemoryBase(mmo) {
		pMOM = mmo.pMOM;
	}
	ManagedMemoryObject& operator=(const ManagedMemoryObject<StatusObject>& mmo) { pMOM = mmo.pMOM; return *this; }
public:
	virtual ~ManagedMemoryObject();
	MemoryObjectManager<MemoryBase> *pMOM; //wheres my mommy?
	virtual std::string toString() const;
};

template <typename MemoryBase>
class MemoryObjectManager {
public:
	MemoryObjectManager() : pMMO() {}
	virtual ~MemoryObjectManager() {}
	void Add(ManagedMemoryObject<MemoryBase> *item);
	void Remove(ManagedMemoryObject<MemoryBase> *item);
	virtual void FreeAll();
protected:
//private:
	//this should be private: need to find a way to friend a template class
	PAL_LIST<ManagedMemoryObject<MemoryBase> *> pMMO;
};

//code:
//mmo
template <typename MemoryBase> ManagedMemoryObject<MemoryBase>::ManagedMemoryObject()
: pMOM(0) {
}

template <typename MemoryBase> ManagedMemoryObject<MemoryBase>::~ManagedMemoryObject() {
	if (pMOM != NULL) { //If we have a MOM
		pMOM->Remove(this);
	}
}

template <typename MemoryBase> std::string ManagedMemoryObject<MemoryBase>::toString() const {
	char buf[16];
	sprintf(buf, "0x%p", this);
	std::string result("ManagedMemoryObject@");
	result += buf;
    return result;
}

//mom
template <typename MemoryBase>
void MemoryObjectManager<MemoryBase>::Add(ManagedMemoryObject<MemoryBase> *item) {
	pMMO.push_back(item);
	item->pMOM=this;
}

template <typename MemoryBase>
void MemoryObjectManager<MemoryBase>::Remove(ManagedMemoryObject<MemoryBase> *item) {
	typename PAL_LIST<ManagedMemoryObject<MemoryBase>*>::iterator obj;
	obj = std::find(pMMO.begin(), pMMO.end(), item);
	if(obj != pMMO.end()) {
		pMMO.erase(obj);
	}
};

template <typename MemoryBase>
void MemoryObjectManager<MemoryBase>::FreeAll() {
	while (!pMMO.empty() ) {
		delete *(pMMO.begin());
		//no need to erase() because the MMO takes care of it.
	}
}

#endif
