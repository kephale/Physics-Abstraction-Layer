#ifndef FACTORY_H
#define FACTORY_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		The base class for all pluggable factory objects.
		Maintains a registry of all classes, and allows construction of any registered class from one interface

		Rewritten to reduce templated arguments -- a specific version of GroupVersionInfo policy
	Author: 
		Adrian Boeing
	Revision History:
		Version 3.4 : 06/12/07 Platform independent DLL (MGF merge)
		Version 3.3 : 03/11/07 Shared factory DLL
		Version 3.2 : 28/06/07 Added group DLL macros
		Version 3.1 : 05/12/04 UpdateRegistry Bugfix and VC8 compliance
		Version 3.0 : 19/08/04 PAL cut
		Version 2.4.1:12/06/04 Internal debugging methods - cant use VC6 optimize for speed - incorrect optimizations
		Version 2.4 : 14/01/04 Hack for VC6 support to elimnate 
		Version 2.3 : 12/01/04 DLL Support
		Version 2.2 : 28/12/03 GCC typedef fix, policy based factory objects 
		Version 2.1 : 20/11/03 VC.NET fix
		Version 2.0 : 20/11/03 Complete rewrite for policy design
		Version 1.2 : 06/10/03 Bugfix for MSCVER >=1300
		Version 1.1 : 05/10/03 Automated Version Control
		Version 1.0 : 0?/10/03
	TODO:
 */


#include <string.h>
#include "common.h"
#include "os.h"

#include <algorithm>
#include <map>
#define PAL_MAP std::map

template <typename FactoryBase> class FactoryObject; //predefined

template <typename FactoryBase> 
class RegistrationInfo {
public:
	RegistrationInfo();
	PAL_STRING mClassName;
	PAL_STRING mGroupName;
	PAL_STRING mUniqueName;
	unsigned long mVersion;
	FactoryObject<FactoryBase> *mConstructor; //who constructed me?
	bool operator == (const RegistrationInfo& ) const;
};

template <typename FactoryBase> RegistrationInfo<FactoryBase>::RegistrationInfo()
			: mClassName(""), mGroupName(""), mUniqueName(""), mVersion(0), mConstructor(0) {
}

template <typename FactoryBase> bool RegistrationInfo<FactoryBase>::operator ==(const RegistrationInfo<FactoryBase> &rRight) const {
	if (  (mClassName == rRight.mClassName)
				&&(mGroupName == rRight.mGroupName)
				&&(mVersion == rRight.mVersion)
				&&(mUniqueName == rRight.mUniqueName))
		return true;
	return false;
}

template <typename FactoryBase>
class FactoryObject : public FactoryBase {
public:
	virtual ~FactoryObject() {}
	virtual void Register(RegistrationInfo<FactoryBase> &RI, PAL_VECTOR<RegistrationInfo<FactoryBase> > &lsInfo );
	virtual FactoryObject* Create() = 0;
	//the following code is ugly, should be some how eliminated and integrated with the normal Register function
	virtual void RegisterWithFactory(PAL_VECTOR<RegistrationInfo<FactoryBase> > &lsInfo) = 0;
};

/**
* Usage note: You must manually instantiate the sInfo field for each
* unique specialization of this template. One such example is:
*
* template <> PAL_VECTOR<RegistrationInfo<myFactoryBase> > * PluggableFactory< ManagedMemoryObject<StatusObject> >::sInfoInstance = 0;
*
* Make sure you instantiate this field only once.
*
* This approach was chosen due to bad interactions with shared
* libraries when a static local variable was used.
*/
template <typename FactoryBase>
class PluggableFactory {
public:
	//the constructor only needed for initializing the group.
	PluggableFactory()
	: mActiveGroup("NONE") {
#ifdef INTERNAL_DEBUG
		printf("PluggableFactory::ctor: this = %p\n", this);
#endif
	}

	virtual ~PluggableFactory() {
#ifdef INTERNAL_DEBUG
		printf("Factory destructor.\n");
#endif
	}
	FactoryObject<FactoryBase> *newObject(PAL_STRING ClassName);

	void UpdateRegistry(typename PAL_VECTOR<RegistrationInfo<FactoryBase> >::iterator Entry);

	void RebuildRegistry();

	// NOTE: this is not multi-threaded safe
	static PAL_VECTOR<RegistrationInfo<FactoryBase> >& sInfo() //this is the 'central repository' for this registration information set
   			 {
		// can't use a static local because it causes bugs with shared libraries with some compilers (e.g., gcc 4.1.2)
		if (sInfoInstance == 0) {
			sInfoInstance = new PAL_VECTOR<RegistrationInfo<FactoryBase> >;
		}
		return *sInfoInstance;
   			 }
#ifdef INTERNAL_DEBUG
	void DisplayContents() {
		typename PAL_MAP <PAL_STRING, FactoryObject<FactoryBase>*>::iterator it;
		it = mRegistry.begin();
		printf("Registry contents:\n");
		while (it!=mRegistry.end()) {
			printf("reg contains:%s\n",(*it).first.c_str());
			it++;
		}
		printf("finished displaying contents.\n");
	}
#endif
	void SetActiveGroup(PAL_STRING GroupName) {
#ifdef INTERNAL_DEBUG
		printf("PluggableFactory::SetActiveGroup: new group = %s, this = %p\n",
					GroupName.c_str(), this);
#endif
		mActiveGroup = GroupName;
		RebuildRegistry();
	}
	PAL_MAP <PAL_STRING, FactoryObject<FactoryBase>*> mRegistry; //the selected registry for this selected pluggable factory.
private:
	PAL_STRING mActiveGroup; //this needs to be private, to stop it being accssesed from non-group supporting factories
	static PAL_VECTOR<RegistrationInfo<FactoryBase> >* sInfoInstance;
};


template <typename FactoryBase> void PluggableFactory<FactoryBase>::UpdateRegistry(typename PAL_VECTOR<RegistrationInfo<FactoryBase> >::iterator Entry) {
	typename PAL_MAP <PAL_STRING, FactoryObject<FactoryBase>*>::iterator itr;

	//check if im part of the right group
	if ((mActiveGroup != Entry->mGroupName) && (Entry->mGroupName!="*")) {
#ifdef INTERNAL_DEBUG
		printf("skipping registration of %s (wrong group %s)\n",
					Entry->mClassName.c_str(), Entry->mGroupName.c_str());
#endif
		return; //we are not, do not register me
	}

	itr=mRegistry.find(Entry->mClassName);
	if (itr!=mRegistry.end() ) {
#ifdef INTERNAL_DEBUG
		printf("itr:%s\n",itr->first.c_str());
#endif
		//something else already exists, test for replacement
		typename PAL_VECTOR<RegistrationInfo<FactoryBase > >::iterator itv;
#ifdef INTERNAL_DEBUG
		printf("sInfo.size:%d\n",sInfo().size());
#endif
		for (itv = sInfo().begin();itv != sInfo().end(); itv++)  {
#ifdef INTERNAL_DEBUG
			printf("%s %p\n",itv->mUniqueName.c_str(),&itv);
#endif
			if (itr->second == itv->mConstructor) {
				//we have found the existing entry
				//lets compare it to me.
				if (Entry->mVersion > itv->mVersion) { //its a higher version lets replace it
					mRegistry.erase(itr); //erase the current entry
#ifdef INTERNAL_DEBUG
					printf("%s:%d: adding class %s to mRegistry with ctor %p\n",
								__FILE__, __LINE__,
								Entry->mClassName.c_str(), Entry->mConstructor);
#endif
					mRegistry.insert(std::make_pair(Entry->mClassName,Entry->mConstructor) ); //insert a new entry
				} 
				//okay were done, either replaced or not
				return;
			}
		}
		if (itv == PluggableFactory::sInfo().end() ) {
			//error! the constructor was not properly set!
			return;
		}
	} else {
#ifdef INTERNAL_DEBUG
		const char* name = Entry->mClassName.c_str();
		if (strcmp(name, "palPhysics") == 0) {
			printf("[got palPhysics!]\n");
		}
		printf("%s:%d: adding class %s as %s to mRegistry (%p) with ctor %p\n",
					__FILE__, __LINE__,
					Entry->mClassName.c_str(), Entry->mGroupName.c_str(),
					&mRegistry, Entry->mConstructor);
#endif
mRegistry.insert(std::make_pair(Entry->mClassName,Entry->mConstructor) );
	}
}


template <typename FactoryBase> void FactoryObject<FactoryBase>::Register(RegistrationInfo<FactoryBase> &RI, PAL_VECTOR<RegistrationInfo<FactoryBase> > &lsInfo) {
#ifdef INTERNAL_DEBUG
	printf("%s:%d: Trying to register %p in sInfo %p [un=%s v=%d]\n",__FILE__,__LINE__,this,&lsInfo,RI.mUniqueName.c_str(),RI.mVersion);
#endif
	typename PAL_VECTOR<RegistrationInfo<FactoryBase> >::iterator itv;
	itv=std::find(lsInfo.begin(),lsInfo.end(),RI);
	if (itv!=lsInfo.end()) {
#ifdef INTERNAL_DEBUG
		printf("%s:%d: Object %p already exists. [v=%d vs %d]\n",__FILE__,__LINE__,this,RI.mVersion,itv->mVersion);
#endif
		return; //this object already exists, dont add it.
	}
	lsInfo.push_back(RI);
}

template <typename FactoryBase> FactoryObject<FactoryBase> *PluggableFactory<FactoryBase>::newObject(PAL_STRING ClassName) {
	typename PAL_MAP <PAL_STRING, FactoryObject<FactoryBase>*>::iterator itr;
	itr=mRegistry.find(ClassName);
	if (itr == mRegistry.end()) {
#ifdef INTERNAL_DEBUG
		printf("%s:%d: Could not find '%s'! (mRegistry=%p)\n",__FILE__,__LINE__,ClassName.c_str(), &mRegistry);
#endif
		return NULL;
	}
	FactoryObject<FactoryBase>* maker = (*itr).second;
#ifdef INTERNAL_DEBUG
	printf("Found %s!\n",ClassName.c_str());
#endif
	return maker->Create();
}

template <typename FactoryBase> void PluggableFactory<FactoryBase>::RebuildRegistry() {
#ifdef INTERNAL_DEBUG
	printf("Rebuilding registry %p from sInfo %p (this=%p)\n", &mRegistry, &(sInfo()), this);
#endif
	mRegistry.clear();
	typename PAL_VECTOR<RegistrationInfo<FactoryBase> >::iterator itv;
	itv = sInfo().begin();
	for (;itv != sInfo().end(); ++itv) {
		UpdateRegistry(itv);				
	}
}

//Helper Macros:
//#define MAKEVERSION(major,minor,udpate) (((major)&255)<<16 | ((minor)&255) << 8 | ((update)&255))
typedef enum {
	FACTORY_REGISTER=1
} FactoryStaticRegisterVariable;

#endif
