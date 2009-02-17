#include "palStatic.h"
#include "palFactory.h"
#include "../framework/cast.h"

FACTORY_CLASS_IMPLEMENTATION(palStaticCompoundBody);

void palStaticBox::Init(Float x, Float y, Float z, Float width, Float height, Float depth) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	Init(m,width,height,depth);
}

void palStaticBox::Init(palMatrix4x4 &pos, Float width, Float height, Float depth) {
	palBoxBase::Init(pos,width,height,depth,0);
	m_Type = PAL_STATIC_BOX;
}

void palStaticConvex::Init(Float x, Float y, Float z, const Float *pVertices, int nVertices) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	Init(m,pVertices,nVertices);
}
	
void palStaticConvex::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices) {
	palConvexBase::Init(pos,pVertices,nVertices,0);
	m_Type = PAL_STATIC_CONVEX;
}

void palStaticConvex::Init(palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices) {
	palConvexBase::Init(pos,pVertices,nVertices,pIndices,nIndices,0);
	m_Type = PAL_STATIC_CONVEX;
}

void palStaticCapsule::Init(Float x, Float y, Float z, Float radius, Float length) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	Init(m,radius,length);
}

void palStaticCapsule::Init(palMatrix4x4 &pos, Float radius, Float length) {
	palCapsuleBase::Init(pos,radius,length,0);
	m_Type = PAL_STATIC_CAPSULE;
}


void palStaticSphere::Init(Float x, Float y, Float z, Float radius) {
	palMatrix4x4 m;
	mat_identity(&m);
	mat_translate(&m,x,y,z);
	Init(m,radius);
}

void palStaticSphere::Init(palMatrix4x4 &pos, Float radius) {
	palSphereBase::Init(pos,radius,0);
	m_Type = PAL_STATIC_SPHERE;
}

//////////////

palStaticCompoundBody::palStaticCompoundBody() {
	m_Type = PAL_STATIC_COMPOUND;
}

void palStaticCompoundBody::Init(Float x, Float y, Float z) {
	palBodyBase::SetPosition(x,y,z);
	m_Type = PAL_STATIC_COMPOUND;
}

void palStaticCompoundBody::Init(palMatrix4x4 &pos) {
	palBodyBase::SetPosition(pos);
	m_Type = PAL_STATIC_COMPOUND;
}

//#define 

palMatrix4x4& palStaticCompoundBody::GetLocationMatrix() {
	return m_mLoc;
}

void palStaticCompoundBody::Finalize() {
	for (unsigned int i=0;i<m_Geometries.size();i++) {
//		palStatic *ps;
		switch (m_Geometries[i]->m_Type) {
			case PAL_GEOM_BOX:
				{
					palBoxGeometry *pxg = polymorphic_downcast<palBoxGeometry *>(m_Geometries[i]);
					PF->CreateObjectAs<palStaticBox>("palStaticBox")->Init(m_Geometries[i]->GetLocationMatrix(),
						pxg->m_fWidth,
						pxg->m_fHeight,
						pxg->m_fDepth);
				}
				break;
			case PAL_GEOM_SPHERE:
				{
					palSphereGeometry *pxg = polymorphic_downcast<palSphereGeometry *>(m_Geometries[i]);
					PF->CreateObjectAs<palStaticSphere>("palStaticSphere")->Init(m_Geometries[i]->GetLocationMatrix(),
						pxg->m_fRadius);
				}
				break;
			case PAL_GEOM_CAPSULE:
				{
					palCapsuleGeometry *pxg = polymorphic_downcast<palCapsuleGeometry *>(m_Geometries[i]);
					PF->CreateObjectAs<palStaticCapsule>("palStaticCapsule")->Init(m_Geometries[i]->GetLocationMatrix(),
						pxg->m_fRadius,
						pxg->m_fLength);
				}
				break;

			case PAL_GEOM_CONVEX:
				{
					palConvexGeometry *pxg = polymorphic_downcast<palConvexGeometry *>(m_Geometries[i]);
					PF->CreateObjectAs<palStaticConvex>("palStaticConvex")->Init(m_Geometries[i]->GetLocationMatrix(),
						&pxg->m_vfVertices[0],
						static_cast<int>(pxg->m_vfVertices.size()/3));
				}
				break;
		}
	}

	//m_DefaultFinalizeBodies
}
