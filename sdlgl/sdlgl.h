#ifndef SDLGL_H
#define SDLGL_H
//(c) Adrian Boeing 2004, see licence.txt (BSD licence)
/*
	Abstract:
		SDLGL - Simplistic SDL/GL based graphics interface
	Author:
		Adrian Boeing
	Revision History:
		Version 0.92: 02/11/07 added get vert position
		Version 0.91: 20/08/07 added point list
		Version 0.9 : 24/12/04 added generic mesh
		Version 0.82: 11/06/04 started checkboard color support, setnormal, custom plane (heightmapped) creation
		Version 0.81: 03/06/04 Added camera support via GLU
		Version 0.8 : 31/05/04
	TODO:
		-remove GLU dependenicies
		-proper texture support
		-proper colored support (checkboard patterns)
		-assert portability
		-optimize cylinder and sphere creation/rendering
		-support GAL
*/


#ifdef _WIN32												// If We're Under MSVC
#include <windows.h>										// We Need The Windows Header
#else														// Otherwhise
#include <stdio.h>											// We Only Need The Standard IO Header
#include <stdlib.h>
#include <string.h>
typedef unsigned long DWORD;
#endif														// And...

#include <math.h>


#ifndef _WIN32
	#include <SDL/SDL.h>										
#else
	#include <SDL.h>
#endif

#ifdef __APPLE__
	#include <OpenGL/gl.h>															
	#include <OpenGL/glu.h>	
#else
	#include <GL/gl.h>															// We're Including The OpenGL Header
	#include <GL/glu.h>															// And The GLu Header
#endif

//#ifdef WIN32																// If We're Under MSVC
//#pragma comment(lib, "OpenGL32.lib")										// We're Telling The Linker To Look For The OpenGL32.lib
//#pragma comment(lib, "GLu32.lib")											// The GLu32.lib Library...
//#pragma comment(lib, "SDLmain.lib")											// The SDLmain.lib And
//#pragma comment(lib, "SDL.lib")												// The SDL.lib Libraries
//#endif				

class SDLGLEngine {
public:
	bool Init(int width, int height, int bpp = 32, bool fullscreen = false);
	void Resize(int width, int height);
	void SetViewMatrix(float eye_x, float eye_y, float eye_z, float lookat_x, float lookat_y, float lookat_z, float up_x, float up_y, float up_z);
	void SetProjMatrix(float fov,float aspect,float nearcp,float farcp);
	void SetViewProj2d(int width, int height);
	void Flip();
	void Clear();
	void SetTitle(char *title);
	void Transparent(bool state) {
		if (state) {
			glBlendFunc(GL_ONE, GL_ONE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
		} else {
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
		}
	}
	void Wireframe(bool state) {
		if (state) {
			 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
		}
	}
private:
	SDL_Surface *m_Screen;
};

class SDLGLObject {
public:
	SDLGLObject();
	~SDLGLObject();
	void SetPosition(float *transform);
	void SetPositionX(float x);
	void SetPositionY(float y);
	void SetPositionZ(float z);
	float GetPositionX(void);
	float GetPositionY(void);
	float GetPositionZ(void);
	void SetPosition(float x, float y, float z);
	void SetPosition(float x, float y, float z, float roll, float pitch, float yaw);
	void SetScale(float x, float y, float z);
	virtual void Render();
/*	Float m_fPosX;
	Float m_fPosY;
	Float m_fPosZ;*/
	void CleanData();

	float *GetTransform() {
		return m_transform;
	}
//protected:
	void Construct(bool Color, bool Texture, int nVertices, int nIndices);
	void GetVertexPosition(int position, float &x, float &y, float &z);
	void SetData(int position, float x, float y, float z, float nx = 0, float ny = 0, float nz = 0, float r = 0, float g = 0, float b = 0, float tx = 0, float ty = 0);
	void SetColor(int position, float r, float g, float b);
	void SetTexture(int position, float tx, float ty);
	void SetIndex(int pos, int x, int y, int z);
	void SetNormal(int position, float x, float y, float z);
	void BuildNormals();
	GLenum m_mode;
	GLenum m_format;
	int m_nVertices;
	int m_nIndices;
	int m_datasize;
	float *m_data;
	DWORD *m_indices;
	float m_transform[16];
	bool m_Textured;
	bool m_Colored;
};
/*
class SDL_Lines : public SDLGLObject {
	void Create(int nVertices, float *Vertices);
};*/

class SDLGLPoints: public SDLGLObject {
public:
	void Create(float x, float y, float z, float *points, int npoints);
	virtual void Render();
};


//generic mesh
class SDL_Mesh  : public SDLGLObject {
public:
	//n vertexdata = 3*vertices
	void Init(int nVertexData, int nIndices, const float *Vertices, const int *Indices ,bool colored = true, bool textured = false) {
		Construct(colored,textured,nVertexData,nIndices);
		int i;
		for (i=0;i<nVertexData;i+=3) {
			SetData(i/3,Vertices[i],Vertices[i+1],Vertices[i+2]);
			SetColor(i/3,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX);
		}
		/*
		for (i=0;i<nIndices;i+=3) {
			SetIndex(i/3,Indices[i],Indices[i+1],Indices[i+2]);
		}
		*/
		for (i=0;i<nIndices;i++) {
			m_indices[i] = Indices[i];
		}
	}
	void SetAllColor(float r, float g, float b) {
		if (!m_Colored) return;
		for (int i=0;i<m_nVertices/3;i++) {
			SetColor(i,r,g,b);
		}
	}
	void SetMode(GLenum mode) {
		m_mode = mode;
	}
};

class SDLGLTexture {
public:
	void Load(char *filename);
	void SetActive(bool active);
protected:
	GLuint m_texture; 
};

class SDLGLPlane : public SDLGLObject {
public:
	void Create(float x, float y, float z, float width, float depth);
	void Create(float x, float y, float z, float width, float depth, int divisions_w, int divisions_d, const float *heightmap);
	float m_fWidth;
	float m_fDepth;
};

class SDLGLBox: public SDLGLObject {
public:
	void Create(float x, float y, float z, float width, float height, float depth);

	float m_fWidth;
	float m_fHeight;
	float m_fDepth;
};

class SDLGLSphere: public SDLGLObject {
public:
	void Create(float x, float y, float z, float radius, int hstrip=5, int vslice=7);

	float m_fRadius;
};

class SDLGLCappedCylinder: public SDL_Mesh {
public:
	void Create(float x, float y, float z, float radius, float length);
	void CreateMesh(float x, float y, float z, float radius, float length, int hstrip=10, int vslice=10);
	float m_fRadius;
	float m_fLength;
};
/*
class SDLCylinder: public SDLGLObject {
public:
	void Create(float x, float y, float z, float radius, float length);

	float m_fRadius;
	float m_fLength;
};
*/
#endif
