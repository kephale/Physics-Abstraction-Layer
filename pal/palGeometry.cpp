#include "palFactory.h"
#include <memory.h>
/*
	Abstract:
		PAL - Physics Abstraction Layer. 
		Implementation File (geom)

	Author: 
		Adrian Boeing
	Revision History:
		Version 0.1 :19/10/07 split from pal.cpp
	TODO:
*/

#ifdef MEMDEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

void palGeometry::GetPosition(palVector3& res) const {
	palMatrix4x4 loc;
	loc = GetLocationMatrix();
	res.x=loc._41;
	res.y=loc._42;
	res.z=loc._43;
}
////////////////////////////////////////
//step1: set the m_pbody
//step2: call init -> 
//		set type
//		set position
//		set properties
//		set mass
//		calculate inertia
//OR: set pos, set properties & mass

palGeometry::palGeometry() {
	m_pBody=NULL;
	mat_identity(&m_mOffset);
	m_nVertices = 0;
	m_nIndices = 0;
	
	m_pVertices = 0;
	m_pIndices = 0;

	m_fInertiaXX = 1;
	m_fInertiaYY = 1;
	m_fInertiaZZ = 1;
}

palBodyBase* palGeometry::GetBaseBody() const {
	return m_pBody;
}

palGeometry::~palGeometry() {
	if (m_pVertices)
		delete [] m_pVertices;
	if (m_pIndices)
		delete [] m_pIndices;
}
/*
void palGeometry::SetPosition(Float x, Float y, Float z) {
	m_fPosX = x;
	m_fPosY = y;
	m_fPosZ = z;
	palMatrix4x4 loc;
	memset(loc._mat,0,sizeof(palMatrix4x4));
	loc._11=1; loc._22=1; loc._33=1; loc._44=1;
	loc._41=x;
	loc._42=y;
	loc._43=z;
	SetPosition(loc);
}
*/

const palMatrix4x4& palGeometry::GetLocationMatrix() const {
	palMatrix4x4 bodyloc=m_pBody->GetLocationMatrix();
	mat_multiply(&m_mLoc,&bodyloc,&m_mOffset);
	return m_mLoc;
}

const palMatrix4x4& palGeometry::GetOffsetMatrix() const {
	return m_mOffset;
}

void palGeometry::ReCalculateOffset() {
	if (m_pBody) {
		palMatrix4x4 temp;
		palMatrix4x4 bodyloc=m_pBody->GetLocationMatrix();
		mat_invert(&temp,&bodyloc);
		mat_multiply(&m_mOffset,&temp,&m_mLoc);
	}
}

void palGeometry::SetPosition(const palMatrix4x4 &location) {
	m_mLoc = location;
	ReCalculateOffset();
}

void palGeometry::SetMass(Float mass) {
	m_fMass = mass;
	CalculateInertia();
}

Float palGeometry::GetMass() const {
	return m_fMass;
}

Float palGeometry::GetMargin() const {
	return -1.0;
}

bool palGeometry::SetMargin(Float margin) {
	return false;
}

/*
void palGeometry::GenericInit(void* params, ...) {
	va_list arg_ptr;
	va_start(params, arg_ptr);
	iGenericInit(params, arg_ptr);
	va_end(arg_ptr);
};
*/
//palSphereGeometry::palSphereGeometry(palBody *pbody) {	m_pBody = pbody;}

//void palSphereGeometry::Init(Float x, Float y, Float z, Float radius, Float mass) {
void palSphereGeometry::Init(const palMatrix4x4 &pos, Float radius, Float mass) {
	m_Type = PAL_GEOM_SPHERE;
	palGeometry::SetPosition(pos);//m_Loc = pos;
//	palGeometry::SetPosition(x,y,z);
	m_fRadius = radius;
	palGeometry::SetMass(mass);
	CalculateInertia();
}

void palSphereGeometry::GenericInit(const palMatrix4x4& pos, const void *param_array) {
	Float *p = (Float *)param_array;
	Init(pos,p[0],p[1]);
}

void palBoxGeometry::GenericInit(const palMatrix4x4& pos, const void *param_array) {
	Float *p = (Float *)param_array;
	Init(pos,p[0],p[1],p[2],p[3]);
}

void palCapsuleGeometry::GenericInit(const palMatrix4x4& pos, const void *param_array) {
	Float *p = (Float *)param_array;
	Init(pos,p[0],p[1],p[2]);
}

/*
void palSphereGeometry::iGenericInit(void *param,va_list arg_ptr) {
}
*/

void palSphereGeometry::CalculateInertia() {
	Float i = 2 * m_fRadius *  m_fRadius / 5;
	m_fInertiaXX = m_fMass * i;
	m_fInertiaYY = m_fMass * i;
	m_fInertiaZZ = m_fMass * i;
}

//palBoxGeometry::palBoxGeometry(palBody *pbody) {	m_pBody = pbody;}

//void palBoxGeometry::Init(Float x, Float y, Float z, Float width, Float height, Float depth, Float mass) {
void palBoxGeometry::Init(const palMatrix4x4 &pos, Float width, Float height, Float depth, Float mass) {
	m_Type = PAL_GEOM_BOX;
	//palGeometry::SetPosition(x,y,z);
	palGeometry::SetPosition(pos);//m_Loc = pos;
	m_fWidth = width;
	m_fHeight = height;
	m_fDepth = depth;
	palGeometry::SetMass(mass);
	CalculateInertia();
}

void palBoxGeometry::CalculateInertia() {
	palVector3 xyz = GetXYZDimensions();
	Float lx = xyz.x;
	Float ly = xyz.y;
	Float lz = xyz.z;
/*	Float i0= 1/(12 * (ly*ly + lz*lz));
	Float i1= 1/(12 * (lx*lx + lz*lz));
	Float i2= 1/(12 * (lx*lx + ly*ly));*/

	Float i0= (1/Float(12)) * (ly*ly + lz*lz);
	Float i1= (1/Float(12)) * (lx*lx + lz*lz);
	Float i2= (1/Float(12)) * (lx*lx + ly*ly);

	m_fInertiaXX = m_fMass * i0;
	m_fInertiaYY = m_fMass * i1;
	m_fInertiaZZ = m_fMass * i2;
}

palVector3 palBoxGeometry::GetXYZDimensions() const {
	palAxis upAxis = palFactory::GetInstance()->GetActivePhysics()->GetUpAxis();
	palVector3 result;
	result[upAxis] = m_fHeight;
	switch (upAxis) {
	case PAL_X_AXIS:
		result.y = m_fDepth;
		result.z = m_fWidth;
		break;
	case PAL_Y_AXIS:
		result.x = m_fWidth;
		result.z = m_fDepth;
		break;
	case PAL_Z_AXIS:
		result.x = m_fWidth;
		result.y = m_fDepth;
		break;
	default:
		throw new palException("Invalid axis is 'up'. This should never happen.");
	}
	return result;
}

/*
   float mass = 3.0f; 
   float Ixx = 0.7f * mass * (y * y + z * z) / 12.0f; 
   float Iyy = 0.7f * mass * (x * x + z * z) / 12.0f; 
   float Izz = 0.7f * mass * (x * x + y * y) / 12.0f; 
   NewtonBodySetMassMatrix (body, mass, Ixx, Iyy, Izz);
*/

//void palCapsuleGeometry::Init(Float x, Float y, Float z, Float radius, Float length, Float mass) {
void palCapsuleGeometry::Init(const palMatrix4x4 &pos, Float radius, Float length, Float mass) {
	m_Type = PAL_GEOM_CAPSULE;
//	palGeometry::SetPosition(x,y,z);
	palGeometry::SetPosition(pos);//m_Loc = pos;
	m_fRadius = radius;
	m_fLength = length;
	palGeometry::SetMass(mass);
	CalculateInertia();
}

void palCapsuleGeometry::CalculateInertia() {
	Float d = m_fRadius*2;
	Float i0 = 1/Float(48) * (3 * d * d + 4 * m_fLength * m_fLength);
	Float i2 = d * d / Float(8);
	m_fInertiaXX = m_fMass * i0;
	m_fInertiaYY = m_fMass * i0;
	m_fInertiaZZ = m_fMass * i2;
}

void palConvexGeometry::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
	m_Type = PAL_GEOM_CONVEX;
	palGeometry::SetPosition(pos);//m_Loc = pos;
	palGeometry::SetMass(mass);
	m_vfVertices.resize(nVertices*3);
	for (int i=0;i<nVertices*3;i++) {
		m_vfVertices[i] = pVertices[i];
	}
	CalculateInertia();
	SetIndices(pIndices,nIndices);
}


void palConvexGeometry::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, Float mass) {
	m_Type = PAL_GEOM_CONVEX;
	palGeometry::SetPosition(pos);//m_Loc = pos;
	palGeometry::SetMass(mass);
	m_vfVertices.resize(nVertices*3);
	for (int i=0;i<nVertices*3;i++) {
		m_vfVertices[i] = pVertices[i];
	}
	CalculateInertia();
}

void palConvexGeometry::SetIndices(const int *pIndices, int nIndices) {
	if (m_pIndices)
		delete [] m_pIndices;
	m_nIndices = nIndices;
	m_pIndices = new int[m_nIndices];
	memcpy(m_pIndices,pIndices,sizeof(int)*m_nIndices);
}

//from http://www.geometrictools.com/Documentation/PolyhedralMassProperties.pdf
void palConvexGeometry::Subexpressions(Float &w0,Float &w1,Float &w2,Float &f1,Float &f2,Float &f3,Float &g0,Float &g1,Float &g2) {
	Float temp0 = w0+w1; 
	f1 = temp0+w2; 
	Float temp1 = w0*w0; 
	Float temp2 = temp1+w1*temp0;
	f2 = temp2+w2*f1; 
	f3 = w0*temp1+w1*temp2+w2*f2;
	g0 = f2+w0*(f1+w0); 
	g1 = f2+w1*(f1+w1); 
	g2 = f2+w2*(f1+w2);
}

void palConvexGeometry::ComputeIntegral(palVector3 p[], int tmax, int index[], Float& mass, palVector3& cm) {
	const Float mult[10] = {1/6,1/24,1/24,1/24,1/60,1/60,1/60,1/120,1/120,1/120};
	Float intg[10] = {0,0,0,0,0,0,0,0,0,0}; // order: 1, x, y, z, x^2, y^2, z^2, xy, yz, zx
	for (int t = 0; t < tmax; t++)
	{
		// get vertices of triangle t
		int i0 = index[3*t]; int i1 = index[3*t+1]; int i2 = index[3*t+2];
		Float x0 = p[i0].x; Float y0 = p[i0].y; Float z0 = p[i0].z;
		Float x1 = p[i1].x; Float y1 = p[i1].y; Float z1 = p[i1].z;
		Float x2 = p[i2].x; Float y2 = p[i2].y; Float z2 = p[i2].z;
		// get edges and cross product of edges
		Float a1 = x1-x0; Float b1 = y1-y0; Float c1 = z1-z0; Float a2 = x2-x0; Float b2 = y2-y0; Float c2 = z2-z0;
		Float d0 = b1*c2-b2*c1; Float d1 = a2*c1-a1*c2; Float d2 = a1*b2-a2*b1;
		// compute integral terms
		Float f1x,f2x,f3x,g0x,g1x,g2x;
		Float f1y,f2y,f3y,g0y,g1y,g2y;
		Float f1z,f2z,f3z,g0z,g1z,g2z;
		Subexpressions(x0,x1,x2,f1x,f2x,f3x,g0x,g1x,g2x);
		Subexpressions(y0,y1,y2,f1y,f2y,f3y,g0y,g1y,g2y);
		Subexpressions(z0,z1,z2,f1z,f2z,f3z,g0z,g1z,g2z);
		// update integrals
		intg[0] += d0*f1x;
		intg[1] += d0*f2x; intg[2] += d1*f2y; intg[3] += d2*f2z;
		intg[4] += d0*f3x; intg[5] += d1*f3y; intg[6] += d2*f3z;
		intg[7] += d0*(y0*g0x+y1*g1x+y2*g2x);
		intg[8] += d1*(z0*g0y+z1*g1y+z2*g2y);
		intg[9] += d2*(x0*g0z+x1*g1z+x2*g2z);
	}

	for (int i = 0; i < 10; i++)
		intg[i] *= mult[i];
	mass = intg[0];
	// center of mass
	cm.x = intg[1]/mass;
	cm.y = intg[2]/mass;
	cm.z = intg[3]/mass;
	// inertia tensor relative to center of mass

	m_fInertiaXX = intg[5]+intg[6]-mass*(cm.y*cm.y+cm.z*cm.z);
	m_fInertiaYY = intg[4]+intg[6]-mass*(cm.z*cm.z+cm.x*cm.x);
	m_fInertiaZZ = intg[4]+intg[5]-mass*(cm.x*cm.x+cm.y*cm.y);

//	inertia.xx = intg[5]+intg[6]-mass*(cm.y*cm.y+cm.z*cm.z);
//	inertia.yy = intg[4]+intg[6]-mass*(cm.z*cm.z+cm.x*cm.x);
//	inertia.zz = intg[4]+intg[5]-mass*(cm.x*cm.x+cm.y*cm.y);
//	inertia.xy = -(intg[7]-mass*cm.x*cm.y);
//	inertia.yz = -(intg[8]-mass*cm.y*cm.z);
//	inertia.xz = -(intg[9]-mass*cm.z*cm.x);
}

void palConvexGeometry::CalculateInertia() {
	m_fInertiaXX = 1;
	m_fInertiaYY = 1;
	m_fInertiaZZ = 1;
}

////////////////////////
palConcaveGeometry::palConcaveGeometry()
: m_pUntransformedVertices(NULL)
{

}

palConcaveGeometry::~palConcaveGeometry() {
	delete m_pUntransformedVertices;
}

void palConcaveGeometry::Init(const palMatrix4x4 &pos, const Float *pVertices, int nVertices, const int *pIndices, int nIndices, Float mass) {
	m_Type = PAL_GEOM_CONCAVE;
	palGeometry::SetPosition(pos);//m_Loc = pos;
	palGeometry::SetMass(mass);
	m_nVertices=nVertices;
	m_nIndices=nIndices;
	m_pUntransformedVertices= new Float[nVertices * 3];
	for (int i = 0; i < nVertices * 3; ++i) {
		m_pUntransformedVertices[i] = pVertices[i];
	}
	m_pIndices=new int[nIndices];
	for (int i = 0; i < nIndices; ++i) {
		m_pIndices[i] = pIndices[i];
	}
	CalculateInertia();
}
void palConcaveGeometry::CalculateInertia() {
	m_fInertiaXX = 1;
	m_fInertiaYY = 1;
	m_fInertiaZZ = 1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//mesh generation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Float *palGeometry::GenerateMesh_Vertices() {
	if (m_pVertices == 0)
		return 0;
	return m_pVertices;
}

int *palGeometry::GenerateMesh_Indices() {
	if (m_pIndices == 0)
		return 0;
	return m_pIndices;
}
int palGeometry::GetNumberOfVertices() const {
	return m_nVertices;
}

int palGeometry::GetNumberOfIndices() const {
	return m_nIndices;
}


Float *palBoxGeometry::GenerateMesh_Vertices() {
	if (m_pVertices)
		return m_pVertices;
  const Float cube_vertices[] =
	{
		-0.5,  0.5,  0.5,
		-0.5,  -0.5,  0.5,
		-0.5,  -0.5,  -0.5,
		-0.5,  0.5,  -0.5,
		0.5,  -0.5,  0.5,
		0.5,  -0.5,  -0.5,
		0.5,  0.5,  0.5,
		0.5,  0.5,  -0.5
	};
	m_nVertices = 8;
	m_pVertices = new Float[m_nVertices*3];
	int i;
	for (i=0;i<m_nVertices;i++) {
		palVector3 v;
		v._vec[0] = cube_vertices[i*3+0]*m_fWidth;
		v._vec[1] = cube_vertices[i*3+1]*m_fHeight;
		v._vec[2] = cube_vertices[i*3+2]*m_fDepth;
		palVector3 r;
		vec_mat_transform(&r,&m_mOffset,&v);
		m_pVertices[i*3+0] = r.x;
		m_pVertices[i*3+1] = r.y;
		m_pVertices[i*3+2] = r.z;
	}
	return m_pVertices;
}
int *palBoxGeometry::GenerateMesh_Indices() {
	static const int faces[] =
	{
		0, 2, 1,
		0, 3, 2,
		0, 1, 4,
		6, 0, 4,
		4, 5, 6,
		5, 7, 6,
		2, 3, 5,
		5, 3, 7, 
		0, 6, 3, 
		3, 6, 7, 
		1, 2, 4, 
		4, 2, 5
	};
	m_nIndices = 12*3;
	return const_cast<int *>(faces);
}
int palBoxGeometry::GetNumberOfVertices() const {
	return 8;
}
int palBoxGeometry::GetNumberOfIndices() const {
	return 12*3;
}

////////////////////////////////////////////////////////////////////////////////



palSphereGeometry::palSphereGeometry() {
	hstrip = 10;
	vslice = 10;

	m_nVertices = hstrip*vslice*6;
	m_nIndices = hstrip*vslice*6;
	
}

void palSphereGeometry::push_back3(Float *v,Float x, Float y, Float z) {
	v[tppos++] = x;
	v[tppos++] = y;
	v[tppos++] = z;
}

Float *palSphereGeometry::GenerateMesh_Vertices() {
	if (m_pVertices)
		return m_pVertices;
	tppos=0;
	float fX1, fY1, fX2, fY2, fX3, fY3, fX4, fY4;	// The vertex positions around each quad we calculate
	float fAngle,fY,fYNext;
	float fRadius=m_fRadius;
	float fHeight=0;
	float fAngleAdd = 360.0f / (float)vslice;
	float fSineAngle = 0;
	float fSineAdd = 180.0f / (hstrip-1);
	int i,j;

	m_pVertices = new Float[m_nVertices*3];
	Float *verts = new Float[m_nVertices*3];
	// Loop around our sphere
	for (i=0; i<hstrip; i++)
	{
		// Reset the angle for this slice
		fAngle = 0;

		fY = cosf(fSineAngle * (Float)DEG2RAD) * fRadius;
		fYNext = cosf((fSineAngle+fSineAdd) * (Float)DEG2RAD) * fRadius;

		// If we're above the midpoint, add half the height to the vertex positions.
		// Otherwise subtract half the height.
		if (i<=(hstrip/2)-1)
			fY += fHeight/2;
		else
			fY -= fHeight/2;
		if (i<=(hstrip/2)-2)
			fYNext += fHeight/2;
		else
			fYNext -= fHeight/2;

		for (j=0; j<vslice; j++)
		{
			// Calculate the X and Y position for the sphere (as if it were a circle viewed from above)
			fX1 = sinf(fAngle * (Float)DEG2RAD) * fRadius * sinf(fSineAngle * (Float)DEG2RAD);
			fY1 = cosf(fAngle * (Float)DEG2RAD) * fRadius * sinf(fSineAngle * (Float)DEG2RAD);
			fX2 = sinf((fAngle+fAngleAdd) * (Float)DEG2RAD) * fRadius * sinf(fSineAngle * (Float)DEG2RAD);
			fY2 = cosf((fAngle+fAngleAdd) * (Float)DEG2RAD) * fRadius * sinf(fSineAngle * (Float)DEG2RAD);
			fX3 = sinf(fAngle * (Float)DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * (Float)DEG2RAD);
			fY3 = cosf(fAngle * (Float)DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * (Float)DEG2RAD);
			fX4 = sinf((fAngle+fAngleAdd) * (Float)DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * (Float)DEG2RAD);
			fY4 = cosf((fAngle+fAngleAdd) * (Float)DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * (Float)DEG2RAD);
			fAngle += fAngleAdd;
		
		push_back3(verts, fX1, fY    , fY1);
		push_back3(verts, fX4, fYNext, fY4);
		push_back3(verts, fX2, fY    , fY2);
		push_back3(verts, fX1, fY    , fY1);
		push_back3(verts, fX3, fYNext, fY3);
		push_back3(verts, fX4, fYNext, fY4);
		
		}


		fSineAngle += fSineAdd;
	}


	for (i=0;i<m_nVertices;i++) {
		palVector3 v;
		v._vec[0] = verts[i*3+0];
		v._vec[1] = verts[i*3+1];
		v._vec[2] = verts[i*3+2];
		palVector3 r;
		vec_mat_transform(&r,&m_mOffset,&v);
		m_pVertices[i*3+0] = r.x;
		m_pVertices[i*3+1] = r.y;
		m_pVertices[i*3+2] = r.z;
	}

	delete [] verts;

	return m_pVertices;
}
int *palSphereGeometry::GenerateMesh_Indices() {
	if (m_pIndices)
		return m_pIndices;
	m_pIndices = new int[m_nIndices];
	int i;
	for (i=0;i<m_nIndices;i++) 
		m_pIndices[i]=i;
	return m_pIndices;
}



////////////////////////////////////////////////////////////////////////////////
palCapsuleGeometry::palCapsuleGeometry() {
	hstrip = 10;
	vslice = 10;

	m_nVertices = hstrip*vslice*6;
	m_nIndices = hstrip*vslice*6;
	
}

void palCapsuleGeometry::push_back3(Float *v,Float x, Float y, Float z) {
	palAxis upAxis = palFactory::GetInstance()->GetActivePhysics()->GetUpAxis();
	switch (upAxis) {
	case PAL_X_AXIS:
		v[tppos++] = y;
		v[tppos++] = -x;
		v[tppos++] = z;
		break;
	case PAL_Y_AXIS:
		v[tppos++] = x;
		v[tppos++] = y;
		v[tppos++] = z;
		break;
	case PAL_Z_AXIS:
		v[tppos++] = -x;
		v[tppos++] = z;
		v[tppos++] = y;
		break;
	default:
		throw new palException("Invalid axis is 'up'. This should never happen.");
	}
}
Float *palCapsuleGeometry::GenerateMesh_Vertices() {
	if (m_pVertices)
		return m_pVertices;
	tppos=0;
	float fX1, fY1, fX2, fY2, fX3, fY3, fX4, fY4;	// The vertex positions around each quad we calculate
	float fAngle,fY,fYNext;
	float fRadius=m_fRadius;
	float fHeight=m_fLength;
	float fAngleAdd = 360.0f / (float)vslice;
	float fSineAngle = 0;
	float fSineAdd = 180.0f / (hstrip-1);
	int i,j;

	m_pVertices = new Float[m_nVertices*3];
	Float *verts = new Float[m_nVertices*3];
	// Loop around our sphere
	for (i=0; i<hstrip; i++)
	{
		// Reset the angle for this slice
		fAngle = 0;

		fY = cosf(fSineAngle * (Float)DEG2RAD) * fRadius;
		fYNext = cosf((fSineAngle+fSineAdd) * (Float)DEG2RAD) * fRadius;

		// If we're above the midpoint, add half the height to the vertex positions.
		// Otherwise subtract half the height.
		if (i<=(hstrip/2)-1)
			fY += fHeight/2;
		else
			fY -= fHeight/2;
		if (i<=(hstrip/2)-2)
			fYNext += fHeight/2;
		else
			fYNext -= fHeight/2;

		for (j=0; j<vslice; j++)
		{
			// Calculate the X and Y position for the sphere (as if it were a circle viewed from above)
			fX1 = sinf(fAngle * (Float)DEG2RAD) * fRadius * sinf(fSineAngle * (Float)DEG2RAD);
			fY1 = cosf(fAngle * (Float)DEG2RAD) * fRadius * sinf(fSineAngle * (Float)DEG2RAD);
			fX2 = sinf((fAngle+fAngleAdd) * (Float)DEG2RAD) * fRadius * sinf(fSineAngle * (Float)DEG2RAD);
			fY2 = cosf((fAngle+fAngleAdd) * (Float)DEG2RAD) * fRadius * sinf(fSineAngle * (Float)DEG2RAD);
			fX3 = sinf(fAngle * (Float)DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * (Float)DEG2RAD);
			fY3 = cosf(fAngle * (Float)DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * (Float)DEG2RAD);
			fX4 = sinf((fAngle+fAngleAdd) * (Float)DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * (Float)DEG2RAD);
			fY4 = cosf((fAngle+fAngleAdd) * (Float)DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * (Float)DEG2RAD);
			fAngle += fAngleAdd;
		
		push_back3(verts, fX1, fY    , fY1);
		push_back3(verts, fX4, fYNext, fY4);
		push_back3(verts, fX2, fY    , fY2);
		push_back3(verts, fX1, fY    , fY1);
		push_back3(verts, fX3, fYNext, fY3);
		push_back3(verts, fX4, fYNext, fY4);
		
		}


		fSineAngle += fSineAdd;
	}


	for (i=0;i<m_nVertices;i++) {
		palVector3 v;
		v._vec[0] = verts[i*3+0];
		v._vec[1] = verts[i*3+1];
		v._vec[2] = verts[i*3+2];
		palVector3 r;
		vec_mat_transform(&r,&m_mOffset,&v);
		m_pVertices[i*3+0] = r.x;
		m_pVertices[i*3+1] = r.y;
		m_pVertices[i*3+2] = r.z;
	}

	delete [] verts;

	return m_pVertices;
}
int *palCapsuleGeometry::GenerateMesh_Indices() {
	if (m_pIndices)
		return m_pIndices;
	m_pIndices = new int[m_nIndices];
	int i;
	for (i=0;i<m_nIndices;i++) 
		m_pIndices[i]=i;
	return m_pIndices;
}


////////////////////////////////////////////////////////////////////////////////
#include "../pal_i/hull.h"

Float *palConvexGeometry::GenerateMesh_Vertices() {
	if (m_pVertices)
		return m_pVertices;
	m_nVertices = GetNumberOfVertices();

	m_pVertices = new Float[m_nVertices*3];

	int i;
	for (i=0;i<m_nVertices;i++) {
		palVector3 v;
		v._vec[0] = m_vfVertices[i*3+0];
		v._vec[1] = m_vfVertices[i*3+1];
		v._vec[2] = m_vfVertices[i*3+2];
		palVector3 r;
		vec_mat_transform(&r,&m_mOffset,&v);
		m_pVertices[i*3+0] = r.x;
		m_pVertices[i*3+1] = r.y;
		m_pVertices[i*3+2] = r.z;
	}

	return m_pVertices;
}

void palConvexGeometry::GenerateHull_Indices(const Float *const srcVerts, const int nVerts, int **pIndices, int& nIndices) {
	HullDesc desc;
	desc.SetHullFlag(QF_TRIANGLES);
	desc.mVcount       = nVerts;
	desc.mVertices     = new double[desc.mVcount*3];
	for (unsigned int  i=0; i<desc.mVcount; i++)
	{
		desc.mVertices[i*3+0] = srcVerts[i*3+0];
		desc.mVertices[i*3+1] = srcVerts[i*3+1];
		desc.mVertices[i*3+2] = srcVerts[i*3+2];
	}
	desc.mVertexStride = sizeof(double)*3;

	HullResult dresult;
	HullLibrary hl;
	// TODO should something be done if this returns an error?
	/*HullError ret = */
	hl.CreateConvexHull(desc,dresult);

	nIndices = dresult.mNumFaces*3;
	*pIndices = new int[nIndices];
	memcpy(*pIndices,dresult.mIndices,sizeof(int)*nIndices);
}

int *palConvexGeometry::GenerateMesh_Indices(){
	if (m_pIndices)
		return m_pIndices;
	
	palConvexGeometry::GenerateHull_Indices(&m_vfVertices[0],GetNumberOfVertices(),&m_pIndices,m_nIndices);
	return m_pIndices;
}

int palConvexGeometry::GetNumberOfVertices() const {
	return (int)(m_vfVertices.size()/3);
}

Float *palConcaveGeometry::GenerateMesh_Vertices() {
	if (m_pVertices)
		return m_pVertices;

	m_pVertices = new Float[m_nVertices*3];

	for (int i=0;i<m_nVertices;i++) {
		palVector3 v;
		v[0] = m_pUntransformedVertices[i*3+0];
		v[1] = m_pUntransformedVertices[i*3+1];
		v[2] = m_pUntransformedVertices[i*3+2];
		palVector3 r;
		vec_mat_transform(&r,&m_mOffset,&v);
		m_pVertices[i*3+0] = r.x;
		m_pVertices[i*3+1] = r.y;
		m_pVertices[i*3+2] = r.z;
	}

	return m_pVertices;
}

