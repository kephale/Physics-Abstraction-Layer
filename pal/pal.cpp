//#include "pal.h"
#include "palFactory.h"

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


void palMaterial::SetParameters(Float static_friction, Float kinetic_friction, Float restitution) {
	m_fStatic = static_friction;
	m_fKinetic = kinetic_friction;
	m_fRestitution = restitution;
}

palMaterialUnique::palMaterialUnique() {
}

void palMaterialUnique::Init(PAL_STRING name,Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterial::SetParameters(static_friction,kinetic_friction,restitution);
	m_Name=name;
}

palMaterialInteraction::palMaterialInteraction() {
	m_pMaterial1=NULL;
	m_pMaterial2=NULL;
}

void palMaterialInteraction::Init(palMaterialUnique *pM1, palMaterialUnique *pM2, Float static_friction, Float kinetic_friction, Float restitution) {
	palMaterial::SetParameters(static_friction,kinetic_friction,restitution);
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

void palMaterials::NewMaterial(PAL_STRING name, Float static_friction, Float kinetic_friction, Float restitution) {
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
	pMU->Init(name,static_friction,kinetic_friction,restitution);
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
	int i;
	for (i=0;i<size;i++) {
		SetIndex(i,pos,pMU);
	//	m_Materials.Set(i,pos,pMU);
	}
	for (i=0;i<size;i++) {
		SetIndex(pos,i,pMU);
		//m_Materials.Set(pos,i,pMU);
	}
}

void palMaterials::SetMaterialInteraction(PAL_STRING name1, PAL_STRING name2, Float static_friction, Float kinetic_friction, Float restitution) {
	if (name1==name2) {
		palMaterial *pm=GetMaterial(name1);
		pm->SetParameters(static_friction,kinetic_friction,restitution);
	} else {
		palFactoryObject *pFO=PF->CreateObject("palMaterialInteraction");
		palMaterialInteraction *pMI = dynamic_cast<palMaterialInteraction *>(pFO);
		pMI->Init(GetMaterial(name1),GetMaterial(name2),static_friction,kinetic_friction,restitution);
		int p1=GetIndex(name1);
		int p2=GetIndex(name2);
		SetIndex(p1,p2,pMI);
		SetIndex(p2,p1,pMI);
		//m_Materials.Set(p1,p2,pMI);
		//m_Materials.Set(p2,p1,pMI);
	}
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
	Float *p=(Float *)param_array;
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
void palPhysics::Init(Float gravity_x, Float gravity_y, Float gravity_z) {
	m_fGravityX=gravity_x;
	m_fGravityY=gravity_y;
	m_fGravityZ=gravity_z;
}

palPhysics::palPhysics() {
	m_fTime=0;
	m_fLastTimestep = 0;
	m_pMaterials = NULL;
	m_bListen = false; //false by default?
//	m_pCollision = 0;
//	m_pSolver = 0;
}

void palPhysics::Update(Float timestep) {
	Iterate(timestep);
	m_fTime+=timestep;
	m_fLastTimestep=timestep;
}

palTerrainType palTerrain::GetType() {
	return m_Type;
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
