#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../sdlgl/sdlgl.h"
#include "../framework/common.h" // for PAL_VECTOR
#include "../pal/palBodyBase.h"
#include "../pal/palTerrain.h"

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
//the graphics object class

extern SDLGLEngine *g_eng;
extern SDLGLObject *terrain_graphics;

class GraphicsObject {
public:
	GraphicsObject() {
		m_pBody=0;
	}
	~GraphicsObject() {
		for (unsigned int i=0;i<m_Graphics.size();i++)
			delete m_Graphics[i];
	}
	void Display();
	palBodyBase *m_pBody;
	PAL_VECTOR <SDLGLObject *> m_Graphics;
	PAL_VECTOR <palGeometry *> m_Geoms;
};

extern PAL_VECTOR <GraphicsObject *> g_Graphics;


extern void BuildTerrainGraphics(palTerrain *pt);
extern void DeleteGraphics(palBodyBase *pb);
extern GraphicsObject* BuildGraphics(palBodyBase *pb);
extern GraphicsObject* BuildGraphics(palTerrain *pt);
#endif

