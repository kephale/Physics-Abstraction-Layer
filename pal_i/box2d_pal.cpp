#include "box2d_pal.h"


FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
FACTORY_CLASS_IMPLEMENTATION(palBox2DPhysics);

FACTORY_CLASS_IMPLEMENTATION(palBox2DCompoundBody);

FACTORY_CLASS_IMPLEMENTATION(palBox2DBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palBox2DSphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palBox2DConvexGeometry);

FACTORY_CLASS_IMPLEMENTATION(palBox2DBox);
FACTORY_CLASS_IMPLEMENTATION(palBox2DSphere);
FACTORY_CLASS_IMPLEMENTATION(palBox2DConvex);

FACTORY_CLASS_IMPLEMENTATION(palBox2DStaticBox);
FACTORY_CLASS_IMPLEMENTATION(palBox2DStaticSphere);

FACTORY_CLASS_IMPLEMENTATION(palBox2DTerrainPlane);
/*
FACTORY_CLASS_IMPLEMENTATION(palBox2DTerrainMesh);
*/
FACTORY_CLASS_IMPLEMENTATION(palBox2DSphericalLink);
FACTORY_CLASS_IMPLEMENTATION(palBox2DRevoluteLink);
FACTORY_CLASS_IMPLEMENTATION(palBox2DPrismaticLink);

FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

b2World *g_World = 0;

palBox2DPhysics::palBox2DPhysics() {
	pb2World = 0;
}

void palBox2DPhysics::Init(palPhysicsDesc& desc) {
   palPhysics::Init(desc);
	b2AABB worldAABB;
	worldAABB.lowerBound.Set(-100.0f, -100.0f);
	worldAABB.upperBound.Set(100.0f, 100.0f);

	// Define the gravity vector.
	b2Vec2 gravity(m_fGravityX, m_fGravityY);

	// Do we want to let bodies sleep?
	bool doSleep = true;

	// Construct a world object, which will hold and simulate the rigid bodies.
	pb2World = new 	b2World(worldAABB, gravity, doSleep);
	g_World = pb2World;
}

void palBox2DPhysics::Cleanup() {
}
const char* palBox2DPhysics::GetVersion() {
	return 0;
}

const char* palBox2DPhysics::GetPALVersion() {
	static char verbuf[512];
	sprintf(verbuf,"PAL SDK V%d.%d.%d\nPAL Box2D V:%d.%d.%d\nFile: %s\nCompiled: %s %s\nModified:%s",
		PAL_SDK_VERSION_MAJOR,PAL_SDK_VERSION_MINOR,PAL_SDK_VERSION_BUGFIX,
		BOX2D_PAL_SDK_VERSION_MAJOR,BOX2D_PAL_SDK_VERSION_MINOR,BOX2D_PAL_SDK_VERSION_BUGFIX,
		__FILE__,__TIME__,__DATE__,__TIMESTAMP__);
	return verbuf;
}

void palBox2DPhysics::Iterate(Float timestep) {

	// Instruct the world to perform a single step of simulation. It is
	// generally best to keep the time step and iterations fixed.
	pb2World->Step(timestep, 1);
}

palBox2DGeometry::palBox2DGeometry() {
	pbShape = 0;
}

palBox2DGeometry::~palBox2DGeometry(){
}

void palBox2DGeometry::Flatten(palMatrix4x4 &pos) {
	pos._43 = 0;
}

palBox2DBodyBase::palBox2DBodyBase() {
	pbBodyDef = 0;
	pBody = 0;
}
palBox2DBodyBase::~palBox2DBodyBase() {
}
palMatrix4x4& palBox2DBodyBase::GetLocationMatrix() {
	mat_identity(&m_mLoc);
	if (pBody) {
		b2Vec2 position = pBody->GetPosition();
		float32 rotation = pBody->GetAngle();
		mat_set_translation(&m_mLoc,position.x,position.y,0);
		mat_rotate(&m_mLoc,rotation*RAD2DEG,0,0,1);
	}
	return m_mLoc;
}
void palBox2DBodyBase::SetPosition(palMatrix4x4& location) {
}
void palBox2DBodyBase::SetMaterial(palMaterial *material) {
	for (int i=0;i<m_Geometries.size();i++) {
		palBox2DGeometry *pbg=dynamic_cast<palBox2DGeometry *> (m_Geometries[i]);
		pbg->pbShape->friction = material->m_fStatic;
		pbg->pbShape->restitution= material->m_fRestitution;

	}
}

void palBox2DBodyBase::BuildBody(Float fx, Float fy, Float mass, bool dynamic) {
	pbBodyDef = new b2BodyDef;
	pbBodyDef->position.Set(fx,fy);

	pBody = g_World->CreateBody(pbBodyDef);
	for (int i=0;i<m_Geometries.size();i++) {
		palBox2DGeometry *pbg=dynamic_cast<palBox2DGeometry *> (m_Geometries[i]);
		if (!dynamic)
			pbg->pbShape->density = 0;
		pBody->CreateShape(pbg->pbShape);
//		pbBodyDef->AddShape(pbg->pbShape);
	}
	//		if (!dynamic)
	//			pbBodyDef->density = 0;
	//		else
	//			pbBodyDef->density = mass; //TODO

	if (dynamic)
		pBody->SetMassFromShapes();

}



palBox2DBody::palBox2DBody() {
}

void palBox2DBody::ApplyForce(Float fx, Float fy, Float fz){}
void palBox2DBody::ApplyTorque(Float tx, Float ty, Float tz){
	pBody->ApplyTorque(tz);
}

void palBox2DBody::ApplyImpulse(Float fx, Float fy, Float fz){}
void palBox2DBody::ApplyAngularImpulse(Float fx, Float fy, Float fz){}

void palBox2DBody::GetLinearVelocity(palVector3& velocity){
	b2Vec2 v = pBody->GetLinearVelocity();
	velocity.x = v.x;
	velocity.y = v.y;
	velocity.z = 0;
}

void palBox2DBody::GetAngularVelocity(palVector3& velocity_rad){
	float r = pBody->GetAngularVelocity();
	velocity_rad.x = 0;
	velocity_rad.y = 0;
	velocity_rad.z = r;
}

void palBox2DBody::SetLinearVelocity(palVector3 velocity){
	pBody->SetLinearVelocity (b2Vec2(velocity.x,velocity.y));
}

void palBox2DBody::SetAngularVelocity(palVector3 velocity_rad){
	pBody->SetAngularVelocity(velocity_rad.z);
}

bool palBox2DBody::IsActive() {
	return !pBody->IsSleeping();
}

void palBox2DBody::SetActive(bool active) {
	if (active)
		pBody->WakeUp();
	else
		pBody->PutToSleep();
}

palBox2DCompoundBody::palBox2DCompoundBody() {
}

void palBox2DCompoundBody::Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ) {
	for (int i=0;i<m_Geometries.size();i++) {
		palBox2DGeometry *pbg=dynamic_cast<palBox2DGeometry *> (m_Geometries[i]);

		palMatrix4x4 m = pbg->GetOffsetMatrix();//GetLocationMatrix();

//		pbg->pbShape->localPosition.Set(m._41,m._42);
	}
	BuildBody(m_fPosX,m_fPosY,finalMass,true);
}
/////////

palBox2DBoxGeometry::palBox2DBoxGeometry(){
	pbBoxShape = 0;
}

void palBox2DBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	Flatten(pos);
	palBoxGeometry::Init(pos,width,height,depth,mass);
	Flatten(m_mOffset);
	pbBoxShape = new b2PolygonDef;
	pbBoxShape->SetAsBox(width/2,height/2,b2Vec2(m_mOffset._41,m_mOffset._42),0);
//	pbBoxShape->extents.Set(width/2,height/2);
	pbBoxShape->density = mass;//TODO
	pbShape = pbBoxShape;
}

palBox2DSphereGeometry::palBox2DSphereGeometry() {
	pbCirShape = 0;
}
void palBox2DSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	Flatten(pos);
	palSphereGeometry::Init(pos,radius,mass);
	Flatten(m_mOffset);
	pbCirShape = new b2CircleDef;
	pbCirShape->radius = radius;
	pbCirShape->density = mass; //TODO
	pbCirShape->localPosition = b2Vec2(m_mOffset._41,m_mOffset._42);
	pbShape = pbCirShape;
}

palBox2DConvexGeometry::palBox2DConvexGeometry() {
	pbPolyShape = 0;
}

void palBox2DConvexGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	Flatten(pos);
	/*
	if (nVertices > b2_maxPolyVertices) {
		//SET_ERROR("Too many vertices");
		return;
	}*/
	palConvexGeometry::Init(pos,pVertices,nVertices,mass);
	pbPolyShape = new b2PolygonDef;
	pbPolyShape->vertexCount = nVertices;
	for (int i=0;i<nVertices;i++) {
		pbPolyShape->vertices[i].Set(pVertices[i*3+0],pVertices[i*3+1]);
	}
	pbPolyShape->density = mass; //TODO
	pbShape = pbPolyShape;
}


palBox2DBox::palBox2DBox() {
}

void palBox2DBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
	BuildBody(x,y,mass,true);
}

palBox2DSphere::palBox2DSphere() {
}
void palBox2DSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
	BuildBody(x,y,mass,true);
}

palBox2DConvex::palBox2DConvex() {
}
void palBox2DConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	BuildBody(x,y,mass,true);
}

palBox2DStaticBox::palBox2DStaticBox() {
}
void palBox2DStaticBox::Init(palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palStaticBox::Init(pos,width,height,depth);
	BuildBody(m_fPosX,m_fPosY,0,false);
	SetPosition(pos);
}
palBox2DStaticSphere::palBox2DStaticSphere() {
}
void palBox2DStaticSphere::Init(palMatrix4x4 &pos, Float radius) {
	palStaticSphere::Init(pos,radius);
	BuildBody(m_fPosX,m_fPosY,0,false);
	SetPosition(pos);
}

palBox2DTerrainPlane::palBox2DTerrainPlane() {
}
void palBox2DTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);
	GenerateDefaultBoxGeom(0.5f);
	BuildBody(x,y,0,false);
}

//////////////////
palBox2DSphericalLink::palBox2DSphericalLink(){
}
void palBox2DSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	palSphericalLink::Init(parent,child,x,y,z);
	palBox2DBodyBase *body0 = dynamic_cast<palBox2DBodyBase *> (parent);
	palBox2DBodyBase *body1 = dynamic_cast<palBox2DBodyBase *> (child);

	m_bHinge = new b2RevoluteJointDef;
	m_bHinge->collideConnected = false;
/*
	m_bHinge->body1 = body0->pBody;
	m_bHinge->body2 = body1->pBody;

	m_bHinge->anchorPoint.Set(x,y);*/
	m_bHinge->Initialize(body0->pBody,body1->pBody,b2Vec2(x,y));
	g_World->CreateJoint(m_bHinge);
	//m_bRJoint=dynamic_cast<b2RevoluteJoint *>();
}

palBox2DRevoluteLink::palBox2DRevoluteLink() {
}
void palBox2DRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palBox2DBodyBase *body0 = dynamic_cast<palBox2DBodyBase *> (parent);
	palBox2DBodyBase *body1 = dynamic_cast<palBox2DBodyBase *> (child);

	m_bHinge = new b2RevoluteJointDef;
	m_bHinge->collideConnected = false;

/*	m_bHinge->body1 = body0->pBody;
	m_bHinge->body2 = body1->pBody;

	m_bHinge->anchorPoint.Set(x,y);*/
	m_bHinge->Initialize(body0->pBody,body1->pBody,b2Vec2(x,y));
	g_World->CreateJoint(m_bHinge);
}

palBox2DPrismaticLink::palBox2DPrismaticLink() {
}
void palBox2DPrismaticLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palPrismaticLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palBox2DBodyBase *body0 = dynamic_cast<palBox2DBodyBase *> (parent);
	palBox2DBodyBase *body1 = dynamic_cast<palBox2DBodyBase *> (child);

	m_bSlider = new b2PrismaticJointDef;
	m_bSlider->collideConnected = false;
	m_bSlider->Initialize(body0->pBody,body1->pBody,b2Vec2(x,y),b2Vec2(axis_x,axis_y));
/*
	m_bSlider->body1 = body0->pBody;
	m_bSlider->body2 = body1->pBody;

	m_bSlider->anchorPoint.Set(x,y);
	m_bSlider->axis.Set(axis_x,axis_y);*/
	g_World->CreateJoint(m_bSlider);
}
