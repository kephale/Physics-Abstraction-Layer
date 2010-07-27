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
  if (!m_pInstance)
    SetInstance(new palFactory());
  palFactory* result = (palFactory *)myFactory::GetInstance();
#ifdef INTERNAL_DEBUG
  printf("palFactory::GetInstance: instance = %p\n", result);
#endif
  return result;
}

void palFactory::SetInstance(palFactory *pf) {
	myFactory::SetInstance(pf);
}

palFactory::palFactory() {
	m_active=NULL;
}

void palFactory::SelectEngine(PAL_STRING name) {
#ifdef INTERNAL_DEBUG
  printf("palFactory::SelectEngine: this = %p\n", this);
#endif
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

	//delete all constraints first
	while (it != pMMO.end() ) {
		palLink *pLink = dynamic_cast<palLink *>(*it);
		if (pLink) {
			delete(*it);
			it=pMMO.begin();
		} else {
			it++;
		}
	}

	it=pMMO.begin();
	//now delete everything except the main physics class
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
#ifdef INTERNAL_DEBUG
  printf("palFactory::CreatePhysics: this = %p\n", this);
#endif
	m_active = 0;
//	myFactoryObject *pmFO = Construct("palPhysics");
//	printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palPhysics");
	#ifdef INTERNAL_DEBUG
	printf("%s:%d:CreateObject:%p\n",__FILE__,__LINE__,pmFO);
	#endif
	palPhysics *pp=static_cast<palPhysics *> (pmFO);
	#ifdef INTERNAL_DEBUG
	printf("%s:%d:palPhysics:%p\n",__FILE__,__LINE__,pp);
	#endif
	//for DLL usage
	if (pp)
		pp->SetFactoryInstance(this);
#ifndef NDEBUG
	printf("Physics:%p\n",pp);
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

palGenericBody *palFactory::CreateGenericBody() {
        palFactoryObject *pmFO = Construct("palGenericBody");
	return Cast<palBody *,palGenericBody *>(pmFO);
}

palGenericBody *palFactory::CreateGenericBody(palMatrix4x4& pos) {
  palGenericBody* body = CreateGenericBody();
  body->Init(pos);
  return body;
}

palStaticConvex *palFactory::CreateStaticConvex() {
   palFactoryObject *pmFO = CreateObject("palStaticConvex");
   return Cast<palBody *,palStaticConvex *>(pmFO);
}

palBoxGeometry *palFactory::CreateBoxGeometry() {
   palFactoryObject *pmFO = CreateObject("palBoxGeometry");
   return Cast<palGeometry *,palBoxGeometry *>(pmFO);
}

palSphereGeometry *palFactory::CreateSphereGeometry() {
   palFactoryObject *pmFO = CreateObject("palSphereGeometry");
   return Cast<palGeometry *,palSphereGeometry *>(pmFO);
}

palCapsuleGeometry *palFactory::CreateCapsuleGeometry() {
   palFactoryObject *pmFO = CreateObject("palCapsuleGeometry");
   return Cast<palGeometry *,palCapsuleGeometry *>(pmFO);
}

palConvexGeometry *palFactory::CreateConvexGeometry() {
   palFactoryObject *pmFO = CreateObject("palConvexGeometry");
   return Cast<palGeometry *,palConvexGeometry *>(pmFO);
}

palConvexGeometry *palFactory::CreateConvexGeometry(palMatrix4x4 &pos,
                                                    const Float *pVertices,
                                                    int nVertices, Float mass) {
  palConvexGeometry* geom = CreateConvexGeometry();
  geom->Init(pos, pVertices, nVertices, mass);
  return geom;
}

palConvexGeometry *palFactory::CreateConvexGeometry(palMatrix4x4 &pos,
                                                    const Float *pVertices,
                                                    int nVertices,
                                                    const int *pIndices,
                                                    int nIndices,
                                                    Float mass) {
  palConvexGeometry* geom = CreateConvexGeometry();
  geom->Init(pos, pVertices, nVertices, pIndices, nIndices, mass);
  return geom;
}
  
palConcaveGeometry *palFactory::CreateConcaveGeometry() {
   palFactoryObject *pmFO = CreateObject("palConcaveGeometry");
   return Cast<palGeometry *,palConcaveGeometry *>(pmFO);
}

palConcaveGeometry *palFactory::CreateConcaveGeometry(palMatrix4x4 &pos,
                                                      const Float *pVertices,
                                                      int nVertices,
                                                      const int *pIndices,
                                                      int nIndices, Float mass) {
  palConcaveGeometry* geom = CreateConcaveGeometry();
  geom->Init(pos, pVertices, nVertices, pIndices, nIndices, mass);
  return geom;
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

palSphericalLink *palFactory::CreateSphericalLink(palBodyBase *parent, palBodyBase *child,
												  Float x, Float y, Float z) {
	palSphericalLink* sphericalLink = CreateSphericalLink();
	sphericalLink->Init(parent, child, x, y, z);
	return sphericalLink;
}

palRevoluteLink *palFactory::CreateRevoluteLink() {
	//myFactoryObject *pmFO = Construct("palRevoluteLink");
	//printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palRevoluteLink");
	return Cast<palLink *,palRevoluteLink *>(pmFO);
}
palRevoluteLink *palFactory::CreateRevoluteLink(palBodyBase *parent, palBodyBase *child,
												Float x, Float y, Float z,
												Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteLink* link = CreateRevoluteLink();
	link->Init(parent, child, x, y, z, axis_x, axis_y, axis_z);
	return link;
}

palRevoluteSpringLink *palFactory::CreateRevoluteSpringLink() {
	//myFactoryObject *pmFO = Construct("palRevoluteLink");
	//printf("%d\n",pmFO);
	palFactoryObject *pmFO = CreateObject("palRevoluteSpringLink");
	return Cast<palLink *,palRevoluteSpringLink *>(pmFO);
}

palRevoluteSpringLink *palFactory::CreateRevoluteSpringLink(palBodyBase *parent, palBodyBase *child,
												Float x, Float y, Float z,
												Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteSpringLink* link = CreateRevoluteSpringLink();
	link->Init(parent, child, x, y, z, axis_x, axis_y, axis_z);
	return link;
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

palPrismaticLink *palFactory::CreatePrismaticLink(palBodyBase *parent, palBodyBase *child,
												  Float x, Float y, Float z,
												  Float axis_x, Float axis_y, Float axis_z) {
	palPrismaticLink* prismaticLink = CreatePrismaticLink();
	prismaticLink->Init(parent, child, x, y, z, axis_x, axis_y, axis_z);
	return prismaticLink;
}

palGenericLink *palFactory::CreateGenericLink() {
	palFactoryObject *pmFO = CreateObject("palGenericLink");
	return Cast<palLink *,palGenericLink *>(pmFO);
}

palGenericLink* palFactory::CreateGenericLink(palBodyBase *parent,
												palBodyBase *child,
												palMatrix4x4& parentFrame,
												palMatrix4x4& childFrame,
												palVector3 linearLowerLimits,
												palVector3 linearUpperLimits,
												palVector3 angularLowerLimits,
												palVector3 angularUpperLimits) {
	palGenericLink* link = CreateGenericLink();
	if (link) {
		link->Init(parent, child, parentFrame, childFrame, linearLowerLimits,
				linearUpperLimits, angularLowerLimits, angularUpperLimits);
	}
	return link;
}

palRigidLink *palFactory::CreateRigidLink() {
	palFactoryObject *pmFO = CreateObject("palRigidLink");
	return Cast<palLink *,palRigidLink *>(pmFO);
}

palRigidLink* palFactory::CreateRigidLink(palBodyBase *parent, palBodyBase *child) {
	palRigidLink* link = CreateRigidLink();
	if (link) {
		link->Init(parent, child);
	}
	return link;
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

palAngularMotor *palFactory::CreateAngularMotor(palRevoluteLink *pLink, Float Max)
{
	palFactoryObject *pmFO = CreateObject("palAngularMotor");
    palAngularMotor* angularMotor = Cast<palActuator*,palAngularMotor*>(pmFO);
    return angularMotor;
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
#ifdef INTERNAL_DEBUG
  printf("palFactory::GetActivePhysics: this = %p\n", this);
#endif
	return m_active;
}

void palFactory::LoadPALfromDLL(const char *szPath) {
#ifdef INTERNAL_DEBUG
  printf("palFactory::LoadPALfromDLL: path = '%s'. about to get palFactory\n",
	  szPath);
#endif
  palFactory* factory = PF;
#ifdef INTERNAL_DEBUG
  printf("palFactory::LoadPALfromDLL: factory = %p. about to get sInfo from method %p\n",
         factory, factory->sInfo);
#endif
  void* info = (void*) &(factory->sInfo());
#ifdef INTERNAL_DEBUG
  printf("palFactory::LoadPALfromDLL: this = %p, PF = %p, info = %p\n",
         this, PF, info);
  printf("palFactory::LoadPALfromDLL: about to call LoadObjects\n");
#endif
  LoadObjects(szPath,PF,info);
#ifdef INTERNAL_DEBUG
  printf("palFactory::LoadPALfromDLL: back from LoadObjects\n");
#endif
}
/*
#ifdef SINGLETON
palFactory* palFactory::m_pInstance=NULL;
#endif
*/
