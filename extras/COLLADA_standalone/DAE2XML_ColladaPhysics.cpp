#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <float.h>
#include <malloc.h>
#include <math.h>

#include "DAE2XML_ColladaPhysics.h"
#include "DAE2XML_TinyXML.h"
#include "DAE2XML_Asc2Bin.h"
#include "DAE2XML_Hull.h"

#include "pal/palFactory.h"

#define USE_PAL_GRAPHICS

#ifdef USE_PAL_GRAPHICS
#include "example/graphics.h"
#endif


namespace DAE2XML
{

// A simple array template class to avoid dragging in all of STL as a container.

template<class ElemType > class CARRAY
{
public:

	typedef CARRAY<ElemType> MyType;
	typedef ElemType * Iterator;
	typedef const ElemType * ConstIterator;

	typedef Iterator iterator;
	typedef ConstIterator const_iterator;

	CARRAY() : first(0), last(0), memEnd(0) {	}
	CARRAY(unsigned n, const ElemType& v = ElemType() ) {	first = allocate(n); fill(first, n, v);	memEnd = last = first + n; }
	CARRAY(const MyType & other)	{	first = allocate(other.size());	memEnd = last = copy(other.begin(), other.end(), first);	}
	 ~CARRAY() {	destroy(first, last);	deallocate(first); first = 0;	last = 0;	memEnd = 0;	}

	MyType& operator=(const MyType& other)
 	{
  	if (this != &other)
 	  {
  		if (other.size() <= size())
 		  {
  			Iterator s = copy(other.begin(), other.end(), first);
  			destroy(s, last);
  			last = first + other.size();
 		  }
  		else if (other.size() <= capacity())
 		  {
  			ConstIterator s = other.begin() + size();
  			copy(other.begin(), s, first);
  			copy(s, other.end(), last);
  			last = first + other.size();
 		  }
  		else
 		  {
  			destroy(first, last);
  			deallocate(first);
  			first = allocate(other.size());
  			last = copy(other.begin(), other.end(), first);
  			memEnd = last;
 		  }
 		}
  	return *this;
 	}

	void reserve(unsigned n)
	{
	  if (capacity() < n)
		{
			iterator s = allocate(n);
			copy(first, last, s);
			destroy(first, last);
			deallocate(first);
			//if we knew we just have atomics we could just do this:
			//first = reallocate(n, first);
			memEnd = s + n;
			last = s + size();
			first = s;
		}
	}

	unsigned capacity() const	{	return (unsigned)(first == 0 ? 0 : memEnd - first);	}

	void resize(unsigned n, const ElemType& v = ElemType())
  {
    if (size() < n)
			insert(end(), n - size(), v);
		else if (n < size())
			erase(begin() + n, end());

		if (first == last)
		{
			deallocate(first);
			first = 0;
			last = 0;
			memEnd = 0;
		}

		if (memEnd > last) //release unused memory
		{
			size_t s = (size_t)(last - first);
			first = reallocate(size(), first);
			memEnd = last  = first + s;
		}
	}

	unsigned size() const	{	return (unsigned)(last - first);	}
	bool isEmpty() const {	return (size() == 0);	}
  bool empty() const	{	return isEmpty();	}

	void pushBack(const ElemType& x)
	{
		if (memEnd <= last)
		{
			reserve((1 + size()) * 2);
		}
		*last = x;
		last ++;
	}

	ElemType & pushBack()
	{
		if (memEnd <= last)
		{
		  reserve((1 + size()) * 2);
		}
		last ++;
		return *(last-1);
	}

	void push_back(const ElemType& x) { pushBack(x);	}
	void popBack()	{	--last;	last->~ElemType();	}
	void pop_back() { popBack();	}
	void clear()	{	destroy(begin(), end());	last = first;	}

	void replaceWithLast(unsigned position)
	{
	  assert(position<size());
	  if (position!=size()-1)
		{
		  ElemType& elem = back();
			(*this)[position] = elem;
		}
		popBack();
	}

  const ElemType & operator[](unsigned n) const {	return *(first + n);	}
	ElemType & operator[](unsigned n)	{	return *(first + n);	}
  Iterator begin() {	return first;	}
	ConstIterator begin() const	{	return first;	}
	Iterator end() {	return last; }
  ConstIterator end() const { return last;	}
	ElemType & front() { assert(last!=first); return *first;	}
	const ElemType & front() const { assert(last!=first); return *first;	}
	ElemType & back()	{	assert(last!=first);	return *(last-1);	}
  const ElemType & back() const	{	assert(last!=first);	return *(last-1);	}

	void insert(Iterator where, unsigned n, const ElemType & x)
	{
	  ElemType tmp = x;

		if (n == 0) return;
		if (capacity() < size() + n)
		{
	  	size_t pos = where - first;
			reserve((n + size()) * 2);
			where = first + pos;
		}
		iterator stop = where + n;
		copy(where,last,stop);
		fill(where,n,x);
		last = last + n;
	}

	void erase(Iterator from, Iterator to)
	{
	  if (to < last)
		{
		  copy(to, last, from);
		}
	  last = last - (to - from);
	}

	void erase(Iterator from)	{	last = from;	}

private:
	void fill(Iterator f, unsigned n, const ElemType & x)	{	for (; 0 < n; --n, ++f)	*f = x;	}
  Iterator copy(ConstIterator f, ConstIterator l,	Iterator p)	{	for (; f != l; ++p, ++f) *p = *f;	return p;	}
	ElemType * allocate(size_t n)	{	return (ElemType *)malloc(n * sizeof(ElemType)); }
	ElemType * reallocate(size_t n, ElemType *old) { return (ElemType *)realloc(old, n * sizeof(ElemType));	}
  void deallocate(ElemType *p) { if(p)	free(p);	}
	void destroy(Iterator f, Iterator l) {	for (; f != l; ++f) { f->~ElemType();	}	}

	Iterator first, last, memEnd;

};


const float DEG_TO_RAD = ((2.0f * 3.14152654f) / 360.0f);
const float RAD_TO_DEG = (360.0f / (2.0f * 3.141592654f));


class C_Vec3
{
public:
	C_Vec3(void) { };
	C_Vec3(float _x,float _y,float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

  C_Vec3& C_Vec3::operator +=(const C_Vec3& v)
	{
	  x += v.x;
  	y += v.y;
  	z += v.z;
  	return *this;
	}

	void set(float _x,float _y,float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

  float x;
  float y;
  float z;
};

class NxPlane
{
public:
	C_Vec3	normal;
	float   d;
};

class C_Quat
{
public:
	C_Quat(void)
	{
	}

	C_Quat(float angle,const C_Vec3 &axis)
	{
		fromAngleAxis(angle,axis);
	}

	void fromAngleAxis(float Angle,const C_Vec3 &axis)
	{
  	x = axis.x;
  	y = axis.y;
  	z = axis.z;

  	// required: Normalize the axis

  	const float i_length =  float(1.0) / sqrtf( x*x + y*y + z*z );

  	x = x * i_length;
  	y = y * i_length;
  	z = z * i_length;

  	// now make a clQuaternionernion out of it
  	float Half = DEG_TO_RAD*(Angle * float(0.5));

  	w = cosf(Half);//this used to be w/o deg to rad.
  	const float sin_theta_over_two = sinf(Half );
  	x = x * sin_theta_over_two;
  	y = y * sin_theta_over_two;
  	z = z * sin_theta_over_two;
	}

  void setXYZW(float _x,float _y,float _z,float _w)
  {
  	x = _x;
  	y = _y;
  	z = _z;
  	w = _w;
  }

	void setXYZW(const float *p)
	{
		x = p[0];
		y = p[1];
		z = p[2];
		w = p[3];
	}

  float x;
  float y;
  float z;
  float w;
};

class C_Matrix33
{
public:
	C_Matrix33(void)
	{
	}

  C_Matrix33(const C_Quat &q)
  {
  	fromQuat(q);
  }

  void id(void)
  {
  	matrix[0][0] = float(1.0);
	  matrix[0][1] = float(0.0);
  	matrix[0][2] = float(0.0);

	  matrix[1][0] = float(0.0);
  	matrix[1][1] = float(1.0);
  	matrix[1][2] = float(0.0);

  	matrix[2][0] = float(0.0);
  	matrix[2][1] = float(0.0);
  	matrix[2][2] = float(1.0);


  }

	void getRowMajor(float *d) const
	{
  	d[0] = matrix[0][0];
  	d[1] = matrix[0][1];
  	d[2] = matrix[0][2];

  	d[3] = matrix[1][0];
  	d[4] = matrix[1][1];
  	d[5] = matrix[1][2];

  	d[6] = matrix[2][0];
  	d[7] = matrix[2][1];
  	d[8] = matrix[2][2];
	}

	void setRowMajor(const float *d)
	{
  	matrix[0][0] = d[0];
  	matrix[0][1] = d[1];
  	matrix[0][2] = d[2];

  	matrix[1][0] = d[3];
  	matrix[1][1] = d[4];
  	matrix[1][2] = d[5];

  	matrix[2][0] = d[6];
  	matrix[2][1] = d[7];
  	matrix[2][2] = d[8];
	}


  void multiply(const C_Matrix33& left, const C_Matrix33& right)
	{
	  float a,b,c, d,e,f, g,h,i;

  	//note: temps needed so that x.multiply(x,y) works OK.

  	a =left.matrix[0][0] * right.matrix[0][0] +left.matrix[0][1] * right.matrix[1][0] +left.matrix[0][2] * right.matrix[2][0];
  	b =left.matrix[0][0] * right.matrix[0][1] +left.matrix[0][1] * right.matrix[1][1] +left.matrix[0][2] * right.matrix[2][1];
  	c =left.matrix[0][0] * right.matrix[0][2] +left.matrix[0][1] * right.matrix[1][2] +left.matrix[0][2] * right.matrix[2][2];

	  d =left.matrix[1][0] * right.matrix[0][0] +left.matrix[1][1] * right.matrix[1][0] +left.matrix[1][2] * right.matrix[2][0];
  	e =left.matrix[1][0] * right.matrix[0][1] +left.matrix[1][1] * right.matrix[1][1] +left.matrix[1][2] * right.matrix[2][1];
  	f =left.matrix[1][0] * right.matrix[0][2] +left.matrix[1][1] * right.matrix[1][2] +left.matrix[1][2] * right.matrix[2][2];

	  g =left.matrix[2][0] * right.matrix[0][0] +left.matrix[2][1] * right.matrix[1][0] +left.matrix[2][2] * right.matrix[2][0];
  	h =left.matrix[2][0] * right.matrix[0][1] +left.matrix[2][1] * right.matrix[1][1] +left.matrix[2][2] * right.matrix[2][1];
  	i =left.matrix[2][0] * right.matrix[0][2] +left.matrix[2][1] * right.matrix[1][2] +left.matrix[2][2] * right.matrix[2][2];


  	matrix[0][0] = a;
  	matrix[0][1] = b;
  	matrix[0][2] = c;

  	matrix[1][0] = d;
  	matrix[1][1] = e;
  	matrix[1][2] = f;

  	matrix[2][0] = g;
  	matrix[2][1] = h;
  	matrix[2][2] = i;
	}

  C_Matrix33&	C_Matrix33::operator*= (const C_Matrix33& mat)
	{
  	multiply(*this, mat);
	  return *this;
	}

	void fromQuat(const C_Quat &q)
	{
  	const float w = q.w;
  	const float x = q.x;
  	const float y = q.y;
  	const float z = q.z;

  	matrix[0][0] = float(1.0) - y*y*float(2.0) - z*z*float(2.0);
  	matrix[0][1] = x*y*float(2.0) - w*z*float(2.0);
  	matrix[0][2] = x*z*float(2.0) + w*y*float(2.0);

  	matrix[1][0] = x*y*float(2.0) + w*z*float(2.0);
  	matrix[1][1] = float(1.0) - x*x*float(2.0) - z*z*float(2.0);
  	matrix[1][2] = y*z*float(2.0) - w*x*float(2.0);

  	matrix[2][0] = x*z*float(2.0) - w*y*float(2.0);
  	matrix[2][1] = y*z*float(2.0) + w*x*float(2.0);
  	matrix[2][2] = float(1.0) - x*x*float(2.0) - y*y*float(2.0);
	}

  void getColumn(int col, C_Vec3 & v) const
	{
  	v.x = matrix[0][col];
	  v.y = matrix[1][col];
  	v.z = matrix[2][col];
	}

  float	matrix[3][3];
};



class C_Matrix34
{
public:

	void id(void)
	{
		M.id();
		t.set(0,0,0);
	}

  C_Matrix33 M;
  C_Vec3  t;
};




static bool getTF(const char *str)
{
	bool ret = false;
	if ( str )
	{
  	if ( stricmp(str,"true") == 0 || stricmp(str,"1") == 0 )
	  	ret = true;
	}
	return ret;
}

#define NODE_STACK 64

// list of collada elements we even care about.
enum CELEMENT
{
	CE_LIBRARY_GEOMETRIES,
	CE_GEOMETRY,	            // attribute 'id'
	CE_MESH,
  CE_CONVEX_MESH,
	CE_SOURCE,                // attribute 'id'
	CE_FLOAT_ARRAY,           // attribute 'count' attribute 'id'
  CE_INT_ARRAY,
  CE_BOOL_ARRAY,
  CE_NAME_ARRAY,
  CE_IDREF_ARRAY,
	CE_TECHNIQUE_COMMON,      //
	CE_ACCESSOR,              // attribute 'count' attribute 'source' attribute 'stride'
	CE_PARAM,									// parameter in the accessor attribute 'name' and 'type'
	CE_VERTICES,              // attribute 'id'
	CE_INPUT,                 // attribute 'semantic' attribute 'source'
	CE_POLYGONS,              // attribute 'material' attribute 'count'
	CE_TRIANGLES,
	CE_POLYLIST,
	CE_VCOUNT,
	CE_P,                     // individual polygon.
	CE_LIBRARY_VISUAL_SCENES, //
	CE_VISUAL_SCENE,          // attribute 'id'
	CE_NODE,                  // attribute 'id' attribute 'type'
	CE_MATRIX,                // attribute 'sid'
	CE_INSTANCE_GEOMETRY,     // attribute 'url'
	CE_TRANSLATE,
	CE_ROTATE,
	CE_LIBRARY_PHYSICS_MATERIALS, //
	CE_PHYSICS_MATERIAL,       // attribute 'id'
	CE_DYNAMIC_FRICTION,
  CE_RESTITUTION,
  CE_STATIC_FRICTION,
  CE_LIBRARY_PHYSICS_MODELS,
  CE_PHYSICS_MODEL,          // attribute 'id'
  CE_RIGID_BODY,             // attribute 'sid'
	CE_INSTANCE_PHYSICS_MATERIAL,	// attribute 'url'
	CE_SHAPE,
	CE_BOX,
	CE_HALF_EXTENTS,
	CE_DYNAMIC,
	CE_MASS,
	CE_MASS_FRAME,
	CE_INERTIA,
	CE_LIBRARY_PHYSICS_SCENES,
	CE_PHYSICS_SCENE,           // attribute 'id'
	CE_INSTANCE_PHYSICS_MODEL,  // attribute 'url'
	CE_INSTANCE_RIGID_BODY,     // attribute 'target' attribute 'body'
	CE_VELOCITY,
	CE_ANGULAR_VELOCITY,
	CE_GRAVITY,
	CE_TIME_STEP,
	CE_SCENE,
	CE_INSTANCE_VISUAL_SCENE,   // attribute 'url'
	CE_INSTANCE_PHYSICS_SCENE,  // attribute 'url'
	CE_DENSITY,
	CE_EQUATION,
	CE_RADIUS,
	CE_HEIGHT,
	CE_CAPSULE,
	CE_PLANE,
	CE_SPHERE,
	CE_CYLINDER,
	CE_FORCE_FIELD,
	CE_INSTANCE_FORCE_FIELD,
	CE_LIBRARY_FORCE_FIELDS,
	CE_RIGID_CONSTRAINT,
	CE_INSTANCE_RIGID_CONSTRAINT,
	CE_TAPERED_CAPSULE,
	CE_TAPERED_CYLINDER,
	CE_RADIUS1,
	CE_RADIUS2,
	CE_DISABLE_COLLISION,
	CE_MIN,
	CE_MAX,
	CE_STIFFNESS,
	CE_DAMPING,
	CE_TARGET_VALUE,
	CE_ENABLED,
	CE_INTERPENETRATE,
	CE_REF_ATTACHMENT,
	CE_ATTACHMENT,
	CE_LINEAR,
	CE_SWING_CONE_AND_TWIST,
	CE_ANGULAR,
	CE_GROUP,
	CE_SKIN_WIDTH,
	CE_WAKEUP_COUNTER,
	CE_LINEAR_DAMPING,
	CE_ANGULAR_DAMPING,
	CE_MAX_ANGULAR_VELOCITY,
	CE_SLEEP_LINEAR_VELOCITY,
	CE_SLEEP_ANGULAR_VELOCITY,
	CE_SOLVER_ITERATION_COUNT,
	CE_DISABLE_GRAVITY,
	CE_KINEMATIC,
	CE_POSE_SLEEP_TEST,
	CE_FILTER_SLEEP_VELOCITY,
	CE_DISABLE_RESPONSE,
	CE_LOCK_COM,
	CE_PROJECTION_MODE,
	CE_PROJECTION_DISTANCE,
	CE_PROJECTION_ANGLE,
  CE_NX_JOINT_DRIVE_DESC,
  CE_DRIVE_TYPE,
	CE_SPRING,
	CE_FORCE_LIMIT,
	CE_DRIVE_POSITION,
	CE_DRIVE_ORIENTATION,
	CE_DRIVE_LINEAR_VELOCITY,
	CE_DRIVE_ANGULAR_VELOCITY,
	CE_GEAR_RATIO,
	CE_IGNORE,
	CE_LAST
};

static const char *CE_NAMES[CE_LAST] =
{
	"library_geometries",
	"geometry",
	"mesh",
  "convex_mesh",
	"source",
	"float_array",
  "int_array",
  "bool_array",
  "Name_array",
  "IDREF_array",
	"technique_common",
	"accessor",
	"param",
	"vertices",
	"input",
	"polygons",
	"triangles",
	"polylist",
	"vcount",
	"p",
	"library_visual_scenes",
	"visual_scene",
	"node",
	"matrix",
	"instance_geometry",
	"translate",
	"rotate",
	"library_physics_materials",
	"physics_material",
	"dynamic_friction",
  "restitution",
  "static_friction",
  "library_physics_models",
  "physics_model",
  "rigid_body",
	"instance_physics_material",
	"shape",
	"box",
	"half_extents",
	"dynamic",
	"mass",
	"mass_frame",
	"inertia",
	"library_physics_scenes",
	"physics_scene",
	"instance_physics_model",
	"instance_rigid_body",
	"velocity",
	"angular_velocity",
	"gravity",
	"time_step",
	"scene",
	"instance_visual_scene",
	"instance_physics_scene",
	"density",
	"equation",
	"radius",
	"height",
	"capsule",
	"plane",
	"sphere",
	"cylinder",
	"force_field",
	"instance_force_field",
	"library_force_fields",
	"rigid_constraint",
	"instance_rigid_constraint",
	"tapered_capsule",
	"tapered_cylinder",
	"radius1",
	"radius2",
	"disable_collision",
	"min",
	"max",
	"stiffness",
	"damping",
	"target_value",
	"enabled",
	"interpenetrate",
	"ref_attachment",
	"attachment",
	"linear",
	"swing_cone_and_twist",
	"angular",
	"group",
	"skin_width",
	"wakeupCounter",
	"linearDamping",
	"angularDamping",
	"maxAngularVelocity",
	"sleepLinearVelocity",
	"sleepAngularVelocity",
	"solverIterationCount",
	"disable_gravity",
	"kinematic",
	"pose_sleep_test",
	"filter_sleep_velocity",
	"disable_response",
	"lock_com",
	"projectionMode",
	"projectionDistance",
	"projectionAngle",
  "NxJointDriveDesc",
  "driveType",
	"spring",
	"forceLimit",
	"drivePosition",
	"driveOrientation",
	"driveLinearVelocity",
	"driveAngularVelocity",
	"gearRatio",
	"ignore"
};



class C_Geometry;
class C_Mesh;
class C_Source;
class C_FloatArray;
class C_Accessor;
class C_Param;
class C_Vertices;
class C_Input;
class C_Polygons;
class C_Polygon;
class C_Scene;
class C_VisualScene;
class C_Node;
class C_PhysicsMaterial;
class C_PhysicsModel;
class C_RigidBody;
class C_PhysicsScene;
class C_Shape;
class C_LibraryPhysicsScenes;
class C_LibraryPhysicsModels;
class C_LibraryPhysicsMaterials;
class C_LibraryVisualScenes;
class C_LibraryGeometries;
class C_RigidConstraint;
class C_InstanceRigidConstraint;

static float getFloat(const char *s)
{
	float ret = 0;
	if ( s )
	{
		if ( stricmp(s,"FLT_MIN") == 0 || stricmp(s,"-INF") == 0 )
			ret = FLT_MIN;
		else if ( stricmp(s,"FLT_MAX") == 0 || stricmp(s,"INF") == 0 )
			ret = FLT_MAX;
		else
			ret = (float) atof(s);
	}
	return ret;
}

static const char * getAttribute(TiXmlElement *element,const char *v)
{
	const char *ret = 0;

  TiXmlAttribute *atr = element->FirstAttribute();
  while ( atr )
  {
  	const char *aname  = atr->Name();
  	const char *avalue = atr->Value();
  	if ( strcmp(v,aname) == 0 )
  	{
  		ret = avalue;
  		break;
  	}
  	atr = atr->Next();
  }

  return ret;
}


class C_Base
{
public:
  C_Base(TiXmlElement *element)
  {
  	mId = getAttribute(element,"id");
  	mSid = getAttribute(element,"sid");
  	mName = getAttribute(element,"name");
  	mUrl = getAttribute(element,"url");
  	mCount = 0;
  	const char *c = getAttribute(element,"count");
  	if ( c )
  		mCount = atoi(c);
  }

  virtual CELEMENT getBaseType(void) const { return CE_IGNORE; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	const char *mId;
  const char *mSid;
  const char *mName;
  const char *mUrl;
  int         mCount;
};


static const char *TF(bool s)
{
	if ( s ) return "true";
	return "false";
}

static const char * fstring(float v)
{
	static char data[64*16];
	static int  index=0;

	char *ret = &data[index*64];
	index++;
	if (index == 16 ) index = 0;

	if ( v == 1 )
	{
		strcpy(ret,"1");
	}
	else if ( v == 0 )
	{
		strcpy(ret,"0");
	}
	else if ( v == - 1 )
	{
		strcpy(ret,"-1");
	}
	else if ( v == FLT_MIN )
	{
		strcpy(ret,"FLT_MIN");
	}
	else if ( v == FLT_MAX )
	{
		strcpy(ret,"FLT_MAX");
	}
	else
	{
		sprintf(ret,"%.9f", v );
		const char *dot = strstr(ret,".");
		if ( dot )
		{
			int len = (int)strlen(ret);
		  char *foo = &ret[len-1];
			while ( *foo == '0' ) foo--;
			if ( *foo == '.' )
				*foo = 0;
			else
				foo[1] = 0;
		}
	}

	return ret;
}

#define WARN(x) printf("%s\r\n", x );

void Make4x4(const C_Matrix34 &mat, palMatrix4x4 &m) {
	mat_identity(&m);
	float dest[9];
	mat.M.getRowMajor(dest);
	for (int y=0;y<3;y++)
		for (int x=0;x<3;x++) {
			m._mat[y+x*4] = dest[x+y*3];
		}
	m._41=mat.t.x;
	m._42=mat.t.y;
	m._43=mat.t.z;
	m._44=1;
}

void MakeVec(const C_Vec3 &vec, palVector3 &v) {
	v.x = vec.x;
	v.y = vec.y;
	v.z = vec.z;
}

static bool colladaMatrix(const char *m,C_Matrix34 &mat)
{
	bool ret = false;

	mat.id();

	float matrix[16];

  void *mem = Asc2Bin(m,16,"f", matrix );

  if ( mem )
	{
		float m[9];

		m[0] = matrix[0];
		m[1] = matrix[1];
		m[2] = matrix[2];
		mat.t.x = matrix[3];

		m[3] = matrix[4];
		m[4] = matrix[5];
		m[5] = matrix[6];
		mat.t.y = matrix[7];

		m[6] = matrix[8];
		m[7] = matrix[9];
		m[8] = matrix[10];
		mat.t.z = matrix[11];

		mat.M.setRowMajor(m);


		ret = true;
	}
	return ret;
}


static void writeMatrix(FILE *fph,const C_Matrix34 &mat, const char* name)
{
	if ( fph )
	{
		fprintf(fph,"        <%s>", name );
  	float m[9];
	  mat.M.getRowMajor( m );
	  for (unsigned int i=0; i<9; i++)
	  {
	  	fprintf(fph,"%s ", fstring(m[i]) );
	  }
	  fprintf(fph,"%s %s %s", fstring(mat.t.x), fstring(mat.t.y), fstring(mat.t.z) );
	  fprintf(fph,"</%s>\r\n", name);
  }
}

class C_Query
{
public:
  virtual C_VisualScene *  locateVisualScene(const char *str)  = 0;
  virtual C_PhysicsScene * locatePhysicsScene(const char *str) = 0;
  virtual C_PhysicsModel * locatePhysicsModel(const char *str) = 0;
  virtual C_Node         * locateNode(const char *str)         = 0;
  virtual C_Geometry     * locateGeometry(const char *str)     = 0;
  virtual int              getMaterialIndex(const char *mat)   = 0;

};

static TiXmlNode * getElement(const char *name,TiXmlNode *root)
{
	TiXmlNode *ret = 0;

	TiXmlNode *node = root->NextSibling();

	while ( node )
	{
		if ( node->Type() == TiXmlNode::ELEMENT )
		{
			if ( strcmp(node->Value(), name) == 0 )
			{
				ret = node;
				break;
			}
		}

	  if ( node->NoChildren() )
	  {
	  	assert( node );
			while ( node->NextSibling() == NULL && node != root )
			{
				node = node->Parent();
			}
			if ( node == root )
			{
				break;
			}
			assert(node);
			node = node->NextSibling();
	  }
	  else
	  {
	  	assert(node);
	  	node = node->FirstChild();
	  }

	}

 	return ret;
}


static const char * getElementText(TiXmlElement *element,const char *name)
{
	const char *ret = 0;

  TiXmlNode *node = getElement(name,element);
  if ( node )
  {
  	TiXmlNode *child = node->FirstChild();
  	if ( child && child->Type() == TiXmlNode::TEXT )
 		{
 			ret = child->Value();
  	}
  }

 	return ret;
}

class C_TriangleMesh
{
public:

	unsigned int vlookup(const C_Vec3 &v)
	{
		const float EPSILON = 0.0001f;
		unsigned int ret;
    for (unsigned int i=0; i<mVertices.size(); i++)
    {
    	const C_Vec3 &c = mVertices[i];
    	float dx = fabsf(c.x - v.x);
    	if ( dx < EPSILON )
    	{
      	float dy = fabsf(c.y - v.y);
      	if ( dy < EPSILON )
      	{
      	  float dz = fabsf(c.z - v.z);
      	  if ( dz < EPSILON )
      	  {
    	      float d  = dx*dx+dy*dy+dz*dz;
    	      if ( d < (EPSILON*EPSILON) )
    	      {
    		      return i;
    	      }
    	    }
    	  }
    	}
    }

    ret = mVertices.size();

    mVertices.push_back(v);

    return ret;
	}

	void addTri(const C_Vec3 &v1,const C_Vec3 &v2,const C_Vec3 &v3)
	{
		unsigned int i1 = vlookup(v1);
		unsigned int i2 = vlookup(v2);
		unsigned int i3 = vlookup(v3);
		mIndices.push_back(i1);
		mIndices.push_back(i2);
		mIndices.push_back(i3);
	}



  CARRAY< C_Vec3 >       mVertices;
  CARRAY< unsigned int > mIndices;
};

class C_Spring
{
public:

  C_Spring(void)
  {
  	mStiffness = 1;
  	mDamping = 0;
  	mTargetValue = 0;
  }

  float mStiffness;
  float mDamping;
  float mTargetValue;
};

enum JointDriveType
{
	JDT_POSITION,
	JDT_VELOCITY,
	JDT_POSITION_VELOCITY,
};


class C_JointDrive
{
public:
  C_JointDrive(void)
  {
  	driveType = JDT_POSITION;
  	spring = 0;
  	damping = 0;
  	forceLimit = FLT_MAX;
  }

	void saveXML(FILE *fph,const char *drive);

  JointDriveType	driveType;
  float           spring;
  float           damping;
  float           forceLimit;
};


class C_RigidConstraint : C_Base
{
public:
  C_RigidConstraint(TiXmlElement *element) : C_Base(element)
  {
  	mRef = true;
  	mLinear = true;

  	mBody1 = 0;
  	mBody2 = 0;
  	mNode1 = 0;
  	mNode2 = 0;
  	mName1 = 0;
  	mName2 = 0;
  	mMatrix1.id();
  	mMatrix2.id();
  	mProjectionMode = "NX_JPM_NONE";
  	mProjectionDistance = 0;
  	mProjectionAngle = 0;
  	mDrive = 0;
		drivePosition.set(0,0,0);
	  driveOrientation.setXYZW(0,0,0,1);
	  driveLinearVelocity.set(0,0,0);
	  driveAngularVelocity.set(0,0,0);
	  gearRatio = 1;
  }


	void setJointDriveDesc(TiXmlElement *element)
	{
		const char *id = getAttribute(element,"id");
		if ( id )
		{
			if ( stricmp(id,"xDrive") == 0 )
			{
				mDrive = &xDrive;
			}
			else if ( stricmp(id,"yDrive") == 0 )
			{
				mDrive = &yDrive;
			}
			else if ( stricmp(id,"zDrive") == 0 )
			{
				mDrive = &zDrive;
			}
			else if ( stricmp(id,"swingDrive") == 0 )
			{
				mDrive = &swingDrive;
			}
			else if ( stricmp(id,"twistDrive") == 0 )
			{
				mDrive = &twistDrive;
			}
			else if ( stricmp(id,"slerpDrive") == 0 )
			{
				mDrive = &slerpDrive;
			}
		}
		else
		{
			mDrive = 0;
		}
	}

  virtual void setText(CELEMENT operation,const char *data)
  {
  	switch ( operation )
  	{
  		case CE_DRIVE_TYPE:
  			if ( mDrive )
  			{
  				if ( stricmp(data,"NX_D6JOINT_DRIVE_POSITION" ) == 0 )
  				{
  					mDrive->driveType =	JDT_POSITION;
  				}
  				else if ( stricmp(data,"NX_D6JOINT_DRIVE_VELOCITY") == 0 )
  				{
  					mDrive->driveType = JDT_VELOCITY;
  				}
  				else if ( stricmp(data,"NX_D6JOINT_DRIVE_POSITION+NX_D6JONT_DRIVE_VELOCITY") == 0 )
  				{
  					mDrive->driveType = JDT_POSITION_VELOCITY;
  				}
  			}
  			break;
  		case CE_SPRING:
  			if ( mDrive )
  				mDrive->spring = getFloat(data);
  			break;
  		case CE_FORCE_LIMIT:
  			if ( mDrive )
  			  mDrive->forceLimit = getFloat(data);
  			break;
  		case CE_DRIVE_POSITION:
  			Asc2Bin(data,3,"f", &drivePosition.x );
  			break;
  		case CE_DRIVE_ORIENTATION:
  			if ( 1 )
  			{
  				float quat[4] = { 0, 0, 0, 1 };
    			Asc2Bin(data,4,"f", quat );
    			driveOrientation.setXYZW(quat[0],quat[1],quat[2],quat[3]);
    		}
  			break;
  		case CE_DRIVE_LINEAR_VELOCITY:
  			Asc2Bin(data,3,"f", &driveLinearVelocity.x );
  			break;
  		case CE_DRIVE_ANGULAR_VELOCITY:
  			Asc2Bin(data,3,"f", &driveAngularVelocity.x );
  			break;
  		case CE_GEAR_RATIO:
  			gearRatio = getFloat(data);
  			break;
  		case CE_PROJECTION_MODE:
  			mProjectionMode = data;
  			break;
  		case CE_PROJECTION_DISTANCE:
  			mProjectionDistance = getFloat(data);
  			break;
  		case CE_PROJECTION_ANGLE:
  			mProjectionAngle = getFloat(data);
  			break;
  		case CE_ENABLED:
    		mEnabled = getTF(data);
  			break;
  		case CE_INTERPENETRATE:
  			mInterpenetrate = getTF(data);
  			break;
  		case CE_TRANSLATE:
  			if ( 1 )
  			{
  				C_Vec3 t(0,0,0);
  				Asc2Bin(data,3,"f",&t.x);
  				if ( mRef )
  					mMatrix1.t+=t;
  				else
  					mMatrix2.t+=t;
  			}
  			break;
  		case CE_ROTATE:
  			if ( 1 )
  			{
  				float aa[4] = { 1, 0, 0, 0 };
  				Asc2Bin(data,4,"f",aa);
  				C_Vec3 axis( aa[0], aa[1], aa[2] );
  				float angle = aa[3];
  				C_Quat q;
					q.fromAngleAxis(angle,axis);
  				C_Matrix33 m(q);
  				if ( mRef )
    				mMatrix1.M*=m;
    			else
    				mMatrix2.M*=m;
  			}
  			break;
  		case CE_MATRIX:
    		if ( mRef )
    		  colladaMatrix(data,mMatrix1);
    		else
    			colladaMatrix(data,mMatrix2);
  			break;
  		case CE_MIN:
  			if ( mLinear )
  				Asc2Bin(data,3,"f",&mLinearMin.x);
  			else
  				Asc2Bin(data,3,"f",&mAngularMin.x);
  			break;
  		case CE_MAX:
  			if ( mLinear )
  				Asc2Bin(data,3,"f",&mLinearMax.x);
  			else
  				Asc2Bin(data,3,"f",&mAngularMax.x);
  			break;
  		case CE_STIFFNESS:
  			if ( mLinear )
  				mLinearSpring.mStiffness = getFloat(data);
  			else
  				mAngularSpring.mStiffness = getFloat(data);
  			break;
  		case CE_DAMPING:
  			if ( mDrive )
  			{
  				mDrive->damping = getFloat(data);
  			}
  			else
  			{
    			if ( mLinear )
    				mLinearSpring.mDamping = getFloat(data);
    			else
    				mAngularSpring.mDamping = getFloat(data);
    		}
  			break;
  		case CE_TARGET_VALUE:
  			if ( mLinear )
  				mLinearSpring.mTargetValue = getFloat(data);
  			else
  				mAngularSpring.mTargetValue = getFloat(data);
  			break;
  	}
  }



	void setRefAttachment(TiXmlElement *element)
	{
		mBody1 = getAttribute(element,"rigid_body");
		mNode1      = getAttribute(element,"node");
		mName1      = getAttribute(element,"name");
		mRef = true;
	}

	void setAttachment(TiXmlElement *element)
	{
		mBody2 = getAttribute(element,"rigid_body");
		mNode2      = getAttribute(element,"node");
		mName2      = getAttribute(element,"name");
		mRef = false;
	}

  virtual CELEMENT getBaseType(void) const { return CE_RIGID_CONSTRAINT; };


	float getMeanRad(float a,float b)
	{
		a = fabsf(a);
		b = fabsf(b);
		if ( b > a ) a = b;
		return a*DEG_TO_RAD;
	}

	bool match(const char *str) const
	{
		bool ret = false;
		if ( mSid && str )
		{
			if ( *str == '#' ) str++;
			if ( strcmp(mSid,str) == 0 )
				ret = true;
		}
		return ret;
	}


  void saveXML(FILE *fph,C_Query *q,C_PhysicsModel *pmodel);
  void loadPAL(C_Query *q,C_PhysicsModel *pmodel);

  palGenericLink *mpGenericLink;

	bool        mRef;
	bool        mLinear;

  const char *mBody1;
  const char *mBody2;

  const char *mNode1;
  const char *mNode2;

  const char *mName1;
  const char *mName2;

  bool        mEnabled;
  bool        mInterpenetrate;
  C_Matrix34     mMatrix1;
  C_Matrix34     mMatrix2;
  C_Vec3      mLinearMin;
  C_Vec3      mLinearMax;
  C_Vec3      mAngularMin;
  C_Vec3      mAngularMax;
  C_Spring    mLinearSpring;
  C_Spring    mAngularSpring;

	const char *mProjectionMode;
	float       mProjectionDistance;
	float       mProjectionAngle;

	C_JointDrive *mDrive; // current drive..

	C_JointDrive xDrive;
	C_JointDrive yDrive;
	C_JointDrive zDrive;
	C_JointDrive swingDrive;
	C_JointDrive twistDrive;
	C_JointDrive slerpDrive;
	C_Vec3			drivePosition;
	C_Quat      driveOrientation;
	C_Vec3      driveLinearVelocity;
	C_Vec3      driveAngularVelocity;
	float       gearRatio;
};

class C_InstanceRigidConstraint : C_Base
{
public:
  C_InstanceRigidConstraint(TiXmlElement *element) : C_Base(element)
  {
  	mConstraint = getAttribute(element,"constraint");
  }

  virtual void setText(CELEMENT operation,const char *data)
  {
  }

	void saveXML(FILE *fph,C_Query *q,C_PhysicsModel *pmodel);
	void loadPAL(C_Query *q,C_PhysicsModel *pmodel);

  virtual CELEMENT getBaseType(void) const { return CE_INSTANCE_RIGID_CONSTRAINT; };

  const char *mConstraint;

};
enum ArrayType
{
  AT_BOOL,
  AT_INT,
  AT_FLOAT,
  AT_NAME,
  AT_IDREF,
  AT_LAST
};

class C_Array : public C_Base
{
public:
	C_Array(TiXmlElement *element) : C_Base(element)
  {
  }
  virtual ArrayType getArrayType(void) const = 0;
  virtual void setText(CELEMENT operation,const char *data) = 0;
};

// attributes:
//                           count  : required
//                              id  : optional
//                             name : optional
class C_BoolArray : C_Array
{
public:
	C_BoolArray(TiXmlElement *element) : C_Array(element)
  {
  }

  virtual ArrayType getArrayType(void) const { return AT_BOOL; };

  virtual void setText(CELEMENT operation,const char *data)
  {
  }

  virtual CELEMENT getBaseType(void) const { return CE_BOOL_ARRAY; };

};

// Attributes:
//                count    (required)
//                   id    (optional)
//                 name    (optional)
//            minInclusive (optional)
//            maxInclusive (optional)
class C_IntArray : public C_Array
{
public:
	C_IntArray(TiXmlElement *element) : C_Array(element)
  {
    mData = 0;
    mMinInclusive = getAttribute(element,"minInclusive");
    mMaxInclusive = getAttribute(element,"maxInclusive");
  }

	~C_IntArray(void)
	{
		delete mData;
	}

  virtual ArrayType getArrayType(void) const { return AT_INT; };

  virtual void setText(CELEMENT operation,const char *data)
  {
  	if ( mCount && !mData )
  		mData = (int *) Asc2Bin(data,mCount,"d",0);
  }

  virtual CELEMENT getBaseType(void) const { return CE_INT_ARRAY; };

  const char *mMinInclusive;
  const char *mMaxInclusive;
  int        *mData;
};

// Attributes:
//                  count   (required)
//                   name   (optional)
//                     id   (optional)
//                  digits  (optional)
//                 magnitude (optional)
//
class C_FloatArray : public C_Array
{
public:
	C_FloatArray(TiXmlElement *element) : C_Array(element)
  {
    mDigits = getAttribute(element,"digits");
    mMagnitude = getAttribute(element,"magnitude");
    mData = 0;
  }

  ~C_FloatArray(void)
  {
    delete mData;
  }

  virtual ArrayType getArrayType(void) const { return AT_FLOAT; };

  virtual void setText(CELEMENT operation,const char *data)
  {
    if ( mCount && !mData )
      mData = (float *)Asc2Bin(data,mCount,"f",0);
  }

	void getV(C_Vec3 &v,unsigned int i)
	{
		v.set(0,0,0);
		assert( i < (unsigned int)( mCount/3) );
		if ( i < (unsigned int)(mCount/3) )
		{
			v.x = mData[i*3+0];
			v.y = mData[i*3+1];
			v.z = mData[i*3+2];
		}
	}

  virtual CELEMENT getBaseType(void) const { return CE_FLOAT_ARRAY; };

  const char *mDigits;
  const char *mMagnitude;
  float      *mData;
};

class C_NameArray : public C_Array
{
public:
  C_NameArray(TiXmlElement *element) : C_Array(element)
  {
  }

  virtual ArrayType getArrayType(void) const { return AT_NAME; };

  virtual void setText(CELEMENT operation,const char *data)
  {
  }

  virtual CELEMENT getBaseType(void) const { return CE_NAME_ARRAY; };

};

class C_IDREFArray : public C_Array
{
public:
  C_IDREFArray(TiXmlElement *element) : C_Array(element)
  {
  }
  virtual ArrayType getArrayType(void) const { return AT_IDREF; };

  virtual void setText(CELEMENT operation,const char *data)
  {
  }

  virtual CELEMENT getBaseType(void) const { return CE_IDREF_ARRAY; };

};

// attributes:
//                             name  : optional
//                              sid  : optional but if present must be unique
//                             type  : required
//                          sematnic : optional
//
class C_Param : public C_Base
{
public:
  C_Param(TiXmlElement *element) : C_Base(element)
  {
    mType = getAttribute(element,"type");
    mSemantic = getAttribute(element,"semantic");
  }

  virtual CELEMENT getBaseType(void) const { return CE_PARAM; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

  const char *mType;
  const char *mSemantic;

};

// attributes:
//                            count    : required
//                            offset   : optional (default is 0)
//                            source   : required
//                            stride   : optional (default is 1)
class C_Accessor : public C_Base
{
public:
  C_Accessor(TiXmlElement *element) : C_Base(element)
  {
    mOffset = 0;
    const char *o = getAttribute(element,"offset");
    if ( o ) mOffset = atoi(o);
    mSource = getAttribute(element,"source");
    mStride = 1;
    const char *s = getAttribute(element,"stride");
    if ( s ) mStride = atoi(s);
  }

  ~C_Accessor(void)
  {
    for (unsigned int i=0; i<mParams.size(); i++)
    {
      C_Param *p = mParams[i];
      delete p;
    }
  }

  virtual CELEMENT getBaseType(void) const { return CE_ACCESSOR; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

  CARRAY< C_Param * > mParams;
  unsigned int         mOffset;
  const char          *mSource;
  unsigned int         mStride;

};

//   attributes:
//                           id   : required
//                         name   : optional
class C_Source : public C_Base
{
public:
  C_Source(TiXmlElement *element) : C_Base(element)
  {
    mAccessor = 0;
    mArray    = 0;
  }

	bool match(const char *str)
	{
		bool ret = false;
		if ( str && mId )
		{
			if ( *str == '#' ) str++;
			if ( strcmp(str,mId) == 0 )
			  ret = true;
		}
		return ret;
	}

  virtual CELEMENT getBaseType(void) const { return CE_SOURCE; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

  C_Accessor  *mAccessor;
  C_Array     *mArray;  // the source array.
};

class C_Input : public C_Base
{
public:
  C_Input(TiXmlElement *element) : C_Base(element)
  {
    mOffset = 0;
    mSet    = 0;
    const char *o   = getAttribute(element,"offset");
    if ( o )
      mOffset = atoi(o);
    mSemantic = getAttribute(element,"semantic");
    mSource   = getAttribute(element,"source");
    const char *s      = getAttribute(element,"set");
    if ( s )
      mSet = atoi(s);
  }

	bool match(const char *str)
	{
		bool ret = false;

		if ( str && mSemantic )
		{
			if ( *str == '#' ) str++;
			if ( strcmp(str,mSemantic) == 0 )
				ret = true;
		}
		return ret;
	}

  virtual CELEMENT getBaseType(void) const { return CE_INPUT; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

  int         mOffset;
  const char *mSemantic;
  const char *mSource;
  int         mSet;

};

// id
// name
class C_Vertices : public C_Base
{
public:
  C_Vertices(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_Vertices(void)
  {
    for (unsigned int i=0; i<mInputs.size(); i++)
    {
      C_Input *ip = mInputs[i];
      delete ip;
    }
  }

  virtual CELEMENT getBaseType(void) const { return CE_VERTICES; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	C_Input * locateInput(const char *semantic)
	{
		C_Input *ret = 0;
		for (unsigned int i=0; i<mInputs.size(); i++)
		{
			C_Input *ip = mInputs[i];
			if ( ip->match(semantic) )
			{
				ret = ip;
				break;
			}
		}
		return ret;
	}

	bool match(const char *str)
	{
		bool ret = false;
		if ( mId && str )
		{
			if ( *str == '#' ) str++;
			if ( strcmp(mId,str) == 0 )
				ret = true;
		}
		return ret;
	}

  CARRAY< C_Input * >  mInputs;
};


enum InputType
{
	IT_TRIANGLES,
	IT_POLYGONS,
	IT_POLYLIST,
	IT_LAST
};

// count, material, name
class C_Triangles : public C_Base
{
public:
  C_Triangles(TiXmlElement *element,InputType type) : C_Base(element)
  {
    mInputType = type;
    mMaterial = getAttribute(element,"material");
  }

  ~C_Triangles(void)
  {
    for (unsigned int i=0; i<mInputs.size(); i++)
    {
      C_Input *ip = mInputs[i];
      delete ip;
    }
  }

  virtual CELEMENT getBaseType(void) const { return CE_TRIANGLES; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	void addVcount(const char *c)
	{
  	int count;
    unsigned int *points = (unsigned int *)Asc2Bin(c,count,"d");
    if ( points )
    {
			for (int i=0; i<count; i++)
			{
				mVcount.push_back( points[i] );
			}
			delete points;
    }
	}

  void addPoints(const char *c)
  {
  	int count;
    unsigned int *points = (unsigned int *)Asc2Bin(c,count,"d");
    if ( points )
    {
			if ( mInputType == IT_POLYGONS )
			{
				assert( count >= 3 );
				mVcount.push_back(count); // number of points int he polygon.
			}
			else
			{
				assert( (count%3) == 0 );
			}
			for (int i=0; i<count; i++)
			{
				mIndices.push_back( points[i] );
			}
			delete points;
    }

  }

	unsigned int getIndex(unsigned int index,const unsigned int *indices,unsigned int offset,unsigned int stride)
	{
		unsigned int base = stride*index;
		return indices[base+offset];
	}

	void addTri(unsigned int i1,unsigned int i2,unsigned int i3,C_TriangleMesh &t,C_Source *vertices)
	{
		// hurrah!
		C_Accessor *accessor = vertices->mAccessor;
	  C_Array *array       = vertices->mArray;
	  if ( array && array->getArrayType() == AT_FLOAT && accessor && accessor->mParams.size() == 3 && accessor->mStride == 3)
	  {

			unsigned int vcount = accessor->mCount;

	  	i1+=accessor->mOffset;
	  	i2+=accessor->mOffset;
	  	i3+=accessor->mOffset;

	  	assert( i1 < vcount );
	  	assert( i2 < vcount );
	  	assert( i3 < vcount );

	  	if ( i1 < vcount && i2 < vcount && i3 < vcount )
	  	{
  	  	C_FloatArray *farray = (C_FloatArray *) array;
				unsigned int fcount = farray->mCount/3;
				assert( fcount == vcount );
				if ( fcount == vcount )
				{
					C_Vec3 v1,v2,v3;

          farray->getV(v1,i1);
          farray->getV(v2,i2);
          farray->getV(v3,i3);

					t.addTri(v1,v2,v3);

				}
			}
	  }

	}

  // we have already computed the 'position' source.
  // now we iterate through the indices
	void getTriangleMesh(C_TriangleMesh &t,C_Source *psource,C_Vertices *vertices)
	{
		int offset = 0;
		bool found = false;

		// things get really wacky here!
		for (unsigned int i=0; i<mInputs.size(); i++)
		{
			C_Input *ip = mInputs[i];
			if ( ip->mSemantic && strcmp(ip->mSemantic,"VERTEX") == 0 )
			{
				if ( vertices->match(ip->mSource) )
				{
  				offset = ip->mOffset;
	  			found  = true;
					break;
	  		}
	  		else
	  		{
	  			// what the hell!??
	  		}
			}
		}

		if ( found ) // ok.. we found a mapping beetween the
		{
			unsigned int icount=0;
			for (unsigned int j=0; j<mInputs.size(); j++)
			{
				C_Input *cp = mInputs[j];
				if ( (unsigned int)cp->mOffset > icount )
					icount = cp->mOffset;
			}

			icount++;


      unsigned int *istart = &mIndices[0];
      unsigned int *indices = istart;

			if ( mInputType == IT_POLYGONS || mInputType == IT_POLYLIST )
			{
				assert( mCount == mVcount.size() );
				if ( (unsigned int)mCount > mVcount.size() )
					mCount = mVcount.size();
			}

      for (int i=0; i<mCount; i++)
      {
      	if ( mInputType == IT_POLYGONS || mInputType == IT_POLYLIST )
      	{
      		unsigned int pcount = mVcount[i];

					if ( mInputType == IT_POLYGONS )
					  pcount = pcount/icount;

      		assert( pcount >= 3 );
      		if ( pcount < 3 ) return; // something is really screwed up!

      		unsigned int i1 = getIndex(0,indices,offset,icount);
      		unsigned int i2 = getIndex(1,indices,offset,icount);
      		unsigned int i3 = getIndex(2,indices,offset,icount);

      		addTri(i1,i2,i3,t,psource);

      		for (unsigned int i=3; i<pcount; i++)
      		{
      			i2 = i3;
      			i3 = getIndex(i,indices,offset,icount);
        		addTri(i1,i2,i3,t,psource);
      		}
      		indices+=pcount*icount;
      	}
      	else
      	{

      		unsigned int i1 = getIndex(0,indices,offset,icount);
      		unsigned int i2 = getIndex(1,indices,offset,icount);
      		unsigned int i3 = getIndex(2,indices,offset,icount);

       		addTri(i1,i2,i3,t,psource);

      		indices+=3*icount;
      	}

				unsigned int dist = indices - istart;

				assert( dist <= mIndices.size() );

				if ( dist > mIndices.size() )
					break;
      }
		}
	}

  InputType               mInputType;
  CARRAY< C_Input * >    mInputs;
  const char             *mMaterial;
  CARRAY< unsigned int > mIndices;
  CARRAY< unsigned int > mVcount;
};


class C_Mesh : public C_Base
{
public:

  C_Mesh(TiXmlElement *element,bool isConvex) : C_Base(element)
  {
    mConvex = isConvex;
    mVertices = 0;
    mConvexHullOf = getAttribute(element,"convex_hull_of");
  }

  ~C_Mesh(void)
  {
    for (unsigned int i=0; i<mSources.size(); i++)
    {
      C_Source *s = mSources[i];
      delete s;
    }
    for (unsigned int i=0; i<mTriangles.size(); i++)
    {
      C_Triangles *t = mTriangles[i];
      delete t;
    }
    delete mVertices;
  }

  virtual CELEMENT getBaseType(void) const { return CE_MESH; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	C_Source * locateSource(const char *str)
	{
		C_Source *ret = 0;
		for (unsigned int i=0; i<mSources.size(); i++)
		{
			C_Source *c = mSources[i];
			if ( c->match(str) )
			{
				ret = c;
				break;
			}
		}
		return ret;
	}

	void getTriangleMesh(C_TriangleMesh &t,C_Query *q);

	bool isConvex(void) const { return mConvex; };

	const char *mConvexHullOf;
  bool                     mConvex;     // true if this is a 'convex' mesh.
  CARRAY< C_Source * >    mSources;
  C_Vertices              *mVertices;
  CARRAY< C_Triangles * > mTriangles;
};

class C_Geometry : public C_Base
{
public:
  C_Geometry(TiXmlElement *element) : C_Base(element)
  {
  	mIndex = 0; // 'array' index.
  };

  ~C_Geometry(void)
  {
    for (unsigned int i=0; i<mMeshes.size(); i++)
    {
      C_Mesh *m = mMeshes[i];
      delete m;
    }
  }

  virtual CELEMENT getBaseType(void) const { return CE_GEOMETRY; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	bool match(const char *str)
	{
		bool ret = false;
		if ( str && mId )
		{
			if ( *str == '#' ) str++;
			if ( strcmp(str,mId) == 0 )
			  ret = true;
		}
		return ret;
	}

	bool isConvex(void)
	{
		bool ret = true;
		for (unsigned int i=0; i<mMeshes.size(); i++)
		{
			C_Mesh *m = mMeshes[i];
			if ( !m->isConvex() )
			{
				ret = false;
				break;
			}
		}
		return ret;
	}

	int getIndex(void) const { return mIndex; };

	void getTriangleMesh(C_TriangleMesh &t,C_Query *q)
	{
		for (unsigned int i=0; i<mMeshes.size(); i++)
		{
			C_Mesh *m = mMeshes[i];
			m->getTriangleMesh(t,q);
		}
	}

  void saveXML(FILE *fph,C_Query *q);
  void loadPAL(C_Query *q);

	int                  mIndex;
  CARRAY< C_Mesh * >  mMeshes;

};

enum C_ShapeType
{
	CST_PLANE,
	CST_BOX,
	CST_SPHERE,
	CST_CYLINDER,
	CST_TAPERED_CYLINDER,
	CST_CAPSULE,
	CST_TAPERED_CAPSULE,
	CST_MESH,
	CST_CONVEX_MESH,
	CST_UNKNOWN
};

class C_Shape : public C_Base
{
public:
	C_Shape(TiXmlElement *element) : C_Base(element)
	{
		mHollow = false;
		mMass  = 1;
		mDensity = 0;
		mTransform.id();
		mInstancePhysicsMaterial = 0;
		mShapeType = CST_UNKNOWN;
		mGeometry = 0;
		mHalfExtents.set(1,1,1);
		mRadius1 = 1;
		mRadius2 = 1;
		mHeight = 1;
		mPlane.normal.set(0,1,0);
		mPlane.d = 0;
		//
		mGroup = 0;
		mSkinWidth = 0.005f; // default
		mDisableCollision = false;
	}

	~C_Shape(void)
	{
	}

  virtual CELEMENT getBaseType(void) const { return CE_SHAPE; };

  virtual void     setText(CELEMENT operation,const char *svalue)
  {
		switch ( operation )
		{
			case CE_GROUP:
				if ( svalue )
				{
					mGroup = atoi(svalue);
				}
				break;
			case CE_SKIN_WIDTH:
				mSkinWidth = getFloat(svalue);
				break;
			case CE_DISABLE_COLLISION:
				mDisableCollision = getTF(svalue);
				break;
			case CE_RADIUS:
				setRadius(svalue);
				break;
			case CE_HEIGHT:
				setHeight(svalue);
				break;
			case CE_MASS:
				setMass(svalue);
				break;
			case CE_DENSITY:
				setDensity(svalue);
				break;
			case CE_EQUATION:
				setEquation(svalue);
				break;
			case CE_TRANSLATE:
  			if ( 1 )
  			{
  				C_Vec3 t(0,0,0);
  				Asc2Bin(svalue,3,"f",&t.x);
  				mTransform.t+=t;
  			}
				break;
			case CE_ROTATE:
  			if ( 1 )
  			{
  				float aa[4] = { 1, 0, 0, 0 };
  				Asc2Bin(svalue,4,"f",aa);
  				C_Vec3 axis( aa[0], aa[1], aa[2] );
  				float angle = aa[3];
  				C_Quat q;
					q.fromAngleAxis(angle,axis);
  				C_Matrix33 m(q);
  				mTransform.M*=m;
  			}
				break;
			case CE_MATRIX:
				setMatrix(svalue);
				break;
			case CE_HALF_EXTENTS:
				setHalfExtents(svalue);
				break;
		}
  }

	void setHalfExtents(const char *v)
	{
		Asc2Bin(v,3,"f", &mHalfExtents.x);
	}

	void setEquation(const char *v)
	{
		float p[4];
		void *ok = Asc2Bin(v,4,"f",p);
		if ( ok )
		{
			mPlane.normal.set( p[0], p[1], p[2] );
			mPlane.d = p[3];
		}
	}

  void setMatrix(const char *m)
  {
  	colladaMatrix(m,mTransform);
  }

	void setRadius(const char *r)
	{
		float rd[2];
		if ( Asc2Bin(r,2,"f",rd) )
		{
			mRadius1 = rd[0];
			mRadius2 = rd[1];
		}
		else
		{
		  mRadius1 = mRadius2 = getFloat(r);
		}
	}

  void setHeight(const char *h)
  {
  	mHeight = getFloat(h);
  }

  void setMass(const char *m)
  {
  	mMass = getFloat(m);
  }

  void setDensity(const char *d)
  {
  	mDensity = getFloat(d);
  }

	void setInstancedGeometry(const char *str)
	{
		mGeometry = str;
	}


	void loadPAL(C_Query *query,unsigned int index);
	void saveXML(FILE *fph,C_Query *query,unsigned int index);

  bool		           mHollow;
  float              mMass;
  float              mDensity;
  C_Matrix34            mTransform;
  const char        *mInstancePhysicsMaterial;
  C_ShapeType        mShapeType;
  const char        *mGeometry;
  C_Vec3             mHalfExtents;
  float              mRadius1;
  float              mRadius2;
  float              mHeight;
	NxPlane            mPlane;
	unsigned int       mGroup;
	float              mSkinWidth;
	bool               mDisableCollision;
};



class C_RigidBody : public C_Base
{
public:
  C_RigidBody(TiXmlElement *element) : C_Base(element)
  {
  	mDynamic = false;
  	mMass = 1;
  	mMassFrame.id();
  	mInertia.set(0,0,0);
  	mInstancePhysicsMaterial = 0;
  	mDisableCollision = false;
  	mDisableResponse = false;
  	mWakeupCounter = 20.0f*0.02f;
  	mLinearDamping = 0;
  	mAngularDamping = 0;
  	mMaxAngularVelocity = -1;
  	mSleepLinearVelocity = -1;
  	mSleepAngularVelocity = -1;
  	mSolverIterationCount = 4;
  	mKinematic = false;
  	mPoseSleepTest = false;
  	mFilterSleepVelocity = false;
  	mGroup = 0;
  	mDensity = 0;
  	mLockCOM = false;
		mDisableGravity = false;
  }

  ~C_RigidBody(void)
  {
  	for (unsigned int i=0; i<mShapes.size(); i++)
  	{
  		C_Shape *s = mShapes[i];
  		delete s;
  	}
  }

  virtual CELEMENT getBaseType(void) const { return CE_RIGID_BODY; };

  virtual void setText(CELEMENT operation,const char *svalue)
  {
		switch ( operation )
		{
			case CE_DISABLE_GRAVITY:
				mDisableGravity = getTF(svalue);
				break;
			case CE_DISABLE_COLLISION:
				mDisableCollision = getTF(svalue);
				break;
			case CE_DISABLE_RESPONSE:
				mDisableResponse = getTF(svalue);
				break;
			case CE_WAKEUP_COUNTER:
				mWakeupCounter = getFloat(svalue);
				break;
			case CE_LINEAR_DAMPING:
				mLinearDamping = getFloat(svalue);
				break;
			case CE_ANGULAR_DAMPING:
				mAngularDamping = getFloat(svalue);
				break;
			case CE_MAX_ANGULAR_VELOCITY:
				mMaxAngularVelocity = getFloat(svalue);
				break;
			case CE_SLEEP_LINEAR_VELOCITY:
				mSleepLinearVelocity = getFloat(svalue);
				break;
			case CE_SLEEP_ANGULAR_VELOCITY:
				mSleepAngularVelocity = getFloat(svalue);
				break;
			case CE_SOLVER_ITERATION_COUNT:
				mSolverIterationCount = atoi(svalue);
				if ( mSolverIterationCount < 1 ) mSolverIterationCount = 1;
				if ( mSolverIterationCount > 65536 ) mSolverIterationCount = 65536;
				break;
			case CE_KINEMATIC:
				mKinematic = getTF(svalue);
				break;
			case CE_POSE_SLEEP_TEST:
				mPoseSleepTest = getTF(svalue);
				break;
			case CE_FILTER_SLEEP_VELOCITY:
				mFilterSleepVelocity = getTF(svalue);
				break;
			case CE_LOCK_COM:
				mLockCOM = getTF(svalue);
				break;
			case CE_GROUP:
				mGroup = getTF(svalue);
				break;
			case CE_DENSITY:
				mDensity = getFloat(svalue);
				break;
			case CE_MASS:
				mMass = getFloat(svalue);
				break;
			case CE_DYNAMIC:
				setDynamic(svalue);
				break;
			case CE_INERTIA:
				setInertia(svalue);
				break;
		}
  }

	void setDynamic(const char *v)
	{
		mDynamic = getTF(v);
	}

  void setInertia(const char *v)
  {
  	Asc2Bin(v,3,"f",&mInertia.x);
  }

	void setMassFrame(const char *v)
	{
		colladaMatrix(v,mMassFrame);
	}

	void setMassFrameTranslate(const char *v)
	{
		C_Vec3 t(0,0,0);
		Asc2Bin(v,3,"f",&t.x);
		mMassFrame.t+=t;
	}

  void setMassFrameRotate(const char *v)
  {
  	float aa[4] = { 1, 0, 0, 0 };
  	Asc2Bin(v,4,"f",aa);
  	float angle = aa[3];
  	C_Vec3 axis( aa[0], aa[1], aa[2] );
  	C_Quat q(angle,axis);
  	C_Matrix33 m;
  	m.fromQuat(q);
  	mMassFrame.M*=m;
  }

	bool match(const char *str)
	{
		bool ret = false;
		if ( str )
		{
			if ( *str == '#' ) str++;
			if ( mSid && strcmp(mSid,str) == 0 ) ret = true;
		}
		return ret;
	}


  void saveXML(FILE *fph,C_Query *q,const C_Matrix34 &mat,const C_Vec3 &velocity,const C_Vec3 &angularVelocity);
  void loadPAL(C_Query *q,const C_Matrix34 &mat,const C_Vec3 &velocity,const C_Vec3 &angularVelocity);

  bool                 mDynamic;
  float                mMass;
  C_Matrix34              mMassFrame;
  C_Vec3               mInertia;
  const char          *mInstancePhysicsMaterial;
  CARRAY< C_Shape * > mShapes;
  palBodyBase *mpBody;

// PhysX specific items.
  bool          mDisableCollision;
  bool	        mDisableResponse;
  float	        mWakeupCounter;
  float	        mLinearDamping;
  float	        mAngularDamping;
  float	        mMaxAngularVelocity;
  float	        mSleepLinearVelocity;
  float	        mSleepAngularVelocity;
  unsigned int	mSolverIterationCount;
  bool	        mKinematic;
  bool	        mPoseSleepTest;
  bool	        mFilterSleepVelocity;
  bool          mLockCOM;
	bool          mDisableGravity;
  unsigned int	mGroup;
  float	        mDensity;
};

class C_DisableCollision : public C_Base
{
public:
  C_DisableCollision(TiXmlElement *element) : C_Base(element)
  {
  	mBody1 = getAttribute(element,"body1");
  	mBody2 = getAttribute(element,"body2");
  }

  virtual CELEMENT getBaseType(void) const { return CE_DISABLE_COLLISION; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	void saveXML(FILE *fph,C_Query *q,C_PhysicsModel *pmodel,unsigned int index);

  const char *mBody1;
  const char *mBody2;
};

class C_PhysicsModel : public C_Base
{
public:
  C_PhysicsModel(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_PhysicsModel(void)
  {
  	for (unsigned int i=0; i<mRigidBodies.size(); i++)
  	{
  		C_RigidBody *c = mRigidBodies[i];
  		delete c;
  	}
  	for (unsigned int i=0; i<mRigidConstraints.size(); i++)
  	{
  		C_RigidConstraint *rc = mRigidConstraints[i];
  		delete rc;
  	}
  }

  virtual CELEMENT getBaseType(void) const { return CE_PHYSICS_MODEL; };
  virtual void     setText(CELEMENT operation,const char *txt) { };


  C_RigidBody * locateRigidBody(const char *str)
  {
  	C_RigidBody *ret = 0;

		if ( str )
		{
  		for (unsigned int i=0; i<mRigidBodies.size(); i++)
  		{
  			C_RigidBody *rb = mRigidBodies[i];
  			if ( rb->match(str) )
  			{
  				ret = rb;
  				break;
  			}
  		}
  	}
		return ret;
  }

  C_RigidConstraint * locateRigidConstraint(const char *str)
  {
  	C_RigidConstraint *ret = 0;
		if ( str )
		{
  		for (unsigned int i=0; i<mRigidConstraints.size(); i++)
  		{
  			C_RigidConstraint *rc = mRigidConstraints[i];
  			if ( rc->match(str) )
  			{
  				ret = rc;
  				break;
  			}
  		}
  	}
		return ret;
  }

	int getActorIndex(const char *str)
	{
		int ret = -1;

		for (unsigned int i=0; i<mRigidBodies.size(); i++)
		{
			C_RigidBody *rb = mRigidBodies[i];
			if ( rb->match(str) )
			{
				ret = i;
				break;
			}
		}
		return ret;
	}

  CARRAY< C_RigidBody * >       mRigidBodies;
  CARRAY< C_RigidConstraint * > mRigidConstraints;
};

class C_LibraryPhysicsModels : public C_Base
{
public:
  C_LibraryPhysicsModels(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_LibraryPhysicsModels(void)
  {
  	for (unsigned int i=0; i<mPhysicsModels.size(); i++)
  	{
  		C_PhysicsModel *m = mPhysicsModels[i];
  		delete m;
  	}
  }

  virtual CELEMENT getBaseType(void) const { return CE_LIBRARY_PHYSICS_MODELS; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	C_PhysicsModel * locatePhysicsModel(const char *str)
	{
		C_PhysicsModel *ret = 0;
		if ( str )
		{
			if ( *str == '#' ) str++;
			for (unsigned int i=0; i<mPhysicsModels.size(); i++)
			{
				C_PhysicsModel *pm = mPhysicsModels[i];
				if ( pm->mId && strcmp(pm->mId,str) == 0 )
				{
					ret = pm;
					break;
				}
			}
		}
		return ret;
	}

  CARRAY< C_PhysicsModel * > mPhysicsModels;
};


// Note..needs to support overrides!
class C_InstanceRigidBody : public C_Base
{
public:

  C_InstanceRigidBody(TiXmlElement *element) : C_Base(element)
  {
  	mBody   = getAttribute(element,"body");
  	mTarget = getAttribute(element,"target");
  	mAngularVelocity.set(0,0,0);
  	mVelocity.set(0,0,0);
  }

  ~C_InstanceRigidBody(void)
  {
  }

  virtual CELEMENT getBaseType(void) const { return CE_INSTANCE_RIGID_BODY; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	void setVelocity(const char *v)
	{
		Asc2Bin(v,3,"f",&mVelocity.x);
	}

  void setAngularVelocity(const char *v)
  {
  	Asc2Bin(v,3,"f",&mAngularVelocity.x);
  }

	void saveXML(FILE *fph,C_Query *q,C_PhysicsModel *pmodel);
	void loadPAL(C_Query *q,C_PhysicsModel *pmodel);

  const char *mBody;
  const char *mTarget;
	C_Vec3      mAngularVelocity;
	C_Vec3      mVelocity;
};

class C_InstancePhysicsModel : public C_Base
{
public:
  C_InstancePhysicsModel(TiXmlElement *element) : C_Base(element)
  {
  	mParent = getAttribute(element,"parent");
  }

  ~C_InstancePhysicsModel(void)
  {
  	for (unsigned int i=0; i<mInstanceRigidBodies.size(); i++)
  	{
  		C_InstanceRigidBody *rb = mInstanceRigidBodies[i];
  		delete rb;
  	}
  	for (unsigned int i=0; i<mInstanceRigidConstraints.size(); i++)
  	{
  		C_InstanceRigidConstraint *irc = mInstanceRigidConstraints[i];
  		delete irc;
  	}
  	for (unsigned int i=0; i<mDisableCollisions.size(); i++)
  	{
  		C_DisableCollision *dc = mDisableCollisions[i];
  		delete dc;
  	}
  }

	void saveXML(FILE *fph,C_Query *query);
	void loadPAL(C_Query *query);

  virtual CELEMENT getBaseType(void) const { return CE_INSTANCE_PHYSICS_MODEL; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	const char                            *mParent;
  CARRAY< C_InstanceRigidBody * >       mInstanceRigidBodies;
  CARRAY< C_InstanceRigidConstraint * > mInstanceRigidConstraints;
  CARRAY< C_DisableCollision * >        mDisableCollisions;

};

class C_PhysicsScene : public C_Base
{
public:
  C_PhysicsScene(TiXmlElement *element) : C_Base(element)
  {
  	mTimeStep = 0;
  	mGravity.set(0,-9.8f,0);
  }

  ~C_PhysicsScene(void)
  {
  	for (unsigned int i=0; i<mInstancePhysicsModels.size(); i++)
  	{
  		C_InstancePhysicsModel *ip = mInstancePhysicsModels[i];
  		delete ip;
  	}
  }

  virtual CELEMENT getBaseType(void) const { return CE_PHYSICS_SCENE; };
  virtual void     setText(CELEMENT operation,const char *txt) { };


	bool match(const char *str)
	{
		bool ret = false;
		if ( str )
		{
			if ( *str == '#' ) str++;
			if ( mId && strcmp(mId,str) == 0 )
				ret = true;
		}
		return ret;
	}

	void setGravity(const char *g)
	{
		Asc2Bin(g,3,"f", &mGravity.x);
	}

	void setTimeStep(const char *t)
	{
		mTimeStep = getFloat(t);
	}


	void saveXML(FILE *fph,C_Query *query);
	void loadPAL(C_Query *query);

  C_Vec3         mGravity;
  float          mTimeStep;

  CARRAY< C_InstancePhysicsModel * > mInstancePhysicsModels;
};

class C_LibraryPhysicsScenes : public C_Base
{
public:
  C_LibraryPhysicsScenes(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_LibraryPhysicsScenes(void)
  {
  	for (unsigned int i=0; i<mPhysicsScenes.size(); i++)
  	{
  		C_PhysicsScene *cp = mPhysicsScenes[i];
  		delete cp;
  	}
  }

  virtual CELEMENT getBaseType(void) const { return CE_LIBRARY_PHYSICS_SCENES; };
  virtual void     setText(CELEMENT operation,const char *txt) { };


	C_PhysicsScene * locatePhysicsScene(const char *str)
	{
		C_PhysicsScene *ret = 0;
  	for (unsigned int i=0; i<mPhysicsScenes.size(); i++)
  	{
  		C_PhysicsScene *cp = mPhysicsScenes[i];
  		if ( cp->match(str) )
  		{
  			ret = cp;
  			break;
  		}
  	}
  	return ret;
	}

  CARRAY< C_PhysicsScene * > mPhysicsScenes;
};

class C_LibraryGeometries : public C_Base
{
public:

  C_LibraryGeometries(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_LibraryGeometries(void)
  {
		for (unsigned int i=0; i<mGeometries.size(); i++)
		{
			C_Geometry *c = mGeometries[i];
			delete c;
		}
  }

  virtual CELEMENT getBaseType(void) const { return CE_LIBRARY_GEOMETRIES; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

  C_Geometry * locateGeometry(const char *str)
  {
  	C_Geometry *ret = 0;
  	for (unsigned int i=0; i<mGeometries.size(); i++)
  	{
  		C_Geometry *g = mGeometries[i];
  		if ( g->match(str) )
  		{
  			ret = g;
  			break;
  		}
  	}
  	return ret;
  }

  CARRAY< C_Geometry * > mGeometries;
};

class C_Node : public C_Base
{
public:

  C_Node(TiXmlElement *element) : C_Base(element)
  {
  	mType = getAttribute(element,"type");
  	mLayer = getAttribute(element,"layer");
  	mTransform.id();
  	mInstanceGeometry = 0;
  }

  virtual CELEMENT getBaseType(void) const { return CE_NODE; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	bool setMatrix(const char *v) // sets the matrix transform from this raw source text.
	{
		return colladaMatrix(v,mTransform);
	}

	void translate(const char *v)
	{
		C_Vec3 t;

		if ( Asc2Bin( v,	3, "f", &t.x ) )
		{
			mTransform.t+= t; // accumulate the translation
		}
	}

  void rotate(const char *r)
  {
  	float aa[4];

    if ( Asc2Bin(r, 4, "f", aa ) )
    {
    	C_Vec3 axis( aa[0], aa[1], aa[2] );
    	float angle = aa[3];
			C_Quat q(angle,axis);
			C_Matrix33 m;
			m.fromQuat(q);
			mTransform.M*=m;
    }

  }

	void setInstanceGeometry(const char *geom)
	{
		mInstanceGeometry = geom;
	}

	bool match(const char *str)
	{
		bool ret = false;
		if ( str && mId ) //&& mType && strcmp(mType,"NODE") == 0 )
		{
			if ( *str == '#' ) str++;
			if ( strcmp(str,mId) == 0 )
			  ret = true;
		}
		return ret;
	}

  const char *mLayer;
  const char *mType;
  C_Matrix34     mTransform;
  const char *mInstanceGeometry;
};

class C_VisualScene : public C_Base
{
public:
	C_VisualScene(TiXmlElement *element) : C_Base(element)
	{
	}

	~C_VisualScene(void)
	{
		for (unsigned int i=0; i<mNodes.size(); i++)
		{
			C_Node *n = mNodes[i];
			delete n;
		}
	}

  virtual CELEMENT getBaseType(void) const { return CE_VISUAL_SCENE; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	bool match(const char *str)
	{
		bool ret = false;
		if ( str )
		{
			if ( *str == '#' ) str++;
			if ( mId && strcmp(str,mId) == 0 ) ret = true;
		}
		return ret;
	}

	C_Node * locateNode(const char *str)
	{
		C_Node *ret = 0;
		for (unsigned int i=0; i<mNodes.size(); i++)
		{
			C_Node *n = mNodes[i];
			if ( n->match(str) )
			{
				ret = n;
				break;
			}
		}
		return ret;
	}

  CARRAY< C_Node *> mNodes;
};

class C_LibraryVisualScenes : public C_Base
{
public:
  C_LibraryVisualScenes(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_LibraryVisualScenes(void)
  {
  	for (unsigned int i=0; i<mVisualScenes.size(); i++)
  	{
  		C_VisualScene *s = mVisualScenes[i];
  		delete s;
  	}
  }

  virtual CELEMENT getBaseType(void) const { return CE_LIBRARY_VISUAL_SCENES; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	C_VisualScene * locateVisualScene(const char *str)
	{
		C_VisualScene *ret = 0;
		for (unsigned int i=0; i<mVisualScenes.size(); i++)
		{
			C_VisualScene *vs = mVisualScenes[i];
			if ( vs->match(str) )
			{
				ret = vs;
				break;
			}
		}
		return ret;
	}


	C_Node * locateNode(const char *str)
	{
		C_Node *ret = 0;
		for (unsigned int i=0; i<mVisualScenes.size(); i++)
		{
			C_VisualScene *vs = mVisualScenes[i];
			ret = vs->locateNode(str);
			if ( ret )
				break;
		}
		return ret;
	}

  CARRAY< C_VisualScene * > mVisualScenes;
};


class C_PhysicsMaterial : public C_Base
{
public:
  C_PhysicsMaterial(TiXmlElement *element,int index) : C_Base(element)
  {
  	mIndex = index;
 		mDynamicFriction = 0.5f;
 		mRestitution = 0;
 		mStaticFriction = 0.5f;
  }

  virtual CELEMENT getBaseType(void) const { return CE_PHYSICS_MATERIAL; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

	bool match(const char *str)
	{
		bool ret = false;
		if ( str && mId )
		{
			if ( *str == '#' ) str++;
			if ( strcmp(str,mId) == 0 )
				ret = true;
		}
		return ret;
	}

	//** Save in the NxuStream format.
	void saveXML(FILE *fph,int index);
	void loadPAL(int index);


  int                 mIndex;
  float               mDynamicFriction;
  float               mRestitution;
  float               mStaticFriction;
};

class C_LibraryPhysicsMaterials : public C_Base
{
public:

  C_LibraryPhysicsMaterials(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_LibraryPhysicsMaterials(void)
  {
  	for (unsigned int i=0; i<mPhysicsMaterials.size(); i++)
  	{
  		C_PhysicsMaterial *m = mPhysicsMaterials[i];
  		delete m;
  	}
  }

  virtual CELEMENT getBaseType(void) const { return CE_LIBRARY_PHYSICS_MATERIALS; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

  int getMaterialIndex(const char *mat)
  {
  	int ret = 0;
  	for (unsigned int i=0; i<mPhysicsMaterials.size(); i++)
  	{
  		C_PhysicsMaterial *m = mPhysicsMaterials[i];
  		if ( m->match(mat) )
  		{
  			ret = i;
  			break;
  		}
  	}
		return ret;
  }

	void saveXML(FILE *fph);
	void loadPAL();


  CARRAY< C_PhysicsMaterial * > mPhysicsMaterials;
};

class C_InstancePhysicsScene : public C_Base
{
public:
  C_InstancePhysicsScene(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_InstancePhysicsScene(void)
  {
  }

	void saveXML(FILE *fph,C_Query *query);
	void loadPAL(C_Query *query);

  virtual CELEMENT getBaseType(void) const { return CE_INSTANCE_PHYSICS_SCENE; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

};

class C_InstanceVisualScene : public C_Base
{
public:
  C_InstanceVisualScene(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_InstanceVisualScene(void)
  {
  }

  virtual CELEMENT getBaseType(void) const { return CE_INSTANCE_VISUAL_SCENE; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

};

class C_Scene : public C_Base
{
public:
  C_Scene(TiXmlElement *element) : C_Base(element)
  {
  }

  ~C_Scene(void)
  {
  	for (unsigned int i=0; i<mInstanceVisualScenes.size(); i++)
  	{
  		C_InstanceVisualScene *iv = mInstanceVisualScenes[i];
  		delete iv;
  	}
  	for (unsigned int i=0; i<mInstancePhysicsScenes.size(); i++)
  	{
  		C_InstancePhysicsScene *ip = mInstancePhysicsScenes[i];
  		delete ip;
  	}
  }


  void saveXML(FILE *fph,C_Query *query);
  void loadPAL(C_Query *query);

  virtual CELEMENT getBaseType(void) const { return CE_SCENE; };
  virtual void     setText(CELEMENT operation,const char *txt) { };

  CARRAY< C_InstanceVisualScene * >  mInstanceVisualScenes;
  CARRAY< C_InstancePhysicsScene * > mInstancePhysicsScenes;
};

class StackEntry
{
public:
  StackEntry(void)
  {
  	mNode = 0;
  	mElement = CE_IGNORE;
  }

  TiXmlNode	*mNode;
  CELEMENT   mElement;
};


class ColladaPhysics : public C_Query
{
public:
  ColladaPhysics(const char *fname)
  {
    mStackPtr = 0;
  	mCurrent = CE_IGNORE;
  	mOperation = CE_IGNORE;

		mScene = 0;
		mLibraryGeometries = 0;
		mLibraryVisualScenes = 0;
		mLibraryPhysicsMaterials = 0;
		mLibraryPhysicsModels = 0;
		mLibraryPhysicsScenes = 0;


		mGeometry          = 0;
    mVertices          = 0;
    mTriangles         = 0;
    mMesh              = 0;
    mSource            = 0;
    mArray             = 0;
    mAccessor          = 0;
    mParam             = 0;
		mVisualScene       = 0;
		mNode              = 0;
		mPhysicsMaterial   = 0;
		mPhysicsModel      = 0;
		mRigidBody         = 0;
		mShape             = 0;
		mInstancePhysicsModel = 0;
		mInstanceRigidBody = 0;
		mInstanceVisualScene = 0;
		mInstancePhysicsScene = 0;
		mDisableCollision = 0;
		mPhysicsScene = 0;
		mInstanceCount = 0;
		mBodyPairCount = 0;
		mJointCount = 0;
		mInstanceRigidConstraint = 0;
		mRigidConstraint = 0;

		mXML = new TiXmlDocument;
    mFph = 0;

    mConvexCount = 0;
    mMeshCount = 0;

		if ( mXML->LoadFile(fname) )
		{
			#if 0 // only for debugging help
      mFph = fopen("collada.txt","wb");
      #endif

  		mCurrent = CE_IGNORE;
	  	traverse(mXML,0);

 		  // ok..now we have to save out triangle meshes and convex hulls!
 		  for (unsigned int i=0; i<mMeshes.size(); i++)
 		  {
 		  	const char *geom = mMeshes[i];
 				C_Geometry *g =	locateGeometry( geom );
 				if ( g )
 				{
 					if ( g->isConvex() )
 					{
 						g->mIndex = mConvexCount;
 						mConvexCount++;
 					}
 					else
 					{
 						g->mIndex = mMeshCount;
 						mMeshCount++;
 					}
 				}
 		  }

			if ( mFph )
			{
         fclose(mFph);
      }


		}
		else
		{
  		if ( 1 )
  		{
  			char scratch[512];
  			sprintf(scratch,"Error parsing file '%s' as XML", fname );
  			printf("%s\r\n",scratch);
				sprintf(scratch,"XML parse error(%d) on Row: %d Column: %d", mXML->ErrorId(), mXML->ErrorRow(), mXML->ErrorCol() );
				printf("%s\r\n",scratch);
				sprintf(scratch,"XML error description: \"%s\"", mXML->ErrorDesc() );
				printf("%s\r\n",scratch);
  		}
			delete mXML;
			mXML = 0;
		}



  }

  ~ColladaPhysics(void)
  {
    if ( mFph )
    {
      fclose(mFph);
    }

	  delete  mScene;
    delete  mLibraryGeometries;
    delete  mLibraryVisualScenes;
    delete	mLibraryPhysicsMaterials;
    delete  mLibraryPhysicsModels;
    delete  mLibraryPhysicsScenes;

    delete mXML;

  }

  CELEMENT getELEMENT(const char *str)
  {
  	CELEMENT ret = CE_IGNORE;

		for (int i=0; i<CE_LAST; i++)
		{
			if ( strcmp(CE_NAMES[i],str) == 0 )
			{
				ret = (CELEMENT) i;
				break;
			}
		}

    return ret;
  }


  void Display(int depth,const char *fmt,...)
  {
    if ( mFph )
    {
      for (int i=0; i<depth; i++)
        fprintf(mFph,"  ");
     	char wbuff[8192];
     	vsprintf(wbuff, fmt, (char *)(&fmt+1));
     	fprintf(mFph,"%s", wbuff);
    }
  }

	void ProcessElement(CELEMENT etype,TiXmlElement *element,int depth)
	{
		switch ( etype )
		{
	    case CE_SCENE:
	    	if ( mScene )
	    	{
	    	  WARN("Encountered unexpected second 'scene' element in the document.");
	    	}
	    	else
	    	{
	    		 mScene = new C_Scene(element);
	    	}
	    	break;
	    case CE_LIBRARY_GEOMETRIES:
	    	if ( mLibraryGeometries )
	    	{
	    		WARN("Encountered unexpected library_geometries in the document.");
	    	}
	    	else
	    	{
	    		mLibraryGeometries = new C_LibraryGeometries(element);
	    	}
	    	break;
	    case CE_LIBRARY_PHYSICS_SCENES:
	    	if ( mLibraryPhysicsScenes )
	    	{
	    		WARN("Encountered unexpected second 'library_physics_scenes' element in the document.");
	    	}
	    	else
	    	{
	    		mLibraryPhysicsScenes = new C_LibraryPhysicsScenes(element);
	    	}
	    	break;
	    case CE_LIBRARY_VISUAL_SCENES:
	    	if ( mLibraryVisualScenes )
	    	{
	    		WARN("Encountered unexpected second 'library_visual_scenes' element in the document.");
	    	}
	    	else
	    	{
	    		mLibraryVisualScenes = new C_LibraryVisualScenes(element);
	    	}
	    	break;
	    case CE_LIBRARY_PHYSICS_MATERIALS:
	    	if ( mLibraryPhysicsMaterials )
	    	{
	    		WARN("Encountered unexpected second 'library_physics_materials' element in the document.");
	    	}
	    	else
	    	{
	    		mLibraryPhysicsMaterials = new C_LibraryPhysicsMaterials(element);
	    	}
	    	break;
      case CE_LIBRARY_PHYSICS_MODELS:
      	if ( mLibraryPhysicsModels )
      	{
	    		WARN("Encountered unexpected second 'library_physics_models' element in the document.");
      	}
      	else
      	{
      		mLibraryPhysicsModels = new C_LibraryPhysicsModels(element);
      	}
      	break;
	    case CE_GEOMETRY:
	    	if ( mLibraryGeometries )
	    	{
	    		mGeometry = new C_Geometry(element);
	    		mLibraryGeometries->mGeometries.push_back(mGeometry);
	    	}
	    	else
	    	{
	    		WARN("Encountered 'geometry' but not inside a geometries library!?");
	    	}
	    	break;
      case CE_CONVEX_MESH:
        if ( mGeometry )
        {
          mMesh = new C_Mesh(element,true);
          mGeometry->mMeshes.push_back(mMesh);
        }
        break;
	    case CE_MESH:
        if ( mGeometry )
        {
          mMesh = new C_Mesh(element,false);
          mGeometry->mMeshes.push_back(mMesh);
        }
	    	break;
	    case CE_SOURCE:
        if ( mMesh )
        {
          mSource = new C_Source(element);
          mMesh->mSources.push_back(mSource);
        }
	    	break;
	    case CE_FLOAT_ARRAY:
        if ( mSource )
        {
          if ( mSource->mArray == 0 )
          {
            C_FloatArray *a = new C_FloatArray(element);
            mArray = (C_Array *) a;
            mSource->mArray = mArray;
            mOperation = etype;
          }
        }
	    	break;
	    case CE_INT_ARRAY:
        if ( mSource )
        {
          if ( mSource->mArray == 0 )
          {
            C_IntArray *a = new C_IntArray(element);
            mArray = (C_Array *) a;
            mSource->mArray = mArray;
            mOperation = etype;
          }
        }
	    	break;
	    case CE_BOOL_ARRAY:
        if ( mSource )
        {
          if ( mSource->mArray == 0 )
          {
            C_BoolArray *a = new C_BoolArray(element);
            mArray = (C_Array *) a;
            mSource->mArray = mArray;
            mOperation = etype;
          }
        }
	    	break;
	    case CE_NAME_ARRAY:
        if ( mSource )
        {
          if ( mSource->mArray == 0 )
          {
            C_NameArray *a = new C_NameArray(element);
            mArray = (C_Array *) a;
            mSource->mArray = mArray;
            mOperation = etype;
          }
        }
	    	break;
	    case CE_IDREF_ARRAY:
        if ( mSource )
        {
          if ( mSource->mArray == 0 )
          {
            C_IDREFArray *a = new C_IDREFArray(element);
            mArray = (C_Array *) a;
            mSource->mArray = mArray;
            mOperation = etype;
          }
        }
	    	break;
	    case CE_ACCESSOR:
        if ( mSource )
        {
          if ( mSource->mAccessor == 0 )
          {
            mAccessor = new C_Accessor(element);
            mSource->mAccessor = mAccessor;
          }
        }
	    	break;
	    case CE_PARAM:
        if ( mAccessor )
        {
          mParam = new C_Param(element);
          mAccessor->mParams.push_back(mParam);
        }
	    	break;

	    case CE_VERTICES:
        if ( mMesh )
        {
          if ( mMesh->mVertices == 0 )
          {
            mVertices = new C_Vertices(element);
            mMesh->mVertices = mVertices;
          }
        }
	    	break;
	    case CE_INPUT:
        if ( 1 )
        {
          C_Input *i = new C_Input(element);
          if ( mTriangles )
          {
            mTriangles->mInputs.push_back(i);
          }
          else if ( mVertices )
          {
            mVertices->mInputs.push_back(i);
          }
        }
	    	break;
	    case CE_POLYGONS:
        if ( mMesh )
        {
          mTriangles = new C_Triangles(element,IT_POLYGONS);
          mMesh->mTriangles.push_back(mTriangles);
        }
	    	break;
	    case CE_TRIANGLES:
        if ( mMesh )
        {
          mTriangles = new C_Triangles(element,IT_TRIANGLES);
          mMesh->mTriangles.push_back(mTriangles);
        }
	    	break;
	    case CE_POLYLIST:
        if ( mMesh )
        {
          mTriangles = new C_Triangles(element,IT_POLYLIST);
          mMesh->mTriangles.push_back(mTriangles);
        }
	    	break;
	    case CE_P:
	    case CE_VCOUNT:
        mOperation = etype;
	    	break;
	    case CE_VISUAL_SCENE:
	    	if ( mLibraryVisualScenes )
	    	{
	    		mVisualScene = new C_VisualScene(element);
	    		mLibraryVisualScenes->mVisualScenes.push_back(mVisualScene);
	    	}
	    	else
	    	{
	    		WARN("Encountered a visual scene element but not inside a library visual scene?");
	    	}
	    	break;
	    case CE_NODE:
	    	if ( mVisualScene )
	    	{
	    		mNode = new C_Node(element);
	    		mVisualScene->mNodes.push_back(mNode);
	    	}
	    	else
	    	{
	    		// It *is* valid to have a 'node' within a Node or 'library_nodes'
	    		WARN("Encountered a node element but not processing a VisualScene");
	    	}
	    	break;
	    case CE_MATRIX:
	    case CE_TRANSLATE:
	    case CE_ROTATE:
	    	mOperation = etype;
	    	break;
	    case CE_INSTANCE_GEOMETRY:
	    	if ( 1 )
	    	{
	    		const char *mesh = getAttribute(element,"url");
	    		if ( mesh )
	    		{
	    			if ( mCurrent == CE_NODE && mNode )
	    			{
	    			  mNode->setInstanceGeometry(mesh);
	    			}
						if ( mCurrent == CE_SHAPE && mShape )
						{

							mShape->setInstancedGeometry(mesh);

							bool found = false;

							for (unsigned int i=0; i<mMeshes.size(); i++)
							{
								const char *m = mMeshes[i];
								if ( strcmp(m,mesh) == 0 )
								{
									found = true;
									break;
								}
							}
							if ( !found )
							{
  							mMeshes.push_back(mesh); // save this in the list of
  						}
						}
	    		}
	    	}
	    	break;
	    case CE_PHYSICS_MATERIAL:
	    	if ( mLibraryPhysicsMaterials )
	    	{
	    		mPhysicsMaterial = new C_PhysicsMaterial(element,mLibraryPhysicsMaterials->mPhysicsMaterials.size());
	    		mLibraryPhysicsMaterials->mPhysicsMaterials.push_back(mPhysicsMaterial);
	    	}
	    	else
	    	{
	    		WARN("Encountered a PhysicsMaterial element outside of the PhysicsLibrary");
	    	}
	    	break;
      case CE_PHYSICS_MODEL:
      	if ( mLibraryPhysicsModels )
      	{
      		mPhysicsModel = new C_PhysicsModel(element);
      		mLibraryPhysicsModels->mPhysicsModels.push_back(mPhysicsModel);
      	}
      	else
      	{
	    		WARN("Encountered a PhysicsModel element outside of the PhysicsModelLibrary");
      	}
      	break;
      case CE_RIGID_CONSTRAINT:
      	if ( mPhysicsModel )
      	{
      		mRigidConstraint = new C_RigidConstraint(element);
      		mPhysicsModel->mRigidConstraints.push_back(mRigidConstraint);
      	}
      	break;
      case CE_RIGID_BODY:
      	if ( mPhysicsModel )
      	{
      		mRigidBody = new C_RigidBody(element);
      		mPhysicsModel->mRigidBodies.push_back(mRigidBody);
      	}
      	else
      	{
	    		WARN("Encountered a RigidBody element but not inside a PhysicsModel");
      	}
      	break;
	    case CE_INSTANCE_PHYSICS_MATERIAL:
	    	if ( 1 )
	    	{
	    		const char *url = getAttribute(element,"url");
	    		if ( url )
	    		{
	    			switch ( mCurrent )
	    			{
	    				case CE_SHAPE:
	    					if ( mShape )
	    					{
	    						mShape->mInstancePhysicsMaterial = url;
	    					}
	    					break;
	    				case CE_RIGID_BODY:
	    					if ( mRigidBody )
	    					{
	    						mRigidBody->mInstancePhysicsMaterial = url;
	    					}
	    					break;
	    			}
	    		}
	    	}
	    	break;

			// material values
	    case CE_DYNAMIC_FRICTION:
      case CE_RESTITUTION:
      case CE_STATIC_FRICTION:
	    	mOperation = etype;
				break;
			// rigid body values
	    case CE_DYNAMIC:
				mOperation = etype;
				break;
	    case CE_MASS:
	    case CE_MASS_FRAME:
	    case CE_INERTIA:
	    // shape values.
	    case CE_DENSITY:
	    case CE_HALF_EXTENTS:
	    case CE_EQUATION:
	    case CE_HEIGHT:
	    	mOperation = etype;
	    	break;
	    case CE_RADIUS:
	    	mOperation = etype;
				break;
	    case CE_SHAPE:
	    	if ( mRigidBody )
	    	{
	    		mShape = new C_Shape(element);
	    		mRigidBody->mShapes.push_back(mShape);
	    	}
	    	else
	    	{
	    		WARN("Encountered a Shape element outside of the RigidBody");
	    	}
	    	break;
	    case CE_CAPSULE:
	    case CE_CYLINDER:
	    case CE_TAPERED_CAPSULE:
	    case CE_TAPERED_CYLINDER:
	    	if ( mShape )
	    	{
	    		mShape->mShapeType = CST_CAPSULE;
	    	}
	    	else
	    	{
	    		WARN("Encountered a Capsule element outside of a Shape");
	    	}
	    	break;
	    case CE_SPHERE:
	    	if ( mShape )
	    	{
	    		mShape->mShapeType = CST_SPHERE;
	    	}
	    	else
	    	{
	    		WARN("Encountered a Sphere element outside of a Shape");
	    	}
	    	break;
	    case CE_PLANE:
	    	if ( mShape )
	    	{
	    		mShape->mShapeType = CST_PLANE;
	    	}
	    	else
	    	{
	    		WARN("Encountered a Plane element outside of a Shape");
	    	}
	    	break;
	    case CE_BOX:
	    	if ( mShape )
	    	{
	    		mShape->mShapeType = CST_BOX;
	    	}
	    	else
	    	{
	    		WARN("Encountered a Box element outside of a Shape");
	    	}
	    	break;
	    case CE_PHYSICS_SCENE:
	    	if ( mLibraryPhysicsScenes )
	    	{
	    		mPhysicsScene = new C_PhysicsScene(element);
	    		mLibraryPhysicsScenes->mPhysicsScenes.push_back(mPhysicsScene);
	    	}
	    	else
	    	{
	    		WARN("Encountered a PhysicsScene element outside of a LibraryPhysicsScene");
	    	}
	    	break;
	    case CE_INSTANCE_PHYSICS_MODEL:
	    	if ( mPhysicsScene )
	    	{
	    		mInstancePhysicsModel = new C_InstancePhysicsModel(element);
	    		mPhysicsScene->mInstancePhysicsModels.push_back(mInstancePhysicsModel);
	    	}
	    	else
	    	{
	    		WARN("Encountered an InstancePhysicsModel element outside of a PhysicsScene");
	    	}
	    	break;
	    case CE_INSTANCE_RIGID_CONSTRAINT:
	    	if ( mInstancePhysicsModel )
	    	{
	    		mInstanceRigidConstraint = new C_InstanceRigidConstraint(element);
	    		mInstancePhysicsModel->mInstanceRigidConstraints.push_back(mInstanceRigidConstraint);
	    		mJointCount++;
	    	}
	    	break;
	    case CE_DISABLE_COLLISION:
	    	if ( mInstancePhysicsModel )
	    	{
	    		mDisableCollision = new C_DisableCollision(element);
	    		mInstancePhysicsModel->mDisableCollisions.push_back(mDisableCollision);
	    		mBodyPairCount++;
	    	}
	    	else
	    	{
	    		mOperation = etype;
	    	}
	    	break;
	    case CE_INSTANCE_RIGID_BODY:
	    	if ( mInstancePhysicsModel )
	    	{
	    		mInstanceRigidBody = new C_InstanceRigidBody(element);
	    		mInstancePhysicsModel->mInstanceRigidBodies.push_back(mInstanceRigidBody);
	    		mInstanceCount++;
	    	}
	    	else
	    	{
	    		WARN("Encountered an InstanceRigidBody element outside of an InstancePhysicsModel");
	    	}
	    	break;
	    case CE_VELOCITY:
	    	if ( mInstanceRigidBody )
	    	{
	    		mOperation = etype;
	    	}
	    	else
	    	{
	    		WARN("Encountered a velocity element outside of an InstanceRigidBody");
	    	}
	    	break;
	    case CE_ANGULAR_VELOCITY:
	    	if ( mInstanceRigidBody )
	    	{
	    		mOperation = etype;
	    	}
	    	else
	    	{
	    		WARN("Encountered an angular velocity element outside of an InstanceRigidBody");
	    	}
	    	break;
	    case CE_GRAVITY:
	    	if ( mPhysicsScene )
	    	{
	    		mOperation = etype;
	    	}
	    	else
	    	{
	    		WARN("Encountered a gravity element outside of a PhysicsScene");
	    	}
	    	break;
	    case CE_TIME_STEP:
	    	if ( mPhysicsScene )
	    	{
	    		mOperation = etype;
	    	}
	    	else
	    	{
	    		WARN("Encountered a time_step element outside of a PhysicsScene");
	    	}
	    	break;
	    case CE_INSTANCE_VISUAL_SCENE:
	    	if ( mScene )
	    	{
	    		mInstanceVisualScene = new C_InstanceVisualScene(element);
	    		mScene->mInstanceVisualScenes.push_back(mInstanceVisualScene);
	    	}
	    	break;
	    case CE_INSTANCE_PHYSICS_SCENE:
	    	if ( mScene )
	    	{
	    		mInstancePhysicsScene = new C_InstancePhysicsScene(element);
					mScene->mInstancePhysicsScenes.push_back(mInstancePhysicsScene);
	    	}
	    	break;
	    case CE_REF_ATTACHMENT:
	    	if ( mRigidConstraint )
	    		mRigidConstraint->setRefAttachment(element);
	    	break;
	    case CE_ATTACHMENT:
	    	if ( mRigidConstraint )
	    		mRigidConstraint->setAttachment(element);
	    	break;
	    case CE_LINEAR:
	    	if ( mRigidConstraint )
	    		mRigidConstraint->mLinear = true;
	    	break;
	    case CE_ANGULAR:
	    case CE_SWING_CONE_AND_TWIST:
	    	if ( mRigidConstraint )
	    		mRigidConstraint->mLinear = false;
	    	break;
	    case CE_NX_JOINT_DRIVE_DESC:
	    	if ( mRigidConstraint )
	    		mRigidConstraint->setJointDriveDesc(element);
	    	break;
			case CE_MIN:
			case CE_MAX:
			case CE_STIFFNESS:
			case CE_DAMPING:
			case CE_TARGET_VALUE:
			case CE_ENABLED:
			case CE_INTERPENETRATE:
			case CE_GROUP:
			case CE_SKIN_WIDTH:
			case CE_WAKEUP_COUNTER:
			case CE_LINEAR_DAMPING:
			case CE_ANGULAR_DAMPING:
			case CE_MAX_ANGULAR_VELOCITY:
			case CE_SLEEP_LINEAR_VELOCITY:
			case CE_SLEEP_ANGULAR_VELOCITY:
			case CE_DISABLE_GRAVITY:
			case CE_KINEMATIC:
			case CE_POSE_SLEEP_TEST:
			case CE_FILTER_SLEEP_VELOCITY:
			case CE_DISABLE_RESPONSE:
			case CE_LOCK_COM:
			case CE_PROJECTION_MODE:
			case CE_PROJECTION_ANGLE:
			case CE_PROJECTION_DISTANCE:
			case CE_SOLVER_ITERATION_COUNT:
		  case CE_DRIVE_TYPE:
		  case CE_SPRING:
		  case CE_FORCE_LIMIT:
		  case CE_DRIVE_POSITION:
		  case CE_DRIVE_ORIENTATION:
		  case CE_DRIVE_LINEAR_VELOCITY:
		  case CE_DRIVE_ANGULAR_VELOCITY:
		  case CE_GEAR_RATIO:
				mOperation = etype;
				break;
	  }
	}


  void ProcessNode(int ntype,const char *svalue,int depth,TiXmlNode *node)
  {
  	char value[43];
  	value[39] = '.';
  	value[40] = '.';
  	value[41] = '.';
  	value[42] = 0;

  	strncpy(value,svalue,39);

  	switch ( ntype )
  	{
  		case TiXmlNode::ELEMENT:
  		case TiXmlNode::DOCUMENT:
  			if ( ntype == TiXmlNode::DOCUMENT )
  				Display(depth,"Node(DOCUMENT): %s\n", value);
  			else
  			{
   				Display(depth,"Node(ELEMENT): %s\n", value);
			    CELEMENT e = getELEMENT(svalue);
			    if ( e != CE_IGNORE )
			    {
      	    TiXmlElement *element = node->ToElement(); // is there an element?  Yes, traverse it's attribute key-pair values.
			    	ProcessElement(e,element,depth);
			    }
   			}
  			break;
  		case TiXmlNode::TEXT:

  			Display(depth,"Node(TEXT): %s\n", value);

				switch ( mCurrent )
				{
					case CE_RIGID_CONSTRAINT:
					case CE_NX_JOINT_DRIVE_DESC:
						if ( mRigidConstraint )
						{
							mRigidConstraint->setText(mOperation,svalue);
						}
						break;
          case CE_TRIANGLES:
          case CE_POLYGONS:
          case CE_POLYLIST:
            if ( mTriangles && mOperation == CE_P )
            {
              mTriangles->addPoints(svalue);
            }
            if ( mTriangles && mOperation == CE_VCOUNT )
            {
              mTriangles->addVcount(svalue);
            }
            break;
          case CE_FLOAT_ARRAY:
          case CE_INT_ARRAY:
          case CE_BOOL_ARRAY:
          case CE_NAME_ARRAY:
          case CE_IDREF_ARRAY:
            if ( mArray )
            {
              mArray->setText(mOperation,svalue);
            }
            break;
					case CE_PHYSICS_SCENE:
						if ( mPhysicsScene )
						{
							switch ( mOperation )
							{
								case CE_GRAVITY:
									mPhysicsScene->setGravity(svalue);
									break;
								case CE_TIME_STEP:
									mPhysicsScene->setTimeStep(svalue);
									break;
							}
						}
						break;
					case CE_INSTANCE_RIGID_BODY:
						if ( mInstanceRigidBody )
						{
							switch ( mOperation )
							{
								case CE_VELOCITY:
								  mInstanceRigidBody->setVelocity(svalue);
								  break;
								case CE_ANGULAR_VELOCITY:
								  mInstanceRigidBody->setAngularVelocity(svalue);
								  break;
							}
						}
						break;
					case CE_SHAPE:
						if ( mShape )
						{
							mShape->setText(mOperation,svalue);

						}
						break;
					case CE_MASS_FRAME:
						if ( mRigidBody )
						{
							switch ( mOperation )
							{
								case CE_TRANSLATE:
									mRigidBody->setMassFrameTranslate(svalue);
									break;
								case CE_ROTATE:
									mRigidBody->setMassFrameRotate(svalue);
									break;
								case CE_MATRIX:
									mRigidBody->setMassFrame(svalue);
									break;
							}
						}
						break;
					case CE_RIGID_BODY:
						if ( mRigidBody )
						{
							mRigidBody->setText(mOperation,svalue);
						}
						break;
					case CE_NODE:
						if ( mNode )
						{
							switch ( mOperation )
							{
								case CE_MATRIX:
      						mNode->setMatrix(svalue);
      						break;
      					case CE_TRANSLATE:
      						mNode->translate(svalue);
      						break;
      					case CE_ROTATE:
      						mNode->rotate(svalue);
      						break;
      				}
  					}
  					break;
					case CE_PHYSICS_MATERIAL:
						if ( mPhysicsMaterial )
						{
							float v = getFloat ( svalue );
							switch ( mOperation )
							{
								case CE_DYNAMIC_FRICTION:
									mPhysicsMaterial->mDynamicFriction = v;
									break;
								case CE_STATIC_FRICTION:
									mPhysicsMaterial->mStaticFriction = v;
									break;
								case CE_RESTITUTION:
									mPhysicsMaterial->mRestitution = v;
									break;
							}
						}
						break;
  			}

  			break;
  		case TiXmlNode::COMMENT:
  			Display(depth,"Node(COMMENT): %s\n", value);
  			break;
  		case TiXmlNode::DECLARATION:
  			Display(depth,"Node(DECLARATION): %s\n", value);
  			break;
  		case TiXmlNode::UNKNOWN:
  			Display(depth,"Node(UNKNOWN): %s\n", value);
  			break;
  		default:
  			Display(depth,"Node(?????): %s\n", value);
  			break;
  	}
  }


	void traverse(TiXmlNode *node,int depth)
	{

   	ProcessNode(node->Type(),node->Value(),depth,node);

		node = node->FirstChild();
		while (node )
		{
			if ( node->NoChildren() )
			{
     	  ProcessNode(node->Type(),node->Value(),depth,node);
			}
			else
			{
				push(node);

				traverse(node,depth+1);
		    closeNode(node); //

		    pop(node);
			}
			node = node->NextSibling();
		}
	}

  void closeNode(TiXmlNode *node)
  {
  	if ( node && node->Type() == TiXmlNode::ELEMENT )
  	{

  		CELEMENT from = getELEMENT( node->Value() );

 			switch ( from )
 			{
 				case CE_NX_JOINT_DRIVE_DESC:
 					if ( mRigidConstraint )
 						mRigidConstraint->mDrive = 0; // cancel out the drive pointer.
 					break;
        case CE_TRIANGLES:
        case CE_POLYGONS:
        case CE_POLYLIST:
          mTriangles = 0;
          break;
        case CE_VERTICES:
          mVertices = 0;
          break;
				case CE_GEOMETRY:
					mGeometry = 0;
 					break;
				case CE_VISUAL_SCENE:
 					mVisualScene = 0;
 					break;
 				case CE_NODE:
 					mNode = 0;
 					break;
 				case CE_PHYSICS_MATERIAL:
 					mPhysicsMaterial = 0;
 					break;
 				case CE_PHYSICS_MODEL:
 					mPhysicsModel = 0;
 					break;
 				case CE_RIGID_BODY:
 					mRigidBody = 0;
 					break;
 				case CE_SHAPE:
 					mShape = 0;
 					break;
 				case CE_INSTANCE_PHYSICS_MODEL:
 					mInstancePhysicsModel = 0;
 					break;
 				case CE_INSTANCE_RIGID_BODY:
 					mInstanceRigidBody = 0;
 					break;
 				case CE_INSTANCE_VISUAL_SCENE:
 					mInstanceVisualScene = 0;
 					break;
 				case CE_INSTANCE_PHYSICS_SCENE:
 					mInstancePhysicsScene = 0;
 					break;
 				case CE_PHYSICS_SCENE:
 					mPhysicsScene = 0;
 					break;
        case CE_MESH:
        case CE_CONVEX_MESH:
          mMesh = 0;
          break;
        case CE_SOURCE:
          mSource = 0;
          break;
        case CE_FLOAT_ARRAY:
        case CE_INT_ARRAY:
        case CE_BOOL_ARRAY:
        case CE_NAME_ARRAY:
        case CE_IDREF_ARRAY:
          mArray = 0;
          break;
        case CE_ACCESSOR:
          mAccessor = 0;
          break;
        case CE_PARAM:
          mParam = 0;
          break;
        case CE_RIGID_CONSTRAINT:
        	mRigidConstraint = 0;
        	break;
 				default:
 				  break;
  		}
  	}
  	mOperation = CE_IGNORE;
	}


  virtual C_Geometry * locateGeometry(const char *str)
  {
  	C_Geometry * ret = 0;
  	if ( mLibraryGeometries )
  	{
  		ret = mLibraryGeometries->locateGeometry(str);
  	}
  	return ret;
  }

  virtual C_VisualScene * locateVisualScene(const char *str)
  {
  	C_VisualScene *ret = 0;

  	if ( mLibraryVisualScenes )
  	{
  		ret = mLibraryVisualScenes->locateVisualScene(str);
  	}

		return ret;
  }

  virtual C_PhysicsScene * locatePhysicsScene(const char *str)
  {
  	C_PhysicsScene *ret = 0;

    if ( mLibraryPhysicsScenes && str )
    {
    	ret = mLibraryPhysicsScenes->locatePhysicsScene(str);
    }
  	return ret;
  }

	virtual C_PhysicsModel * locatePhysicsModel(const char *str)
	{
		C_PhysicsModel *ret = 0;
		if ( mLibraryPhysicsModels && str )
		{
			ret = mLibraryPhysicsModels->locatePhysicsModel(str);
		}
		return ret;
	}


  virtual int              getMaterialIndex(const char *mat)
  {
		int ret = 0;

    if ( mLibraryPhysicsMaterials )
    {
    	ret = mLibraryPhysicsMaterials->getMaterialIndex(mat);
    }
  	return ret;
  }

	C_Node * locateNode(const char *str)
	{
		C_Node *ret = 0;
		if ( mLibraryVisualScenes )
		{
			ret = mLibraryVisualScenes->locateNode(str);
		}
		return ret;
	}

	bool loadPAL() {
		bool ret = false;
			ret = true;
			C_Vec3 gravity(0,-9.8f,0);

			if ( mLibraryPhysicsScenes ) {
				if ( mLibraryPhysicsScenes->mPhysicsScenes.size() )	{
					C_PhysicsScene *p = mLibraryPhysicsScenes->mPhysicsScenes[0];
					gravity = p->mGravity;
				}
			}
			unsigned int matcount = 0;

			if ( mLibraryPhysicsMaterials )	{
				matcount = mLibraryPhysicsMaterials->mPhysicsMaterials.size();
			}

			palPhysics *pp = PF->GetActivePhysics();
			pp->Init(gravity.x,gravity.y,gravity.z);

			

#if 1
			// write out the convex hulls
			if ( mConvexCount )	{
				for (unsigned int i=0; i<mMeshes.size(); i++) {
					const char *geom = mMeshes[i];
					C_Geometry *g =	locateGeometry( geom );
					if ( g ) {
						if ( g->isConvex() ) {
							g->loadPAL(this);
						}
					}
				}
			}
#endif
//materials
			if ( mLibraryPhysicsMaterials )	{
				mLibraryPhysicsMaterials->loadPAL();
			}
//scene
  		if ( mScene ) {

			mScene->loadPAL(this);
  		}

	
		return ret;
	}
	bool saveNxuStream(const char *fname)
	{
		bool ret = false;

		mFph = fopen(fname,"wb");
		if ( mFph )
		{
			ret = true;

			C_Vec3 gravity(0,-9.8f,0);

  		if ( mLibraryPhysicsScenes )
  		{
  			if ( mLibraryPhysicsScenes->mPhysicsScenes.size() )
  			{
  				C_PhysicsScene *p = mLibraryPhysicsScenes->mPhysicsScenes[0];
  				gravity = p->mGravity;
  			}
  		}

			unsigned int matcount = 0;

			if ( mLibraryPhysicsMaterials )
			{
				matcount = mLibraryPhysicsMaterials->mPhysicsMaterials.size();
			}


      fprintf(mFph,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
      fprintf(mFph,"<NXUSTREAM  asset=\"\" sdkversion=\"244\">\r\n");
      fprintf(mFph,"  <NxPhysicsSDK version=\"244\">\r\n");
      fprintf(mFph,"    <numParams>0</numParams>\r\n");
      fprintf(mFph,"    <numConvexes>%d</numConvexes>\r\n", mConvexCount);
      fprintf(mFph,"    <numTrimesh>%d</numTrimesh>\r\n", mMeshCount);
      fprintf(mFph,"    <numHeightFields>0</numHeightFields>\r\n");
      fprintf(mFph,"    <numSkeletons>0</numSkeletons>\r\n");
      fprintf(mFph,"    <numScenes>1</numScenes>\r\n");
      fprintf(mFph,"  </NxPhysicsSDK>\r\n");

			// write out the convex hulls
			if ( mConvexCount )
			{
  		  for (unsigned int i=0; i<mMeshes.size(); i++)
  		  {
  		  	const char *geom = mMeshes[i];
  				C_Geometry *g =	locateGeometry( geom );
  				if ( g )
  				{
  					if ( g->isConvex() )
  					{
  						g->saveXML(mFph,this);
  					}
  				}
  		  }
			}

			// write out the triangle meshes.
			if ( mMeshCount )
			{
  		  for (unsigned int i=0; i<mMeshes.size(); i++)
  		  {
  		  	const char *geom = mMeshes[i];
  				C_Geometry *g =	locateGeometry( geom );
  				if ( g )
  				{
  					if ( !g->isConvex() )
  					{
  						g->saveXML(mFph,this);
  					}
  				}
  		  }
			}


      fprintf(mFph,"  <NxSceneDesc id=\"Scene_0\">\r\n");
      fprintf(mFph,"    <groundPlane>true</groundPlane>\r\n");
      fprintf(mFph,"    <boundsPlanes>false</boundsPlanes>\r\n");
      fprintf(mFph,"    <gravity>%s %s %s</gravity>\r\n",fstring( gravity.x), fstring(gravity.y), fstring(gravity.z) );
      fprintf(mFph,"    <timeStepMethod>NX_TIMESTEP_FIXED</timeStepMethod>\r\n");
      fprintf(mFph,"    <maxTimestep>0.01</maxTimestep>\r\n");
      fprintf(mFph,"    <maxIter>2</maxIter>\r\n");
      fprintf(mFph,"    <simType>NX_SIMULATION_SW</simType>\r\n");
      fprintf(mFph,"    <hwSceneType>NX_HW_SCENE_TYPE_RB</hwSceneType>\r\n");
      fprintf(mFph,"    <pipelineSpec>NX_HW_PIPELINE_FULL</pipelineSpec>\r\n");
      fprintf(mFph,"    <hasLimits>false</hasLimits>\r\n");
      fprintf(mFph,"    <hasBounds>false</hasBounds>\r\n");
      fprintf(mFph,"    <internalThreadCount>0</internalThreadCount>\r\n");
      fprintf(mFph,"    <threadMask>1431655764</threadMask>\r\n");
      fprintf(mFph,"    <NxScene id=\"Scene_0_contents\">\r\n");
      fprintf(mFph,"      <name>Scene_0_contents</name>\r\n");
      fprintf(mFph,"      <matCount>%d</matCount>\r\n", matcount );
      fprintf(mFph,"      <actorCount>%d</actorCount>\r\n", mInstanceCount);
      fprintf(mFph,"      <jointCount>0</jointCount>\r\n");
      fprintf(mFph,"      <pairFlagCount>0</pairFlagCount>\r\n");
      fprintf(mFph,"      <effectorCount>0</effectorCount>\r\n");
      fprintf(mFph,"      <collisionGroupCount>0</collisionGroupCount>\r\n");
      fprintf(mFph,"      <fluidCount>0</fluidCount>\r\n");
      fprintf(mFph,"      <clothCount>0</clothCount>\r\n");

			if ( mLibraryPhysicsMaterials )
			{
				mLibraryPhysicsMaterials->saveXML(mFph);
			}



  		if ( mScene )
	  	{
		  	mScene->saveXML(mFph,this);
  		}

      fprintf(mFph,"    </NxScene>\r\n");
      fprintf(mFph,"  </NxSceneDesc>\r\n");
      fprintf(mFph,"</NXUSTREAM>\r\n");

			fclose(mFph);
		}
		return ret;
	}


private:

	void push(TiXmlNode *node)
	{
		assert( mStackPtr < NODE_STACK );
		CELEMENT e = getELEMENT(node->Value() );
		if ( isBlock(e) )
		{
  		if ( mStackPtr < NODE_STACK )
  		{
  			mStack[mStackPtr].mNode = node;
  			mStack[mStackPtr].mElement = e;
  			mStackPtr++;
  		}
   		mCurrent = e;
    }
	}

	void pop(TiXmlNode *node)
	{
		assert( mStackPtr < NODE_STACK );
		CELEMENT e = getELEMENT(node->Value() );
		if ( isBlock(e) )
		{
  		if ( mStackPtr )
  		{
  			mStackPtr--;
  			if ( mStackPtr )
  			{
  				mStack[mStackPtr].mElement = CE_IGNORE;
  				mStack[mStackPtr].mNode    = 0;
  				mCurrent = mStack[mStackPtr-1].mElement;
  			}
  			else
  			{
  				mCurrent = CE_IGNORE;
  			}
  		}
  	}
	}

	void printStack(const char *action)
	{
		Display(mStackPtr,"STACK REPORT: %s\r\n", action );
		for (int i=0; i<mStackPtr; i++)
		{
			const char *v = mStack[i].mNode->Value();
			int j = mStack[i].mElement;
			const char *str = CE_NAMES[j];
			Display(mStackPtr,"%d : %s  : %s\r\n", i+1, str, v );
		}
		Display(mStackPtr,"\r\n");
	}

	bool isBlock(CELEMENT e) const // if it a 'block' section we push/pop off of the stack.
  {
  	bool ret = false;
  	switch ( e )
  	{
    	case CE_SCENE:
    	case CE_LIBRARY_GEOMETRIES:
    	case CE_LIBRARY_PHYSICS_SCENES:
    	case CE_LIBRARY_VISUAL_SCENES:
    	case CE_LIBRARY_PHYSICS_MATERIALS:
    	case CE_LIBRARY_PHYSICS_MODELS:
    	case CE_GEOMETRY:
    	case CE_VISUAL_SCENE:
    	case CE_NODE:
    	case CE_PHYSICS_MATERIAL:
    	case CE_PHYSICS_MODEL:
    	case CE_RIGID_BODY:
    	case CE_SHAPE:
    	case CE_PHYSICS_SCENE:
    	case CE_INSTANCE_PHYSICS_MODEL:
    	case CE_INSTANCE_VISUAL_SCENE:
    	case CE_INSTANCE_PHYSICS_SCENE:
    	case CE_INSTANCE_RIGID_BODY:
    	case CE_MASS_FRAME:
      case CE_CONVEX_MESH:
      case CE_MESH:
      case CE_SOURCE:
      case CE_FLOAT_ARRAY:
			case CE_INT_ARRAY:
			case CE_BOOL_ARRAY:
			case CE_NAME_ARRAY:
			case CE_IDREF_ARRAY:
      case CE_ACCESSOR:
      case CE_PARAM:
      case CE_TRIANGLES:
      case CE_POLYLIST:
      case CE_POLYGONS:
      case CE_VERTICES:
      case CE_RIGID_CONSTRAINT:
    		ret = true;
    		break;
  	}
		return ret;
  }


  FILE                      *mFph;
  TiXmlDocument             *mXML;

	int                        mStackPtr;
	StackEntry                 mStack[NODE_STACK];

	CELEMENT									 mCurrent; // current major element type we are parsing.
	CELEMENT                   mOperation;

//
  C_Geometry                *mGeometry;              // current geometry being processed.
  C_Mesh                    *mMesh;
  C_Triangles               *mTriangles;
  C_Vertices                *mVertices;
  C_Source                  *mSource;
  C_Array                   *mArray;
  C_Accessor                *mAccessor;
  C_Param                   *mParam;
  C_VisualScene             *mVisualScene;
  C_Node                    *mNode;
  C_PhysicsMaterial         *mPhysicsMaterial;
  C_PhysicsModel            *mPhysicsModel;
  C_RigidBody               *mRigidBody;
  C_Shape                   *mShape;
  C_InstancePhysicsModel    *mInstancePhysicsModel;
  C_InstanceRigidBody       *mInstanceRigidBody;
  C_InstanceVisualScene     *mInstanceVisualScene;
  C_InstancePhysicsScene    *mInstancePhysicsScene;
  C_PhysicsScene            *mPhysicsScene;
  C_RigidConstraint         *mRigidConstraint;
  C_InstanceRigidConstraint *mInstanceRigidConstraint;
  C_DisableCollision        *mDisableCollision;
  int                        mInstanceCount;
  int                        mJointCount;
  int                        mBodyPairCount;
//


	C_Scene                   *mScene;									     // describes the instancing of visual and physics scenes.

  C_LibraryGeometries			  *mLibraryGeometries;           // all of the geometries in the file
  C_LibraryPhysicsMaterials	*mLibraryPhysicsMaterials;     // all of the physics materials in the file.

  C_LibraryVisualScenes     *mLibraryVisualScenes;         // all of the visual scenes.
  C_LibraryPhysicsModels    *mLibraryPhysicsModels;        // all of the physics models.
  C_LibraryPhysicsScenes    *mLibraryPhysicsScenes;        // all of the physics scenes.

  int                        mConvexCount; // number of unique convex hulls instantiated.
  int                        mMeshCount;   // number of unique triangle meshes instantiated.
	CARRAY< const char * >    mMeshes; // list of meshes that are instantiated by the physics as either or convex hulls.


};

ColladaPhysics * loadColladaPhysics(const char *collada_name)
{
	ColladaPhysics *ret = new ColladaPhysics(collada_name);
	return ret;
}

bool loadPAL(ColladaPhysics *cp) {
	bool ret = false;

  if ( cp )
  {
  	ret = cp->loadPAL();
  }

  return ret;
}

bool             saveNxuStream(ColladaPhysics *cp,const char *nxustream_name)
{
	bool ret = false;

  if ( cp )
  {
  	ret = cp->saveNxuStream(nxustream_name);
  }

  return ret;
}

void             releaseColladaPhysics(ColladaPhysics *cp)
{
	delete cp;
}

void C_Mesh::getTriangleMesh(C_TriangleMesh &t,C_Query *q)
{
	if ( mConvexHullOf )
	{
		C_Geometry *geom = q->locateGeometry(mConvexHullOf);
		if ( geom )
		{
			geom->getTriangleMesh(t,q);
		}
	}
	if ( mVertices ) // if we have 'vertices' defined.
	{
 		C_Input *input = mVertices->locateInput("POSITION"); // locate the source input that uses this semantic!
 		if ( input ) // ok, we have the 'position' input.
 		{
 			C_Source *source = locateSource( input->mSource );
 			// ok..so far we have identified that the vertices specification has a 'position' field and we have located the source for it.
 			if ( source )
 			{
 				for (unsigned int i=0; i<mTriangles.size(); i++)
				{
 					C_Triangles *tris = mTriangles[i];
 					tris->getTriangleMesh(t, source, mVertices );
 				}
 			}
 		}
 	}
}

//**********************************************************************
//**********************************************************************
//*** Methods to write COLLADA format physics out in NxuStream format
//**********************************************************************
//**********************************************************************
void C_InstanceRigidBody::saveXML(FILE *fph,C_Query *q,C_PhysicsModel *pmodel)
{
  C_RigidBody *rb = pmodel->locateRigidBody(mBody);
  if ( rb )
  {
  	C_Matrix34 mat;
  	mat.id();
	  if ( mTarget )
		{
			C_Node *node = q->locateNode(mTarget);
			if ( node )
			{
				mat = node->mTransform;
			}
		}
    rb->saveXML(fph,q,mat,mVelocity,mAngularVelocity);
  }
}

void C_InstanceRigidBody::loadPAL(C_Query *q,C_PhysicsModel *pmodel)
{
  C_RigidBody *rb = pmodel->locateRigidBody(mBody);
  if ( rb )
  {
  	C_Matrix34 mat;
  	mat.id();
	  if ( mTarget )
		{
			C_Node *node = q->locateNode(mTarget);
			if ( node )
			{
				mat = node->mTransform;
			}
		}
    rb->loadPAL(q,mat,mVelocity,mAngularVelocity);
  }
}


void C_InstanceRigidConstraint::saveXML(FILE *fph,C_Query *q,C_PhysicsModel *pmodel)
{
  C_RigidConstraint *rc = 0;
	if ( pmodel && mConstraint )
	{
		rc = pmodel->locateRigidConstraint(mConstraint);
		if ( rc )
		{
			rc->saveXML(fph,q,pmodel);
		}
	}

}


void C_InstanceRigidConstraint::loadPAL(C_Query *q,C_PhysicsModel *pmodel)
{
  C_RigidConstraint *rc = 0;
	if ( pmodel && mConstraint )
	{
		rc = pmodel->locateRigidConstraint(mConstraint);
		if ( rc )
		{
			rc->loadPAL(q,pmodel);
		}
	}

}

void C_DisableCollision::saveXML(FILE *fph,C_Query *q,C_PhysicsModel *pmodel,unsigned int index)
{
  fprintf(fph,"     <NxPairFlagDesc id=\"PairFlag_%d\">\r\n", index);
  fprintf(fph,"       <mFlag>\r\n");
  fprintf(fph,"         <NX_IGNORE_PAIR>true</NX_IGNORE_PAIR>\r\n");
  fprintf(fph,"       </mFlag>\r\n");
  fprintf(fph,"       <mActor0>%d</mActor0>\r\n", pmodel->getActorIndex( mBody1 ) );
  fprintf(fph,"       <mShapeIndex0>-1</mShapeIndex0>\r\n");
  fprintf(fph,"       <mActor1>%d</mActor1>\r\n", pmodel->getActorIndex( mBody2 ) );
  fprintf(fph,"       <mShapeIndex1>-1</mShapeIndex1>\r\n");
  fprintf(fph,"     </NxPairFlagDesc>\r\n");
}


static const char * getMotion(float a,float b,bool &limit)
{
	const char *ret;

	limit = false;

	if ( a == 0 && b == 0 )
		ret = "NX_D6JOINT_MOTION_LOCKED";
	else if ( a == FLT_MIN && b == FLT_MAX )
		ret = "NX_D6JOINT_MOTION_FREE";
	else
	{
		ret = "NX_D6JOINT_MOTION_LIMITED";
		limit = true;
	}
	return ret;
}

static	float getMeanRad(float a,float b)
{
	return fabsf((a-b)*0.5f+b)*DEG_TO_RAD;
}

void C_RigidConstraint::saveXML(FILE *fph,C_Query *q,C_PhysicsModel *pmodel)
{

	C_Vec3 normal0,axis0,normal1,axis1,anchor0,anchor1;

	mMatrix1.M.getColumn(0, normal0 );
	mMatrix1.M.getColumn(2, axis0 );

	anchor0 = mMatrix1.t;

	mMatrix2.M.getColumn(0, normal1 );
	mMatrix2.M.getColumn(2, axis1 );

	anchor1 = mMatrix2.t;

  fprintf(fph,"    <NxJointDesc id=\"%s\">\r\n", mSid );
  fprintf(fph,"      <type>NX_JOINT_D6</type>\r\n");
  fprintf(fph,"      <mRefAttachActorDesc>%d</mRefAttachActorDesc>\r\n", pmodel->getActorIndex( mBody1 ) );
  fprintf(fph,"      <mAttachActorDesc>%d</mAttachActorDesc>\r\n", pmodel->getActorIndex(mBody2) );

  fprintf(fph,"      <localNormal0>%s %s %s</localNormal0>\r\n", fstring(normal0.x), fstring(normal0.y), fstring(normal0.z) );
  fprintf(fph,"      <localAxis0>%s %s %s</localAxis0>\r\n", fstring(axis0.x), fstring(axis0.y), fstring(axis0.z) );
  fprintf(fph,"      <localAnchor0>%s %s %s</localAnchor0>\r\n", fstring( anchor0.x), fstring(anchor0.y), fstring(anchor0.z) );

  fprintf(fph,"      <localNormal1>%s %s %s</localNormal1>\r\n", fstring(normal1.x), fstring(normal1.y), fstring(normal1.z) );
  fprintf(fph,"      <localAxis1>%s %s %s</localAxis1>\r\n", fstring(axis1.x), fstring(axis1.y), fstring(axis1.z) );
  fprintf(fph,"      <localAnchor1>%s %s %s</localAnchor1>\r\n", fstring(anchor1.x), fstring(anchor1.y), fstring(anchor1.z) );

  fprintf(fph,"      <name>%s</name>\r\n", mName);

	bool limit[6];

  fprintf(fph,"      <xMotion>%s</xMotion>\r\n",getMotion( mLinearMin.x, mLinearMax.x, limit[0]) );
  fprintf(fph,"      <yMotion>%s</yMotion>\r\n",getMotion( mLinearMin.y, mLinearMax.y, limit[1]) );
  fprintf(fph,"      <zMotion>%s</zMotion>\r\n",getMotion( mLinearMin.z, mLinearMax.z, limit[2]) );

  fprintf(fph,"      <swing1Motion>%s</swing1Motion>\r\n", getMotion( mAngularMin.x, mAngularMax.x, limit[3]) );
  fprintf(fph,"      <swing2Motion>%s</swing2Motion>\r\n", getMotion( mAngularMin.y, mAngularMax.y, limit[4]) );
  fprintf(fph,"      <twistMotion>%s</twistMotion>\r\n"  , getMotion( mAngularMin.z, mAngularMax.z, limit[5]) );

	if ( limit[0] || limit[1] || limit[2] )
	{
		// The PhysX SDK does not support independent linear limits on X, Y, and Z.  Just a single spherical distance limit.
		// However, you can lock certain linear degrees of freedom to make it act as a cylindrical or planer limit.
		float l = 0;

		if ( limit[0] )
		{
			l = fabsf( (mLinearMax.x - mLinearMin.x)*0.5f);
		}
		if ( limit[1] )
		{
			float y = fabsf( (mLinearMax.y - mLinearMin.y)*0.5f);
			if ( y > l ) l = y;
		}
		if ( limit[2] )
		{
			float z = fabsf( (mLinearMax.z - mLinearMin.z)*0.5f);
			if ( z > l ) l = z;
		}


    fprintf(fph,"          <NxJointLimitSoftDesc id=\"linearLimit\">\r\n");
    fprintf(fph,"            <value>FLT_MAX</value>\r\n");
    fprintf(fph,"            <restitution>0</restitution>\r\n");
    fprintf(fph,"            <spring>%s</spring>\r\n", fstring( mLinearSpring.mStiffness) );
    fprintf(fph,"            <damping>%s</damping>\r\n", fstring( mLinearSpring.mDamping) );
    fprintf(fph,"          </NxJointLimitSoftDesc>\r\n");
  }

	if ( limit[3] )
	{
    fprintf(fph,"          <NxJointLimitSoftDesc id=\"swing1Limit\">\r\n");
    fprintf(fph,"            <value>%s</value>\r\n", fstring(getMeanRad( mAngularMax.x, mAngularMin.x )) );
    fprintf(fph,"            <restitution>0</restitution>\r\n");
    fprintf(fph,"            <spring>%s</spring>\r\n", fstring( mAngularSpring.mStiffness) );
    fprintf(fph,"            <damping>%s</damping>\r\n", fstring( mAngularSpring.mDamping) );
    fprintf(fph,"          </NxJointLimitSoftDesc>\r\n");
  }

	if ( limit[4] )
	{
    fprintf(fph,"          <NxJointLimitSoftDesc id=\"swing2Limit\">\r\n");
    fprintf(fph,"            <value>%s</value>\r\n", fstring(getMeanRad( mAngularMax.y, mAngularMin.y )) );
    fprintf(fph,"            <restitution>0</restitution>\r\n");
    fprintf(fph,"            <spring>%s</spring>\r\n", fstring( mAngularSpring.mStiffness) );
    fprintf(fph,"            <damping>%s</damping>\r\n", fstring( mAngularSpring.mDamping) );
    fprintf(fph,"          </NxJointLimitSoftDesc>\r\n");
  }

  if ( limit[5] )
  {
    fprintf(fph,"          <NxJointLimitSoftPairDesc id=\"twistLimit\">\r\n");
    fprintf(fph,"            <NxJointLimitSoftDesc id=\"low\">\r\n");
    fprintf(fph,"              <value>%s</value>\r\n", fstring( mAngularMin.z*DEG_TO_RAD ) );
    fprintf(fph,"              <restitution>0</restitution>\r\n");
    fprintf(fph,"              <spring>%s</spring>\r\n", fstring( mAngularSpring.mStiffness) );
    fprintf(fph,"              <damping>%s</damping>\r\n", fstring( mAngularSpring.mDamping) );
    fprintf(fph,"            </NxJointLimitSoftDesc>\r\n");
    fprintf(fph,"            <NxJointLimitSoftDesc id=\"high\">\r\n");
    fprintf(fph,"              <value>%s</value>\r\n", fstring( mAngularMax.z*DEG_TO_RAD) );
    fprintf(fph,"              <restitution>0</restitution>\r\n");
    fprintf(fph,"              <spring>%s</spring>\r\n", fstring( mAngularSpring.mStiffness) );
    fprintf(fph,"              <damping>%s</damping>\r\n", fstring( mAngularSpring.mDamping) );
    fprintf(fph,"            </NxJointLimitSoftDesc>\r\n");
    fprintf(fph,"          </NxJointLimitSoftPairDesc>\r\n");
  }

	xDrive.saveXML(fph,"xDrive");
	yDrive.saveXML(fph,"yDrive");
	zDrive.saveXML(fph,"zDrive");

	swingDrive.saveXML(fph,"swingDrive");
	twistDrive.saveXML(fph,"twistDrive");
	slerpDrive.saveXML(fph,"slerpDrive");

  fprintf(fph,"        <drivePosition>%s %s %s</drivePosition>\r\n", fstring( drivePosition.x), fstring(drivePosition.y), fstring(drivePosition.z) );
  fprintf(fph,"        <driveOrientation>%s %s %s %s</driveOrientation>\r\n", fstring( driveOrientation.x), fstring(driveOrientation.y), fstring(driveOrientation.z), fstring(driveOrientation.w) );
  fprintf(fph,"        <driveLinearVelocity>%s %s %s</driveLinearVelocity>\r\n", fstring( driveLinearVelocity.x), fstring(driveLinearVelocity.y), fstring(driveLinearVelocity.z) );
  fprintf(fph,"        <driveAngularVelocity>%s %s %s</driveAngularVelocity>\r\n", fstring( driveAngularVelocity.x), fstring(driveAngularVelocity.y), fstring(driveAngularVelocity.z) );
  fprintf(fph,"        <gearRatio>%s</gearRatio>\r\n", fstring( gearRatio ) );

  fprintf(fph,"      <projectionMode>%s</projectionMode>\r\n", mProjectionMode );
  fprintf(fph,"      <projectionDistance>%s</projectionDistance>\r\n", fstring( mProjectionDistance ));
  fprintf(fph,"      <projectionAngle>%s</projectionAngle>\r\n", fstring( mProjectionAngle ) );

  fprintf(fph,"    </NxJointDesc>\r\n");
}

std::vector<palBody *> g_bodies;

palBody *SafeGetBody(int index) {
	if (index<0) return 0;
	if (index>=g_bodies.size()) return 0;
	return g_bodies[index];
}


void C_RigidConstraint::loadPAL(C_Query *q,C_PhysicsModel *pmodel)
{

	C_Vec3 normal0,axis0,normal1,axis1,anchor0,anchor1;

	mMatrix1.M.getColumn(0, normal0 );
	mMatrix1.M.getColumn(2, axis0 );

	anchor0 = mMatrix1.t;

	mMatrix2.M.getColumn(0, normal1 );
	mMatrix2.M.getColumn(2, axis1 );

	anchor1 = mMatrix2.t;

	palBody *parent=SafeGetBody(pmodel->getActorIndex( mBody1 ));
	palBody *child =SafeGetBody(pmodel->getActorIndex( mBody2 ));
	if ((parent == 0) || (child ==0)) {
		printf("Failed to create link: missing bodies\n");
		return;
	}
	palMatrix4x4 m0;
	Make4x4(mMatrix1,m0);
	palMatrix4x4 m1;
	Make4x4(mMatrix2,m1);

	palVector3 lmin,lmax,amin,amax;
	
	MakeVec(mLinearMin,lmin);
	MakeVec(mLinearMax,lmax);
	
	MakeVec(mAngularMin,amin);
	MakeVec(mAngularMax,amax);

	palGenericLink *pgl =dynamic_cast<palGenericLink *>(PF->CreateObject("palGenericLink"));

	if (pgl) {
		printf("Creating link\n");
	pgl->Init(parent,child,
		m0,m1,
		lmin,lmax,
		amin,amax);
	} else {
		printf("Could not create generic link\n");
	}
	mpGenericLink = pgl;
}

class convexpoints {
public:
	std::string m_name;
	std::vector<Float> m_convex;
};

std::vector<convexpoints> g_mesh_data;

void C_Geometry::loadPAL(C_Query *q) {
	C_TriangleMesh t;
	getTriangleMesh(t,q);

	if ( isConvex() )
	{



    if ( t.mVertices.size() )
    {
  	  HullDesc desc(QF_TRIANGLES, t.mVertices.size(), &t.mVertices[0].x, sizeof(C_Vec3) );
  	  desc.mMaxVertices = 32;
		  desc.mMaxFaces = 32;
  	  HullLibrary hl;
  	  HullResult result;
      HullError ok = hl.CreateConvexHull(desc,result);

      if ( ok == QE_OK )
      {

			  //;'
				convexpoints cp;
				cp.m_name=mId;

			  for (unsigned int i=0; i<result.mNumOutputVertices; i++)
			  {
				  const float *p = &result.mOutputVertices[i*3];

				  cp.m_convex.push_back(p[0]);
				  cp.m_convex.push_back(p[1]);
				  cp.m_convex.push_back(p[2]);
			  }
      	hl.ReleaseResult(result);
		g_mesh_data.push_back(cp);
      }

    }
  }
  else
  {
#if 0
    fprintf(fph,"  <NxTriangleMeshDesc id=\"%s\">\r\n",mId );
    fprintf(fph,"  </NxTriangleMeshDesc>\r\n");
#endif
  }

}

void C_Geometry::saveXML(FILE *fph,C_Query *q)
{
	C_TriangleMesh t;
	getTriangleMesh(t,q);

	if ( isConvex() )
	{

    fprintf(fph,"  <NxConvexMeshDesc id=\"%s\">\r\n", mId );

    if ( t.mVertices.size() )
    {
  	  HullDesc desc(QF_TRIANGLES, t.mVertices.size(), &t.mVertices[0].x, sizeof(C_Vec3) );
  	  desc.mMaxVertices = 32;
		  desc.mMaxFaces = 32;
  	  HullLibrary hl;
  	  HullResult result;
      HullError ok = hl.CreateConvexHull(desc,result);

      if ( ok == QE_OK )
      {
			  fprintf(fph,"    <numVertices>%d</numVertices>\r\n", result.mNumOutputVertices );
			  fprintf(fph,"    <pointStrideBytes>12</pointStrideBytes>\r\n");

			  fprintf(fph,"    <points>");
			  for (unsigned int i=0; i<result.mNumOutputVertices; i++)
			  {
				  const float *p = &result.mOutputVertices[i*3];
				  fprintf(fph,"%s %s %s  ", fstring(p[0]), fstring(p[1]), fstring(p[2]) );
				  if ( ((i+1)&3) == 0 )
				  {
					  fprintf(fph,"\r\n");
				 	  fprintf(fph,"      ");
				  }
			  }
			  fprintf(fph,"\r\n");
			  fprintf(fph,"      </points>\r\n");
			  fprintf(fph,"      <numTriangles>%d</numTriangles>\r\n", result.mNumFaces );
			  fprintf(fph,"      <triangleStrideBytes>%d</triangleStrideBytes>\r\n", 12 );
			  fprintf(fph,"      <triangles>");

			  for (unsigned int i=0; i<result.mNumFaces; i++)
			  {
				  unsigned int i1 = result.mIndices[i*3+0];
				  unsigned int i2 = result.mIndices[i*3+1];
				  unsigned int i3 = result.mIndices[i*3+2];
				  fprintf(fph,"%d %d %d  ", i1, i2, i3 );
				  if ( ((i+1)&3) == 0 )
				  {
					  fprintf(fph,"\r\n");
					  fprintf(fph,"      ");
				  }
			  }

			  fprintf(fph,"\r\n");

			  fprintf(fph,"      </triangles>\r\n");
			  fprintf(fph,"    <mCookedDataSize>0</mCookedDataSize>\r\n");

      	hl.ReleaseResult(result);
      }

    }

    fprintf(fph,"  </NxConvexMeshDesc>\r\n");
  }
  else
  {
    fprintf(fph,"  <NxTriangleMeshDesc id=\"%s\">\r\n",mId );
    fprintf(fph,"  </NxTriangleMeshDesc>\r\n");
  }

}


void C_Shape::saveXML(FILE *fph,C_Query *query,unsigned int index)
{
	int gindex = 0;
	if ( mGeometry )
	{
		C_Geometry *g = query->locateGeometry(mGeometry);
		gindex = g->getIndex();
		if ( g && g->isConvex() )
		{
			mShapeType = CST_CONVEX_MESH;
		}
		else
		{
			mShapeType = CST_MESH;
		}
	}
	fprintf(fph,"        <NxShapeDesc id=\"Shape_%d\">\r\n", index );
	switch ( mShapeType )
	{
		case CST_PLANE:
			fprintf(fph,"          <type>NX_SHAPE_PLANE</type>\r\n");
			fprintf(fph,"          <normal>%s %s %s</normal>\r\n", fstring(mPlane.normal.x), fstring(mPlane.normal.y), fstring(mPlane.normal.z) );
			fprintf(fph,"          <d>%s</d>\r\n", fstring(mPlane.d) );
			break;
		case CST_BOX:
			fprintf(fph,"				 <type>NX_SHAPE_BOX</type>\r\n");
			fprintf(fph,"        <dimensions>%s %s %s</dimensions>\r\n", fstring( mHalfExtents.x), fstring(mHalfExtents.y), fstring(mHalfExtents.z ) );
			break;
		case CST_SPHERE:
			fprintf(fph,"        <type>NX_SHAPE_SPHERE</type>\r\n");
			fprintf(fph,"        <radius>%s</radius>\r\n", fstring(mRadius1) );
			break;
		case CST_CYLINDER:
		case CST_TAPERED_CYLINDER:
		case CST_CAPSULE:
		case CST_TAPERED_CAPSULE:
			fprintf(fph,"         <type>NX_SHAPE_CAPSULE</type>\r\n");
			fprintf(fph,"         <height>%s</height>\r\n", fstring(mHeight) );
			fprintf(fph,"         <radius>%s</radius>\r\n", fstring(mRadius1) );
			break;
		case CST_MESH:
			fprintf(fph,"         <type>NX_SHAPE_MESH</type>\r\n");
			fprintf(fph,"         <mTriangleMeshDesc>%d</mTriangleMeshDesc>\r\n",gindex);
			break;
		case CST_CONVEX_MESH:
			fprintf(fph,"         <type>NX_SHAPE_CONVEX</type>\r\n");
			fprintf(fph,"         <mConvexMeshDesc>%d</mConvexMeshDesc>\r\n",gindex);
			break;
	}


  writeMatrix(fph,mTransform,"localPose" );

	fprintf(fph,"					 <skinWidth>%s</skinWidth>\r\n", fstring( mSkinWidth ) );
	fprintf(fph,"          <group>%d</group>\r\n", mGroup );

  fprintf(fph,"          <shapeFlags>\r\n");
  fprintf(fph,"            <NX_SF_VISUALIZATION>true</NX_SF_VISUALIZATION>\r\n");
  fprintf(fph,"            <NX_SF_DISABLE_COLLISION>%s</NX_SF_DISABLE_COLLISION>\r\n", TF( mDisableCollision ) );
  fprintf(fph,"					 </shapeFlags>\r\n");

	fprintf(fph,"          <mass>%s</mass>\r\n", fstring(mMass) );
	fprintf(fph,"          <density>%s</density>\r\n", fstring(mDensity) );
	fprintf(fph,"          <materialIndex>%d</materialIndex>\r\n", query->getMaterialIndex( mInstancePhysicsMaterial) );

	fprintf(fph,"				 </NxShapeDesc>\r\n");

}

void C_Shape::loadPAL(C_Query *query,unsigned int index)
{
	int gindex = 0;
	if ( mGeometry )
	{
		C_Geometry *g = query->locateGeometry(mGeometry);
		gindex = g->getIndex();
		if ( g && g->isConvex() )
		{
			mShapeType = CST_CONVEX_MESH;
		}
		else
		{
			mShapeType = CST_MESH;
		}
	}
#if 0
	fprintf(fph,"        <NxShapeDesc id=\"Shape_%d\">\r\n", index );
	switch ( mShapeType )
	{
		case CST_PLANE:
			fprintf(fph,"          <type>NX_SHAPE_PLANE</type>\r\n");
			fprintf(fph,"          <normal>%s %s %s</normal>\r\n", fstring(mPlane.normal.x), fstring(mPlane.normal.y), fstring(mPlane.normal.z) );
			fprintf(fph,"          <d>%s</d>\r\n", fstring(mPlane.d) );
			break;
		case CST_BOX:
			fprintf(fph,"				 <type>NX_SHAPE_BOX</type>\r\n");
			fprintf(fph,"        <dimensions>%s %s %s</dimensions>\r\n", fstring( mHalfExtents.x), fstring(mHalfExtents.y), fstring(mHalfExtents.z ) );
			break;
		case CST_SPHERE:
			fprintf(fph,"        <type>NX_SHAPE_SPHERE</type>\r\n");
			fprintf(fph,"        <radius>%s</radius>\r\n", fstring(mRadius1) );
			break;
		case CST_CYLINDER:
		case CST_TAPERED_CYLINDER:
		case CST_CAPSULE:
		case CST_TAPERED_CAPSULE:
			fprintf(fph,"         <type>NX_SHAPE_CAPSULE</type>\r\n");
			fprintf(fph,"         <height>%s</height>\r\n", fstring(mHeight) );
			fprintf(fph,"         <radius>%s</radius>\r\n", fstring(mRadius1) );
			break;
		case CST_MESH:
			fprintf(fph,"         <type>NX_SHAPE_MESH</type>\r\n");
			fprintf(fph,"         <mTriangleMeshDesc>%d</mTriangleMeshDesc>\r\n",gindex);
			break;
		case CST_CONVEX_MESH:
			fprintf(fph,"         <type>NX_SHAPE_CONVEX</type>\r\n");
			fprintf(fph,"         <mConvexMeshDesc>%d</mConvexMeshDesc>\r\n",gindex);
			break;
	}


  writeMatrix(fph,mTransform,"localPose" );

//	fprintf(fph,"					 <skinWidth>%s</skinWidth>\r\n", fstring( mSkinWidth ) );
	fprintf(fph,"          <group>%d</group>\r\n", mGroup );
/*
  fprintf(fph,"          <shapeFlags>\r\n");
  fprintf(fph,"            <NX_SF_VISUALIZATION>true</NX_SF_VISUALIZATION>\r\n");
  fprintf(fph,"            <NX_SF_DISABLE_COLLISION>%s</NX_SF_DISABLE_COLLISION>\r\n", TF( mDisableCollision ) );
  fprintf(fph,"					 </shapeFlags>\r\n");
*/
	fprintf(fph,"          <mass>%s</mass>\r\n", fstring(mMass) );
	fprintf(fph,"          <density>%s</density>\r\n", fstring(mDensity) );
	fprintf(fph,"          <materialIndex>%d</materialIndex>\r\n", query->getMaterialIndex( mInstancePhysicsMaterial) );

//	fprintf(fph,"				 </NxShapeDesc>\r\n");
#endif
}


void C_RigidBody::loadPAL(C_Query *q,const C_Matrix34 &mat,const C_Vec3 &velocity,const C_Vec3 &angularVelocity)
{
	for (unsigned int i=0; i<mShapes.size(); i++)
//  unsigned int i=0;
	{
		C_Shape *s = mShapes[i];
		s->loadPAL(q,i);
	}


	C_Shape *s = mShapes[0];

	int gindex = 0;
	if ( s->mGeometry )
	{
		C_Geometry *g = q->locateGeometry(s->mGeometry);
		gindex = g->getIndex();
		if ( g && g->isConvex() )
		{
			s->mShapeType = CST_CONVEX_MESH;
		}
		else
		{
			s->mShapeType = CST_MESH;
		}
	}

	/*
	CST_BOX,
	CST_SPHERE,
	CST_CYLINDER,
	CST_TAPERED_CYLINDER,
	CST_CAPSULE,
	CST_TAPERED_CAPSULE,
	*/
	
	palMatrix4x4 m_shape;
	palBody *pb = 0;
	switch (s->mShapeType) {
		case CST_BOX:
			{
				palBox *pbox = PF->CreateBox();
#ifndef NDEBUG
				printf("creating box [%f,%f,%f] m:%f\n",s->mHalfExtents.x,s->mHalfExtents.y,s->mHalfExtents.z,s->mMass);
#endif
				if (pbox)
				pbox->Init(mat.t.x,mat.t.y,mat.t.z, 
				s->mHalfExtents.x,
				s->mHalfExtents.y,
				s->mHalfExtents.z,
				s->mMass);
				pb = pbox;
				Make4x4(s->mTransform,m_shape);
			}
			break;

		case CST_SPHERE:
			{
				palSphere *ps = PF->CreateSphere();
#ifndef NDEBUG
				printf("creating sphere [%f]\n",s->mRadius1);
#endif
				if (ps)
					ps->Init(mat.t.x,mat.t.y,mat.t.z, 
						s->mRadius1,
						s->mMass);
				pb = ps;
				Make4x4(s->mTransform,m_shape);
			}
			break;

		case CST_CYLINDER:
		case CST_TAPERED_CYLINDER:
		case CST_CAPSULE:
		case CST_TAPERED_CAPSULE:
			{
				palCapsule *pcyl = PF->CreateCapsule();
#ifndef NDEBUG
				printf("creating cyl [%f %f] m:%f\n",s->mRadius1,s->mHeight,s->mMass);
#endif
				if (pcyl) {
					pcyl->Init(mat.t.x,mat.t.y,mat.t.z, 
						s->mRadius1,
						s->mHeight,
						s->mMass);
					pb = pcyl;
					Make4x4(s->mTransform,m_shape);
				}
				
			}
			break;
		case CST_CONVEX_MESH:
			{
#ifndef NDEBUG
			printf("creating mesh id:%d\n",gindex);
#endif
			if (gindex<0) break;
			if (gindex>g_mesh_data.size()) break;
			palConvex *pcon = dynamic_cast<palConvex *>(PF->CreateObject("palConvex"));
			if (pcon) {
#ifndef NDEBUG					
				printf("mesh id:%d has %d entries\n",gindex,g_mesh_data[gindex].m_convex.size());
#endif
//virtual void Init(Float x, Float y, Float z, const Float *pVertices, int nVertices, Float mass);
				pcon->Init(mat.t.x,mat.t.y,mat.t.z,
					&g_mesh_data[gindex].m_convex[0],
					g_mesh_data[gindex].m_convex.size()/3,
					s->mMass);
#ifndef NDEBUG
				printf("pcon:%x\n",pcon);
#endif
					pb = pcon;
					Make4x4(s->mTransform,m_shape);
			}
			}
			break;
		default:
			printf("UNSUPPORTED SHAPE TYPE!\n");
			break;
	}

	// writeMatrix(fph,mTransform,"localPose" );

	palMatrix4x4 m;
	palMatrix4x4 m_body;
	Make4x4(mat,m_body);
	mat_multiply(&m,&m_body,&m_shape);

	if (pb) {
		pb->SetPosition(m);
#ifdef USE_PAL_GRAPHICS
		GraphicsObject *go = BuildGraphics(pb);
#ifndef NDEBUG
		printf("go:%x\n",go);
#endif
#endif
		mpBody	 = (pb);
		g_bodies.push_back(pb);
	}
}



void C_RigidBody::saveXML(FILE *fph,C_Query *q,const C_Matrix34 &mat,const C_Vec3 &velocity,const C_Vec3 &angularVelocity)
{
	fprintf(fph,"      <NxActorDesc id=\"%s\">\r\n", mSid );
	writeMatrix(fph,mat,"globalPose");
	fprintf(fph,"        <hasBody>%s</hasBody>\r\n", TF(mDynamic) );

	if ( mDynamic )
	{
		fprintf(fph,"        <NxBodyDesc>\r\n");
		writeMatrix(fph,mMassFrame,"massLocalPose");
		fprintf(fph,"          <massSpaceInertia>%s %s %s</massSpaceInertia>\r\n", fstring( mInertia.x), fstring(mInertia.y), fstring(mInertia.z) );
		fprintf(fph,"          <mass>%s</mass>\r\n", fstring(mMass) );
		fprintf(fph,"          <linearVelocity>%s %s %s</linearVelocity>\r\n", fstring(velocity.x), fstring(velocity.y), fstring(velocity.z) );
		fprintf(fph,"          <angularVelocity>%s %s %s</angularVelocity>\r\n", fstring(angularVelocity.x), fstring(angularVelocity.y), fstring(angularVelocity.z) );
    fprintf(fph,"          <solverIterationCount>%d</solverIterationCount>\r\n", mSolverIterationCount );
		fprintf(fph,"				 </NxBodyDesc>\r\n");
	}

  fprintf(fph,"        <name>%s</name>\r\n", mName );

  fprintf(fph,"        <numShapes>%d</numShapes>\r\n", mShapes.size() );
	for (unsigned int i=0; i<mShapes.size(); i++)
	{
		C_Shape *s = mShapes[i];
		s->saveXML(fph,q,i);
	}

	fprintf(fph,"      </NxActorDesc>\r\n");
}


void C_PhysicsMaterial::loadPAL(int index)
{
	char name[256];
	sprintf(name,"mat_%.3d",index);
	palMaterials *pm = PF->CreateMaterials();
	pm->NewMaterial(name,mStaticFriction,mDynamicFriction,mRestitution);
}


void C_PhysicsMaterial::saveXML(FILE *fph,int index)
{
	fprintf(fph,"      <NxMaterialDesc id=\"Material_%d\">\r\n", index);
	fprintf(fph,"        <index>%d</index>\r\n", index );
	fprintf(fph,"        <dynamicFriction>%s</dynamicFriction>\r\n", fstring( mDynamicFriction) );
	fprintf(fph,"        <staticFriction>%s</staticFriction>\r\n", fstring(mStaticFriction) );
	fprintf(fph,"        <restitution>%s</restitution>\r\n", fstring(mRestitution) );
	fprintf(fph,"			 </NxMaterialDesc>\r\n");
}


void C_LibraryPhysicsMaterials::loadPAL()
{
	for (unsigned int i=0; i<mPhysicsMaterials.size(); i++)
	{
		C_PhysicsMaterial *p = mPhysicsMaterials[i];
		p->loadPAL(i);
	}
}

void C_LibraryPhysicsMaterials::saveXML(FILE *fph)
{
	for (unsigned int i=0; i<mPhysicsMaterials.size(); i++)
	{
		C_PhysicsMaterial *p = mPhysicsMaterials[i];
		p->saveXML(fph,i);
	}
}


void C_InstancePhysicsScene::saveXML(FILE *fph,C_Query *query)
{
	C_PhysicsScene *ps = query->locatePhysicsScene(mUrl);
	if ( ps )
	{
		ps->saveXML(fph,query);
	}
}

void C_InstancePhysicsScene::loadPAL(C_Query *query)
{
	C_PhysicsScene *ps = query->locatePhysicsScene(mUrl);
	if ( ps )
	{
		ps->loadPAL(query);
	}
}


void C_InstancePhysicsModel::loadPAL(C_Query *query)
{
	C_PhysicsModel *pm = query->locatePhysicsModel(mUrl);

	for (unsigned int i=0; i<mInstanceRigidBodies.size(); i++)
	{
		C_InstanceRigidBody *rb = mInstanceRigidBodies[i];
		rb->loadPAL(query,pm);
	}

	for (unsigned int i=0; i<mInstanceRigidConstraints.size(); i++)
	{
		C_InstanceRigidConstraint *rc = mInstanceRigidConstraints[i];
		rc->loadPAL(query,pm);
	}
/*
	for (unsigned int i=0; i<mDisableCollisions.size(); i++)
	{
		C_DisableCollision *dc = mDisableCollisions[i];
		dc->saveXML(fph,query,pm,i);
	}*/
}

void C_InstancePhysicsModel::saveXML(FILE *fph,C_Query *query)
{
	C_PhysicsModel *pm = query->locatePhysicsModel(mUrl);

	for (unsigned int i=0; i<mInstanceRigidBodies.size(); i++)
	{
		C_InstanceRigidBody *rb = mInstanceRigidBodies[i];
		rb->saveXML(fph,query,pm);
	}

	for (unsigned int i=0; i<mInstanceRigidConstraints.size(); i++)
	{
		C_InstanceRigidConstraint *rc = mInstanceRigidConstraints[i];
		rc->saveXML(fph,query,pm);
	}

	for (unsigned int i=0; i<mDisableCollisions.size(); i++)
	{
		C_DisableCollision *dc = mDisableCollisions[i];
		dc->saveXML(fph,query,pm,i);
	}
}

void C_PhysicsScene::saveXML(FILE *fph,C_Query *query)
{
	for (unsigned int i=0; i<mInstancePhysicsModels.size(); i++)
	{
		C_InstancePhysicsModel *ipm = mInstancePhysicsModels[i];
		ipm->saveXML(fph,query);
	}
}
void C_PhysicsScene::loadPAL(C_Query *query)
{
	for (unsigned int i=0; i<mInstancePhysicsModels.size(); i++)
	{
		C_InstancePhysicsModel *ipm = mInstancePhysicsModels[i];
		ipm->loadPAL(query);
	}
}

void C_Scene::saveXML(FILE *fph,C_Query *query)
{
	for (unsigned int i=0; i<mInstancePhysicsScenes.size(); i++)
	{
		C_InstancePhysicsScene *ip = mInstancePhysicsScenes[i];
		ip->saveXML(fph,query);
	}
}

void C_Scene::loadPAL(C_Query *query)
{
	for (unsigned int i=0; i<mInstancePhysicsScenes.size(); i++)
	{
		C_InstancePhysicsScene *ip = mInstancePhysicsScenes[i];
		ip->loadPAL(query);
	}
}



void C_JointDrive::saveXML(FILE *fph,const char *drive)
{
  const char *dtype = "";

	switch ( driveType )
	{
		case JDT_POSITION:
			dtype = "NX_D6JOINT_DRIVE_POSITION";
			break;
		case JDT_VELOCITY:
			dtype = "NX_D6JOINT_DRIVE_VELOCITY";
			break;
		case JDT_POSITION_VELOCITY:
			dtype = "NX_D6JOINT_DRIVE_POSITION+NX_D6JOINT_DRIVE_VELOCITY";
			break;
	}

  fprintf(fph,"          <NxJointDriveDesc id=\"%s\">\r\n", drive);
  fprintf(fph,"            <driveType>%s</driveType>\r\n", dtype);
  fprintf(fph,"            <spring>%s</spring>\r\n", fstring(spring));
  fprintf(fph,"            <damping>%s</damping>\r\n", fstring(damping) );
  fprintf(fph,"            <forceLimit>%s</forceLimit>\r\n", fstring(forceLimit) );
  fprintf(fph,"          </NxJointDriveDesc>\r\n");

}



palBodyBase*	 palGetColladaBody(ColladaPhysics *cp, const char *model_name, const char *rigid_body_name) {
	C_PhysicsModel *pm = cp->locatePhysicsModel(model_name);//"Scene0-PhysicsModel"
	if (!pm) return 0;
	C_RigidBody *prb = pm->locateRigidBody(rigid_body_name);
	if (!prb) return 0;
	return prb->mpBody;
}

palLink*		 palGetColladaLink(ColladaPhysics *cp, const char *model_name, const char *constraint_name) {
	C_PhysicsModel *pm = cp->locatePhysicsModel(model_name);//"Scene0-PhysicsModel"
	if (!pm) return 0;
	 C_RigidConstraint *prc = pm->locateRigidConstraint(constraint_name);
	 if (!prc) return 0;
	 return prc->mpGenericLink;
}

}; // End DAE2XML namespace