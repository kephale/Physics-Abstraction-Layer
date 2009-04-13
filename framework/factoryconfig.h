#ifndef FACTORYCONFIG_H
#define FACTORYCONFIG_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/**
	Abstract:
		The "configuration" setting for the factory templates I choose to use.
	Author:
		Adrian Boeing
	Revision History:
		Version 1.1  :06/12/07 Update merge with MGF, myFactory singleton and DLL factory set instance
		Version 1.0.4:18/08/04 PAL modifications
		Version 1.0.3:04/08/04 Virtual freeobjects
		Version 1.0.2:12/06/04 Debugging methods - dont compile with VC6 - speed optimizations
		Version 1.0.1:19/01/04 LoadObjects win32 bugfix
		Version 1.0 : 14/01/04
	TODO:
		proper virtual inheritance of free objects
*/
//#define INTERNAL_DEBUG
#ifdef INTERNAL_DEBUG
#include <stdio.h>
#endif

//#include "statushelpers.h"
#include "factory.h"
#include "statusobject.h"
#include "managedmemoryobject.h"
//#include "serialize.h" //need this to support dynamic loading of serializable objects (jic)

typedef ManagedMemoryObject<StatusObject> myFactoryBase;
//typedef GroupVersionInfo<myFactoryBase> myFactoryInfo;
//#define myFactoryParameters myFactoryInfo,myFactoryBase
typedef RegistrationInfo<myFactoryBase> myFactoryInfo;
#define myFactoryParameters myFactoryBase
typedef FactoryObject<myFactoryBase> myFactoryObject;
typedef PluggableFactory<myFactoryBase> myPluggableFactory;

class myFactory : public myPluggableFactory, public MemoryObjectManager<StatusObject> {
public:
	static void LoadObjects(char *szPath = NULL, void *factoryPointer = 0, void *factoryInfoPointer=0);
	virtual void FreeObjects(void);
	myFactoryObject *Construct(PAL_STRING ClassName);
#ifdef INTERNAL_DEBUG
	void DisplayAllObjects();
#endif
#if 1 //singleton
public:
	static void SetInstance(void *ptr) {
		m_pInstance = (myFactory *)ptr;
	}
	static myFactory *GetInstance() {
		if (m_pInstance == NULL)
			m_pInstance = new myFactory ;
		return m_pInstance;
	}
private:
	static myFactory *m_pInstance;
#endif
};


#ifdef INTERNAL_DEBUG
#define FACTORY_CLASS(name,ClassName,GroupName,Version) public: \
	name(FactoryStaticRegisterVariable reg) { \
		 RegisterWithFactory(PluggableFactory<myFactoryParameters>::sInfo());} \
void RegisterWithFactory(PAL_VECTOR<myFactoryInfo> &lsInfo) { \
		myFactoryInfo ri; \
		ri.mUniqueName=#name; \
		ri.mClassName=#ClassName; \
		ri.mVersion=Version; \
		ri.mGroupName=#GroupName; \
		ri.mConstructor=(myFactoryObject *) this; \
		printf("%s:%d: Registering %s (%p) with sInfo:%p (size:%d)\n",__FILE__,__LINE__,#name,this,&lsInfo,lsInfo.size()); \
		Register(ri,lsInfo); \
} \
myFactoryObject* Create() {return new name;} \
	private:
#else
#define FACTORY_CLASS(name,ClassName,GroupName,Version) public: \
name(FactoryStaticRegisterVariable reg) { \
		 RegisterWithFactory(PluggableFactory<myFactoryParameters>::sInfo());} \
void RegisterWithFactory(PAL_VECTOR<myFactoryInfo> &lsInfo) { \
		myFactoryInfo ri; \
		ri.mUniqueName=#name; \
		ri.mClassName=#ClassName; \
		ri.mVersion=Version; \
		ri.mGroupName=#GroupName; \
		ri.mConstructor=(myFactoryObject *) this; \
		Register(ri,lsInfo); \
} \
virtual myFactoryObject* Create() {return new name;} \
	private:
#endif //INTERNAL_DEBUG






//a single dll component
#if defined (DLL_IMPLEMENTATION)

#if defined (OS_WINDOWS) || defined(OS_LINUX) || defined (OS_OSX)
#define FACTORY_CLASS_IMPLEMENTATION(name) extern "C" DLL_FUNC void *CreateComponent() {return new name;}
#else
#error Unsuported DLL implementation for this environment
#endif

#endif



//a group of components
#if defined (DLL_GROUP_IMPLEMENTATION)

#if defined (OS_WINDOWS) || defined(OS_LINUX) || defined (OS_OSX)

#define FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP extern "C" DLL_FUNC void Group_SetFactory(void *value, void *psvv) { \
	myFactory::SetInstance(value); \
	PAL_VECTOR<myFactoryInfo >* psv = (std::vector<myFactoryInfo >*) psvv;	  \
   if (psv != &myFactory::GetInstance()->sInfo()) {\
      \
      PAL_VECTOR<myFactoryInfo >::iterator it; for(it = psv->begin();  it!=psv->end(); it++) { \
      	myFactory::GetInstance()->sInfo().push_back(	  *it); \
	   }  \
	}\
} \
extern "C" DLL_FUNC void *Group_CreateComponent(int value) { \
	static size_t *sizep = 0; \
	static PAL_VECTOR<myFactoryObject *> s_constructors; \
	if (value<0) { \
		if (s_constructors.size()<1) \
			goto create_constructors;  \
		goto return_size; \
	} \
	if (value>(int)s_constructors.size()-1) \
		return 0; \
	return s_constructors[value]->Create(); \
create_constructors:


#define FACTORY_CLASS_IMPLEMENTATION(name) s_constructors.push_back(new name);

#define FACTORY_CLASS_IMPLEMENTATION_END_GROUP return_size: \
	if (sizep == 0) \
		sizep = new size_t; \
	*sizep = s_constructors.size(); \
		return sizep; \
}

#else
#error Unsuported DLL implementation for this environment
#endif

#endif //if defined (DLL_GROUP_IMPLEMENTATION)

//a static implementation
#if !defined (DLL_IMPLEMENTATION) && !defined (DLL_GROUP_IMPLEMENTATION)
#define FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP
#define FACTORY_CLASS_IMPLEMENTATION(name) static name g_pluggablefactory_static_ ## name (FACTORY_REGISTER);
#define FACTORY_CLASS_IMPLEMENTATION_END_GROUP
#endif //DLL_IMPLEMENTATION




#endif



