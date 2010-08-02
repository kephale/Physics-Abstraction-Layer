#include "palStatic.h"
#include "palFactory.h"

FACTORY_CLASS_IMPLEMENTATION(palStaticCompoundBody);

void palStaticBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	Init(m,width,height,depth);
}

void palStaticBox::Init(const palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palBoxBase::Init(pos,width,height,depth,0);
	m_Type = PAL_STATIC_BOX;
}

void palStaticConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	Init(m,pVertices,nVertices);
}
	
void palStaticConvex::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices) {
	palConvexBase::Init(pos,pVertices,nVertices,0);
	m_Type = PAL_STATIC_CONVEX;
}

void palStaticConvex::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palConvexBase::Init(pos,pVertices,nVertices,pIndices,nIndices,0);
	m_Type = PAL_STATIC_CONVEX;
}

void palStaticCapsule::Init(Float x, Float y, Float z, Float radius, Float length) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	Init(m,radius,length);
}

void palStaticCapsule::Init(const palMatrix4x4 &pos, Float radius, Float length) {
	palCapsuleBase::Init(pos,radius,length,0);
	m_Type = PAL_STATIC_CAPSULE;
}


void palStaticSphere::Init(Float x, Float y, Float z, Float radius) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	Init(m,radius);
}

void palStaticSphere::Init(const palMatrix4x4 &pos, Float radius) {
	palSphereBase::Init(pos,radius,0);
	m_Type = PAL_STATIC_SPHERE;
}

//////////////

palStaticCompoundBody::palStaticCompoundBody(){
	m_Type = PAL_STATIC_COMPOUND;
}

void palStaticCompoundBody::Init(Float x, Float y, Float z) {
	palBodyBase::SetPosition(x,y,z);
	m_Type = PAL_STATIC_COMPOUND;
}

void palStaticCompoundBody::Init(const palMatrix4x4 &pos) {
	palBodyBase::SetPosition(pos);
	m_Type = PAL_STATIC_COMPOUND;
}

//#define 

const palMatrix4x4& palStaticCompoundBody::GetLocationMatrix() const {
	return m_mLoc;
}

void palStaticCompoundBody::Finalize() {
// Geoms already created after they are added, so there is no need to create them again.
//	for (unsigned int i=0;i<m_Geometries.size();i++) {
////		palStatic *ps;
//		switch (m_Geometries[i]->m_Type) {
//			case PAL_GEOM_BOX:
//				{
//					palStaticBox *psx = dynamic_cast<palStaticBox *> (PF->CreateObject("palStaticBox"));
//					palBoxGeometry *pxg = dynamic_cast<palBoxGeometry *>(m_Geometries[i]);
//					psx->Init(m_Geometries[i]->GetLocationMatrix(),
//						pxg->m_fWidth,
//						pxg->m_fHeight,
//						pxg->m_fDepth);
//				}
//				break;
//			case PAL_GEOM_SPHERE:
//				{
//					palStaticSphere *psx = dynamic_cast<palStaticSphere *> (PF->CreateObject("palStaticSphere"));
//					palSphereGeometry *pxg = dynamic_cast<palSphereGeometry *>(m_Geometries[i]);
//					psx->Init(m_Geometries[i]->GetLocationMatrix(),
//						pxg->m_fRadius);
//				}
//				break;
//			case PAL_GEOM_CAPSULE:
//				{
//					palStaticCapsule *psx = dynamic_cast<palStaticCapsule *> (PF->CreateObject("palStaticCapsule"));
//					palCapsuleGeometry *pxg = dynamic_cast<palCapsuleGeometry *>(m_Geometries[i]);
//					psx->Init(m_Geometries[i]->GetLocationMatrix(),
//						pxg->m_fRadius,
//						pxg->m_fLength);
//				}
//				break;
//
//			case PAL_GEOM_CONVEX:
//				{
//					palStaticConvex *psx = dynamic_cast<palStaticConvex *> (PF->CreateObject("palStaticConvex"));
//					palConvexGeometry *pxg = dynamic_cast<palConvexGeometry *>(m_Geometries[i]);
//					psx->Init(m_Geometries[i]->GetLocationMatrix(),
//						&pxg->m_vfVertices[0],
//						(int)(pxg->m_vfVertices.size()/3));
//				}
//				break;
//		}
//	}
}
