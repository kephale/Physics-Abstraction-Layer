#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../sdlgl/sdlgl.h"
#include "../pal/pal.h"

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
	VECTOR <SDLGLObject *> m_Graphics;
	VECTOR <palGeometry *> m_Geoms;
};

extern VECTOR <GraphicsObject *> g_Graphics;


void BuildTerrainGraphics(palTerrain *pt);
void DeleteGraphics(palBodyBase *pb);
GraphicsObject* BuildGraphics(palBodyBase *pb);
GraphicsObject* BuildGraphics(palTerrain *pt);
#endif