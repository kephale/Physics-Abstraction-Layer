#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#include "newton_pal.h"
#include "newton_palVehicle.h"
#include "os.h"
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*
	Abstract:
		PAL - Physics Abstraction Layer. Newton implementation.
		This enables the use of newton via PAL.

		Implementation
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.52: 08/08/04 - fixed heightmap terrain bug
		Version 0.51: 11/07/04 - fixed update step rate
		Version 0.5 : 06/06/04 
	TODO:
		-get to 1.0 (ie: same as pal.h)
*/

NewtonWorld* g_nWorld = NULL;
float g_gravityX=0;
float g_gravityY=0;
float g_gravityZ=0;
NewtonBody* g_floorBody; 

NewtonWorld* palNewtonPhysics::GetNewtonWorld() {
	return g_nWorld;
}

FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
FACTORY_CLASS_IMPLEMENTATION(palNewtonPhysics);

FACTORY_CLASS_IMPLEMENTATION(palNewtonMaterialUnique);
FACTORY_CLASS_IMPLEMENTATION(palNewtonMaterialInteraction);

FACTORY_CLASS_IMPLEMENTATION(palNewtonBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palNewtonSphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palNewtonCylinderGeometry);
FACTORY_CLASS_IMPLEMENTATION(palNewtonConvexGeometry);

FACTORY_CLASS_IMPLEMENTATION(palNewtonConvex);
FACTORY_CLASS_IMPLEMENTATION(palNewtonBox);
FACTORY_CLASS_IMPLEMENTATION(palNewtonSphere);
FACTORY_CLASS_IMPLEMENTATION(palNewtonCylinder);
FACTORY_CLASS_IMPLEMENTATION(palNewtonCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palNewtonStaticBox);
FACTORY_CLASS_IMPLEMENTATION(palNewtonStaticCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palNewtonSphericalLink);
FACTORY_CLASS_IMPLEMENTATION(palNewtonRevoluteLink);
FACTORY_CLASS_IMPLEMENTATION(palNewtonPrismaticLink);


FACTORY_CLASS_IMPLEMENTATION(palNewtonOrientatedTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palNewtonTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palNewtonTerrainMesh);
FACTORY_CLASS_IMPLEMENTATION(palNewtonTerrainHeightmap);

FACTORY_CLASS_IMPLEMENTATION(palNewtonPSDSensor);
FACTORY_CLASS_IMPLEMENTATION(palNewtonContactSensor);

FACTORY_CLASS_IMPLEMENTATION(palNewtonCar);

FACTORY_CLASS_IMPLEMENTATION(palNewtonForceActuator);
FACTORY_CLASS_IMPLEMENTATION(palNewtonAngularMotor);
FACTORY_CLASS_IMPLEMENTATION_END_GROUP;
/*
palNewtonMaterial::palNewtonMaterial() {
};

void palNewtonMaterial::Init(Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterial::Init(static_friction,kinetic_friction,restitution);
	m_groupID = NewtonMaterialCreateGroupID(g_nWorld);
};
*/

NewtonBody* g_ContactBody0;
NewtonBody* g_ContactBody1;

PAL_MAP<NewtonBody*,palContact> g_ContactsData;


PAL_MAP<std::pair<palGroup,palGroup>,bool> g_Collideable;

int CDECL GenericContactBegin (const NewtonMaterial* material, const NewtonBody* body0, const NewtonBody* body1)
{
	g_ContactBody0 = const_cast<NewtonBody*>(body0);
	g_ContactBody1 = const_cast<NewtonBody*>(body1);
	palNewtonBodyData* d0=static_cast<palNewtonBodyData *>(NewtonBodyGetUserData(body0));
	palNewtonBodyData* d1=static_cast<palNewtonBodyData *>(NewtonBodyGetUserData(body1));
	
	//did we disable these colissions in our group?
	if ((d0)&&(d1)) {
		PAL_MAP <std::pair<palGroup,palGroup>,bool>::iterator itr;
		itr = g_Collideable.find(std::make_pair(d0->groupID,d1->groupID));
		if (itr!=g_Collideable.end()) {
			if (itr->second == false)
					return 0; //we don't want to process this.
		}
	}
	// return one the tell Newton the application wants to proccess this contact
	return 1;
}
// this callback is called for every contact between the two bodies
int CDECL GenericContactProcess (const NewtonMaterial* material, const NewtonContact* contact)
{
	float pos[3];
	float norm[3];
	NewtonMaterialGetContactPositionAndNormal(material, pos, norm);

	palContactPoint pcp;
	vec_set(&pcp.m_vContactPosition,pos[0],pos[1],pos[2]);
	vec_set(&pcp.m_vContactNormal,norm[0],norm[1],norm[2]);

	palNewtonBodyData* d0=static_cast<palNewtonBodyData *>(NewtonBodyGetUserData(g_ContactBody0));
	palNewtonBodyData* d1=static_cast<palNewtonBodyData *>(NewtonBodyGetUserData(g_ContactBody1));

	if (d0) 
		pcp.m_pBody1 = d0->pb;
	if (d1)
		pcp.m_pBody2 = d1->pb;
		

	PAL_MAP<NewtonBody*,palContact> ::iterator itr;
	itr=g_ContactsData.find(g_ContactBody0);
	if (itr!=g_ContactsData.end()) {
			(*itr).second.m_ContactPoints.push_back(pcp);
	}

	itr=g_ContactsData.find(g_ContactBody1);
	if (itr!=g_ContactsData.end()) {
		(*itr).second.m_ContactPoints.push_back(pcp);
	}


//	printf("contact at : %f %f %f\n",pos[0],pos[1],pos[2]);
	// return one to tell Newton we want to accept this contact
	return 1;
}

float pickedParam;
NewtonBody* pickedBody;

class NewtonRayData : public palRayHit {
public:
	float nRange;
	palVector3 nOrigin; 
	palVector3 nDirection; 
};

float CDECL RayCastPlacement (const NewtonBody* body, const float* normal, int collisionID, void* userData, float intersetParam)
{
	float range = 0;
	palVector3 dir;
	palVector3 pos;
	NewtonRayData *prh = static_cast<NewtonRayData *> (userData);
	if (prh) {
		range = prh->nRange;
		dir = prh->nDirection;
		pos = prh->nOrigin;
		prh->Clear();
		prh->m_bHit=false;
	}
	if (intersetParam <  pickedParam) {
		
		pickedParam = intersetParam;
		pickedBody = (NewtonBody*)body;

		if (prh) {
			palNewtonBodyData* d=static_cast<palNewtonBodyData *>(NewtonBodyGetUserData(pickedBody));
			
			prh->m_bHit=true;
			prh->m_pBody = d->pb;
			prh->m_fDistance = pickedParam*range;
			
			prh->m_bHitPosition = true;
			//hit pos = origin + dir*dist
			vec_mul(&dir,prh->m_fDistance);
			vec_add(&prh->m_vHitPosition,&pos,&dir);

			prh->m_bHitNormal = true;
			memcpy(prh->m_vHitNormal._vec,normal,sizeof(float)*3);
			
		}
	}
	return intersetParam;
}

unsigned CDECL newtonRaycastPreFilter(const NewtonBody *body, const NewtonCollision *collision, void* userData)
{
	return 1;
}

palNewtonMaterialUnique::palNewtonMaterialUnique() {
}

void palNewtonMaterialUnique::Init(PAL_STRING name,Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterialUnique::Init(name,static_friction,kinetic_friction,restitution);
	m_GroupID = NewtonMaterialCreateGroupID(g_nWorld);

	NewtonMaterialSetDefaultCollidable (g_nWorld, m_GroupID , m_GroupID , 1);
	NewtonMaterialSetDefaultElasticity (g_nWorld, m_GroupID , m_GroupID , restitution);
	NewtonMaterialSetDefaultFriction (  g_nWorld, m_GroupID , m_GroupID , static_friction, kinetic_friction);
	NewtonMaterialSetCollisionCallback (g_nWorld, m_GroupID , m_GroupID , NULL, GenericContactBegin , GenericContactProcess, NULL); 
}

palNewtonMaterialInteraction::palNewtonMaterialInteraction() {
}

void palNewtonMaterialInteraction::Init(palMaterialUnique *pM1, palMaterialUnique *pM2, Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterialInteraction::Init(pM1,pM2,static_friction,kinetic_friction,restitution);
	palNewtonMaterialUnique *pNM1 = dynamic_cast<palNewtonMaterialUnique *> (pM1);
	palNewtonMaterialUnique *pNM2 = dynamic_cast<palNewtonMaterialUnique *> (pM2);

	NewtonMaterialSetDefaultCollidable(g_nWorld, pNM1->m_GroupID , pNM2->m_GroupID , 1);
	NewtonMaterialSetDefaultElasticity(g_nWorld, pNM1->m_GroupID , pNM2->m_GroupID , restitution);
	NewtonMaterialSetDefaultFriction(  g_nWorld, pNM1->m_GroupID , pNM2->m_GroupID , static_friction, kinetic_friction);
	NewtonMaterialSetCollisionCallback(g_nWorld, pNM1->m_GroupID , pNM2->m_GroupID , NULL, GenericContactBegin, GenericContactProcess, NULL); 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
	float fluidDensity;
	float fluidLinearViscosity;
	float fluidAngularViscosity;
	float plane[4];
} palNewtonWaterData;

bool g_water=false;
palNewtonWaterData g_waterdata;

palNewtonPhysics::palNewtonPhysics() {
};

void palNewtonPhysics::InitWater(Float fluidDensity, Float fluidLinearViscosity, Float fluidAngularViscosity, Float plane_a, Float plane_b, Float plane_c, Float plane_d) {
	g_water=true;
	g_waterdata.fluidDensity=fluidDensity;
	g_waterdata.fluidLinearViscosity=fluidLinearViscosity;
	g_waterdata.fluidAngularViscosity=fluidAngularViscosity;
	g_waterdata.plane[0]=plane_a;
	g_waterdata.plane[1]=plane_b;
	g_waterdata.plane[2]=plane_c;
	g_waterdata.plane[3]=plane_d;
}

int CDECL mygetbuoyancyplane(const int collisionID, void *context, const float* globalSpaceMatrix, float* globalSpacePlane) {
	//printf("%d %d %d\n",context,globalSpaceMatrix,globalSpacePlane);
/*	globalSpacePlane[0]=0;
	globalSpacePlane[1]=1;
	globalSpacePlane[2]=0;
	globalSpacePlane[3]=0;*/
	memcpy(globalSpacePlane,g_waterdata.plane,sizeof(float)*4);
	return 0;
}
//	typedef int (*NewtonGetBuoyancyPlane) (const int collisionID, void *context, const dFloat* globalSpaceMatrix, dFloat* globalSpacePlane);

// set the tranformation of a rigid body
// is this absolute ^$$%^ or WHAT!
void CDECL PhysicsApplyForceAndTorque (const NewtonBody* body)
{
	float mass;
	float Ixx;
	float Iyy;
	float Izz;

	NewtonBodyGetMassMatrix (body, &mass, &Ixx, &Iyy, &Izz);
	//dVector force (mass * NewtonPhysics::m_Gravity/*0.0f, -mass * 9.8f, 0.0f*/);
	float gravity[3];
/*	gravity[0]=g_gravityX;
	gravity[1]=g_gravityY;
	gravity[2]=g_gravityZ;*/

	gravity[0]=mass*g_gravityX;
	gravity[1]=mass*g_gravityY;
	gravity[2]=mass*g_gravityZ;


	palNewtonBodyData* data=(palNewtonBodyData *) NewtonBodyGetUserData(body);
	if (data->set_force)
		NewtonBodySetForce (body, data->force);
	else
		NewtonBodySetForce (body, gravity);
	if (data->set_torque)
		NewtonBodySetTorque(body, data->torque);
	data->set_force=false;
	data->set_torque=false;

	if (data->add_force) 
		NewtonBodyAddForce(body, data->aforce);
	if (data->add_torque)
		NewtonBodyAddTorque(body, data->atorque);
	data->add_force=false;
	data->add_torque=false;
	memset(data->aforce,0,sizeof(float)*3);
	memset(data->atorque,0,sizeof(float)*3);

	//NewtonBodyAddBuoyancyForce(body,
/*
	if (g_water)
		NewtonBodyAddBuoyancyForce(body,
		g_waterdata.fluidDensity,
		g_waterdata.fluidLinearViscosity,
		g_waterdata.fluidAngularViscosity,
		gravity,
		mygetbuoyancyplane,
	//	NewtonGetBuoyancyPlane((float *)g_waterdata.plane),
	//	(float *)g_waterdata.plane,
	//	NewtonGetBuoyancyPlane
		NULL);

*/
/*	//NewtonGetBuoyancyPlane
//typedef void (_cdecl *NewtonGetBuoyancyPlane) (void *context, const float* globalSpaceMatrix, float* globalSpacePlane);

		*/
}

const char* palNewtonPhysics::GetVersion(){
	static char verbuf[256];
	int v=0;
	if (g_nWorld)
		v=NewtonWorldGetVersion(g_nWorld);
	else
	{
		sprintf(verbuf,"Newton:Error requesting version");
		return verbuf;
	}
	sprintf(verbuf,"Newton V %d.%2d",v/100,v%100);
	return verbuf;
}

void palNewtonPhysics::SetGroupCollision(palGroup a, palGroup b, bool enabled) {
	g_Collideable.insert(std::make_pair(std::make_pair(a,b),enabled));
}

void palNewtonPhysics::Init(Float gravity_x, Float gravity_y, Float gravity_z) {
	palPhysics::Init(gravity_x, gravity_y, gravity_z); //set member variables
	g_gravityX = gravity_x;
	g_gravityY = gravity_y;
	g_gravityZ = gravity_z;
	g_nWorld = NewtonCreate (NULL, NULL);

	float worldSizeMin[3] = {-1500, -500, -1500}; 
	float worldSizeMax[3] = {1500, 500, 1500}; 
	NewtonSetWorldSize(g_nWorld, worldSizeMin, worldSizeMax);

	//set the default collision callback for the contact sensor
	int defaultgroup = NewtonMaterialGetDefaultGroupID(g_nWorld);
	NewtonMaterialSetCollisionCallback (g_nWorld, defaultgroup , defaultgroup , NULL, GenericContactBegin, GenericContactProcess, NULL); 
};


void palNewtonPhysics::SetCollisionAccuracy(Float fAccuracy) {
	//TODO
}
	
void palNewtonPhysics::RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range, palRayHit& hit) {
	float pos0[3];
	float pos1[3];
	
	pos0[0] = x;
	pos0[1] = y;
	pos0[2] = z;

	pos1[0]=pos0[0]+dx*range;
	pos1[1]=pos0[1]+dy*range;
	pos1[2]=pos0[2]+dz*range;
	
	NewtonRayData nrdhit;

	nrdhit.nRange = range;
	vec_set(&nrdhit.nDirection,dx,dy,dz);
	vec_set(&nrdhit.nOrigin,x,y,z);

	pickedParam = 1.2f; //need this for newton filtering multiple hits

	NewtonWorldRayCast(g_nWorld,pos0,pos1,RayCastPlacement,&nrdhit,newtonRaycastPreFilter);

	hit = nrdhit;
/*	hit.m_bHit = nrdhit.m_bHit;
	hit.m_bHitPosition = nrdhit.m_bHitPosition;
	hit.m_vHitPosition = nrdhit.m_vHitPosition;*/
}

void palNewtonPhysics::NotifyCollision(palBodyBase *a, palBodyBase *b, bool enabled) {
	NotifyCollision(a,enabled);
	NotifyCollision(b,enabled);
}

void palNewtonPhysics::NotifyCollision(palBodyBase *pBody, bool enabled) {
	//PAL_MAP<NewtonBody*,palContact> g_ContactsData;
	PAL_MAP<NewtonBody*, palContact> ::iterator itr;

	palNewtonBody *pb = dynamic_cast<palNewtonBody *>(pBody);
	itr=g_ContactsData.find(pb->m_pntnBody);
	
	if (itr!=g_ContactsData.end()) {
		if (enabled)		
			return;//already listening to this contact & we want to listen
		//else 
			//g_ContactsData.erase(itr); //otherwise remove the listening?...
	} else {
		if (enabled)  {
			palContact pc;
			g_ContactsData.insert(std::make_pair(pb->m_pntnBody,pc));
		}
	}

}

void palNewtonPhysics::GetContacts(palBodyBase *pBody, palContact& contact) {
}

void palNewtonPhysics::GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact) {
}

/*
void palNewtonPhysics::SetDefaultMaterial(palMaterial *pmat) {
	palPhysics::SetDefaultMaterial(pmat);
	if (pmat) {
	int defaultID;
	defaultID = NewtonMaterialGetDefaultGroupID (g_nWorld);
	NewtonMaterialSetDefaultElasticity (g_nWorld, defaultID, defaultID, pmat->m_fRestitution);
	//NewtonMaterialSetDefaultCollidable (g_nWorld, defaultID, defaultID, 1);
	NewtonMaterialSetDefaultFriction (g_nWorld, defaultID, defaultID, pmat->m_fStatic, pmat->m_fKinetic);
	}
}
*/
/*
void palNewtonPhysics::SetGroundPlane(bool enabled, Float size) {
	if (enabled) {
	NewtonCollision* collision;

//	BoxPrimitive* floor;
	NewtonBody* floorBody; 

	// create the the floor collision, and body with default values
	collision = NewtonCreateBox (g_nWorld, size, 0.1f, size, NULL); 
	floorBody = NewtonCreateBody (g_nWorld, collision);
	NewtonReleaseCollision (g_nWorld, collision);

	palMatrix4x4 matrix;
	memset(&matrix,0,sizeof(palMatrix4x4));
	matrix._11=1;
	matrix._22=1;
	matrix._33=1;
	matrix._41=0;
	matrix._42=-0.05f;
	matrix._43=0;
	matrix._44=1;

	// set the transformation for this rigid body
	NewtonBodySetMatrix (floorBody, matrix._mat);
	
	// set a destrutor for this rigid body
	//NewtonBodySetDestructorCallback (floorBody, PhysicsBodyDestructor);
	}
}
*/
void palNewtonPhysics::Cleanup() {
	NewtonDestroyAllBodies(g_nWorld);
}

void palNewtonPhysics::Iterate(Float timestep) {

	//clear out old contact points.
	PAL_MAP<NewtonBody*, palContact> ::iterator itr;
	itr = g_ContactsData.begin();
	while (itr!=g_ContactsData.end()) {
		itr->second.m_ContactPoints.clear();
		itr++;
	}

	NewtonSetMinimumFrameRate(g_nWorld,1.0f/timestep);
	NewtonUpdate(g_nWorld,timestep);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void palNewtonBody::SetGroup(palGroup group) {
	m_callbackdata.groupID=group;
}
palNewtonBody::palNewtonBody() {
	m_pntnBody = NULL;
	memset(&m_callbackdata,0,sizeof(m_callbackdata));
	m_callbackdata.set_force=false;
	m_callbackdata.set_torque=false;
	m_callbackdata.add_force=false;
	m_callbackdata.add_torque=false;
	m_callbackdata.pb = this;
	static_body = false;
}

palNewtonBody::~palNewtonBody() {
	if (m_pntnBody) {
		/*while (!m_Geometries.empty()) {
			m_Geometries.erase(m_Geometries.begin());
		}*/
		Cleanup();
		NewtonDestroyBody(g_nWorld,m_pntnBody);
	}
}

/*
void palNewtonBody::SetPosition(Float x, Float y, Float z) {
	if (m_pntnBody) {
		// set the trasformation matrix
		palMatrix4x4 matrix;
		memset(matrix._mat,0,sizeof(palMatrix4x4));
		matrix._11=1;
		matrix._22=1;
		matrix._33=1;
		matrix._41=x;
		matrix._42=y;
		matrix._43=z;
		matrix._44=1;
		NewtonBodySetMatrix (m_pntnBody, matrix._mat);
	}
}*/

void palNewtonBody::SetPosition(palMatrix4x4 &location) {
	if (m_pntnBody) {
		NewtonBodySetMatrix (m_pntnBody, location._mat);
	}
	palBody::SetPosition(location);
}

//extern void mat_multiply(palMatrix4x4 *m, const palMatrix4x4 *a, const palMatrix4x4 *b);
//extern void mat_rotate(palMatrix4x4 *m, Float angle, Float x, Float y, Float z);

void palNewtonBody::SetActive(bool active) {
	if (active)
		NewtonWorldUnfreezeBody(g_nWorld,m_pntnBody);
	else
		NewtonWorldFreezeBody(g_nWorld,m_pntnBody);
	
}

#if 0
void palNewtonBody::SetForce(Float fx, Float fy, Float fz) {
	palBody::SetForce(fx,fy,fz);
	//float force[3];
	//force[0]=fx;
	//force[1]=fy;
	//force[2]=fz;
	//NewtonBodySetForce(m_pntnBody,force);
	m_callbackdata.set_force=true;
	m_callbackdata.force[0]=fx;
	m_callbackdata.force[1]=fy;
	m_callbackdata.force[2]=fz;
}

void palNewtonBody::GetForce(palVector3& force) {
	NewtonBodyGetForce(m_pntnBody,force._vec);
}

void palNewtonBody::AddForce(Float fx, Float fy, Float fz) {
	m_callbackdata.add_force=true;
	m_callbackdata.aforce[0]+=fx;
	m_callbackdata.aforce[1]+=fy;
	m_callbackdata.aforce[2]+=fz;
}

void palNewtonBody::AddTorque(Float tx, Float ty, Float tz) {
	m_callbackdata.add_torque=true;
	m_callbackdata.atorque[0]+=tx;
	m_callbackdata.atorque[1]+=ty;
	m_callbackdata.atorque[2]+=tz;
}

void palNewtonBody::SetTorque(Float tx, Float ty, Float tz) {
	palBody::SetTorque(tx,ty,tz);
//	float torque[3];
//	torque[0]=tx;
//	torque[1]=ty;
//	torque[2]=tz;
//	NewtonBodySetTorque(m_pntnBody,torque);
	m_callbackdata.set_torque=true;
	m_callbackdata.torque[0]=tx;
	m_callbackdata.torque[1]=ty;
	m_callbackdata.torque[2]=tz;
}
void palNewtonBody::GetTorque(palVector3& torque) {
	NewtonBodyGetTorque(m_pntnBody,torque._vec);
}


void palNewtonBody::ApplyImpulse(Float fx, Float fy, Float fz) {
	palVector3 center;
	palVector3 imp;
	GetPosition(center);
	imp.x=fx;
	imp.y=fy;
	imp.z=fz;
	NewtonAddBodyImpulse(m_pntnBody, imp._vec, center._vec); 
	//NEWTON_API void NewtonAddBodyImpulse (const NewtonBody* body, const float* pointDeltaVeloc, const float* pointPosit);
}


void palNewtonBody::ApplyAngularImpulse(Float fx, Float fy, Float fz) {
	palVector3 omega;
	NewtonBodyGetOmega(m_pntnBody,omega._vec);
	omega.x+=fx/m_fMass;
	omega.y+=fy/m_fMass;
	omega.z+=fz/m_fMass;
	//p=mv
	NewtonBodySetOmega(m_pntnBody,omega._vec);
}

#endif


void palNewtonBody::ApplyForce(Float fx, Float fy, Float fz) {
	m_callbackdata.add_force=true;
	m_callbackdata.aforce[0]+=fx;
	m_callbackdata.aforce[1]+=fy;
	m_callbackdata.aforce[2]+=fz;
}

void palNewtonBody::ApplyTorque(Float tx, Float ty, Float tz) {
	m_callbackdata.add_torque=true;
	m_callbackdata.atorque[0]+=tx;
	m_callbackdata.atorque[1]+=ty;
	m_callbackdata.atorque[2]+=tz;
}


void palNewtonBody::GetLinearVelocity(palVector3& velocity) {
	NewtonBodyGetVelocity(m_pntnBody,velocity._vec);
}
void palNewtonBody::GetAngularVelocity(palVector3& velocity_rad) {
	NewtonBodyGetOmega(m_pntnBody,velocity_rad._vec);
}


void palNewtonBody::SetLinearVelocity(palVector3 velocity) {
	NewtonBodySetVelocity(m_pntnBody,velocity._vec);
}
void palNewtonBody::SetAngularVelocity(palVector3 velocity_rad) {
	NewtonBodySetOmega(m_pntnBody,velocity_rad._vec);
}

void palNewtonBody::SetMaterial(palMaterial *material) {
	palNewtonMaterialUnique * pnmU = dynamic_cast<palNewtonMaterialUnique *> (material);
	NewtonBodySetMaterialGroupID(m_pntnBody,pnmU->m_GroupID);
	palBody::SetMaterial(material);
}

palMatrix4x4& palNewtonBody::GetLocationMatrix() {
	if (m_pntnBody) {
	palMatrix4x4 m;
	NewtonBodyGetMatrix(m_pntnBody,m._mat );
/*	memset(m_mLoc._mat,0,sizeof(palMatrix4x4));
	m_mLoc._11 = 1;
	m_mLoc._22 = 1;
	m_mLoc._33 = 1;
	m_mLoc._44 = 1;
	m_mLoc._41 = m._41;//m._mat[0];
	m_mLoc._42 = m._42;//m._mat[4+1];
	m_mLoc._43 = m._43;//m._mat[2*4+2];

	for (int x=0;x<4;x++) {
	printf("%f %f %f %f\n",m._mat[x*4+0],m._mat[x*4+1],m._mat[x*4+2],m._mat[x*4+3]);
	}
	printf("\n%f %f %f\n",m._41,m._42,m._43);//m._mat[0],m._mat[4+1],m._mat[2*4+2]);
	printf("\n");*/
	memcpy(m_mLoc._mat,m._mat,sizeof(palMatrix4x4));

	/*
		for (int x=0;x<4;x++) {
			m_mLoc._mat[x+3*4]=m._mat[x*4+3];
		}*/
	}
	return m_mLoc;
}
/*

palMatrix4x4& palNewtonCylinder::GetLocationMatrix() {
	if (m_pntnBody) {
	palMatrix4x4 m;
	NewtonBodyGetMatrix(m_pntnBody,m._mat );

	Float x=m._41;
	Float y=m._42;
	Float z=m._43;
	m._41=0;
	m._42=0;
	m._43=0;
	
	palMatrix4x4 rot;
	mat_identity(&rot);
	mat_rotate(&rot,-90,0,0,1);
		
	mat_multiply(&m_mLoc,&rot,&m);
	m_mLoc._41=x;
	m_mLoc._42=y;
	m_mLoc._43=z;
	//memcpy(m_mLoc._mat,m._mat,sizeof(palMatrix4x4));
	}
	return m_mLoc;
}

void palNewtonCylinder::SetPosition(palMatrix4x4& location) {
	if (m_pntnBody) {
		Float x=location._41;
		Float y=location._42;
		Float z=location._43;
		location._41=0;
		location._42=0;
		location._43=0;
		palMatrix4x4 rot,out;

		mat_identity(&rot);
		mat_rotate(&rot,90,0,0,1);
		
		mat_multiply(&out,&rot,&location);
		out._41=x;
		out._42=y;
		out._43=z;
		NewtonBodySetMatrix (m_pntnBody, out._mat);
	}
	palBody::SetPosition(location);
}

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
palMatrix4x4& palNewtonGeometry::GetLocationMatrix() {

	return m_mLoc; //wrong code!
}*/

palNewtonGeometry::~palNewtonGeometry() {
	if (m_pntnCollision)
		NewtonReleaseCollision(g_nWorld,m_pntnCollision);
}

palNewtonBoxGeometry::palNewtonBoxGeometry() {
}

void palNewtonBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	//convert to relative positioning
	palNewtonBody* pnb = dynamic_cast<palNewtonBody *>(m_pBody);
	if (!pnb->static_body) {
		palVector3 bpos;
		m_pBody->GetPosition(bpos);
		pos._41 -= bpos.x;
		pos._42 -= bpos.y;
		pos._43 -= bpos.z;
	}
	//m_pntnCollision = NewtonCreateBox (g_nWorld, m_fWidth, m_fHeight, m_fDepth, NULL);  //center offset specified in NULL
	m_pntnCollision = NewtonCreateBox (g_nWorld, m_fWidth, m_fHeight, m_fDepth, pos._mat);  //center offset specified in NULL
}

palNewtonSphereGeometry::palNewtonSphereGeometry() {
}

void palNewtonSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
	//convert to relative positioning
	palVector3 bpos;
	m_pBody->GetPosition(bpos);
	pos._41 -= bpos.x;
	pos._42 -= bpos.y;
	pos._43 -= bpos.z;
	m_pntnCollision = NewtonCreateSphere  (g_nWorld, m_fRadius, m_fRadius, m_fRadius, pos._mat);  
}

palNewtonCylinderGeometry::palNewtonCylinderGeometry() {
}

void palNewtonCylinderGeometry::Init(palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	palCapsuleGeometry::Init(pos,radius,length,mass);
	//convert to relative positioning
	palVector3 bpos;
	m_pBody->GetPosition(bpos);
	pos._41 -= bpos.x;
	pos._42 -= bpos.y;
	pos._43 -= bpos.z;
//	m_pntnCollision = NewtonCreateSphere  (g_nWorld, m_fRadius, m_fRadius, m_fRadius, pos._mat);  

	palMatrix4x4 rot,out;
	mat_identity(&rot);
	mat_rotate(&rot,90,0,0,1);

	mat_multiply(&out,&rot,&pos);
	m_pntnCollision = NewtonCreateCapsule(g_nWorld, m_fRadius, m_fLength+m_fRadius*2, out._mat );  
}

palNewtonConvexGeometry::palNewtonConvexGeometry() {
}


void palNewtonConvexGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	palConvexGeometry::Init(pos,pVertices,nVertices,mass);
	#pragma message("todo: pos set in convex geom")
	m_pntnCollision = NewtonCreateConvexHull(g_nWorld,nVertices,pVertices,sizeof(Float)*3,NULL);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void palNewtonBody::BuildBody(Float x, Float y, Float z) {
	palNewtonGeometry *png=dynamic_cast<palNewtonGeometry *> (m_Geometries[0]);
	// create the ridid body
	m_pntnBody = NewtonCreateBody (g_nWorld, png->m_pntnCollision);
	palBody::SetPosition(x,y,z);
	palBody::SetOrientation(0,0,0);
//	SetMass(mass);
	NewtonBodySetForceAndTorqueCallback (m_pntnBody, PhysicsApplyForceAndTorque);
	NewtonBodySetUserData(m_pntnBody, &m_callbackdata); //set the user data pointer to the callback data structure
	// Get Rid of the collision
	NewtonReleaseCollision (g_nWorld, png->m_pntnCollision);
}

palNewtonConvex::palNewtonConvex() {
}

void palNewtonConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	BuildBody(x,y,z);

	m_fMass = mass;
	palNewtonConvexGeometry *png=dynamic_cast<palNewtonConvexGeometry *> (m_Geometries[0]);
	png->SetMass(mass);
	
	NewtonBodySetMassMatrix (m_pntnBody, m_fMass, png->m_fInertiaXX, png->m_fInertiaYY, png->m_fInertiaZZ);

}

palNewtonStaticBox::palNewtonStaticBox() {
	static_body = true;
}

void palNewtonStaticBox::Init(palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palStaticBox::Init(pos,width,height,depth); //create geom

	palNewtonGeometry *png=dynamic_cast<palNewtonGeometry *> (m_Geometries[0]);
	// create the rigid body
	NewtonBody *m_pntnBody = 0;
	m_pntnBody = NewtonCreateBody (g_nWorld, png->m_pntnCollision);

	//SetPosition(pos);
//	SetMass(mass);
//	NewtonBodySetForceAndTorqueCallback (m_pntnBody, PhysicsApplyForceAndTorque);
//	NewtonBodySetUserData(m_pntnBody, &m_callbackdata); //set the user data pointer to the callback data structure
	// Get Rid of the collision
	NewtonReleaseCollision (g_nWorld, png->m_pntnCollision);
}

palNewtonBox::palNewtonBox() {

}


void palNewtonBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
		
	//NewtonCollision* collision;
	// create the collsion shape
	//collision = NewtonCreateBox (g_nWorld, m_fWidth, m_fHeight, m_fDepth, NULL);  //center offset specified in NULL
//	palNewtonBoxGeometry *png=dynamic_cast<palNewtonBoxGeometry *> (m_Geometries[0]);

	BuildBody(x,y,z);
	SetMass(mass);
/*	// create the ridid body
	m_pntnBody = NewtonCreateBody (g_nWorld, png->m_pntnCollision);
	palBody::SetPosition(x,y,z);
	palBody::SetOrientation(0,0,0);


	NewtonBodySetForceAndTorqueCallback (m_pntnBody, PhysicsApplyForceAndTorque);
	NewtonBodySetUserData(m_pntnBody, &m_callbackdata); //set the user data pointer to the callback data structure

	// Get Rid of the collision
	NewtonReleaseCollision (g_nWorld, png->m_pntnCollision);*/
}

void palNewtonBox::SetMass(Float fMass) {
	m_fMass = fMass;
	/*
	float mass = fMass; 
	float x = 1.5;
	float y=0.3;
	float z=0.3;
	float Ixx = 0.7f * mass * (y * y + z * z) / 12.0f; 
	float Iyy = 0.7f * mass * (x * x + z * z) / 12.0f; 
	float Izz = 0.7f * mass * (x * x + y * y) / 12.0f; 
	NewtonBodySetMassMatrix (m_pntnBody, m_fMass, Ixx,Iyy,Izz);
	*/
	palNewtonBoxGeometry *png=dynamic_cast<palNewtonBoxGeometry *> (m_Geometries[0]);
	png->SetMass(fMass);
	
	NewtonBodySetMassMatrix (m_pntnBody, m_fMass, png->m_fInertiaXX, png->m_fInertiaYY, png->m_fInertiaZZ);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palNewtonSphere::palNewtonSphere() {
}

void palNewtonSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
//	NewtonCollision* collision;
//	// create the collsion shape
//	collision = NewtonCreateSphere  (g_nWorld, m_fRadius, NULL);  //center offset specified in NULL
/*	palNewtonSphereGeometry *png=dynamic_cast<palNewtonSphereGeometry *> (m_Geometries[0]);
	// create the ridid body
	m_pntnBody = NewtonCreateBody (g_nWorld, png->m_pntnCollision);
	// Get Rid of the collision
	NewtonReleaseCollision (g_nWorld, png->m_pntnCollision);
	palBody::SetPosition(x,y,z);
	palBody::SetOrientation(0,0,0);
	SetMass(mass);

	NewtonBodySetForceAndTorqueCallback (m_pntnBody, PhysicsApplyForceAndTorque);
	NewtonBodySetUserData(m_pntnBody, &m_callbackdata); //set the user data pointer to the callback data structure
*/
	BuildBody(x,y,z);
	SetMass(mass);
}

void palNewtonSphere::SetMass(Float mass) {
	m_fMass = mass;
	//float i = 2 * m_fRadius *  m_fRadius / 5.0f;
	palNewtonSphereGeometry *png=dynamic_cast<palNewtonSphereGeometry *> (m_Geometries[0]);
	png->SetMass(mass);

	NewtonBodySetMassMatrix (m_pntnBody, m_fMass, png->m_fInertiaXX, png->m_fInertiaYY, png->m_fInertiaZZ);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palNewtonCylinder::palNewtonCylinder() {
}

void palNewtonCylinder::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	palCapsule::Init(x,y,z,radius,length,mass);
	BuildBody(x,y,z);
	SetMass(mass);
	/*
	palNewtonCylinderGeometry *png=dynamic_cast<palNewtonCylinderGeometry *> (m_Geometries[0]);

//	NewtonCollision* collision;
	// create the collsion shape
//	collision = NewtonCreateCapsule(g_nWorld, m_fRadius, m_fLength, NULL);  //center offset specified in NULL
	// create the ridid body
	m_pntnBody = NewtonCreateBody (g_nWorld, png->m_pntnCollision);
	// Get Rid of the collision
	NewtonReleaseCollision (g_nWorld, png->m_pntnCollision);
	palBody::SetPosition(x,y,z);
	palBody::SetOrientation(0,0,0);
	SetMass(mass);

	NewtonBodySetForceAndTorqueCallback (m_pntnBody, PhysicsApplyForceAndTorque);
	NewtonBodySetUserData(m_pntnBody, &m_callbackdata); //set the user data pointer to the callback data structure
	*/
}

void palNewtonCylinder::SetMass(Float mass) {
	m_fMass = mass;
/*	float d = m_fRadius*2;
	float i0 = 1/48.0f * (3 * d * d + 4 * m_fLength * m_fLength);
	float i2 = d * d / 8.0f;*/
	palNewtonCylinderGeometry *png=dynamic_cast<palNewtonCylinderGeometry *> (m_Geometries[0]);
	png->SetMass(mass);
	NewtonBodySetMassMatrix (m_pntnBody, m_fMass, png->m_fInertiaXX, png->m_fInertiaYY, png->m_fInertiaZZ);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palNewtonStaticCompoundBody::palNewtonStaticCompoundBody() {
	static_body = true;
}

void palNewtonStaticCompoundBody::Finalize() {
	NewtonCollision **array = new NewtonCollision * [m_Geometries.size()];
	for (unsigned int i=0;i<m_Geometries.size();i++) {
		palNewtonGeometry *png=dynamic_cast<palNewtonGeometry *> (m_Geometries[i]);
		array[i]=png->m_pntnCollision;
	}
	NewtonCollision* collision;
	collision=NewtonCreateCompoundCollision(g_nWorld,(int)m_Geometries.size(),array);

	// create the ridid body
	m_pntnBody = NewtonCreateBody (g_nWorld, collision);
	NewtonBodySetUserData(m_pntnBody, &m_callbackdata); //set the user data pointer to the callback data structure
	// Get Rid of the collision
	NewtonReleaseCollision (g_nWorld, collision);

	delete [] array;
}

palNewtonCompoundBody::palNewtonCompoundBody() {
}

void palNewtonCompoundBody::Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ) {
	NewtonCollision **array = new NewtonCollision * [m_Geometries.size()];
	for (unsigned int i=0;i<m_Geometries.size();i++) {
		palNewtonGeometry *png=dynamic_cast<palNewtonGeometry *> (m_Geometries[i]);
		array[i]=png->m_pntnCollision;
	}
	NewtonCollision* collision;
	collision=NewtonCreateCompoundCollision(g_nWorld,(int)m_Geometries.size(),array);

	// create the ridid body
	m_pntnBody = NewtonCreateBody (g_nWorld, collision);
	// Get Rid of the collision
	NewtonReleaseCollision (g_nWorld, collision);

	palBody::SetPosition(m_fPosX,m_fPosY,m_fPosZ);
	palBody::SetOrientation(0,0,0);
	
	NewtonBodySetMassMatrix (m_pntnBody, finalMass, iXX, iYY, iZZ);

	NewtonBodySetForceAndTorqueCallback (m_pntnBody, PhysicsApplyForceAndTorque);
	NewtonBodySetUserData(m_pntnBody, &m_callbackdata); //set the user data pointer to the callback data structure

	delete [] array;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palNewtonLink::palNewtonLink() {
	m_pntnJoint = NULL;
	memset(&m_callbackdata,0,sizeof(m_callbackdata));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palNewtonSphericalLink::palNewtonSphericalLink() {
};

void palNewtonSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	palSphericalLink::Init(parent,child,x,y,z);
	palNewtonBody *body0 = dynamic_cast<palNewtonBody *> (parent);
	palNewtonBody *body1 = dynamic_cast<palNewtonBody *> (child);
//	printf("%d and %d\n",body0,body1);
	float pivot[3];
	pivot[0]=x;
	pivot[1]=y;
	pivot[2]=z;
	m_pntnJoint= NewtonConstraintCreateBall(g_nWorld,pivot,body1->m_pntnBody,body0->m_pntnBody);
};

void palNewtonSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {
	palSphericalLink::SetLimits(cone_limit_rad,twist_limit_rad);
	palVector3 pivot;
	pivot.x= m_pParent->m_fPosX - m_pChild->m_fPosX;
	pivot.y= m_pParent->m_fPosY - m_pChild->m_fPosY;
	pivot.z= m_pParent->m_fPosZ - m_pChild->m_fPosZ;
	vec_norm(&pivot);
	NewtonBallSetConeLimits(m_pntnJoint,pivot._vec,cone_limit_rad,twist_limit_rad);
}
/*
void palNewtonSphericalLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	palSphericalLink::SetLimits(lower_limit_rad,upper_limit_rad);
	
	palVector3 pivot;
	pivot.x= m_pParent->m_fPosX - m_pChild->m_fPosX;
	pivot.y= m_pParent->m_fPosY - m_pChild->m_fPosY;
	pivot.z= m_pParent->m_fPosZ - m_pChild->m_fPosZ;
	vec_norm(&pivot);

	printf("only supporting one distance WARNING EVIL CODE\n");

	NewtonBallSetConeLimits(m_pntnJoint,pivot._vec,upper_limit_rad,0);
}

void palNewtonSphericalLink::SetTwistLimits(Float lower_limit_rad, Float upper_limit_rad) {
	palSphericalLink::SetTwistLimits(lower_limit_rad,upper_limit_rad);

	palVector3 pivot;
	pivot.x= m_pParent->m_fPosX - m_pChild->m_fPosX;
	pivot.y= m_pParent->m_fPosY - m_pChild->m_fPosY;
	pivot.z= m_pParent->m_fPosZ - m_pChild->m_fPosZ;
	vec_norm(&pivot);

	printf("only supporting one distance WARNING EVIL CODE\n");

	NewtonBallSetConeLimits(m_pntnJoint,pivot._vec,0,upper_limit_rad);
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palNewtonAngularMotor::palNewtonAngularMotor() {
}

void palNewtonAngularMotor::Init(palRevoluteLink *pLink, Float Max) {
	palAngularMotor::Init(pLink,Max);
	m_pnrl = dynamic_cast<palNewtonRevoluteLink *> (m_link);
}

void palNewtonAngularMotor::Update(Float targetVelocity) {
	if (m_pnrl) {
		m_pnrl->m_callbackdata.motor_force=m_fMax;
		m_pnrl->m_callbackdata.motor_velocity=-targetVelocity;
	}
}

void palNewtonAngularMotor::Apply() {
}

palNewtonRevoluteLink::palNewtonRevoluteLink() {
}

static unsigned DoubleDoorUserCallback (const NewtonJoint* hinge, NewtonHingeSliderUpdateDesc* desc)
{
	float angle;

	//palNewtonRevoluteLink *link= dynamic_cast<palNewtonRevoluteLink *>(NewtonJointGetUserData(hinge));

	//C - style cast (TODO:static cast?)
	palNewtonLinkData *link = (palNewtonLinkData *) NewtonJointGetUserData(hinge);
	
	float angleLimit0 = link->limit_upper;
	float angleLimit1 = link->limit_lower;


	
	angle = NewtonHingeGetJointAngle (hinge);
	
	
	link->data1 = angle;
	link->data2 = NewtonHingeGetJointOmega(hinge);



	if (link->limit_enabled) {

	if (angle > angleLimit0) {
		// if the joint angle is large than the predifine intervale, stop the hinge
		desc->m_accel = NewtonHingeCalculateStopAlpha (hinge, desc, angleLimit0);
		return 1;
	} else if (angle < angleLimit1) {
		// if the joint angle is large than the predifine intervale, stop the hinge
		desc->m_accel = NewtonHingeCalculateStopAlpha (hinge, desc, angleLimit1);
		return 1;
	}

	}

	if (link->motor_force>0) { //motor is enabled
		desc->m_minFriction = -link->motor_force;
		desc->m_maxFriction = link->motor_force;

		//change the acceleration
		desc->m_accel = 0.25 * (link->motor_velocity - link->data2) / desc->m_timestep;
		return 1; 
	}

	// no action need it if the joint angle is with the limits
	return 0;
}

Float palNewtonRevoluteLink::GetAngle() {
	float x = palRevoluteLink::GetAngle();
	return x;
//	return -m_callbackdata.data1;
}
Float palNewtonRevoluteLink::GetAngularVelocity() {
	return m_callbackdata.data2;
}

void palNewtonRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	palRevoluteLink::SetLimits(lower_limit_rad,upper_limit_rad);
	m_callbackdata.limit_lower=lower_limit_rad;
	m_callbackdata.limit_upper=upper_limit_rad;
	m_callbackdata.limit_enabled=true;
	
	NewtonJointSetUserData(m_pntnJoint, &m_callbackdata);
	NewtonHingeSetUserCallback (m_pntnJoint, DoubleDoorUserCallback);
}

void palNewtonRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palNewtonBody *body0 = dynamic_cast<palNewtonBody *> (parent);
	palNewtonBody *body1 = dynamic_cast<palNewtonBody *> (child);
//	printf("%d and %d\n",body0,body1);
	float pivot[3];
	pivot[0]=x;
	pivot[1]=y;
	pivot[2]=z;
	float axis[3];
	axis[0] = axis_x;
	axis[1] = axis_y;
	axis[2] = axis_z;
	m_pntnJoint=NewtonConstraintCreateHinge(g_nWorld,pivot,axis,body1->m_pntnBody,body0->m_pntnBody);
	m_callbackdata.limit_enabled=false;
	NewtonJointSetUserData(m_pntnJoint, &m_callbackdata);
	NewtonHingeSetUserCallback (m_pntnJoint, DoubleDoorUserCallback);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palNewtonPrismaticLink::palNewtonPrismaticLink() {
}

void palNewtonPrismaticLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palPrismaticLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palNewtonBody *body0 = dynamic_cast<palNewtonBody *> (parent);
	palNewtonBody *body1 = dynamic_cast<palNewtonBody *> (child);
//	printf("%d and %d\n",body0,body1);
	float pivot[3];
	pivot[0]=x;
	pivot[1]=y;
	pivot[2]=z;
	float axis[3];
	axis[0] = axis_x;
	axis[1] = axis_y;
	axis[2] = axis_z;
	m_pntnJoint=NewtonConstraintCreateSlider(g_nWorld,pivot,axis,body1->m_pntnBody,body0->m_pntnBody);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palNewtonTerrain::palNewtonTerrain() {
	m_pntnBody = 0;
}

void palNewtonTerrain::SetMaterial(palMaterial *material) {
	palNewtonMaterialUnique * pnmU = dynamic_cast<palNewtonMaterialUnique *> (material);
	if (m_pntnBody)
		NewtonBodySetMaterialGroupID(m_pntnBody,pnmU->m_GroupID);
}


palMatrix4x4& palNewtonTerrain::GetLocationMatrix() {
	if (m_pntnBody) {
	palMatrix4x4 m;
	NewtonBodyGetMatrix(m_pntnBody,m._mat );
	memcpy(m_mLoc._mat,m._mat,sizeof(palMatrix4x4));
	}
	return m_mLoc;
}

palNewtonTerrainPlane::palNewtonTerrainPlane() {
}


void palNewtonTerrainPlane::Init(Float x, Float y, Float z, Float size) {
	palTerrainPlane::Init(x,y,z,size);

	NewtonCollision* collision;

	// create the the floor collision, and body with default values
	collision = NewtonCreateBox (g_nWorld, size, 0.1f, size, NULL); 
	m_pntnBody = NewtonCreateBody (g_nWorld, collision);
	NewtonReleaseCollision (g_nWorld, collision);

	palMatrix4x4 matrix;
	memset(&matrix,0,sizeof(palMatrix4x4));
	matrix._11=1;
	matrix._22=1;
	matrix._33=1;
	matrix._41=m_fPosX;
	matrix._42=-0.05f+m_fPosY;
	matrix._43=m_fPosZ;
	matrix._44=1;

	// set the transformation for this rigid body
	NewtonBodySetMatrix (m_pntnBody, matrix._mat);
}



palNewtonOrientatedTerrainPlane::palNewtonOrientatedTerrainPlane() {
}

void palNewtonOrientatedTerrainPlane::Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size) {
	palOrientatedTerrainPlane::Init(x,y,z,nx,ny,nz,min_size);
	NewtonCollision* collision;

	collision = NewtonCreateBox (g_nWorld, m_fSize, 0.1f, m_fSize, NULL); 
	m_pntnBody = NewtonCreateBody (g_nWorld, collision);
	NewtonReleaseCollision (g_nWorld, collision);

	// set the transformation for this rigid body
	NewtonBodySetMatrix (m_pntnBody, m_mLoc._mat);
}


palMatrix4x4& palNewtonOrientatedTerrainPlane::GetLocationMatrix() {
	return palNewtonTerrain::GetLocationMatrix();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palNewtonTerrainMesh::palNewtonTerrainMesh(){
}

void palNewtonTerrainMesh::Init(Float px, Float py, Float pz, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palTerrainMesh::Init(px,py,pz,pVertices,nVertices,pIndices,nIndices);
		
	NewtonCollision* collision;
	collision=NewtonCreateTreeCollision(g_nWorld,NULL);
	NewtonTreeCollisionBeginBuild(collision);
	for (int i=0;i<nIndices/3;i++) {
		Float tris[3*3];
		memcpy(tris+0,pVertices + pIndices[i*3+0]*3,sizeof(Float)*3);
		memcpy(tris+3,pVertices + pIndices[i*3+1]*3,sizeof(Float)*3);
		memcpy(tris+6,pVertices + pIndices[i*3+2]*3,sizeof(Float)*3);
		NewtonTreeCollisionAddFace(collision, 3, tris, sizeof(Float)*3, 0);
	}
	
	NewtonTreeCollisionEndBuild(collision,0);

	m_pntnBody = NewtonCreateBody (g_nWorld, collision);

	NewtonReleaseCollision (g_nWorld, collision);
	//}

	palMatrix4x4 matrix;
	memset(&matrix,0,sizeof(palMatrix4x4));
	matrix._11=1;
	matrix._22=1;
	matrix._33=1;
	matrix._41=m_fPosX;
	matrix._42=m_fPosY;
	matrix._43=m_fPosZ;
	matrix._44=1;

	// set the transformation for this rigid body
	NewtonBodySetMatrix (m_pntnBody, matrix._mat);

	NewtonBodySetMassMatrix (m_pntnBody, 0.0f, 0.f, 0.f, 0.f); 
 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


palNewtonTerrainHeightmap::palNewtonTerrainHeightmap() {
}

void palNewtonTerrainHeightmap::SetMaterial(palMaterial *material) {
	palNewtonTerrainMesh::SetMaterial(material);
}

palMatrix4x4& palNewtonTerrainHeightmap::GetLocationMatrix() {
	return palNewtonTerrainMesh::GetLocationMatrix();
}

void palNewtonTerrainHeightmap::Init(Float px, Float py, Float pz, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap) {
	palTerrainHeightmap::Init(px,py,pz,width,depth,terrain_data_width,terrain_data_depth,pHeightmap);
	int iTriIndex;
	float fTerrainX, fTerrainZ;
	int x,z;

	int nv=m_iDataWidth*m_iDataDepth;
	int ni=(m_iDataWidth-1)*(m_iDataDepth-1)*2*3;

	Float *v = new Float[nv*3];
	int *ind = new int[ni];

	// Set the vertex values
	fTerrainZ = -m_fDepth/2;
	for (z=0; z<m_iDataDepth; z++)
	{
		fTerrainX = -m_fWidth/2;
		for (x=0; x<m_iDataWidth; x++)
		{
			v[(x + z*m_iDataWidth)*3+0]=fTerrainX;
			v[(x + z*m_iDataWidth)*3+1]=pHeightmap[x+z*m_iDataWidth];
			v[(x + z*m_iDataWidth)*3+2]=fTerrainZ;
		fTerrainX += (m_fWidth / (m_iDataWidth-1));
		}
		fTerrainZ += (m_fDepth / (m_iDataDepth-1));
	}

	iTriIndex = 0;
	int xDim=m_iDataWidth;
	int yDim=m_iDataDepth;
	int y;
	for (y=0;y < yDim-1;y++)
	for (x=0;x < xDim-1;x++) {
		ind[iTriIndex*3+0]=(y*xDim)+x;
		ind[iTriIndex*3+1]=(y*xDim)+xDim+x;
		ind[iTriIndex*3+2]=(y*xDim)+x+1;
		// Move to the next triangle in the array
		iTriIndex += 1;
		
		ind[iTriIndex*3+0]=(y*xDim)+x+1;
		ind[iTriIndex*3+1]=(y*xDim)+xDim+x;
		ind[iTriIndex*3+2]=(y*xDim)+x+xDim+1;
		// Move to the next triangle in the array
		iTriIndex += 1;
	}
	palNewtonTerrainMesh::Init(px,py,pz,v,nv,ind,ni);

	delete [] v;
	delete [] ind;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palNewtonPSDSensor::palNewtonPSDSensor() {

}

void palNewtonPSDSensor::Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range) {
	palPSDSensor::Init(body,x,y,z,dx,dy,dz,range);
	palVector3 pos;
	body->GetPosition(pos);
	m_fRelativePosX = m_fPosX - pos.x;
	m_fRelativePosY = m_fPosY - pos.y;
	m_fRelativePosZ = m_fPosZ - pos.z;
}


Float palNewtonPSDSensor::GetDistance() {
	float pos0[3];
	float pos1[3];
	float PosX,PosY,PosZ;

	palMatrix4x4 m;
	m=m_pBody->GetLocationMatrix();

	PosX=m._41;
	PosY=m._42;
	PosZ=m._43;

	palMatrix4x4 bodypos = m_pBody->GetLocationMatrix();
	palMatrix4x4 out;

	mat_identity(&m);
	mat_translate(&m,m_fRelativePosX,m_fRelativePosY,m_fRelativePosZ);
	mat_multiply(&out,&bodypos,&m);
	pos0[0]=out._41;
	pos0[1]=out._42;
	pos0[2]=out._43;

	

/*	pos0[0]=m_fPosX+PosX;
	pos0[1]=m_fPosY+PosY;
	pos0[2]=m_fPosZ+PosZ;
	*/

	mat_identity(&m);
	mat_translate(&m,m_fAxisX,m_fAxisY,m_fAxisZ);
	mat_multiply(&out,&bodypos,&m);

	palVector3 newaxis;
	newaxis.x=out._41-bodypos._41;
	newaxis.y=out._42-bodypos._42;
	newaxis.z=out._43-bodypos._43;
	vec_norm(&newaxis);


	pos1[0]=pos0[0]+newaxis.x*m_fRange;
	pos1[1]=pos0[1]+newaxis.y*m_fRange;
	pos1[2]=pos0[2]+newaxis.z*m_fRange;

//	printf("pos0: %f %f %f\n",pos0[0],pos0[1],pos0[2]);
//	printf("pos1: %f %f %f\n",pos1[0],pos1[1],pos1[2]);

	//float norm[3];
	float dist;
	dist=1.2f;
	pickedParam = 1.2f;
//#pragma message("implement ray caster again")
	NewtonWorldRayCast(g_nWorld,pos0,pos1,RayCastPlacement,0,newtonRaycastPreFilter);

//	NewtonWorldRayCast(g_nWorld,pos0,pos1,RayCastPlacement,&dist);
//	printf("pickedParam :%f\n",pickedParam);
	if (pickedParam>1) return m_fRange;
	return pickedParam*m_fRange;
	//dist=NewtonBodyRayIntersect(g_floorBody,pos0,pos1,norm);
	//if (dist>1) return m_fRange;
	//return dist*m_fRange;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class palNewtonContactSensor: public palContactSensor {
	palNewtonContactSensor();
//	void Init(palBody *body) = 0; //location and size?
	void GetContactPosition(palVector3& contact) = 0;
protected:
	FACTORY_CLASS(palNewtonContactSensor,palContactSensor,Newton,1);
};*/

palNewtonContactSensor::palNewtonContactSensor() {
	memset(&m_Contact ,0,sizeof(m_Contact));
}

void palNewtonContactSensor::Init(palBody *body) {
	//add a contact listener for this sensor.
	palContactSensor::Init(body);
	PAL_MAP<NewtonBody*, palContact> ::iterator itr;
	palNewtonBody *pb = dynamic_cast<palNewtonBody *>(body);
	itr=g_ContactsData.find(pb->m_pntnBody);
	if (itr == g_ContactsData.end()) { //nothing found, make a new pair
		palContact pc;
		g_ContactsData.insert(std::make_pair(pb->m_pntnBody,pc));
	} 
}

void palNewtonContactSensor::GetContactPosition(palVector3& contact) {
	palNewtonBody *pb = dynamic_cast<palNewtonBody *>(m_pBody);
	PAL_MAP<NewtonBody*, palContact> ::iterator itr;
	itr=g_ContactsData.find(pb->m_pntnBody);
	if (itr != g_ContactsData.end()) { //found a contact
		contact = itr->second.m_ContactPoints[0].m_vContactPosition;
	}
}

////////////////////////////

palNewtonForceActuator::palNewtonForceActuator() {
}

void palNewtonForceActuator::Init(palBody *pbody, Float px, Float py, Float pz, Float axis_x, Float axis_y, Float axis_z) {
		palNewtonBody *pnb = dynamic_cast<palNewtonBody *>(pbody);
		NewtonBodySetAutoFreeze(pnb->m_pntnBody,0);
		palForceActuator::Init(pbody,px,py,pz,axis_x,axis_y,axis_z);
	}


#ifdef STATIC_CALLHACK
void pal_newton_call_me_hack() {
	printf("%s I have been called!!\n", __FILE__);
};
#endif