#include "sdlgl.h"
#include <vector>

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
#ifdef _MSC_VER
#ifndef NDEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif
#endif

/*
	Abstract:
		SDLGL - Simplistic SDL/GL based graphics interface
		Implementation file
	Author:
		Adrian Boeing
	Revision History:
		Version 0.8 : 31/05/04
	TODO:
*/

void SDLGLEngine::Clear() {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
}
void SDLGLEngine::SetTitle(char *title) {
	SDL_WM_SetCaption(title, NULL);										// We're Setting The Window Caption
}

bool SDLGLEngine ::Init(int width, int height, int bpp, bool fullscreen) {
	Uint32		Vflags;														// Our Video Flags
	if(SDL_Init(SDL_INIT_VIDEO)<0)											// Init The SDL Library, The VIDEO Subsystem
	{
		return false;
	}
	atexit(SDL_Quit);														// SDL's Been init, Now We're Making Sure Thet SDL_Quit Will Be Called In Case of exit()
	Vflags = SDL_HWSURFACE|SDL_OPENGLBLIT;									// We Want A Hardware Surface And Special OpenGLBlit Mode
	if (fullscreen)
		Vflags|=SDL_FULLSCREEN;												// If Yes, Add The Fullscreen Flag To Our Init

	if (bpp<32) {
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );								// In order to use SDL_OPENGLBLIT we have to
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );							// set GL attributes first
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	} else {
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );								// In order to use SDL_OPENGLBLIT we have to
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );							// set GL attributes first
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	}
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );							// colors and doublebuffering

	if(!(m_Screen = SDL_SetVideoMode(width, height, bpp, Vflags)))							// We're Using SDL_SetVideoMode To Create The Window
	{
		return false;														// If It Fails, We're Returning False
	}

	SDL_FillRect(m_Screen, NULL, SDL_MapRGBA(m_Screen->format,0,0,0,0));		// A key event! God knows all the time I've spent just to
																			// figure out this basic thing. We have to open the Screen Alpha Channel!
	Resize(width,height);

	glClearColor(0.0f,0.0f,0.0f,0.5f);							// Black Background
	glClearDepth(1.0f);											// Depth Buffer Setup
	glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing (Less Or Equal)
	glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	glShadeModel(GL_SMOOTH);									// Select Smooth Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate

	glLoadIdentity();											// Reset The Modelview Matrix
	return true;
};

void SDLGLEngine::Resize(int width, int height) {
	glViewport(0,0,(GLsizei)(width),(GLsizei)(height));						// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);											// Select The Projection Matrix
	glLoadIdentity();														// Reset The Projection Matrix */

	gluPerspective(45.0f,(GLfloat)(width)/(GLfloat)(height),1.0f,100.0f);	// Calculate The Aspect Ratio Of The Window
	glMatrixMode(GL_MODELVIEW);												// Select The Modelview Matrix
	glLoadIdentity();														// Reset The Modelview Matrix
}

void SDLGLEngine::Flip() {
	SDL_GL_SwapBuffers();										// And Swap The Buffers (We're Double-Buffering, Remember?)
}

void SDLGLEngine::SetViewMatrix(float eye_x, float eye_y, float eye_z, float lookat_x, float lookat_y, float lookat_z, float up_x, float up_y, float up_z) {
/*	float t[16];
	//find a basis for the new position
	Vector3 fwd = ((*lookat) - (*eye));	fwd.Normalize();
	Vector3 right = (fwd * (*up));	right.Normalize();
	Vector3 aup = right * fwd;
	//set up a new projection matrix
	t[ 0] = right.x; t[ 1] = aup.x; t[ 2] = -fwd.x; t[ 3] = 0.0f;
	t[ 4] = right.y; t[ 5] = aup.y; t[ 6] = -fwd.y; t[ 7] = 0.0f;
	t[ 8] = right.z; t[ 9] = aup.z; t[10] = -fwd.z; t[11] = 0.0f;
	t[12] = eye->Dot(right); t[13] = eye->Dot(aup); t[14] = eye->Dot(fwd);
	t[15] = 1.0f;
	//plunck it in
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(t);*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye_x,eye_y,eye_z,lookat_x,lookat_y,lookat_z,up_x,up_y,up_z);
}

void SDLGLEngine::SetProjMatrix(float fov,float aspect,float nearcp,float farcp) {
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();										
				gluPerspective(fov*(180.0f/3.14159265358979323846f),aspect,nearcp,farcp);
};

void SDLGLEngine::SetViewProj2d(int width, int height) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
		glScalef(2.0f / (float)width, -2.0f / (float)height, 1.0f);
        glTranslatef(-((float)width / 2.0f), -((float)height / 2.0f), 0.0f);
        glViewport(0, 0, width, height); // viewport size in pixels 
}


//helper macro
#define M(x,y) m_transform[((y)-1)+((x)-1)*4]

SDLGLObject::SDLGLObject() {
	m_indices=NULL;
	m_data=NULL;
	memset(m_transform,0,sizeof(float)*16);
	M(1,1) = 1;
	M(2,2) = 1;
	M(3,3) = 1;
	M(4,4) = 1;
}

SDLGLObject::~SDLGLObject() {
	if (m_indices)
		delete [] m_indices;
	if (m_data)
		delete [] m_data;
}
void SDLGLObject::SetScale(float sx, float sy, float sz) {
	M(1,1)*=sx;
	M(2,1)*=sx;
	M(3,1)*=sx;

	M(1,2)*=sy;
	M(2,2)*=sy;
	M(3,2)*=sy;

	M(1,3)*=sz;
	M(2,3)*=sz;
	M(3,3)*=sz;
}

void SDLGLObject::SetPositionX(float x) {
	M(4,1) = x;
}
void SDLGLObject::SetPositionY(float y) {
	M(4,2) = y;
}
void SDLGLObject::SetPositionZ(float z) {
	M(4,3) = z;
}

float SDLGLObject::GetPositionX(void) {
	return M(4,1) ;
}

float SDLGLObject::GetPositionY(void) {
	return M(4,2) ;
}

float SDLGLObject::GetPositionZ(void) {
	return M(4,3) ;
}


void SDLGLObject::SetPosition(float x, float y, float z) {
	M(4,1) = x;
	M(4,2) = y;
	M(4,3) = z;
}

void SDLGLObject::SetPosition(float x, float y, float z, float roll, float pitch, float yaw) {

	float sinroll = (float)sin(roll), cosroll = (float)cos(roll);
	float sinpitch = (float)sin(pitch), cospitch = (float)cos(pitch);
	float sinyaw = (float)sin(yaw), cosyaw = (float)cos(yaw);

	M(1,1) = cosroll*cosyaw;
	M(1,2) = cosroll*sinyaw*sinpitch - sinroll*cospitch;
	M(1,3) = sinroll*sinpitch + cosroll*sinyaw*cospitch;
	M(1,4) = 0.0f;
	M(2,1) = sinroll*cosyaw;
	M(2,2) = cosroll*cospitch + sinroll*sinyaw*sinpitch;
	M(2,3) = sinroll*sinyaw*cospitch - cosroll*sinpitch;
	M(2,4) = 0.0f;
	M(3,1) = -sinyaw;
	M(3,2) = cosyaw*sinpitch;
	M(3,3) = cosyaw*cospitch;
	M(3,4) = 0.0f;
	M(4,1) = x;
	M(4,2) = y;
	M(4,3) = z;
	M(4,4) = 1.0f;
}

void SDLGLObject::SetPosition(float *transform) {
	memcpy(m_transform,transform,sizeof(float)*16);
}

void SDLGLObject::Render() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(m_transform);
	GLenum hr;

		glInterleavedArrays(m_format , 0, m_data);
		hr=glGetError();
		//if (hr!=GL_NO_ERROR) LOG_DEBUG("gl error: 0x%x",hr);
		glDrawElements(m_mode,m_nIndices,GL_UNSIGNED_INT, m_indices);
		hr=glGetError();
		//if (hr!=GL_NO_ERROR) LOG_DEBUG("gl error: 0x%x",hr);

	glPopMatrix();
}

void SDLGLObject::Construct(bool Color, bool Texture, int nVertices, int nIndices) {
	m_nVertices=nVertices;
	m_nIndices=nIndices;
	m_mode=GL_TRIANGLES;
	m_Colored=Color;
	m_Textured=Texture;
	if (Color)
		if (Texture) {
			m_format=GL_T2F_C4F_N3F_V3F;
			m_datasize=2+4+3+3;
		} else {
			m_format=GL_C4F_N3F_V3F;
			m_datasize=4+3+3;
		}
	else
		if (Texture) {
			m_datasize=2+3+3;
			m_format=GL_T2F_N3F_V3F;
		} else {
			m_datasize=3+3;
			m_format=GL_N3F_V3F;
		}
	m_data = new float[m_datasize*nVertices];
	m_indices = new unsigned long[nIndices];	
}

void SDLGLObject::CleanData() {
	m_nVertices=0;
	m_nIndices=0;
	delete [] m_data;
	delete [] m_indices;
}

void SDLGLObject::GetVertexPosition(int pos, float &x, float &y, float &z) {
	if (pos<0) return;
	int offset=0;
	if (m_Textured) {
		offset+=2;
	}
	if (m_Colored) {
		offset+=4;
	}
	offset+=3;
	x=m_data[pos*m_datasize+offset+0];
	y=m_data[pos*m_datasize+offset+1];
	z=m_data[pos*m_datasize+offset+2];
}

void SDLGLObject::SetData(int pos, float x, float y, float z, float nx, float ny, float nz, float r, float g, float b, float tx, float ty) {
	if (pos<0) return;
	int offset=0;
	if (m_Textured) {
		m_data[pos*m_datasize+offset+0]=tx;
		m_data[pos*m_datasize+offset+1]=ty;
		offset+=2;
	}
	if (m_Colored) {
		m_data[pos*m_datasize+offset+0]=r;
		m_data[pos*m_datasize+offset+1]=g;
		m_data[pos*m_datasize+offset+2]=b;
		m_data[pos*m_datasize+offset+3]=1;
		offset+=4;
	}
	m_data[pos*m_datasize+offset+0]=nx;
	m_data[pos*m_datasize+offset+1]=ny;
	m_data[pos*m_datasize+offset+2]=nz;
	offset+=3;
	m_data[pos*m_datasize+offset+0]=x;
	m_data[pos*m_datasize+offset+1]=y;
	m_data[pos*m_datasize+offset+2]=z;
}

void SDLGLObject::SetNormal(int pos, float x, float y, float z) {
	if (pos<0) return;
	int offset=0;
	if (m_Textured) offset+=2;
	if (m_Colored) offset+=4;
	m_data[pos*m_datasize+offset+0]=x;
	m_data[pos*m_datasize+offset+1]=y;
	m_data[pos*m_datasize+offset+2]=z;
}

void SDLGLObject::SetColor(int pos, float r, float g, float b) {
	int offset=0;
	if (!m_Colored) return;
	if (m_Textured) offset+=2;
	if (pos<0) {
		for (int i=0;i<m_nVertices;i++) {
		m_data[i*m_datasize+offset+0]=r;
		m_data[i*m_datasize+offset+1]=g;
		m_data[i*m_datasize+offset+2]=b;
		}
	} else {
		m_data[pos*m_datasize+offset+0]=r;
		m_data[pos*m_datasize+offset+1]=g;
		m_data[pos*m_datasize+offset+2]=b;
	}
}

void SDLGLObject::SetTexture(int pos, float tx, float ty) {
	if (pos<0) return;
	if (m_Textured) {
		m_data[pos*m_datasize+0] = tx;
		m_data[pos*m_datasize+1] = ty;
	}
}

void SDLGLObject::SetIndex(int pos, int x, int y, int z) {
	m_indices[pos*3+0]=x;
	m_indices[pos*3+1]=y;
	m_indices[pos*3+2]=z;
}

void SDLGLPoints::Create(float x, float y, float z, float *points, int npoints) {
			SDLGLObject::SetPosition(x,y,z);
			Construct(false,false,npoints,0);
			m_mode = GL_POINTS;
			for (int i=0;i<npoints;i++) {
				SetData(i,points[i*3+0],points[i*3+1],points[i*3+2]);
			}
	}

void SDLGLPoints::Render() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(m_transform);
		glColor3f(1,1,1);
		glBegin(GL_POINTS);
		for (int i=0;i<m_nVertices;i++) {
			int offset = 0;
			if (m_Colored) {
				offset+=4;
			}
			//skip n
			offset+=3;
			glVertex3f(	m_data[i*m_datasize+offset+0],
						m_data[i*m_datasize+offset+1],
						m_data[i*m_datasize+offset+2]);
		}
		glEnd();
		glPopMatrix();
	}

//#define SPHERE_VSLICES 14
//#define SPHERE_HSTRIPS 10
//#define SPHERE_VSLICES 7
//#define SPHERE_HSTRIPS 5
//#define SPHERE_TRIANGLECOUNT ((SPHERE_HSTRIPS-1) * SPHERE_VSLICES * 2)
#define DEG2RAD 0.01745f

#define CYLINDER_VSLICES 10
#define CYLINDER_HSTRIPS 10
#define CYLINDER_TRIANGLECOUNT ((CYLINDER_HSTRIPS-1) * CYLINDER_VSLICES * 2)

template<typename T> void push_back3(std::vector<T> &v, T x, T y, T z) {
	v.push_back(x);
	v.push_back(y);
	v.push_back(z);
}

void SDLGLCappedCylinder::CreateMesh(float x, float y, float z, float radius, float length, int hstrip, int vslice) {
	SDLGLObject::SetPosition(x,y,z);
		
	m_fRadius=radius;
	m_fLength=length;

	float fX1, fY1, fX2, fY2, fX3, fY3, fX4, fY4;	// The vertex positions around each quad we calculate
	float fAngle,fY,fYNext;
	float fRadius=m_fRadius;
	float fHeight=m_fLength;
	float fAngleAdd = 360.0f / (float)vslice;
	float fSineAngle = 0;
	float fSineAdd = 180.0f / (hstrip-1);
	int i,j;

	std::vector<float> verts;
	// Loop around our sphere
	for (i=0; i<hstrip; i++)
	{
		// Reset the angle for this slice
		fAngle = 0;

		fY = cosf(fSineAngle * DEG2RAD) * fRadius;
		fYNext = cosf((fSineAngle+fSineAdd) * DEG2RAD) * fRadius;

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
			fX1 = sinf(fAngle * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fY1 = cosf(fAngle * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fX2 = sinf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fY2 = cosf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fX3 = sinf(fAngle * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fY3 = cosf(fAngle * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fX4 = sinf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fY4 = cosf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fAngle += fAngleAdd;
		
		push_back3<float>(verts, fX1, fY    , fY1);
		push_back3<float>(verts, fX4, fYNext, fY4);
		push_back3<float>(verts, fX2, fY    , fY2);
		push_back3<float>(verts, fX1, fY    , fY1);
		push_back3<float>(verts, fX3, fYNext, fY3);
		push_back3<float>(verts, fX4, fYNext, fY4);
		
		}


		fSineAngle += fSineAdd;
	}
	std::vector<int> idx;
	for (i=0;i<hstrip*vslice*6;i++)
		idx.push_back(i);

	Init(hstrip*vslice*6*3,hstrip*vslice*6,&verts[0],&idx[0]);
}

void SDLGLCappedCylinder::Create(float x, float y, float z, float radius, float length) {
	SDLGLObject::SetPosition(x,y,z);
	bool color=true;
	bool tex=false;
	
	m_fRadius=radius;
	m_fLength=length;

	float r1,g1,b1,r2,g2,b2;
	r1=0;g1=0;b1=1;
	r2=1;g2=0;b2=0;
	Construct(color,tex,CYLINDER_HSTRIPS*CYLINDER_VSLICES*6,CYLINDER_HSTRIPS*CYLINDER_VSLICES*6);
	int i,j;
	float fX1, fY1, fX2, fY2, fX3, fY3, fX4, fY4;	// The vertex positions around each quad we calculate
	float fAngle,fY,fYNext;
	float fRadius=m_fRadius;
	float fHeight=m_fLength;
	float fAngleAdd = 360.0f / (float)CYLINDER_VSLICES;
	float fSineAngle = 0;
	float fSineAdd = 180.0f / (CYLINDER_HSTRIPS-1);


	int vcount=0;
	// Loop around our sphere
	for (i=0; i<CYLINDER_HSTRIPS; i++)
	{
		// Reset the angle for this slice
		fAngle = 0;
#if 1
	
		fY = cosf(fSineAngle * DEG2RAD) * fRadius;
		fYNext = cosf((fSineAngle+fSineAdd) * DEG2RAD) * fRadius;

		// If we're above the midpoint, add half the height to the vertex positions.
		// Otherwise subtract half the height.
		if (i<=(CYLINDER_HSTRIPS/2)-1)
			fY += fHeight/2;
		else
			fY -= fHeight/2;
		if (i<=(CYLINDER_HSTRIPS/2)-2)
			fYNext += fHeight/2;
		else
			fYNext -= fHeight/2;
#else
		fY = (i/CYLINDER_HSTRIPS) * fHeight;
		fYNext = ((i+1)/CYLINDER_HSTRIPS) * fHeight;
#endif
		for (j=0; j<CYLINDER_VSLICES; j++)
		{
			// Calculate the X and Y position for the sphere (as if it were a circle viewed from above)
			fX1 = sinf(fAngle * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fY1 = cosf(fAngle * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fX2 = sinf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fY2 = cosf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fX3 = sinf(fAngle * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fY3 = cosf(fAngle * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fX4 = sinf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fY4 = cosf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);

			SetData(vcount++, fX1, fY    , fY1);
			if (color) SetColor(vcount-1,r1,g1,b1);
//			SetTexture(vcount-1,sinf(fAngle * DEG2RAD),sinf(fSineAngle * DEG2RAD));
			SetData(vcount++, fX4, fYNext, fY4);
			if (color) SetColor(vcount-1,r1,g1,b1);
//			SetTexture(vcount-1,sinf((fAngle+fAngleAdd) * DEG2RAD),sinf((fSineAngle + fSineAdd) * DEG2RAD));
			SetData(vcount++, fX2, fY    , fY2);
			if (color) SetColor(vcount-1,r1,g1,b1);
//			SetTexture(vcount-1,sinf((fAngle+fAngleAdd) * DEG2RAD),sinf((fSineAngle) * DEG2RAD));
			
			SetData(vcount++, fX1, fY    , fY1);
			if (color) SetColor(vcount-1,r2,g2,b2);
//			SetTexture(vcount-1,sinf(fAngle * DEG2RAD),sinf(fSineAngle * DEG2RAD));
			SetData(vcount++, fX3, fYNext, fY3);
			if (color) SetColor(vcount-1,r2,g2,b2);
//			SetTexture(vcount-1,sinf((fAngle) * DEG2RAD),sinf((fSineAngle + fSineAdd) * DEG2RAD));
			SetData(vcount++, fX4, fYNext, fY4);
			if (color) SetColor(vcount-1,r2,g2,b2);
//			SetTexture(vcount-1,sinf((fAngle+fAngleAdd) * DEG2RAD),sinf((fSineAngle + fSineAdd) * DEG2RAD));

			
			// Move to the next angle
			fAngle += fAngleAdd;
		}

		fSineAngle += fSineAdd;
	}
	for (i=0;i<m_nIndices;i++)
		m_indices[i]=i;

	//SetColor(-1,0.5f,0.5f,0.5f);
}

void SDLGLSphere::Create(float x, float y, float z, float radius, int sphere_hstrips, int sphere_vslices) {

	bool color=true;
	bool tex=false;
	float r1,g1,b1,r2,g2,b2;
	r1=0;g1=1;b1=0;
	r2=1;g2=0;b2=0;
	SDLGLObject::SetPosition(x,y,z);
	m_fRadius=radius;
	
	Construct(color,tex,
		(sphere_hstrips) * sphere_vslices * 6,sphere_hstrips*sphere_vslices*6);
	
	int i,j;
	// Calculate the angle to add each time as we divide the cylined into the appropriate number of strips
	float fAngleAdd = 360.0f / (float)sphere_vslices;
	float fSineAngle = 0;
	float fSineAdd = 180.0f / (sphere_hstrips-1);
	float fX1, fY1, fX2, fY2, fX3, fY3, fX4, fY4;	// The vertex positions around each quad we calculate
	float fAngle,fY,fYNext;
	float fRadius=m_fRadius;
	
	int vcount = 0;

	// Loop around our sphere
	for (i=0; i<sphere_hstrips; i++)
	{

		// Reset the angle for this slice
		fAngle = 0;
		fY = cosf(fSineAngle * DEG2RAD) * fRadius;
		fYNext = cosf((fSineAngle+fSineAdd) * DEG2RAD) * fRadius;

		for (j=0; j<sphere_vslices; j++)
		{
			// Calculate the X and Y position for the sphere (as if it were a circle viewed from above)
			fX1 = sinf(fAngle * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fY1 = cosf(fAngle * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fX2 = sinf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fY2 = cosf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf(fSineAngle * DEG2RAD);
			fX3 = sinf(fAngle * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fY3 = cosf(fAngle * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fX4 = sinf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);
			fY4 = cosf((fAngle+fAngleAdd) * DEG2RAD) * fRadius * sinf((fSineAngle + fSineAdd) * DEG2RAD);

			SetData(vcount++, fX1, fY    , fY1);
			if (tex) SetTexture(vcount-1,sinf(fAngle * DEG2RAD),sinf(fSineAngle * DEG2RAD));
			if (color) SetColor(vcount-1,r1,g1,b1);
			SetData(vcount++, fX4, fYNext, fY4);
			if (tex) SetTexture(vcount-1,sinf((fAngle+fAngleAdd) * DEG2RAD),sinf((fSineAngle + fSineAdd) * DEG2RAD));
			if (color) SetColor(vcount-1,r1,g1,b1);
			SetData(vcount++, fX2, fY    , fY2);
			if (tex) SetTexture(vcount-1,sinf((fAngle+fAngleAdd) * DEG2RAD),sinf((fSineAngle) * DEG2RAD));
			if (color) SetColor(vcount-1,r1,g1,b1);
			
			SetData(vcount++, fX1, fY    , fY1);
			if (tex) SetTexture(vcount-1,sinf(fAngle * DEG2RAD),sinf(fSineAngle * DEG2RAD));
			if (color) SetColor(vcount-1,r2,g2,b2);
			SetData(vcount++, fX3, fYNext, fY3);
			if (tex) SetTexture(vcount-1,sinf((fAngle) * DEG2RAD),sinf((fSineAngle + fSineAdd) * DEG2RAD));
			if (color) SetColor(vcount-1,r2,g2,b2);
			SetData(vcount++, fX4, fYNext, fY4);
			if (tex) SetTexture(vcount-1,sinf((fAngle+fAngleAdd) * DEG2RAD),sinf((fSineAngle + fSineAdd) * DEG2RAD));
			if (color) SetColor(vcount-1,r2,g2,b2);
			// Move to the next angle
			fAngle += fAngleAdd;
		}
		fSineAngle += fSineAdd;
	}
	for (i=0;i<m_nIndices;i++)
		m_indices[i]=i;
	//SetColor(-1,0.5,0.5,0.5);
}

void SDLGLPlane::Create(float x, float y, float z, float width, float depth) {
	SDLGLObject::SetPosition(x,y,z);
	m_fWidth = width;
	m_fDepth = depth;
	Construct(true,false,4,2*3);
	SetData(0,-0.5f*width,0,-0.5f*depth);
	SetData(1, 0.5f*width,0,-0.5f*depth);
	SetData(2, 0.5f*width,0, 0.5f*depth);
	SetData(3,-0.5f*width,0, 0.5f*depth);
	SetColor(0,1,0,0);
	SetColor(1,0,1,0);
	SetColor(2,0,0,1);
	SetColor(3,1,1,1);
	SetIndex(0,0,1,2);
	SetIndex(1,2,0,3);
	
}

void SDLGLPlane::Create(float px, float py, float pz, float width, float depth, int divisions_w, int divisions_d, const float *heightmap) {
	bool tex=false;
	bool color=true;

	float r1,g1,b1,r2,g2,b2;
	r1=0;g1=1;b1=1;
	r2=1;g2=1;b2=0;
	
	SDLGLObject::SetPosition(px,py,pz);
	m_fWidth = width;
	m_fDepth = depth;
	int xDim=divisions_w;
	int yDim=divisions_d;
	Construct(color,tex,xDim*yDim,(xDim-1)*(yDim-1)*6);

	float sizeX=width;
	float sizeY=depth;
	float hsx=width/2;
	float hsy=depth/2;

	int x,y;
	
	for (y=0;y < yDim;y++)
	for (x=0;x < xDim;x++) {
		if (heightmap)
			SetData(x+y*xDim,x/(float)(xDim-1)*sizeX-hsx, heightmap[x+y*xDim] , y/(float)(yDim-1)*sizeY-hsy,0,0,1);
			else
			SetData(x+y*xDim,x/(float)(xDim-1)*sizeX-hsx, 0 , y/(float)(yDim-1)*sizeY-hsy,0,0,1);

		if (tex) SetTexture(x+y*xDim,x/(float)(xDim-1),y/(float)(yDim-1));
		if (y&1) {
			if (x&1) {
				if (color) SetColor(x+y*xDim,r1,g1,b1);
				else {
					if (color) SetColor(x+y*xDim,r2,g2,b2);
				}
			}
		} else {
			if (x&1) {
				if (color) SetColor(x+y*xDim,r2,g2,b2);
				else {
					if (color) SetColor(x+y*xDim,r1,g1,b1);
				}
			}
		}
	}
	for (y=0;y < yDim-1;y++)
	for (x=0;x < xDim-1;x++) {
		SetIndex(((x+y*(xDim-1))*2)+0,(y*xDim)+x,(y*xDim)+xDim+x,(y*xDim)+x+1);
		SetIndex(((x+y*(xDim-1))*2)+1,(y*xDim)+x+1,(y*xDim)+xDim+x,(y*xDim)+x+xDim+1);
	}
//	SetColor(-1,0.8,0,0.5);
}





void SDLGLBox::Create(float x, float y, float z, float width, float height, float depth) {
	SDLGLObject::SetPosition(x,y,z);
	m_fWidth = width;
	m_fHeight = height;
	m_fDepth = depth;
	Construct(true,false,12,12*3);

	float verts[]={
0,0,0, 
1,0,0, 
1,1,0, 
0,1,0, 
1,0,1, 
1,1,1, 
0,1,1, 
0,0,1, 
0,1,1, 
0,1,0, 
1,0,1, 
1,0,0};
	int inds[36] = {   0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6, 
            7,3,0,   7,6,3,   9,5,2,   9,8,5,   0,11,10,   0,10,7};
	int i;
	for (i=0;i<12;i++) {
		SetData(i,(verts[i*3+0]-0.5f)*width,(verts[i*3+1]-0.5f)*height,(verts[i*3+2]-0.5f)*depth);
		SetColor(i,verts[i*3+0],verts[i*3+1],verts[i*3+2]);
	}
	for (i=0;i<12;i++) {
		SetIndex(i,inds[i*3+0],inds[i*3+1],inds[i*3+2]);
	}
#if 0
	SetData(0,-0.5f*width,-0.5f*height,-0.5f*depth);
	SetData(1, 0.5f*width,-0.5f*height,-0.5f*depth);
	SetData(2, 0.5f*width, 0.5f*height,-0.5f*depth);
	SetData(3,-0.5f*width, 0.5f*height,-0.5f*depth);
	SetData(4,-0.5f*width,-0.5f*height, 0.5f*depth);
	SetData(5, 0.5f*width,-0.5f*height, 0.5f*depth);
	SetData(6, 0.5f*width, 0.5f*height, 0.5f*depth);
	SetData(7,-0.5f*width, 0.5f*height, 0.5f*depth);
	
	SetColor(0,1,0,0);
	SetColor(1,0,1,0);
	SetColor(2,0,0,1);
	SetColor(3,1,1,1);
	SetColor(4,1,0,1);
	SetColor(5,0,1,1);
	SetColor(6,1,1,0);
	SetColor(7,0,0,0);

	SetIndex(0,0,1,2);
	SetIndex(1,2,0,3);
	
	SetIndex(2,4,5,6);
	SetIndex(3,6,4,7);

	SetIndex(4,0,1,4);
	SetIndex(5,1,4,5);

	SetIndex(6,3,2,7);
	SetIndex(7,2,7,6);

	SetIndex(8,0,3,4);
	SetIndex(9,4,3,7);

	SetIndex(10,1,2,5);
	SetIndex(11,5,2,6);
#endif
/*	m_mode=GL_TRIANGLES;
	m_format=GL_C3F_V3F;

	m_data = new float[(3+3)*4];
	//color
	m_data[0+0*(3+3)] = 1;
	m_data[1+0*(3+3)] = 0;
	m_data[2+0*(3+3)] = 0;
	//vertex
	m_data[3+0*(3+3)] = 0;
	m_data[4+0*(3+3)] = 0;
	m_data[5+0*(3+3)] = 0;

	//color
	m_data[0+1*(3+3)] = 0;
	m_data[1+1*(3+3)] = 1;
	m_data[2+1*(3+3)] = 0;
	//vertex
	m_data[3+1*(3+3)] = 1;
	m_data[4+1*(3+3)] = 0;
	m_data[5+1*(3+3)] = 0;

	//color
	m_data[0+2*(3+3)] = 0;
	m_data[1+2*(3+3)] = 0;
	m_data[2+2*(3+3)] = 1;
	//vertex
	m_data[3+2*(3+3)] = 1;
	m_data[4+2*(3+3)] = 1;
	m_data[5+2*(3+3)] = 0;

	//color
	m_data[0+3*(3+3)] = 1;
	m_data[1+3*(3+3)] = 1;
	m_data[2+3*(3+3)] = 1;
	//vertex
	m_data[3+3*(3+3)] = 0;
	m_data[4+3*(3+3)] = 1;
	m_data[5+3*(3+3)] = 0;

	m_indices = new unsigned long[6];
	m_indices[0]=0;
	m_indices[1]=1;
	m_indices[2]=2;
	m_indices[3]=2;
	m_indices[4]=0;
	m_indices[5]=3;

	m_nVertices=4;
	m_nIndices=6;
	*/
}

void SDLGLTexture::Load(char *filename) {
	GLenum hr;
	SDL_Surface *TextureImage; 
	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
    if ( ( TextureImage = SDL_LoadBMP( filename ) ) )
        {
	    /* Create The Texture */
	    glGenTextures( 1, &m_texture );
		hr=glGetError();
		if (hr!=GL_NO_ERROR) printf("gl error: 0x%x",hr);

		glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	    /* Typical Texture Generation Using Data From The Bitmap */
	    glBindTexture( GL_TEXTURE_2D, m_texture );
		hr=glGetError();
		if (hr!=GL_NO_ERROR) printf("gl error: 0x%x",hr);

	    /* Generate The Texture */
	    glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage->w,
			  TextureImage->h, 0, GL_RGB,//GL_BGR,
			  GL_UNSIGNED_BYTE, TextureImage->pixels );
		hr=glGetError();
		if (hr!=GL_NO_ERROR) printf("gl error: 0x%x",hr);

	    /* Linear Filtering */
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        }

    /* Free up any memory we may have used */
    if ( TextureImage )
	    SDL_FreeSurface( TextureImage );
}

void SDLGLTexture::SetActive(bool active) {
	if (active) {
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_texture );
	} else {
		glDisable( GL_TEXTURE_2D );
	}
}
