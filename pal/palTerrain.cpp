#include "palTerrain.h"
#include "palFactory.h"

/*
	Abstract:
		PAL - Physics Abstraction Layer. 
		Implementation File (terrain)

	Author: 
		Adrian Boeing
	Revision History:
		Version 0.1 :11/12/07 split from pal.cpp
	TODO:
*/

#ifdef MEMDEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

////////////////////////////////////////
palTerrainPlane::palTerrainPlane() {
	m_Type=PAL_TERRAIN_PLANE;
}

void palTerrainPlane::Init(Float x, Float y, Float z, Float min_size) {
	palTerrain::SetPosition(x,y,z);
	m_fSize = min_size;
	m_Type=PAL_TERRAIN_PLANE;
}

Float palTerrainPlane::GetMinimumSize() {
	return m_fSize;
}

void palTerrainPlane::GenerateDefaultBoxGeom(Float height) {
	palFactoryObject *pFO=PF->CreateObject("palBoxGeometry");
	palBoxGeometry *m_pGeom = dynamic_cast<palBoxGeometry *>(pFO); //create the geometry
	m_Geometries.push_back(m_pGeom);
	SetGeometryBody(m_pGeom);

	palMatrix4x4 gloc;
	memcpy(&gloc,&m_mLoc,sizeof(palMatrix4x4));
	gloc._42-=height*0.5f;
	m_pGeom->Init(gloc,m_fSize,height,m_fSize,0);
}

palOrientatedTerrainPlane::palOrientatedTerrainPlane() {
	m_Type=PAL_TERRAIN_PLANE; //todo
}


void palOrientatedTerrainPlane::Init(Float x, Float y, Float z, Float nx, Float ny, Float nz,  Float min_size) {
	palTerrain::SetPosition(x,y,z);
	m_fNormX = nx;
	m_fNormY = ny;
	m_fNormZ = nz;
	m_fSize = min_size;
	CalcualteOrientationMatrixFromNormals();
}

void palOrientatedTerrainPlane::Init(Float a, Float b, Float c, Float d, Float min_size) {
	//closest point on plane to origin:
	//http://en.wikipedia.org/wiki/Point_on_plane_closest_to_origin
	Float sqrmag = a*a+b*b+c*c;
	this->Init(a*d/sqrmag,b*d/sqrmag,c*d/sqrmag,a,b,c,min_size);
}

Float palOrientatedTerrainPlane::GetMinimumSize() {
	return m_fSize;
}

Float palOrientatedTerrainPlane::CalculateD() {
	palVector3 pos;
	palVector3 norm;
	vec_set(&norm,m_fNormX,m_fNormY,m_fNormZ);
	vec_set(&pos,m_fPosX,m_fPosY,m_fPosZ);
	return -vec_dot(&norm,&pos);

}

void palOrientatedTerrainPlane::CalcualteOrientationMatrixFromNormals() {
	palVector3 pivot;
	palVector3 norm1;
	palVector3 norm2;
	vec_set(&norm1,0,1,0);
	vec_set(&norm2,m_fNormX,m_fNormY,m_fNormZ);
	vec_cross(&pivot,&norm1,&norm2);
	//normalized cross product
	vec_norm(&pivot);
	//theta = arccos(a.b / |a||b|)
	Float dot = vec_dot(&norm1,&norm2);
	Float denom = vec_mag(&norm1) * vec_mag(&norm2);
	Float amount = acos(dot/denom);

	//from  haegarr :
	palVector3 b0,b1,b2; //basis vectors
	const Float sine = sin(amount);
	const Float cosine = cos(amount);
	const Float oneMinusCosine = 1.0f-cosine;
	Float temp;
	temp = oneMinusCosine*pivot.x;
	b0.x = pivot.x*temp+cosine;
	b0.y = pivot.y*temp+sine*pivot.z;
	b0.z = pivot.z*temp-sine*pivot.y;
	temp = oneMinusCosine*pivot.y;
	b1.x = pivot.x*temp-sine*pivot.z;
	b1.y = pivot.y*temp+cosine;
	b1.z = pivot.z*temp+sine*pivot.x;
	temp = oneMinusCosine*pivot.z;
	b2.x = pivot.x*temp+sine*pivot.y;
	b2.y = pivot.y*temp-sine*pivot.x;
	b2.z = pivot.z*temp+cosine;

	mat_identity(&m_mLoc);

	m_mLoc._11 = b0.x;
	m_mLoc._12 = b0.y;
	m_mLoc._13 = b0.z;

	m_mLoc._21 = b1.x;
	m_mLoc._22 = b1.y;
	m_mLoc._23 = b1.z;

	m_mLoc._31 = b2.x;
	m_mLoc._32 = b2.y;
	m_mLoc._33 = b2.z;

	mat_set_translation(&m_mLoc,m_fPosX,m_fPosY,m_fPosZ);
}


palMatrix4x4& palOrientatedTerrainPlane::GetLocationMatrix() {
	return m_mLoc;
}


void palTerrainHeightmap::Init(Float x, Float y, Float z, Float width, Float depth, int terrain_data_width, int terrain_data_depth, const Float *pHeightmap) {
	palTerrain::SetPosition(x,y,z);
	m_fWidth = width;
	m_fDepth = depth;
	m_iDataWidth = terrain_data_width;
	m_iDataDepth = terrain_data_depth;
	m_pHeightmap = new Float[m_iDataWidth * m_iDataDepth];
	memcpy(m_pHeightmap,pHeightmap,sizeof(Float) * m_iDataWidth * m_iDataDepth);
	m_Type = PAL_TERRAIN_HEIGHTMAP;
}

palTerrainHeightmap::palTerrainHeightmap() {
	m_pHeightmap  = NULL;
	m_Type = PAL_TERRAIN_HEIGHTMAP;
}

palTerrainHeightmap::~palTerrainHeightmap() {
	if (m_pHeightmap )
		delete m_pHeightmap  ;
}

const Float *palTerrainHeightmap::GetHeightMap() {
	return m_pHeightmap;
}
Float palTerrainHeightmap::GetWidth() {
	return m_fWidth;
}
Float palTerrainHeightmap::GetDepth() {
	return m_fDepth;
}
int palTerrainHeightmap::GetDataWidth() {
	return m_iDataWidth;
}
int palTerrainHeightmap::GetDataDepth() {
	return m_iDataDepth;
}

palTerrainMesh::palTerrainMesh(){
	m_Type=PAL_TERRAIN_MESH;
}
void palTerrainMesh::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palTerrain::SetPosition(x,y,z);
	m_nVertices=nVertices;
	m_nIndices=nIndices;
	m_pVertices=(Float *) pVertices;
	m_pIndices=(int *) pIndices;
}


palTerrain::palTerrain() {
	m_pMaterial=NULL;
	m_Type = PAL_TERRAIN_NONE;
}
///////////////////////////////////////
