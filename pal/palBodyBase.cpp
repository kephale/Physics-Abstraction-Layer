
#include "palBodies.h"
#include "palFactory.h"
#include <memory.h>
#include <sstream>

/*
	Abstract:
		PAL - Physics Abstraction Layer.
		Implementation File (body base)

	Author:
		Adrian Boeing
	Revision History:
		Version 0.1   : 11/12/07 - Original
	TODO:
*/

palBodyBase::palBodyBase() {
	m_pMaterial=NULL;
	memset(&m_mLoc,0,sizeof(palMatrix4x4));
	m_mLoc._11 = 1;
	m_mLoc._22 = 1;
	m_mLoc._33 = 1;
	m_mLoc._44 = 1;
	m_Group = 0;
	m_pUserData = 0;
}

palBodyBase::~palBodyBase() {
	Cleanup();
}

void palBodyBase::SetGeometryBody(palGeometry *pgeom) {
	if (pgeom)
		pgeom->m_pBody=this;
}

void palBodyBase::ClearGeometryBody(palGeometry *pgeom) {
	if (pgeom)
		pgeom->m_pBody=NULL;
}

void palBodyBase::SetMaterial(palMaterial *material) {
	m_pMaterial = material;
}

palGroup palBodyBase::GetGroup() const {
	return m_Group;
}

void palBodyBase::SetGroup(palGroup group) {
	m_Group = group;
}

Float palBodyBase::GetSkinWidth() const {
	return -1.0f; // unsupported
}

bool palBodyBase::SetSkinWidth(Float)
{
	return false;
}

void palBodyBase::SetUserData(void *dataPtr) {
	m_pUserData = dataPtr;
}

void *palBodyBase::GetUserData() const {
	return m_pUserData;
}

void palBodyBase::SetPosition(Float x, Float y, Float z) {
	palMatrix4x4 loc;
	//mat_identity(&loc);
	loc = GetLocationMatrix();
	loc._41=x;
	loc._42=y;
	loc._43=z;
	loc._44=1;
	SetPosition(loc); //call virtual?
}

void palBodyBase::GetPosition(palVector3& res) const {
	const palMatrix4x4& loc(GetLocationMatrix());
	res.x=loc._41;
	res.y=loc._42;
	res.z=loc._43;
}

void palBodyBase::SetPosition(const palMatrix4x4 &location) {
	m_mLoc = location;
}


void palBodyBase::Cleanup() {
	m_Geometries.clear();
}

std::string palBodyBase::toString() const {
	std::string result(palFactoryObject::toString());
	result.append("[");
	std::ostringstream oss;
	oss << GetLocationMatrix();
	result.append(oss.str());
	result.append("]");
	return result;
}

////////////////////////////////////////

void palBoxBase::Init(const palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	SetPosition(pos);

	//create the geom
	palFactoryObject *pFO=PF->CreateObject("palBoxGeometry");
	palBoxGeometry *m_pGeom = dynamic_cast<palBoxGeometry *>(pFO); //create the geometry
	m_Geometries.push_back(m_pGeom);
	SetGeometryBody(m_pGeom);

	//init the geom
	m_pGeom->Init(m_mLoc,width,height,depth,mass);
}

Float palBoxBase::GetWidth() {
	if (m_Geometries.size()>0) {
		palBoxGeometry *m_pGeom = dynamic_cast<palBoxGeometry *>(m_Geometries[0]);
		if (m_pGeom)
			return m_pGeom->m_fWidth;
	}
	return 0;
}

Float palBoxBase::GetHeight() {
	if (m_Geometries.size()>0) {
		palBoxGeometry *m_pGeom = dynamic_cast<palBoxGeometry *>(m_Geometries[0]);
		if (m_pGeom)
			return m_pGeom->m_fHeight;
	}
	return 0;
}

Float palBoxBase::GetDepth() {
	if (m_Geometries.size()>0) {
		palBoxGeometry *m_pGeom = dynamic_cast<palBoxGeometry *>(m_Geometries[0]);
		if (m_pGeom)
			return m_pGeom->m_fDepth;
	}
	return 0;
}

void palConvexBase::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
//	palConvexBase::Init(pos,pVertices,nVertices,mass);
//	palConvexGeometry *m_pGeom = dynamic_cast<palConvexGeometry *>(m_Geometries[0]);
//	if (m_pGeom) {
//		m_pGeom->SetIndices(pIndices,nIndices);
//	}

	SetPosition(pos);

	palFactoryObject *pFO=PF->CreateObject("palConvexGeometry");
	palConvexGeometry *m_pGeom = dynamic_cast<palConvexGeometry *>(pFO); //create the geometry
	m_Geometries.push_back(m_pGeom);
	SetGeometryBody(m_pGeom);
	m_pGeom->Init(m_mLoc,pVertices,nVertices,pIndices, nIndices, mass);
}

void palConvexBase::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	SetPosition(pos);

	palFactoryObject *pFO=PF->CreateObject("palConvexGeometry");
	palConvexGeometry *m_pGeom = dynamic_cast<palConvexGeometry *>(pFO); //create the geometry
	m_Geometries.push_back(m_pGeom);
	SetGeometryBody(m_pGeom);
	m_pGeom->Init(m_mLoc,pVertices,nVertices,mass);
}

void palSphereBase::Init(const palMatrix4x4 &pos, Float radius, Float mass) {
	SetPosition(pos);

	palFactoryObject *pFO=PF->CreateObject("palSphereGeometry");
	palSphereGeometry *m_pGeom = dynamic_cast<palSphereGeometry *>(pFO); //create the geometry
	m_Geometries.push_back(m_pGeom);
	SetGeometryBody(m_pGeom);
	m_pGeom->Init(m_mLoc,radius,mass);
}

void palCapsuleBase::Init(const palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	SetPosition(pos);

	palFactoryObject *pFO=PF->CreateObject("palCapsuleGeometry");
	palCapsuleGeometry *m_pGeom = dynamic_cast<palCapsuleGeometry *>(pFO); //create the geometry
	m_Geometries.push_back(m_pGeom);
	SetGeometryBody(m_pGeom);

	m_pGeom->Init(m_mLoc,radius,length,mass);


}



////////////////////////////
/////////////////////////////

palSphereGeometry *palCompoundBodyBase::AddSphere() {
	palFactoryObject *pFO=PF->CreateObject("palSphereGeometry");
	palSphereGeometry *pGeom = dynamic_cast<palSphereGeometry *>(pFO); //create the geometry
	SetGeometryBody(pGeom);
	m_Geometries.push_back(pGeom);
	return pGeom;
}

palBoxGeometry *palCompoundBodyBase::AddBox() {
	palFactoryObject *pFO=PF->CreateObject("palBoxGeometry");
	palBoxGeometry *pGeom = dynamic_cast<palBoxGeometry *>(pFO); //create the geometry
	SetGeometryBody(pGeom);
	m_Geometries.push_back(pGeom);
	return pGeom;
}

palCapsuleGeometry *palCompoundBodyBase::AddCapsule() {
	palFactoryObject *pFO=PF->CreateObject("palCapsuleGeometry");
	palCapsuleGeometry *pGeom = dynamic_cast<palCapsuleGeometry *>(pFO); //create the geometry
	SetGeometryBody(pGeom);
	m_Geometries.push_back(pGeom);
	return pGeom;
}

palConvexGeometry *palCompoundBodyBase::AddConvex() {
	palFactoryObject *pFO=PF->CreateObject("palConvexGeometry");
	palConvexGeometry *pGeom = dynamic_cast<palConvexGeometry *>(pFO); //create the geometry
	SetGeometryBody(pGeom);
	m_Geometries.push_back(pGeom);
	return pGeom;
}

palGeometry *palCompoundBodyBase::AddGeometry(PAL_STRING type) {
	palFactoryObject *pFO=PF->CreateObject(type);
	palGeometry *pGeom = dynamic_cast<palGeometry *>(pFO); //create the geometry
	SetGeometryBody(pGeom);
	m_Geometries.push_back(pGeom);
	return pGeom;
}
