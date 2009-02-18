#if defined(_MSC_VER)
#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#endif
#include "tokamak_pal.h"
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)

/*
	Abstract:
		PAL - Physics Abstraction Layer. Tokamak implementation.
		This enables the use of tokamak via PAL.

		Implementation
	Author: 
		Adrian Boeing
	Revision History:
		Version 0.7 : 28/06/04 - actually implemented force,torque,and velocity functions
		Version 0.6+: 11/06/04 - see header for mods, also, cleaned up from backup version to remove some old code for positioning and floor building.
		Version 0.5 : 04/06/04 - Created 
	TODO:
*/

#ifndef NDEBUG
 #ifdef MICROSOFT_VC
#pragma warning( disable : 4786 ) // ident trunc to '255' chars in debug info
#ifdef MEMDEBUG
 #include <crtdbg.h>
 #define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif
 #endif
#endif

//FACTORY_CLASS_IMPLEMENTATION(palTokamakMaterial);
FACTORY_CLASS_IMPLEMENTATION_BEGIN_GROUP;
FACTORY_CLASS_IMPLEMENTATION(palTokamakMaterialUnique);
FACTORY_CLASS_IMPLEMENTATION(palTokamakMaterialInteraction);

FACTORY_CLASS_IMPLEMENTATION(palTokamakBoxGeometry);
FACTORY_CLASS_IMPLEMENTATION(palTokamakSphereGeometry);
FACTORY_CLASS_IMPLEMENTATION(palTokamakCylinderGeometry);

#ifdef USE_QHULL
FACTORY_CLASS_IMPLEMENTATION(palTokamakConvexGeometry);
FACTORY_CLASS_IMPLEMENTATION(palTokamakConvex);
#endif

FACTORY_CLASS_IMPLEMENTATION(palTokamakPhysics);

FACTORY_CLASS_IMPLEMENTATION(palTokamakBox);
FACTORY_CLASS_IMPLEMENTATION(palTokamakSphere);
FACTORY_CLASS_IMPLEMENTATION(palTokamakCylinder);
FACTORY_CLASS_IMPLEMENTATION(palTokamakCompoundBody);


FACTORY_CLASS_IMPLEMENTATION(palTokamakSphericalLink);
FACTORY_CLASS_IMPLEMENTATION(palTokamakRevoluteLink);
FACTORY_CLASS_IMPLEMENTATION(palTokamakPrismaticLink);

FACTORY_CLASS_IMPLEMENTATION(palTokamakOrientatedTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palTokamakTerrainPlane);
FACTORY_CLASS_IMPLEMENTATION(palTokamakTerrainHeightmap);
FACTORY_CLASS_IMPLEMENTATION(palTokamakTerrainMesh);

FACTORY_CLASS_IMPLEMENTATION(palTokamakPSDSensor);
FACTORY_CLASS_IMPLEMENTATION(palTokamakContactSensor);
FACTORY_CLASS_IMPLEMENTATION_END_GROUP;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//globals:
void gGetLocationMatrix(palMatrix4x4 &Loc, neT3 t) {
	Loc._11 = t.rot[0][0];
	Loc._21 = t.rot[1][0];
	Loc._31 = t.rot[2][0];
	Loc._41 = t.pos[0];

	Loc._12 = t.rot[0][1];
	Loc._22 = t.rot[1][1];
	Loc._32 = t.rot[2][1];
	Loc._42 = t.pos[1];

	Loc._13 = t.rot[0][2];
	Loc._23 = t.rot[1][2];
	Loc._33 = t.rot[2][2];
	Loc._43 = t.pos[2];

	Loc._14 = 0;
	Loc._24 = 0;
	Loc._34 = 0;
	Loc._44 = 1.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//int palTokamakMaterial::g_materialcount = 1;
neSimulator *gSim = NULL;
neAnimatedBody *gFloor = NULL;
static int g_materialcount = 1;
/*
TokamakMaterial::TokamakMaterial() {
};

void TokamakMaterial::Init(Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterial::Init(static_friction,kinetic_friction,restitution);
	m_Index=g_materialcount;
	if (gSim) 
		gSim->SetMaterial(m_Index, m_fStatic, m_fRestitution);
	else 
		printf("ERROR HERE\n");
	g_materialcount++;
};
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakMaterialInteraction::palTokamakMaterialInteraction() {
}

void palTokamakMaterialInteraction::Init(palMaterialUnique *pM1, palMaterialUnique *pM2, Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterialInteraction::Init(pM1,pM2,static_friction,kinetic_friction,restitution);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakMaterialUnique::palTokamakMaterialUnique() {
}

void palTokamakMaterialUnique::Init(PAL_STRING name,Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterialUnique::Init(name,static_friction,kinetic_friction,restitution);
	m_Index=g_materialcount;
	if (gSim) {
		gSim->SetMaterial(m_Index, m_fStatic, m_fRestitution);
	}
	g_materialcount++;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakPhysics::palTokamakPhysics() {
};

const char* palTokamakPhysics::GetVersion() {
	static char verbuf[256];
	sprintf(verbuf,"Tokamak V%d.%d.%d",TOKAMAK_VERSION_MAJOR,TOKAMAK_VERSION_MINOR,TOKAMAK_VERSION_BUGFIX);
	return verbuf;
}

const char* palTokamakPhysics::GetPALVersion() {
	static char verbuf[512];
	sprintf(verbuf,"PAL SDK V%d.%d.%d\nPAL Tokamak V:%d.%d.%d\nFile: %s\nCompiled: %s %s\nModified:%s",
		PAL_SDK_VERSION_MAJOR,PAL_SDK_VERSION_MINOR,PAL_SDK_VERSION_BUGFIX,
		TOKAMAK_PAL_SDK_VERSION_MAJOR,TOKAMAK_PAL_SDK_VERSION_MINOR,TOKAMAK_PAL_SDK_VERSION_BUGFIX,
		__FILE__,__TIME__,__DATE__,__TIMESTAMP__);
	return verbuf;
}

void palTokamakPhysics::Init(Float gravity_x, Float gravity_y, Float gravity_z) {
	palPhysics::Init(gravity_x, gravity_y, gravity_z); //set member variables

	neV3 gravity;		// A vector to store the direction and intensity of gravity
	gravity.Set(m_fGravityX,m_fGravityY,m_fGravityZ);

	neSimulatorSizeInfo sizeInfo;	// SizeInfo stores data about how many objects we are going to model
	sizeInfo.constraintBufferSize = 8096;
	//sizeInfo.terrainNodesStartCount=50;
	//sizeInfo.terrainNodesGrowByCount=10;
	 sizeInfo.rigidBodiesCount = 500;
	 sizeInfo.geometriesCount = 550;

	// Create and initialise the simulator
	gSim = neSimulator::CreateSimulator(sizeInfo, NULL, &gravity);
};

void palTokamakPhysics::Cleanup() {
	neSimulator::DestroySimulator(gSim);
	gSim = NULL;
};

void palTokamakPhysics::Iterate(Float timestep) {
	gSim->Advance(timestep);
};

neSimulator* palTokamakPhysics::TokamakGetSimulator() {
	return gSim;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


palTokamakBody::palTokamakBody() {
	if (gSim!=NULL) {
	m_ptokBody = gSim->CreateRigidBody();
//	m_ptokGeom = m_ptokBody->AddGeometry();
	}
};

palTokamakBody::~palTokamakBody() {
if (m_ptokBody) {
		gSim->FreeRigidBody(m_ptokBody);
		Cleanup();
//		delete m_ptokBody;
	}
};
/*
void palTokamakBody::SetPosition(Float x, Float y, Float z) {
	palBody::SetPosition(x,y,z);
	neV3 pos;
	pos.Set(x,y,z);
	m_ptokBody->SetPos(pos);
};*/

void BuildRotMatrix(neM3 &rot,const palMatrix4x4& loc) {
	rot[0][0] = loc._11;
	rot[1][0] = loc._21;
	rot[2][0] = loc._31;
	
	rot[0][1] = loc._12;
	rot[1][1] = loc._22;
	rot[2][1] = loc._32;
	
	rot[0][2] = loc._13;
	rot[1][2] = loc._23;
	rot[2][2] = loc._33;
}


void palTokamakBody::SetPosition(palMatrix4x4& loc) {
	neV3 pos;
	pos.Set(loc._41,loc._42,loc._43);
	m_ptokBody->SetPos(pos);
	neM3 rot;
	BuildRotMatrix(rot,loc);
/*
	rot[0][0] = loc._11;
	rot[1][0] = loc._21;
	rot[2][0] = loc._31;
	
	rot[0][1] = loc._12;
	rot[1][1] = loc._22;
	rot[2][1] = loc._32;
	
	rot[0][2] = loc._13;
	rot[1][2] = loc._23;
	rot[2][2] = loc._33;
*/
	m_ptokBody->SetRotation(rot);
}

palMatrix4x4& palTokamakBody::GetLocationMatrix() {

	gGetLocationMatrix(m_mLoc,m_ptokBody->GetTransform());
	return m_mLoc;
}


palMatrix4x4& palTokamakGeometry::GetLocationMatrix() {
	if (!m_ptokGeom) {
		mat_identity(&m_mLoc);
		return m_mLoc;
	}
	palMatrix4x4 bmat;
	palMatrix4x4 gmat;
	gGetLocationMatrix(gmat,m_ptokGeom->GetTransform());
	bmat = m_pBody->GetLocationMatrix();
	mat_multiply(&m_mLoc,&bmat,&gmat);
	return m_mLoc;
}


void palTokamakBody::SetActive(bool active) {
	neRigidBody * hint = 0;
	m_ptokBody->Active(active,hint);
}
#if 0
void palTokamakBody::SetForce(Float fx, Float fy, Float fz) {
	neV3 force;
	force.Set(fx,fy,fz);
	m_ptokBody->SetForce(force);
}

void palTokamakBody::GetForce(palVector3& force) {
	neV3 tf = m_ptokBody->GetForce();
	tf.Get(force._vec);
}

void palTokamakBody::SetTorque(Float tx, Float ty, Float tz) {
	neV3 force;
	force.Set(tx,ty,tz);
	m_ptokBody->SetTorque(force);
}

void palTokamakBody::GetTorque(palVector3& torque) {
	neV3 tf = m_ptokBody->GetTorque();
	tf.Get(torque._vec);
}
#endif

void palTokamakBody::ApplyImpulse(Float fx, Float fy, Float fz) {
	neV3 force;
	force.Set(fx,fy,fz);
	m_ptokBody->ApplyImpulse(force);
}

void palTokamakBody::ApplyAngularImpulse(Float fx, Float fy, Float fz) {
	neV3 force;
	force.Set(fx,fy,fz);
	m_ptokBody->ApplyTwist(force);
}



void palTokamakBody::GetLinearVelocity(palVector3& velocity) {
	neV3 vel = m_ptokBody->GetVelocity();
	vel.Get(velocity._vec);
}

void palTokamakBody::GetAngularVelocity(palVector3& velocity) {
	neV3 vel = m_ptokBody->GetAngularVelocity();
	vel.Get(velocity._vec);
}

void palTokamakBody::SetLinearVelocity(palVector3 velocity) {
	neV3 vel;
	vel.Set(velocity._vec);
	m_ptokBody->SetVelocity(vel);
}
void palTokamakBody::SetAngularVelocity(palVector3 velocity_rad) {
	//SetAngularMomentum ? arg!
}

void palTokamakBody::SetMaterial(palMaterial *material) {
	palTokamakMaterialUnique *ptmU = dynamic_cast<palTokamakMaterialUnique *> (material);
	if (ptmU) {
		for (unsigned int i=0;i<m_Geometries.size();i++) {
			palTokamakGeometry *ptG = dynamic_cast<palTokamakGeometry *> (m_Geometries[i]);
			ptG->SetMaterial(ptmU);
		}
	}
	palBody::SetMaterial(material);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakGeometry::palTokamakGeometry() {
	m_ptokGeom = NULL;
}
/*
palMatrix4x4& palTokamakGeometry::GetLocationMatrix() {
	gGetLocationMatrix(m_mLoc,m_ptokGeom->GetTransform());
	return m_mLoc;
}*/

void palTokamakGeometry::SetPosition(palMatrix4x4& loc) {
	palMatrix4x4 bloc;
	if (m_pBody) {
		bloc=m_pBody->GetLocationMatrix();
	}
	neT3 t;
	t.pos.Set(loc._41-bloc._41,loc._42-bloc._42,loc._43-bloc._43);
	t.rot[0][0] = loc._11;
	t.rot[1][0] = loc._21;
	t.rot[2][0] = loc._31;
	
	t.rot[0][1] = loc._12;
	t.rot[1][1] = loc._22;
	t.rot[2][1] = loc._32;
	
	t.rot[0][2] = loc._13;
	t.rot[1][2] = loc._23;
	t.rot[2][2] = loc._33;
	m_ptokGeom->SetTransform(t);
}

void palTokamakGeometry::SetMaterial(palMaterial *material) {
	palTokamakMaterialUnique *ptmU = dynamic_cast<palTokamakMaterialUnique *> (material);
	if (ptmU) 
		m_ptokGeom->SetMaterialIndex(ptmU->m_Index);
}


palTokamakBoxGeometry::palTokamakBoxGeometry() {
}

void palTokamakBoxGeometry::Init(palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	palBoxGeometry::Init(pos,width,height,depth,mass);
	if (m_pBody) {
		palTokamakBody *ptb=dynamic_cast<palTokamakBody *>(m_pBody);
		if (ptb) {
			m_ptokGeom = ptb->TokamakGetRigidBody()->AddGeometry();
			SetDimensions(width,height,depth);
		}
	}
	palTokamakGeometry::SetPosition(pos);
}

void palTokamakBoxGeometry::SetDimensions(Float width, Float height, Float depth) {
	if (m_pBody) {
		palTokamakBody *ptb=dynamic_cast<palTokamakBody *>(m_pBody);
		if (ptb) {
			neV3 boxSize1;
			boxSize1.Set(m_fWidth,m_fHeight,m_fDepth);
			m_ptokGeom->SetBoxSize(boxSize1[0],boxSize1[1],boxSize1[2]);
		}
	}
}

palTokamakSphereGeometry::palTokamakSphereGeometry() {
}

void palTokamakSphereGeometry::Init(palMatrix4x4 &pos, Float radius, Float mass) {
	palSphereGeometry::Init(pos,radius,mass);
	if (m_pBody) {
		palTokamakBody *ptb=dynamic_cast<palTokamakBody *>(m_pBody);
		if (ptb) {
			m_ptokGeom = ptb->TokamakGetRigidBody()->AddGeometry();
		}
	}
	palTokamakGeometry::SetPosition(pos);
}

void palTokamakSphereGeometry::SetRadius(Float radius) {
	m_fRadius = radius;
	if (m_pBody) {
		palTokamakBody *ptb=dynamic_cast<palTokamakBody *>(m_pBody);
		if (ptb) {
			m_ptokGeom->SetSphereDiameter(m_fRadius*2);
		}
	}
}

palTokamakCylinderGeometry::palTokamakCylinderGeometry() {
}

void palTokamakCylinderGeometry::Init(palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	palCapsuleGeometry::Init(pos,radius,length,mass);
	if (m_pBody) {
		palTokamakBody *ptb=dynamic_cast<palTokamakBody *>(m_pBody);
		if (ptb) {
			m_ptokGeom = ptb->TokamakGetRigidBody()->AddGeometry();
		}
	}
	palTokamakGeometry::SetPosition(pos);
}

void palTokamakCylinderGeometry::SetRadiusLength(Float radius, Float length) {
	m_fRadius = radius;
	m_fLength = length;
	if (m_pBody) {
		palTokamakBody *ptb=dynamic_cast<palTokamakBody *>(m_pBody);
		if (ptb) {
			m_ptokGeom->SetCylinder(m_fRadius*2,length);
		}
	}
}

#ifdef USE_QHULL
typedef struct {
	neV3 normal;
	f32 k;
} tokFace;

typedef struct  {
	s32 numFace;
	s32 numVerts;
	tokFace* faces;
	neV3* verts;
} tokConvex;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



extern "C"
{
#include "qhull_a.h"
}
#pragma comment(lib, "qhull.lib")

const int INIT_ARRAY_SIZE = 100;

struct ConvexFace
{
	int neighbourFaces;
	int neighbourVerts;
	int neighbourEdges;
};
struct ConvexVert
{
	int neighbourEdges;
};
struct ConvexEdge
{
	neByte f1;
	neByte f2;
	neByte v1;
	neByte v2;
};

template <class T> class FArray
{
public:
	FArray() : nextFree(0), size(INIT_ARRAY_SIZE) 
	{
		dataArray =  new T[INIT_ARRAY_SIZE];

		keyArray = new u32[INIT_ARRAY_SIZE];
	}

	T & operator [] (int i)
	{ 
		assert(i < nextFree); return dataArray[i];
	}

	void Add(u32 key, const T & item) 
	{
		for (int i = 0; i < nextFree; i++)
		{
			if (keyArray[i] == key)
			{
				return;
			}
		}
		if (nextFree == size)
			Grow();
		
		keyArray[nextFree] = key;

		dataArray[nextFree++] = item;
	}
	int GetIndex(u32 key)
	{
		for (int i = 0; i < nextFree; i++)
		{
			if (keyArray[i] == key)
			{
				return i;
			}
		}
		return -1;
	}

	int Get(u32 key, T & item)
	{
		for (int i = 0; i < nextFree; i++)
		{
			if (keyArray[i] == key)
			{
				item = dataArray[i];

				return i;
			}
		}
		return -1;
	}
	int GetCount() {
		return nextFree;
	}

public:
	T * dataArray;
	u32 * keyArray;
	int nextFree;
	int size;

private:
	void Grow() {
		T * newData = new T[size * 2];

		u32 * newKey = new u32[size * 2];

		for (int i = 0; i < nextFree; i++)
		{
			newData[i] = dataArray[i];

			newKey[i] = keyArray[i];
		}
		size = size * 2;

		delete dataArray;

		delete keyArray;

		dataArray = newData;

		keyArray = newKey;
	}
};

class neighbourItem
{
public:
	s32 neighbour;
	s32 nextNeighbour;

	neighbourItem() : neighbour (-1), nextNeighbour(-1) {}
};

#define BUF_SIZE 1024

u32 MakeEdgeKey(neByte v1, neByte v2)
{
	if (v1 < v2)
	{
		return (v1 << 16 | v2);
	}
	else
	{
		return (v2 << 16 | v1);
	}
}

#include "mFILE.h"

#define FILETYPE mFILE
#define FOPEN mfopen
#define FWRITE mfwrite
#define FCLOSE mfclose

BYTE *palTokamakConvexGeometry::GenerateConvexData(const Float *pVertices, int nVertices) {
	int i;
	int vertexCount = nVertices;
	double * vertArray = new double[vertexCount * 3];
	for (i=0;i<nVertices*3;i++)
		vertArray[i] = pVertices[i];

	qh_init_A (stdin, stdout, stderr, 0, NULL);
	int exitcode= setjmp (qh errexit);
	char options [200];
	sprintf (options, "qhull");
	qh_initflags (options);
	qh_init_B (vertArray, vertexCount, 3, true);
	qh_qhull();
	qh_vertexneighbors();
	qh_check_output();
	qh_triangulate();
	qh_findgood_all (qh facet_list); 
	FArray<facetT *> faceRecord;
	FArray<ridgeT *> edgeRecord;
	FArray<ConvexEdge> edgeRecord2;
	FArray<vertexT *> vertexRecord;
{
	vertexT *vertex, **vertexp;

	FORALLvertex_(qh vertex_list)
	{
		vertexRecord.Add(vertex->id, vertex);
	}

	facetT *facet;

	FORALLfacet_(qh facet_list)
	{
		faceRecord.Add(facet->id, facet);
	}
	FORALLfacet_(qh facet_list)
	{
		if (0)//facet->ridges)
		{
			ridgeT * ridge, ** ridgep;

			FOREACHridge_(facet->ridges)
			{
				edgeRecord.Add(ridge->id, ridge);
			}
		}
		else
		{
			int vertexIndex[3], c = 0;

			FOREACHvertex_(facet->vertices)
			{
				neByte h = vertexRecord.GetIndex(vertex->id);

				ASSERT(h < qh num_vertices);

				vertexIndex[c++] = h;
			}
			c = 0;

			facetT * neighbor, ** neighborp;

			FOREACHneighbor_(facet)
			{
				ConvexEdge e;

				e.f1 = faceRecord.GetIndex(facet->id);
				e.f2 = faceRecord.GetIndex(neighbor->id);

				ASSERT(e.f1 < qh num_facets);
				ASSERT(e.f2 < qh num_facets);

				e.v1 = vertexIndex[(c+1)%3];
				e.v2 = vertexIndex[(c+2)%3];

				u32 key = MakeEdgeKey(e.v1, e.v2);

				edgeRecord2.Add(key, e);

				c++;
			}
		}
	}
}

		int edgeCount = edgeRecord2.GetCount();
	int zero = 0;
	
	FILETYPE * ff = FOPEN("xxx.xxx", "m");

	FWRITE(&(qh num_facets), sizeof(int), 1, ff);
	FWRITE(&(qh num_vertices), sizeof(int), 1, ff);
	FWRITE(&edgeCount, sizeof(int), 1, ff);
	FWRITE(&zero, sizeof(int), 1, ff);
	
	//TOKAMAK_OUTPUT_3("%d, %d, %d \n", qh num_facets, qh num_vertices, edgeCount);

	f32 f;

	for (i = 0; i < faceRecord.GetCount(); i++)
	{
		facetT * facet = faceRecord[i];
		
		for (int j = 0; j < 3; j++)
		{
			f = facet->normal[j];

			FWRITE(&f, sizeof(f), 1, ff);

			//TOKAMAK_OUTPUT_1("%f ", f);
		}
		f = facet->offset;

		FWRITE(&f, sizeof(f), 1, ff);

		//TOKAMAK_OUTPUT_1("%f \n", f);
	}
	for (i = 0; i < vertexRecord.GetCount(); i++)
	{
		vertexT *vertex	= vertexRecord[i];

		for (int j = 0; j < 3; j++)
		{
			f = vertex->point[j];

			FWRITE(&f, sizeof(f), 1, ff);

			//TOKAMAK_OUTPUT_1("%f ", f);
		}
		FWRITE(&zero, sizeof(f), 1, ff);

		//TOKAMAK_OUTPUT("\n");
	}

	int offset = sizeof(int) * 4;
	offset += sizeof(float) * 4 * (faceRecord.GetCount() + qh num_vertices);
	offset += sizeof(ConvexFace) * faceRecord.GetCount();
	offset += sizeof(ConvexVert) * vertexRecord.GetCount();

	if (edgeRecord.GetCount() != 0)
		offset += sizeof(ConvexEdge) * edgeRecord.GetCount();
	else
		offset += sizeof(ConvexEdge) * edgeRecord2.GetCount();

	ConvexFace cface;

	//TOKAMAK_OUTPUT("Face Records \n");

	for (i = 0; i < faceRecord.GetCount(); i++)
	{
		facetT * facet = faceRecord[i];

		cface.neighbourFaces = offset;	offset += sizeof(neByte) * 3;//cface.numberFaceNeighbour;
		cface.neighbourVerts = offset;	offset += sizeof(neByte) * 3;//cface.numberVertNeighbour;
		cface.neighbourEdges = offset;	offset += sizeof(neByte) * 3;//cface.numberFaceNeighbour;

		int ss = sizeof(cface);

		FWRITE(&cface, sizeof(cface), 1, ff);
		
		//TOKAMAK_OUTPUT_3("Face %d has %d face neighbour, %d vertics\n", i, 3, 3);
	}

	//TOKAMAK_OUTPUT("Vertex Records \n");

	ConvexVert cvert;

	for (i = 0; i < vertexRecord.GetCount(); i++)
	{
		vertexT * vertex = vertexRecord[i];

		int numberEdgeNeighbour = 0;

		facetT * neighbor, ** neighborp;
		
		FOREACHneighbor_(vertex)
		{
			numberEdgeNeighbour++;
		}
		cvert.neighbourEdges = offset;	offset += sizeof(neByte) * (numberEdgeNeighbour + 1);

		int ss = sizeof(cvert);

		FWRITE(&cvert, ss, 1, ff);

		//TOKAMAK_OUTPUT_2("Vertex %d has %d edges\n", i, numberEdgeNeighbour);
	}
	ConvexEdge cedge;

	if (edgeRecord.GetCount() != 0)
	{
		for (i = 0; i < edgeRecord.GetCount(); i++)
		{
			ridgeT * ridge = edgeRecord[i];

			cedge.f1 = faceRecord.GetIndex(ridge->top->id);
			
			cedge.f2 = faceRecord.GetIndex(ridge->bottom->id);

			vertexT * vertex, **vertexp;
			int v[2], c = 0;
			FOREACHvertex_(ridge->vertices)
			{
				ASSERT(c < 2);

				v[c] = vertex->id;

				c++;
			}
			cedge.v1 = vertexRecord.GetIndex(v[0]);

			cedge.v2 = vertexRecord.GetIndex(v[1]);

			FWRITE(&cedge, sizeof(ConvexEdge), 1, ff);

			//TOKAMAK_OUTPUT_1("Edge %d = ", i);
			
			//TOKAMAK_OUTPUT_4(" faces %d, %d, verts %d, %d\n", cedge.f1, cedge.f2, cedge.v1, cedge.v2);
		}
	}
	else
	{
		for (i = 0; i < edgeRecord2.GetCount(); i++)
		{
			cedge = edgeRecord2[i];

			FWRITE(&cedge, sizeof(ConvexEdge), 1, ff);

			//TOKAMAK_OUTPUT_1("Edge %d = ", i);
			
			//TOKAMAK_OUTPUT_4(" faces %d, %d, verts %d, %d\n", cedge.f1, cedge.f2, cedge.v1, cedge.v2);
		}
	}
	for (i = 0; i < faceRecord.GetCount(); i++)
	{
		facetT * facet = faceRecord[i];

		facetT * neighbor, ** neighborp;

		//TOKAMAK_OUTPUT_1("Face %d = {", i);
		
		FOREACHneighbor_(facet)
		{
			neByte bb = faceRecord.GetIndex(neighbor->id);

			FWRITE(&bb, sizeof(neByte), 1, ff);

			//TOKAMAK_OUTPUT_1("%d,", bb);
		}
		//TOKAMAK_OUTPUT("}, {");

		vertexT * vertex, ** vertexp;
		
		FOREACHvertex_(facet->vertices)
		{
			neByte bb = vertexRecord.GetIndex(vertex->id);

			FWRITE(&bb, sizeof(neByte), 1, ff);

			//TOKAMAK_OUTPUT_1("%d,", bb);
		}
		//TOKAMAK_OUTPUT("}, {");

		if (0)//facet->ridges)
		{
			ridgeT * ridge, ** ridgep;

			FOREACHridge_(facet->ridges)
			{
				neByte bb = edgeRecord.GetIndex(ridge->id);

				FWRITE(&bb, sizeof(neByte), 1, ff);

				//TOKAMAK_OUTPUT_1("%d,", bb);
			}
		}
		else
		{
			int vertexIndex[3], c = 0;

			FOREACHvertex_(facet->vertices)
			{
				vertexIndex[c++] = vertexRecord.GetIndex(vertex->id);
			}
			c = 0;

			facetT * neighbor, ** neighborp;

			FOREACHneighbor_(facet)
			{
				neByte v1 = vertexIndex[(c+1)%3];
				neByte v2 = vertexIndex[(c+2)%3];
				u32 key = MakeEdgeKey(v1, v2);

				neByte bb = edgeRecord2.GetIndex(key);

				FWRITE(&bb, sizeof(neByte), 1, ff);

				//TOKAMAK_OUTPUT_1("%d,", bb);
				c++;
			}
		}
	//	TOKAMAK_OUTPUT("}\n");
	}

	// vertex adjacent edges

	for (i = 0; i < vertexRecord.GetCount(); i++)
	{
		neByte * ridgeNeigbour = new neByte[100];

		vertexT * vert = vertexRecord[i];

		facetT * neighbor, ** neighborp;

		int numEdge = 0;

		FOREACHneighbor_(vert)
		{
			if (0)//neighbor->ridges)
			{
				ridgeT * ridge, ** ridgep;

				FOREACHridge_(neighbor->ridges)
				{
					vertexT * vertex, ** vertexp;

					FOREACHvertex_(ridge->vertices)
					{
						if (vertexRecord.GetIndex(vertex->id) == i)
						{
							//add this ridge
							neByte eindex = edgeRecord.GetIndex(ridge->id);

							bool found = false;

							for (int k = 0; k < numEdge; k++)
							{
								if (ridgeNeigbour[k] == eindex)
								{	
									found = true;
									break;
								}
							}
							if (!found)
							{
								ridgeNeigbour[numEdge++] = eindex;
							}
						}
					}
				}
			}
			else
			{
				vertexT * vertex, ** vertexp;

				neByte vs[3]; int c = 0;

				FOREACHvertex_(neighbor->vertices)
				{
					vs[c++] = vertexRecord.GetIndex(vertex->id);
				}
				for (int ii = 0; ii < 3; ii++)
				{
					neByte v1 = vs[(ii+1)%3];

					neByte v2 = vs[(ii+2)%3];

					if (v1 != i && v2 != i)
						continue;

					u32 key = MakeEdgeKey(v1, v2);

					neByte eindex = edgeRecord2.GetIndex(key);

					bool found = false;

					for (int k = 0; k < numEdge; k++)
					{
						if (ridgeNeigbour[k] == eindex)
						{
							found = true;

							break;
						}
					}
					if (!found)
					{
						ridgeNeigbour[numEdge++] = eindex;
					}
				}
			}
		}
		//TOKAMAK_OUTPUT_1("Vertex %d = {", i);

		for (int j = 0; j < numEdge; j++)
		{
			neByte bb = ridgeNeigbour[j];

			FWRITE(&bb, sizeof(neByte), 1, ff);

			//TOKAMAK_OUTPUT_1("%d,", bb);
		}
		neByte f = 0xff;

		FWRITE(&f, sizeof(neByte), 1, ff);

		//TOKAMAK_OUTPUT("}\n");
		
		delete ridgeNeigbour;
	}
	//FCLOSE(ff);
	qh NOerrexit= True;

	delete vertArray;

	return (BYTE *)ff->data;

	return 0;
}


struct DCDFace
{
	neByte *neighbourFaces;
	neByte *neighbourVerts;
	neByte *neighbourEdges;
};

struct DCDVert
{
	neByte * neighbourEdges;
};

struct DCDEdge
{
	neByte f1;
	neByte f2;
	neByte v1;
	neByte v2;
};
bool palTokamakConvexGeometry::ReadConvexData(char * filename, neByte *& adjacency) {
	FILE * ff =	fopen(filename, "r");

	if (ff == NULL)
		return false;

	fseek(ff, 0, SEEK_END);
	
	fpos_t pos ;

	fgetpos(ff, &pos);

	fclose(ff);

	ff = fopen(filename, "rb");

	neByte * d = new neByte[pos];

	//fseek(ff, 0, SEEK_SET);

	int r = fread(d,1, pos, ff);

	fclose(ff);
	PreProcess(d);
	adjacency = d;
	return true;
}

void palTokamakConvexGeometry::PreProcess(neByte *& d) {
	s32 numFaces = *(int*)d;

	s32 numVerts = *((int*)d + 1);

	s32 numEdges = *((int*)d + 2);

	DCDFace * f = (DCDFace *)(d + sizeof(int) * 4 + numFaces * sizeof(f32) * 4 + numVerts * sizeof(f32) * 4);

	DCDVert * v = (DCDVert *)(f + numFaces);

	s32 i;

	for ( i  = 0; i < numFaces; i++)
	{
		f[i].neighbourEdges += (int)d;
		f[i].neighbourVerts += (int)d;
		f[i].neighbourFaces += (int)d;
	}
	for (i = 0; i < numVerts; i++)
	{
		v[i].neighbourEdges += (int)d;
	}
	
	DCDEdge * e = (DCDEdge*)(&v[numVerts]);

	int diff = (neByte*)e - d;
}

void palTokamakConvexGeometry::TokamakInitQHull(palMatrix4x4 &pos, neByte *data) {
//	palConvexGeometry::Init(pos,0,0,0);
	if (m_pBody) {
		palTokamakBody *ptb=dynamic_cast<palTokamakBody *>(m_pBody);
		if (ptb) {
				m_ptokGeom = ptb->m_ptokBody->AddGeometry();
				m_ptokGeom->SetConvexMesh(data);
				ptb->m_ptokBody->UpdateBoundingInfo();
		}
	}
	palTokamakGeometry::SetPosition(pos);
}

void palTokamakConvexGeometry::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	palConvexGeometry::Init(pos,pVertices,nVertices,mass);
	if (m_pBody) {
		palTokamakBody *ptb=dynamic_cast<palTokamakBody *>(m_pBody);
		if (ptb) {
//			m_ptokGeom = ptb->m_ptokBody->AddGeometry();
			neByte *data = 0;
			data = GenerateConvexData(pVertices,nVertices);
			PreProcess(data);
			TokamakInitQHull(pos,data);
		}
	}
	palTokamakGeometry::SetPosition(pos);
}


palTokamakConvex::palTokamakConvex() {
}

void palTokamakConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass) {
	palConvex::Init(x,y,z,pVertices,nVertices,mass);
	m_ptokBody->UpdateBoundingInfo();
	SetMass(mass);
}

void palTokamakConvex::SetMass(Float fMass) {
	//todo fix this:
	m_ptokBody->SetInertiaTensor(neBoxInertiaTensor(1, 1, 1, fMass));
	m_ptokBody->SetMass(fMass);	
}
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakBox::palTokamakBox() {
};

void palTokamakBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
	palBox::Init(x,y,z,width,height,depth,mass);
	SetDimensions(width,height,depth);
	SetMass(mass);
};
/*
void palTokamakBox::SetPosition(Float x, Float y, Float z) {
	palTokamakBody::SetPosition(x,y,z); //ensure correct dominance
};
*/
void palTokamakBox::SetDimensions(Float fWidth, Float fHeight, Float fDepth) {
//	m_fWidth=fWidth;
//	m_fHeight=fHeight;
//	m_fDepth=fDepth;
	palTokamakBoxGeometry *ptokBoxGeom = dynamic_cast<palTokamakBoxGeometry *>(m_Geometries[0]);
	if (ptokBoxGeom)
		ptokBoxGeom->SetDimensions(fWidth,fHeight,fDepth);
	m_ptokBody->UpdateBoundingInfo();
}

void palTokamakBox::SetMass(Float fMass) {
	neV3 boxSize1;
	palTokamakBoxGeometry *ptokBoxGeom = dynamic_cast<palTokamakBoxGeometry *>(m_Geometries[0]);
	if (ptokBoxGeom)
		boxSize1.Set(ptokBoxGeom->m_fWidth,ptokBoxGeom->m_fHeight,ptokBoxGeom->m_fDepth);
	m_ptokBody->SetInertiaTensor(neBoxInertiaTensor(boxSize1[0], boxSize1[1], boxSize1[2], fMass));
	m_ptokBody->SetMass(fMass);	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakSphere::palTokamakSphere() {
};

void palTokamakSphere::Init(Float x, Float y, Float z, Float radius, Float mass) {
	palSphere::Init(x,y,z,radius,mass);
	palBody::SetPosition(x,y,z);
	SetRadius(radius);
	SetMass(mass);
};

void palTokamakSphere::SetRadius(Float radius) {
	//m_fRadius = radius;
	palTokamakSphereGeometry *ptokSphereGeom = dynamic_cast<palTokamakSphereGeometry *>(m_Geometries[0]);
	if (ptokSphereGeom)
		ptokSphereGeom->SetRadius(radius);
	m_ptokBody->UpdateBoundingInfo();
	SetMass(m_fMass);
}

void palTokamakSphere::SetMass(Float mass) {
	m_fMass=mass;
	palTokamakSphereGeometry *ptokSphereGeom = dynamic_cast<palTokamakSphereGeometry *>(m_Geometries[0]);
	if (ptokSphereGeom)
		m_ptokBody->SetInertiaTensor(neSphereInertiaTensor(ptokSphereGeom->m_fRadius*2, mass));
	m_ptokBody->SetMass(mass);	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palTokamakCylinder::palTokamakCylinder() {
};

void palTokamakCylinder::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
	palCapsule::Init(x,y,z,radius,length,mass);
	palBody::SetPosition(x,y,z);
	SetRadiusLength(radius,length);
	SetMass(mass);
}

void palTokamakCylinder::SetRadiusLength(Float radius, Float length) {
//	m_fRadius = radius;
//	m_fLength = length;
//	m_ptokGeom->SetCylinder(m_fRadius*2,length);
	palTokamakCylinderGeometry *ptokCylinderGeom = dynamic_cast<palTokamakCylinderGeometry *>(m_Geometries[0]);
	if (ptokCylinderGeom)
		ptokCylinderGeom->SetRadiusLength(radius,length);
	m_ptokBody->UpdateBoundingInfo();
	SetMass(m_fMass);
}

void palTokamakCylinder::SetMass(Float mass) {
	m_fMass=mass;
	palTokamakCylinderGeometry *ptokCylinderGeom = dynamic_cast<palTokamakCylinderGeometry *>(m_Geometries[0]);
	if (ptokCylinderGeom) {
		ptokCylinderGeom->SetMass(mass);
		m_ptokBody->SetInertiaTensor(neCylinderInertiaTensor(ptokCylinderGeom->m_fRadius*2, ptokCylinderGeom->m_fLength, mass));
	}
	m_ptokBody->SetMass(mass);	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palTokamakCompoundBody::palTokamakCompoundBody() {
}

void palTokamakCompoundBody::Finalize(Float finalMass, Float iXX, Float iYY, Float iZZ) {
	palBody::SetPosition(m_fPosX,m_fPosY,m_fPosZ);
	m_ptokBody->UpdateBoundingInfo();
	neV3 inertia; 
	inertia.Set(iXX, iYY, iZZ);
	m_ptokBody->SetInertiaTensor(inertia);
	m_ptokBody->SetMass(finalMass);	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palTokamakLink::palTokamakLink() {
	m_ptokJoint = NULL;
}
/*
void TokamakLink::SetAnchor(Float x, Float y, Float z) {
	neT3 jointFrame;
	jointFrame.SetIdentity();
	jointFrame.pos.Set(x, y, z);
	if (m_ptokJoint)
//		m_ptokJoint->SetJointFrameA(jointFrame);
		m_ptokJoint->SetJointFrameWorld(jointFrame);
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakSphericalLink::palTokamakSphericalLink() {
};

void palTokamakSphericalLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z) {
	palSphericalLink::Init(parent,child,x,y,z);
	palTokamakBody *body0 = dynamic_cast<palTokamakBody *> (parent);
	palTokamakBody *body1 = dynamic_cast<palTokamakBody *> (child);
//	printf("%d and %d\n",body0,body1);

//	neT3 jointFrame;
//	jointFrame.SetIdentity();
//	m_ptokJoint->SetJointFrameWorld(jointFrame);
	m_ptokJoint = gSim->CreateJoint(body0->m_ptokBody , body1->m_ptokBody); //a crash here? Check the maximum joint count allowed

	SetAnchor(x,y,z);

	m_ptokJoint->SetType(neJoint::NE_JOINT_BALLSOCKET);
	m_ptokJoint->Enable(true);
}

void palTokamakSphericalLink::SetAnchor(Float x, Float y, Float z) {
	neT3 jointFrame;
	jointFrame.SetIdentity();
	jointFrame.pos.Set(x, y, z);
	if (m_ptokJoint)
//		m_ptokJoint->SetJointFrameA(jointFrame);
		m_ptokJoint->SetJointFrameWorld(jointFrame);
}

void palTokamakSphericalLink::SetLimits(Float cone_limit_rad, Float twist_limit_rad) {
	palSphericalLink::SetLimits(cone_limit_rad,twist_limit_rad);
	printf("INCOMPLETE FUNCTION!\n");
}

/*
void palTokamakSphericalLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	palSphericalLink::SetLimits(lower_limit_rad,upper_limit_rad);

	m_ptokJoint->SetUpperLimit(0.1f);//m_fUpperLimit);
	m_ptokJoint->SetLowerLimit(0.0f);//m_fLowerLimit);

	//m_ptokJoint->SetEpsilon(0.0f);
	//m_ptokJoint->SetIteration(5);

	//m_ptokJoint->EnableLimit(true);
	//m_ptokJoint->Enable(true);
}

void palTokamakSphericalLink::SetTwistLimits(Float lower_limit_rad, Float upper_limit_rad) {
	palSphericalLink::SetTwistLimits(lower_limit_rad,upper_limit_rad);

	m_ptokJoint->SetUpperLimit2(0.2f);//m_fUpperTwistLimit);
	m_ptokJoint->SetLowerLimit2(0.1f);//m_fLowerTwistLimit);

	m_ptokJoint->SetEpsilon(0.0f);
	m_ptokJoint->SetIteration(5);

	m_ptokJoint->EnableLimit(true);
	m_ptokJoint->Enable(true);
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakRevoluteLink::palTokamakRevoluteLink() {
}

void palTokamakRevoluteLink::SetLimits(Float lower_limit_rad, Float upper_limit_rad) {
	palRevoluteLink::SetLimits(lower_limit_rad,upper_limit_rad);

	m_ptokJoint->SetUpperLimit(m_fUpperLimit);
	m_ptokJoint->SetLowerLimit(m_fLowerLimit);

	m_ptokJoint->SetEpsilon(0.0f);
	m_ptokJoint->SetIteration(5);

	m_ptokJoint->EnableLimit(true);
//	m_ptokJoint->Enable(true);
}

void palTokamakRevoluteLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palRevoluteLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palTokamakBody *body0 = dynamic_cast<palTokamakBody *> (parent);
	palTokamakBody *body1 = dynamic_cast<palTokamakBody *> (child);
//	printf("%d and %d\n",body0,body1);

	neV3 jointPos;
	jointPos.Set(x,y,z);

	neT3 trans;
	trans.SetIdentity();
	trans.rot.M[1].Set(axis_x,axis_y,axis_z); //set the y-axis as the rotatable place
	trans.pos = jointPos;

	m_ptokJoint = gSim->CreateJoint(body0->m_ptokBody , body1->m_ptokBody); 
	m_ptokJoint->SetJointFrameWorld(trans);
	m_ptokJoint->SetType(neJoint::NE_JOINT_HINGE);
	m_ptokJoint->Enable(true);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakPrismaticLink::palTokamakPrismaticLink() {
}

void palTokamakPrismaticLink::Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z) {
	palPrismaticLink::Init(parent,child,x,y,z,axis_x,axis_y,axis_z);
	palTokamakBody *body0 = dynamic_cast<palTokamakBody *> (parent);
	palTokamakBody *body1 = dynamic_cast<palTokamakBody *> (child);
//	printf("%d and %d\n",body0,body1);

	m_ptokJoint = gSim->CreateJoint(body0->m_ptokBody , body1->m_ptokBody); //a crash here? Check the maximum joint count allowed
	neV3 jointPos;
	jointPos.Set(x,y,z);
	
	neT3 trans;
	memset(&trans,0,sizeof(neT3));
	trans.pos = jointPos;
	trans.rot.M[1].Set(axis_x,axis_y,axis_z); //set the y-axis as the rotatable place

	m_ptokJoint->SetJointFrameWorld(trans);

	m_ptokJoint->SetType(neJoint:: NE_JOINT_SLIDE);
	m_ptokJoint->Enable(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
palTokamakOrientatedTerrainPlane::palTokamakOrientatedTerrainPlane() {
}

void palTokamakOrientatedTerrainPlane::Init(Float x, Float y, Float z, Float nx, Float ny, Float nz, Float min_size) {
	palOrientatedTerrainPlane::Init(x,y,z,nx,ny,nz,min_size);

	neGeometry *geom;	// Pointer to a Geometry object which we'll use to define the shape/size of each cube
	
	// Create an animated body for the floor
	gFloor = gSim->CreateAnimatedBody();
	// Add geometry to the floor and set it to be a box with size as defined by the FLOORSIZE constant
	geom = gFloor->AddGeometry();
	neV3 boxSize1;		// The length, width and height of the cube
	boxSize1.Set(min_size, 0.0f, min_size);
	geom->SetBoxSize(boxSize1[0],boxSize1[1],boxSize1[2]);
	gFloor->UpdateBoundingInfo();
	// Set the position of the box within the simulator
	neV3 pos;			// The position of each object
	pos.Set(x, y, z);
	gFloor->SetPos(pos);

	neM3 rot;
	BuildRotMatrix(rot,m_mLoc);
	gFloor->SetRotation(rot);

	neRigidBody * hint = NULL;
	gFloor->Active(true,hint);

}

void palTokamakOrientatedTerrainPlane::SetMaterial(palMaterial *material) {
	palTokamakMaterialUnique *ptmU = dynamic_cast<palTokamakMaterialUnique *> (material);

	gFloor->BeginIterateGeometry();
	neGeometry * geom = gFloor->GetNextGeometry();
	while (geom) {
		geom->SetMaterialIndex(ptmU->m_Index);
		geom = gFloor->GetNextGeometry();
	}
}

palTokamakTerrainPlane::palTokamakTerrainPlane() {
}

void palTokamakTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrainPlane::Init(x,y,z,min_size);
	neGeometry *geom;	// Pointer to a Geometry object which we'll use to define the shape/size of each cube
	
	// Create an animated body for the floor
	gFloor = gSim->CreateAnimatedBody();
	// Add geometry to the floor and set it to be a box with size as defined by the FLOORSIZE constant
	geom = gFloor->AddGeometry();
	neV3 boxSize1;		// The length, width and height of the cube
	boxSize1.Set(min_size, 0.0f, min_size);
	geom->SetBoxSize(boxSize1[0],boxSize1[1],boxSize1[2]);
	gFloor->UpdateBoundingInfo();
/*	// Set the material for the floor
	if (m_pMaterial!=NULL) {
		palTokamakMaterial *ptm = dynamic_cast<palTokamakMaterial *>(m_pMaterial);
		if (ptm!=NULL) {
			geom->SetMaterialIndex(ptm->m_Index);
		}
	}*/
	// Set the position of the box within the simulator
	neV3 pos;			// The position of each object
	pos.Set(x, y, z);
	gFloor->SetPos(pos);
	neRigidBody * hint = NULL;
	gFloor->Active(true,hint);
}

void palTokamakTerrainPlane::SetMaterial(palMaterial *material) {
	palTokamakMaterialUnique *ptmU = dynamic_cast<palTokamakMaterialUnique *> (material);

	gFloor->BeginIterateGeometry();
	neGeometry * geom = gFloor->GetNextGeometry();
	while (geom) {
		geom->SetMaterialIndex(ptmU->m_Index);
		geom = gFloor->GetNextGeometry();
	}
}

palMatrix4x4& palTokamakTerrainPlane::GetLocationMatrix() {
	if (gFloor)
		gGetLocationMatrix(m_mLoc,gFloor->GetTransform());
	return m_mLoc;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palTokamakTerrainHeightmap::palTokamakTerrainHeightmap() {
}

void palTokamakTerrainHeightmap::SetMaterial(palMaterial *material) {
	palTokamakTerrainMesh::SetMaterial(material);
}

palMatrix4x4& palTokamakTerrainHeightmap::GetLocationMatrix() {
	return palTokamakTerrainMesh::GetLocationMatrix();
}

void palTokamakTerrainHeightmap::Init(Float px, Float py, Float pz, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap) {
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
	palTokamakTerrainMesh::Init(px,py,pz,v,nv,ind,ni);

	delete [] v;
	delete [] ind;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

palTokamakTerrainMesh::palTokamakTerrainMesh(){
}

void palTokamakTerrainMesh::SetMaterial(palMaterial *material) {
	palTokamakMaterialUnique *ptmU = dynamic_cast<palTokamakMaterialUnique *> (material);
	if (gFloor) {
	gFloor->BeginIterateGeometry();
	neGeometry * geom = gFloor->GetNextGeometry();
	while (geom) {
		geom->SetMaterialIndex(ptmU->m_Index);
		geom = gFloor->GetNextGeometry();
	}
	}
}

palMatrix4x4& palTokamakTerrainMesh::GetLocationMatrix() {
	memset(&m_mLoc,0,sizeof(m_mLoc));
	m_mLoc._11=1;m_mLoc._22=1;m_mLoc._33=1;m_mLoc._44=1;
	m_mLoc._41=m_fPosX;
	m_mLoc._42=m_fPosY;
	m_mLoc._43=m_fPosZ;
	return m_mLoc;
}

void palTokamakTerrainMesh::Init(Float px, Float py, Float pz, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palTerrainMesh::Init(px,py,pz,pVertices,nVertices,pIndices,nIndices);
	int i;
	neTriangleMesh triMesh;
	triMesh.vertexCount = m_nVertices;
	triMesh.triangleCount = m_nIndices/3;

	neV3 *triVertices = new neV3[nVertices];
	neTriangle *triData = new neTriangle[m_nIndices/3];

	for (i=0;i<m_nIndices/3;i++) {
		triData[i].indices[0]=pIndices[i*3+0];
		triData[i].indices[1]=pIndices[i*3+1];
		triData[i].indices[2]=pIndices[i*3+2];
	}
	
	for (i=0;i<m_nVertices;i++) {
		triVertices[i].Set(pVertices[i*3+0]+m_fPosX,pVertices[i*3+1]+m_fPosY,pVertices[i*3+2]+m_fPosZ);
	}

	triMesh.vertices = triVertices;
	triMesh.triangles = triData;

	// Tell the simulator about our mesh
	gSim->SetTerrainMesh(&triMesh);

	delete [] triVertices;
	delete [] triData;
}


palTokamakPSDSensor::palTokamakPSDSensor() {

}

void palTokamakPSDSensor::Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz,Float range) {
	palPSDSensor::Init(body,x,y,z,dx,dy,dz,range);
	m_cb.m_pSensor=this;
	palTokamakBody *tb = dynamic_cast<palTokamakBody *> (body);
	m_ptokSensor = tb->TokamakGetRigidBody()->AddSensor();
	neV3 pos;
	neV3 dir;
	pos.Set(m_fPosX,m_fPosY,m_fPosZ);
	dir.Set(m_fAxisX*m_fRange,m_fAxisY*m_fRange,m_fAxisZ*m_fRange);
	m_ptokSensor->SetLineSensor(pos,dir);
	m_ptokController = tb->TokamakGetRigidBody()->AddController(&m_cb, 0);
	tb->TokamakGetRigidBody()->UpdateBoundingInfo(); //this is evil
}

Float palTokamakPSDSensor::GetDistance() {
	return m_fRange-m_distance;//m_ptokSensor->GetDetectDepth();
}


//neRigidBody* g_ContactBody0;
//neRigidBody* g_ContactBody1;
PAL_MAP<neRigidBody*,PAL_VECTOR<palTokamakContactSensor *> > g_ContactData;

void CollisionCallback (neCollisionInfo & collisionInfo)
{
	PAL_MAP<neRigidBody*,PAL_VECTOR<palTokamakContactSensor *> > ::iterator itr;
	f32 pos[3];
	if (collisionInfo.typeA == NE_RIGID_BODY)
	{
		neRigidBody * rbA = (neRigidBody *)collisionInfo.bodyA;
		collisionInfo.worldContactPointA.Get(pos);
		itr=g_ContactData.find(rbA);
		unsigned int i;
		if (itr!=g_ContactData.end()) {
			for (i=0;i<(*itr).second.size();i++) {
				(*itr).second[i]->m_Contact.x = pos[0];
				(*itr).second[i]->m_Contact.y = pos[1];
				(*itr).second[i]->m_Contact.z = pos[2];
			}
		}
	}
	if (collisionInfo.typeB == NE_RIGID_BODY)
	{
		neRigidBody * rbB = (neRigidBody *)collisionInfo.bodyB;
		collisionInfo.worldContactPointB.Get(pos);
		itr=g_ContactData.find(rbB);
		unsigned int i;
		if (itr!=g_ContactData.end()) {
			for (i=0;i<(*itr).second.size();i++) {
				(*itr).second[i]->m_Contact.x = pos[0];
				(*itr).second[i]->m_Contact.y = pos[1];
				(*itr).second[i]->m_Contact.z = pos[2];
			}
		}
	}
}

palTokamakContactSensor::palTokamakContactSensor() {

}

void palTokamakContactSensor::Init(palBody *body) {
	palTokamakBody *tb = dynamic_cast<palTokamakBody *> (body);

	PAL_MAP<neRigidBody*,PAL_VECTOR<palTokamakContactSensor *> > ::iterator itr;
	
	itr=g_ContactData.find(tb->TokamakGetRigidBody());
	if (itr == g_ContactData.end()) { //nothing found, make a new pair
		PAL_VECTOR<palTokamakContactSensor *> v;
		v.push_back(this);
		g_ContactData.insert(std::make_pair(tb->TokamakGetRigidBody(),v) );
	} else {
		(*itr).second.push_back(this);
	}

	gSim->GetCollisionTable()->Set(0, 0, neCollisionTable::RESPONSE_IMPULSE_CALLBACK);
	gSim->SetCollisionCallback(CollisionCallback);
}

void palTokamakContactSensor::GetContactPosition(palVector3 &contact) {
	contact = m_Contact;
}

//neBool CustomCDRB2RBCallback(neRigidBody * bodyA, neRigidBody * bodyB, neCustomCDInfo & cdInfo);
//sim->SetCustomCDRB2RBCallback(CustomCDRB2RBCallback);
//boxBody[i] = sim->CreateRigidBody();
//boxBody[i]->UseCustomCollisionDetection(true, &obb, extend);


#ifdef STATIC_CALLHACK
void pal_tokamak_call_me_hack() {
	printf("%s I have been called!!\n", __FILE__);
};
#endif