#include "palFactory.h"
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL Factory -	Physics Abstraction Layer. 
						The factory required to create all objects
		Implementation
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.81: 05/07/08 - Notifications
		Version 0.8 : 06/06/04 
	TODO:
*/

#ifdef MEMDEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

palFactory *palFactory::GetInstance() {
	return (palFactory *)myFactory::GetInstance();
}
void palFactory::SetInstance(palFactory *pf) {
	myFactory::SetInstance(pf);
}

palFactory::palFactory() {
	m_active=NULL;
}

void palFactory::SelectEngine(PAL_STRING name) {
	SetActiveGroup(name);
	RebuildRegistry(); //lets just make sure the factory information is up to date.
}

void palFactory::Cleanup() {
	//FreeAll();
	/*while (!pMMO.empty() ) {
		delete *(pMMO.begin());
		//no need to erase() because the MMO takes care of it.
	}*/
	std::list<myFactoryBase *>::iterator it;
	
	//delete all items, except the physics class
	it=pMMO.begin();
	while (it != pMMO.end() ) {
		palPhysics * pPhysics = dynamic_cast<palPhysics *>(*it);
//		printf("\ntesting:%d\n",*it);
		if (pPhysics) {
//			printf("\n%d is a physics object\n",pPhysics);
			it++;
		} else {
				pPhysics = NULL;
				delete(*it);
				it=pMMO.begin();
		}
	}
	//now cleanup physics class, and delete it
	it=pMMO.begin();
	while (it != pMMO.end() ) {
		palPhysics * pPhysics = dynamic_cast<palPhysics *>(*it);
		if (pPhysics) {
			pPhysics->Cleanup();
		}
		delete(*it);
		it=pMMO.begin();
	}
//	MessageBox(NULL,"hi","hi",MB_OK);
	m_active=NULL;

	//delete this;
}

template <typename iType, typename fType> fType Cast(palFactoryObject *obj) {
#ifdef INTERNAL_DEBUG
	iType i = dynamic_cast<iType> (obj);
#ifndef NDEBUG
	printf("i:%d\n",i);
#endif
	fType f = dynamic_cast<fType> (i);
#ifndef NDEBUG
	printf("f:%d\n",f);
#endif
#else
	fType f = dynamic_cast<fType> (obj);
#endif
	return f;
}

palMaterials *palFactory::CreateMaterials() {
	//myFactoryObject *pmFO = Construct("palMaterials");
	//printf("%d\n",pmFO);
	if (!m_active) return NULL; //is there an active physics?
	if (m_active) { //there is
		if (m_active->m_pMaterials) //does it have a material?
			return m_active->m_pMaterials; //it does, return it
	}
	palFactoryObject *pmFO = CreateObject("palMaterials");
	palMaterials *pm=dynamic_cast<palMaterials *> (pmFO);
	if (m_active)
		if (m_active->m_pMaterials == 0)
			m_active->m_pMaterials=pm;
	return pm;
}

unsigned int palFactory::GetPALAPIVersion() {
	return PAL_SDK_VERSION_MAJOR << 16 | PAL_SDK_VERSION_MINOR << 8 | PAL_SDK_VERSION_BUGFIX;
}

palPhysics *palFactory::CreatePhysics() {
	m_active = 0;
//	myFactoryObject *pmFO = Construct("palPhysics");
//	printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palPhysics");
	#ifdef INTERNAL_DEBUG
	printf("%s:%d:CreateObject:%x\n",__FILE__,__LINE__,pmFO);
	#endif
	palPhysics *pp=static_cast<palPhysics *> (pmFO);
	#ifdef INTERNAL_DEBUG
	printf("%s:%d:palPhysics:%x\n",__FILE__,__LINE__,pp);
	#endif
	//for DLL usage
	if (pp)
		pp->SetFactoryInstance(this);
#ifndef NDEBUG
	printf("Physics:%d\n",pp);
#endif
	m_active=pp; //set active physics
	return pp;
}

palBox *palFactory::CreateBox() {
//	myFactoryObject *pmFO = Construct("palBox");
//	printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palBox");
	return Cast<palBody *,palBox *>(pmFO);
/*	palBody *pb=dynamic_cast<palBody *> (pmFO);
	printf("%d\n",pb);
	palBox *p = dynamic_cast<palBox *> (pb);
	printf("%d\n",p);
	// = (palBox *) mFO;
	//tpalBox<> *p=dynamic_cast<tpalBox<> *>(Construct("palBox"));
	return p;*/
}

palSphere *palFactory::CreateSphere() {
	//myFactoryObject *pmFO = Construct("palSphere");
	palFactoryObject *pmFO = CreateObject("palSphere");
	return Cast<palBody *,palSphere *>(pmFO);
/*	palBody *pb=dynamic_cast<palBody *> (pmFO);
	printf("%d\n",pb);
	palSphere *p = dynamic_cast<palSphere *> (pb);
	printf("%d\n",p);
	return p;*/
}

palCapsule *palFactory::CreateCapsule() {
	palFactoryObject *pmFO = CreateObject("palCapsule");
	return Cast<palBody *,palCapsule *>(pmFO);
}

palConvex *palFactory::CreateConvex() {
	palFactoryObject *pmFO = CreateObject("palConvex");
	return Cast<palBody *,palConvex *>(pmFO);
}

palCompoundBody *palFactory::CreateCompoundBody() {
	//myFactoryObject *pmFO = Construct("palCompoundBody");
	palFactoryObject *pmFO = CreateObject("palCompoundBody");
	return Cast<palBody *,palCompoundBody *>(pmFO);
}

palSphericalLink *palFactory::CreateSphericalLink() {
	//myFactoryObject *pmFO = Construct("palSphericalLink");
	//printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palSphericalLink");
/*	palLink *pl=dynamic_cast<palLink *> (pmFO);
	printf("%d\n",pl);
	palSphericalLink *p = dynamic_cast<palSphericalLink *> (pl);
	printf("%d\n",p);
	return p;*/
	return Cast<palLink *,palSphericalLink *>(pmFO);
}

palRevoluteLink *palFactory::CreateRevoluteLink() {
	//myFactoryObject *pmFO = Construct("palRevoluteLink");
	//printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palRevoluteLink");
	palLink *pl=dynamic_cast<palLink *> (pmFO);
	return Cast<palLink *,palRevoluteLink *>(pmFO);
}

palPrismaticLink *palFactory::CreatePrismaticLink() {
	//myFactoryObject *pmFO = Construct("palPrismaticLink");
	palFactoryObject *pmFO = CreateObject("palPrismaticLink");
	/*//printf("%d\n",pmFO);
	palLink *pl=dynamic_cast<palLink *> (pmFO);
	printf("%d\n",pl);
	palPrismaticLink *p = dynamic_cast<palPrismaticLink *> (pl);
	printf("%d\n",p);
	return p;*/
	return Cast<palLink *,palPrismaticLink *>(pmFO);
}

palTerrainPlane* palFactory::CreateTerrainPlane() {
	
	//myFactoryObject *pmFO = Construct("palTerrainPlane");
	//printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palTerrainPlane");
	/*palTerrain *pt=dynamic_cast<palTerrain *>(pmFO);
	printf("%d\n",pt);
	palTerrainPlane *p = dynamic_cast<palTerrainPlane *> (pt);
	printf("%d\n",p);
	return p;*/
	return Cast<palTerrain *,palTerrainPlane *>(pmFO);
}

palTerrainHeightmap* palFactory::CreateTerrainHeightmap() {
	palFactoryObject *pmFO = CreateObject("palTerrainHeightmap");
	return Cast<palTerrain *,palTerrainHeightmap *>(pmFO);
}

palTerrainMesh* palFactory::CreateTerrainMesh() {
	palFactoryObject *pmFO = CreateObject("palTerrainMesh");
	return Cast<palTerrain *,palTerrainMesh *>(pmFO);
}

palInclinometerSensor *palFactory::CreateInclinometerSensor() {
	//myFactoryObject *pmFO = Construct("palInclinometerSensor");
	//printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palInclinometerSensor");
	return Cast<palSensor *,palInclinometerSensor *>(pmFO);
	/*
	palSensor *ps = dynamic_cast<palSensor *>(pmFO);
	printf("%d\n",ps);
	palInclinometerSensor *p = dynamic_cast<palInclinometerSensor *> (ps);
	printf("%d\n",p);
	return p;*/
}

palPSDSensor *palFactory::CreatePSDSensor() {
	//myFactoryObject *pmFO = Construct("palPSDSensor");
	//printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palPSDSensor");
	return Cast<palSensor *,palPSDSensor *>(pmFO);
	/*palSensor *ps = dynamic_cast<palSensor *>(pmFO);
	printf("%d\n",ps);
	palPSDSensor *p = dynamic_cast<palPSDSensor *> (ps);
	printf("%d\n",p);
	return p;*/
}

palContactSensor *palFactory::CreateContactSensor() {
	//myFactoryObject *pmFO = Construct("palContactSensor");
	//printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palContactSensor");
	return Cast<palSensor *,palContactSensor *>(pmFO);
	/*palSensor *ps = dynamic_cast<palSensor *>(pmFO);
	printf("%d\n",ps);
	palContactSensor *p = dynamic_cast<palContactSensor *> (ps);
	printf("%d\n",p);
	return p;*/
}

palGyroscopeSensor *palFactory::CreateGyroscopeSensor() {
	palFactoryObject *pmFO = CreateObject("palGyroscopeSensor");
	return Cast<palSensor *,palGyroscopeSensor *>(pmFO);
/*	myFactoryObject *pmFO = Construct("palGyroscopeSensor");
	printf("%d\n",pmFO);
	palSensor *ps = dynamic_cast<palSensor *>(pmFO);
	printf("%d\n",ps);
	palGyroscopeSensor *p = dynamic_cast<palGyroscopeSensor *> (ps);
	printf("%d\n",p);
	return p;*/
}

palVelocimeterSensor *palFactory::CreateVelocimeterSensor() {
	palFactoryObject *pmFO = CreateObject("palVelocimeterSensor");
	return Cast<palSensor *,palVelocimeterSensor *>(pmFO);
/*	myFactoryObject *pmFO = Construct("palVelocimeterSensor");
	printf("%d\n",pmFO);
	palSensor *ps = dynamic_cast<palSensor *>(pmFO);
	printf("%d\n",ps);
	palVelocimeterSensor *p = dynamic_cast<palVelocimeterSensor *> (ps);
	printf("%d\n",p);
	return p;*/
}


palGPSSensor *palFactory::CreateGPSSensor() {
	palFactoryObject *pmFO = CreateObject("palGPSSensor");
	return Cast<palSensor *,palGPSSensor *>(pmFO);
}

palCompassSensor* palFactory::CreateCompassSensor() {
	palFactoryObject *pmFO = CreateObject("palCompassSensor");
	return Cast<palSensor *,palCompassSensor *>(pmFO);
}

palFactoryObject *palFactory::CreateObject(PAL_STRING name) {
	myFactoryObject *pmFO = Construct(name);
	#ifdef INTERNAL_DEBUG
	printf("%s:%d:Construct:%x\n",__FILE__,__LINE__,pmFO);
	#endif
	//printf("co:%d [%s]\n",pmFO,name.c_str());
	palFactoryObject *p = dynamic_cast<palFactoryObject *> (pmFO);
	#ifdef INTERNAL_DEBUG
	printf("%s:%d:palFactoryObject:%x\n",__FILE__,__LINE__,p);
	#endif
	//printf("m_active is: %d\n",m_active);
	if (p) {
		if (m_active) {
			p->SetParent(dynamic_cast<StatusObject *>(m_active));
			if (m_active->m_bListen) {
				palGeometry *pg = dynamic_cast<palGeometry *>(p);
				if (pg) {
					m_active->NotifyGeometryAdded(pg);
					return p;
				}
				palBodyBase *pb = dynamic_cast<palBodyBase *>(p);
				if (pb) {
					m_active->NotifyBodyAdded(pb);
					return p;
				}
			}
		}
		else
			p->SetParent(dynamic_cast<StatusObject *>(this));
	}
	return p;
}

palPhysics * palFactory::GetActivePhysics() {
	return m_active;
}

void palFactory::LoadPALfromDLL(char *szPath) {
	LoadObjects(szPath,PF,(void *)&PF->sInfo());
}
/*
#ifdef SINGLETON
palFactory* palFactory::m_pInstance=NULL;
#endif
*/
