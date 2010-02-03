//#include "pal.h"
#include "palFactory.h"
#include <algorithm>
/*
	Abstract:
		PAL - Physics Abstraction Layer.
		Implementation File

	Author:
		Adrian Boeing
	Revision History:
		Version 0.84:19/09/06 GPS, remerged
		Version 0.83:17/02/05 velocimeter update
		Version 0.82:16/02/05 Changed velocimeter to relative coordinates
		Version 0.81:10/06/04 Correction to palBody::SetPosition
		Version 0.8 : 3/06/04
	TODO:
		-saferize vertex copyign for terrain heightmap and mesh
		-defines for infninity for joint limts
*/

#ifdef MEMDEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif


void palPhysics::SetFactoryInstance(palFactory *pf) {
	palFactory::SetInstance(pf);
}

void palPhysics::AddAction(palAction *action) {
	if (action != NULL)
		m_Actions.push_back(action);
}

void palPhysics::RemoveAction(palAction *action) {
	if (action != NULL)
	{
		PAL_LIST<palAction*>::iterator found = std::find(m_Actions.begin(),m_Actions.end(),action);
		if (found != m_Actions.end()) {
			m_Actions.erase(found);
		}
	}
}

void palPhysics::SetDebugDraw(palDebugDraw* debugDraw) {
	m_pDebugDraw = debugDraw;
}

palDebugDraw* palPhysics::GetDebugDraw() {
	return m_pDebugDraw;
}

struct ActionCaller {
	void operator()(palAction *action)
	{
		(*action)(m_fTimeStep);
	}
	float m_fTimeStep;
};

void palPhysics::CallActions(Float timestep) {
	ActionCaller ac;
	ac.m_fTimeStep = timestep;
	std::for_each(m_Actions.begin(), m_Actions.end(), ac);
}

#if 0
void paldebug_printmatrix(palMatrix4x4 *pm) {
	for (int i=0;i<16;i++) {
		printf("%f ",pm->_mat[i]);
		if (i%4 == 3)
			printf("\n");
	}
}
void paldebug_printvector3(palVector3 *pv) {
	printf("%f %f %f\n",pv->x,pv->y,pv->z);
}

//void paldebug_printvector4(palVector4 *pv) {
//	printf("%f %f %f %f\n",pv->x,pv->y,pv->z,pv->w);
//}
#endif

FACTORY_CLASS_IMPLEMENTATION(palMaterials);
FACTORY_CLASS_IMPLEMENTATION(palMaterialUnique);
FACTORY_CLASS_IMPLEMENTATION(palMaterialInteraction);

palMaterialDesc::palMaterialDesc()
: m_fStatic(0.0)
, m_fKinetic(0.0)
, m_fRestitution(0.5)
, m_bEnableAnisotropicFriction(false)
, m_bDisableStrongFriction(false)
{
	for (unsigned i = 0; i < 3; ++i)
	{
		m_vStaticAnisotropic._vec[i] = 1.0f;
		m_vKineticAnisotropic._vec[i] = 1.0f;
	}
	m_vDirAnisotropy.x = 1.0f;
	m_vDirAnisotropy.y = 0.0f;
	m_vDirAnisotropy.z = 0.0f;

}

void palMaterial::SetParameters(const palMaterialDesc& matDesc) {
	m_fStatic = matDesc.m_fStatic;
	m_fKinetic = matDesc.m_fKinetic;
	m_fRestitution = matDesc.m_fRestitution;
	m_vStaticAnisotropic = matDesc.m_vStaticAnisotropic;
   m_vKineticAnisotropic = matDesc.m_vKineticAnisotropic;
   m_vDirAnisotropy = matDesc.m_vDirAnisotropy;
	m_bEnableAnisotropicFriction = matDesc.m_bEnableAnisotropicFriction;
   m_bDisableStrongFriction = matDesc.m_bDisableStrongFriction;
}

palMaterialUnique::palMaterialUnique() : m_Name("") {
}

void palMaterialUnique::Init(PAL_STRING name, const palMaterialDesc& matDesc) {
	SetParameters(matDesc);
	m_Name=name;
}

palMaterialInteraction::palMaterialInteraction()
: m_pMaterial1(0), m_pMaterial2(0) {
}

void palMaterialInteraction::Init(palMaterialUnique *pM1, palMaterialUnique *pM2, const palMaterialDesc& matDesc) {
	palMaterial::SetParameters(matDesc);
	m_pMaterial1 = pM1;
	m_pMaterial2 = pM2;
}

/*
class palMaterials : public palFactoryObject {
public:
	void NewMaterial(PAL_STRING name, Float static_friction, Float kinetic_friction, Float restitution); //default values
	void SetMaterialInteraction(PAL_STRING name1, PAL_STRING name2, Float static_friction, Float kinetic_friction, Float restitution);
protected:
	vector<PAL_STRING> m_MaterialNames;
	std_matrix<palMaterial *> m_Materials;

	FACTORY_CLASS(palMaterials,palMaterials,All,1);
};
*/

palMaterials::palMaterials() {

};

int palMaterials::GetIndex(PAL_STRING name) {
//	PAL_VECTOR<PAL_STRING>::iterator obj;
//	obj = std::find(m_MaterialNames.begin(), m_MaterialNames.end(), name);
	for (unsigned int i=0;i<m_MaterialNames.size();i++)
		if (m_MaterialNames[i] == name)
			return i;
	return -1;
}

palMaterialUnique *palMaterials::GetMaterial(PAL_STRING name) {
	int pos = GetIndex(name);
	if (pos<0) return NULL;
	palMaterial *pM= m_Materials.Get(pos,pos);
	return dynamic_cast<palMaterialUnique *> (pM);
}

void palMaterials::SetIndex(int posx, int posy, palMaterial *pm) {
	m_Materials.Set(posx,posy,pm);
}

void palMaterials::SetNameIndex(PAL_STRING name) {
	m_MaterialNames.push_back(name);
}

void palMaterials::CombineMaterials(const palMaterialDesc& one, const palMaterialDesc& two, palMaterialDesc& result) {
	result.m_bDisableStrongFriction = one.m_bDisableStrongFriction && two.m_bDisableStrongFriction;
	result.m_fKinetic = one.m_fKinetic * two.m_fKinetic;
	result.m_fStatic = one.m_fStatic * two.m_fStatic;
	result.m_fRestitution = one.m_fRestitution * two.m_fRestitution;
	result.m_bEnableAnisotropicFriction = one.m_bEnableAnisotropicFriction || two.m_bEnableAnisotropicFriction;

	// Combining anisotropic friction makes no sense, so we pick one.
	if (one.m_bEnableAnisotropicFriction) {
		result.m_vKineticAnisotropic = one.m_vKineticAnisotropic;
		result.m_vStaticAnisotropic = one.m_vStaticAnisotropic;
	} else if (two.m_bEnableAnisotropicFriction) {
		result.m_vKineticAnisotropic = two.m_vKineticAnisotropic;
		result.m_vStaticAnisotropic = two.m_vStaticAnisotropic;
	}
}

void palMaterials::NewMaterial(PAL_STRING name, const palMaterialDesc& matDesc) {
	if (GetIndex(name)!=-1) {
		SET_WARNING("Can not replace existing materials!");
		return;
	}

	palFactoryObject *pFO=PF->CreateObject("palMaterialUnique");
	palMaterialUnique *pMU = dynamic_cast<palMaterialUnique *>(pFO);
	if (pMU == NULL) {
		SET_ERROR("Could not create material");
		return;
	}
	pMU->Init(name,matDesc);
	//error?
	SetNameIndex(name);

	int size,check;
	m_Materials.GetDimensions(size,check);
	if (size!=check) {
		SET_ERROR("Material size is non-equal. Might be out of memory");
		return;
	}
	m_Materials.Resize(size+1,size+1);
	//error?
	m_Materials.GetDimensions(size,check);
	if (size!=check) {
		SET_ERROR("Material size is non-equal. Might be out of memory");
		return;
	}
	int pos = GetIndex(name);
	//m_Materials.Set(pos,pos,pMU);
	SetIndex(pos,pos,pMU);

	palMaterialDesc temp;

	// or all the old materials, generate combined versions for the new one.
	for (int i=0; i < size - 1; i++) {
		palMaterialUnique* matUniq = dynamic_cast<palMaterialUnique*>(m_Materials.Get(i,i));
		if (matUniq != NULL) {
			CombineMaterials(*matUniq, *pMU, temp);
			// The matrix handling code forces all othe
			SetMaterialInteraction(matUniq->m_Name, pMU->m_Name, temp);
		}
		else
		{
			// Can't combine them for now if it's not a unique material, although this really shouldn't happen.
			SetIndex(i,pos,pMU);
			SetIndex(pos,i,pMU);
		}
	}
}

void palMaterials::SetMaterialInteraction(PAL_STRING name1, PAL_STRING name2, const palMaterialDesc& matDesc) {
	if (name1==name2) {
		palMaterial *pm=GetMaterial(name1);
		pm->SetParameters(matDesc);
	} else {
		palFactoryObject *pFO=PF->CreateObject("palMaterialInteraction");
		palMaterialInteraction *pMI = dynamic_cast<palMaterialInteraction *>(pFO);
		pMI->Init(GetMaterial(name1),GetMaterial(name2),matDesc);
		int p1=GetIndex(name1);
		int p2=GetIndex(name2);
		SetIndex(p1,p2,pMI);
		SetIndex(p2,p1,pMI);
		//m_Materials.Set(p1,p2,pMI);
		//m_Materials.Set(p2,p1,pMI);
	}
}

palMaterialInteraction *palMaterials::GetMaterialInteraction(PAL_STRING name1, PAL_STRING name2)
{
	int pos1 = GetIndex(name1);
	if (pos1 < 0)
		return NULL;
	int pos2 = GetIndex(name2);
	if (pos2 < 0)
		return NULL;
	palMaterial *pM = m_Materials.Get(pos1, pos2);
	return dynamic_cast<palMaterialInteraction*> (pM);
}

////////////////////////////////////////


void palSphere::GenericInit(palMatrix4x4 &pos, void *param_array) {
	Float *p=(Float *)param_array;
	Init(pos._41,pos._42,pos._43,p[0],p[1]);
	//SetPosition(pos);
}

void palCapsule::GenericInit(palMatrix4x4 &pos, void *param_array) {
	Float *p=(Float *)param_array;
	Init(pos._41,pos._42,pos._43,p[0],p[1],p[2]);
	//SetPosition(pos);
}

void palCompoundBody::GenericInit(palMatrix4x4 &pos, void *param_array) {
	Init(pos._41,pos._42,pos._43);
	//SetPosition(pos);
}

/*
void palBox::GenericInit(void *param, ...) {
	Float p[7];
	va_list args;
	va_start( args, param);

	void *ptr;
	char *szParam;

	p[0]=atof( (char *)param );
	for (int i=1;i<7;i++) {
		ptr = va_arg( args, void *);
		szParam = (char *)ptr;
		p[i]=atof(szParam);
	}
	this->Init(p[0],p[1],p[2], p[3],p[4],p[5], p[6]);
}
*/
/*
void palSphere::GenericInit(void *param, ...) {
	Float p[5];
	va_list args;
	va_start( args, param);

	void *ptr;
	char *szParam;

	p[0]=atof( (char *)param );
	for (int i=1;i<5;i++) {
		ptr = va_arg( args, void *);
		szParam = (char *)ptr;
		p[i]=atof(szParam);
	}
	this->Init(p[0],p[1],p[2], p[3],p[4]);
}
*/

/*
void palCapsule::GenericInit(void *param, ...) {
	Float p[6];
	va_list args;
	va_start( args, param);

	void *ptr;
	char *szParam;

	p[0]=atof( (char *)param );
	for (int i=1;i<6;i++) {
		ptr = va_arg( args, void *);
		szParam = (char *)ptr;
		p[i]=atof(szParam);
	}
	this->Init(p[0],p[1],p[2], p[3],p[4],p[5]);
}
*/


/*
void palTriangleMesh::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palBody::SetPosition(x,y,z);
	m_nVertices=nVertices;
	m_nIndices=nIndices;
	m_pVertices=(float *) pVertices;
	m_pIndices=(int *) pIndices;
}*/

////////////////////////////////////////
////////////////////////////////////////
const FLOAT palPhysicsDesc::DEFAULT_GRAVITY_X = 0.0f;
const FLOAT palPhysicsDesc::DEFAULT_GRAVITY_Y = -9.80665f; //!< Standard gravity, according to NIST Special Publication 330, p. 39
const FLOAT palPhysicsDesc::DEFAULT_GRAVITY_Z = 0.0f;

palPhysicsDesc::palPhysicsDesc()
: m_nUpAxis(1)
{
	m_vGravity.x = DEFAULT_GRAVITY_X;
   m_vGravity.y = DEFAULT_GRAVITY_Y;
   m_vGravity.z = DEFAULT_GRAVITY_Z;
}

////////////////////////////////////////
////////////////////////////////////////
void palPhysics::Init(palPhysicsDesc& desc) {
	m_fGravityX=desc.m_vGravity.x;
	m_fGravityY=desc.m_vGravity.y;
	m_fGravityZ=desc.m_vGravity.z;
	m_nUpAxis=desc.m_nUpAxis;

	// if the up axis is out of bounds, just set it to the default.
	if (m_nUpAxis > 2)
	{
		m_nUpAxis = 1;
	}

	m_Properties=desc.m_Properties;
}

palPhysics::palPhysics()
  : m_bListen(false), m_pMaterials(0), m_fGravityX(0), m_fGravityY(0), m_fGravityZ(0), m_fLastTimestep(0),
    m_fTime(0), m_nUpAxis(0), m_pDebugDraw(0) {
//	m_pCollision = 0;
//	m_pSolver = 0;
}

void palPhysics::Update(Float timestep) {
	if (GetDebugDraw() != NULL) {
		GetDebugDraw()->Clear();
	}
	CallActions(timestep);
	Iterate(timestep);
	m_fTime+=timestep;
	m_fLastTimestep=timestep;
}

palTerrainType palTerrain::GetType() {
	return m_Type;
}

void palPhysics::GetGravity(palVector3& g) {
	g.x = m_fGravityX;
	g.y = m_fGravityY;
	g.z = m_fGravityZ;
}

Float palPhysics::GetTime() {
	return m_fTime;
}

Float palPhysics::GetLastTimestep() {
	return m_fLastTimestep;
}

void palPhysics::SetGroupCollision(palGroup a, palGroup b, bool enabled) {
}

//virtual void NotifyGeometryAdded(palGeometry* pGeom);
//virtual void NotifyBodyAdded(palBodyBase* pBody);
void palPhysics::NotifyGeometryAdded(palGeometry* pGeom) {
	//m_Geometries.push_back(pGeom);
}

void palPhysics::NotifyBodyAdded(palBodyBase* pBody) {
	//m_Bodies.push_back(pBody);
}
