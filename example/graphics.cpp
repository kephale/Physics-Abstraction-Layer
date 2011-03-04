#include "graphics.h"

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
//the graphics object class

PAL_VECTOR <GraphicsObject *> g_Graphics;
SDLGLEngine *g_eng=NULL;
SDLGLObject *terrain_graphics=NULL;

//#define DOUBLE_PRECISION

void GraphicsObject::Display()
{
	palMatrix4x4 m;
	if (!m_pBody) {
		for (unsigned int i=0;i<m_Graphics.size();i++) {
            SDLGLObject* sdlglObj = m_Graphics[i];
            sdlglObj->Render();
		}
		return;
	}
	m = m_pBody->GetLocationMatrix();
	float matrix[16];
	for (unsigned int i=0;i<m_Graphics.size();i++) {
		m = m_Geoms[i]->GetLocationMatrix();
		for (int j=0;j<16;j++)
			matrix[j]=(float)m._mat[j];
		m_Graphics[i]->SetPosition(matrix);
		m_Graphics[i]->Render();
	}
}

GraphicsObject* BuildGraphics(palBodyBase *pb)
 {
		if (!pb) return 0;
		unsigned int j;
		GraphicsObject *g;
		g = new GraphicsObject; //create a graphics object 
		g->m_pBody = pb; //set the physical body, this will give us our position
		for (j=0;j<pb->m_Geometries.size();j++) {
			SDLGLObject *psg = 0;
			palGeometry *pg = pb->m_Geometries[j];
			switch (pg->m_Type) {
			case PAL_GEOM_BOX:
				SDLGLBox *pSGBox;
				palBoxGeometry *pBoxG;
				pBoxG=dynamic_cast<palBoxGeometry *>(pg);

				pSGBox = new SDLGLBox;
				pSGBox->Create(0,0,0,pBoxG->m_fWidth,pBoxG->m_fHeight,pBoxG->m_fDepth);
				psg=pSGBox;
				break;
			case PAL_GEOM_SPHERE:
				SDLGLSphere *pSGSphere;
				palSphereGeometry *pSphereG;
				pSphereG=dynamic_cast<palSphereGeometry *>(pg);
				pSGSphere = new SDLGLSphere;
				pSGSphere->Create(0,0,0,pSphereG->m_fRadius);
				psg=pSGSphere;
				break;
			case PAL_GEOM_CAPSULE:
				SDLGLCappedCylinder *pSGcyl;
				palCapsuleGeometry *pCylG;
				pCylG=dynamic_cast<palCapsuleGeometry *>(pg);
				pSGcyl = new SDLGLCappedCylinder;
//				pSGcyl->Create(0,0,0,pCylG->m_fRadius,pCylG->m_fLength);
				pSGcyl->CreateMesh(0,0,0,pCylG->m_fRadius,pCylG->m_fLength);
				psg=pSGcyl;
				break;
			case PAL_GEOM_CONVEX:
				SDLGLPoints *pSGp;
				palConvexGeometry *pgc;
				pgc = dynamic_cast<palConvexGeometry *>(pg);
				pSGp = new SDLGLPoints;
#ifndef DOUBLE_PRECISION
				pSGp->Create(0,0,0,&pgc->m_vfVertices[0],((int)pgc->m_vfVertices.size())/3);
#endif
				psg =  pSGp;
				break;
			}
			if ((pg)&&(psg)) {
				g->m_Geoms.push_back(pg);
				g->m_Graphics.push_back(psg);
				palMatrix4x4 mb,mg,diff;
				mb=g->m_pBody->GetLocationMatrix();
				mg=pg->GetLocationMatrix();
				mat_sub(&diff,&mb,&mg);
			}
		}
		if (g->m_Graphics.size()>0) {
			g_Graphics.push_back(g);
			return g;
		}
		else
			delete g;
		return NULL;
	}


 void DeleteGraphics(palBodyBase *pb) {
	for (unsigned int i=0;i<g_Graphics.size();i++) {
		if (g_Graphics[i]->m_pBody == pb) {
			delete g_Graphics[i];
			g_Graphics.erase(g_Graphics.begin() + i);
		}
	}
}


GraphicsObject* BuildGraphics(palTerrain *pt) {
	BuildTerrainGraphics(pt);
	return 0;
}

 
void BuildTerrainGraphics(palTerrain *pt) {
	SDLGLObject *pSGterrain = 0;
	palMatrix4x4 m;
	m=pt->GetLocationMatrix();
	switch (pt->GetType() ) {
		case PAL_TERRAIN_PLANE:
			{
			palTerrainPlane *ptp;
			ptp= dynamic_cast<palTerrainPlane *>(pt);
			if (ptp) {
			SDLGLPlane *pSGplane;
			pSGplane = new SDLGLPlane;
			pSGplane->Create(m._41,m._42,m._43,ptp->GetMinimumSize(),ptp->GetMinimumSize());
			
			pSGterrain = pSGplane;
			}
			}
		break;
		case PAL_TERRAIN_HEIGHTMAP:
			{
			palTerrainHeightmap *pth;
			pth= dynamic_cast<palTerrainHeightmap *>(pt);
			SDLGLPlane *pSGplane;
			pSGplane = new SDLGLPlane;
			printf("made heightmap [%f %f %f]\n",m._41,m._42,m._43);
#ifndef DOUBLE_PRECISION
			pSGplane->Create(m._41,m._42,m._43,pth->GetWidth(),pth->GetDepth(),pth->GetDataWidth(),pth->GetDataDepth(),pth->GetHeightMap());
#endif
			pSGterrain = pSGplane;
			}
		break;
		case PAL_TERRAIN_MESH:
			{
			palTerrainMesh *ptm;
			ptm = dynamic_cast<palTerrainMesh *>(pt);
			SDL_Mesh  *pSGmesh = NULL;
			pSGmesh = new SDL_Mesh;
			pSGmesh->Init(ptm->m_nVertices*3,ptm->m_nIndices,ptm->m_pVertices ,ptm->m_pIndices);

			pSGterrain = pSGmesh;
			}
			break;
		default:
			printf("unkown terrain type!!\n");	
		break;
	}

	if (pSGterrain) {
		GraphicsObject *g;
		g = new GraphicsObject; //create a graphics object 
		g->m_Graphics.push_back(pSGterrain);
		g_Graphics.push_back(g);
	}

}

GraphicsObject* BuildGraphics(palLink* link) {
	SDLGLObject* sglObj;
	switch (link->m_Type) {
	case PAL_LINK_REVOLUTE:
      {
		SDLGLSphere* sphere = new SDLGLSphere();
		sphere->Create(link->m_fPosX, link->m_fPosY, link->m_fPosZ, 1.0f);
		sglObj = sphere;
      }
      break;
	default:
        sglObj = 0;
		break;
	}
	GraphicsObject *g;
	if (sglObj) {
		g = new GraphicsObject; //create a graphics object
		//g->m_pPositioned = link;
		g->m_Graphics.push_back(sglObj);
		g_Graphics.push_back(g);
	}
	else {
		g = 0;
	}
	return g;
}
