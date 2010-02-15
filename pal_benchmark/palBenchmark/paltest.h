#ifndef PALTEST_H
#define PALTEST_H

#ifdef _WIN32
	#pragma warning(disable:4355)	// Disables: warning C4355: 'this' : used in base member initializer list
	#pragma comment(lib, "libpal.lib") //only for MSVC!
#endif

#include "pal/palFactory.h"

#include "main.h"
#include "../test_classes/pal_test.h"
#include "strings.h"

class PALTestScene : public DemoScene, public PALTest {
	
public:
	PALTestScene() {
		m_render_to_physics_ratio=1;
	}
	int m_render_to_physics_ratio;
	int m_render_count;
	u32 m_last_time;
	virtual int CreatePhysics() {
		ErasevBO();
		return doCreatePhysics();
	}
	virtual void StoreData()=0;

	virtual void Render() {
		m_render_count++;
		if (m_render_count%m_render_to_physics_ratio != 0) 
			return;
		doRender();
	}

	virtual void doRender() {
		ITimer *t = g_device->getTimer();
		u32 time = t->getTime();
		u32 tdiff = time - m_last_time;
		if (tdiff < 10) {
			g_device->sleep(10-tdiff);
		}
		g_driver->beginScene(true, true, m_clear_color);
		g_smgr->drawAll();
		g_gui->drawAll();
		g_driver->endScene();
		m_last_time = time;
	}

	virtual	void MakeSkybox() {
   	// create skybox
	g_driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
	g_smgr->addSkyBoxSceneNode(
		g_driver->getTexture( IOUpImage.c_str() ),
		g_driver->getTexture( IODownImage.c_str() ),
		g_driver->getTexture( IOLeftImage.c_str() ),
		g_driver->getTexture( IORightImage.c_str() ),
		g_driver->getTexture( IOFooterImage.c_str() ),
		g_driver->getTexture( IOBackImage.c_str() ));
	g_driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);
	}
	void UpdateGUI(std::string enginename) {

		//----------------------------------------------------------------------
		// Construct Engine image name to load
		std::string engineNameLowercase = enginename;
		// Make engine name all lowercase
		for(unsigned int i = 0; i < engineNameLowercase.size(); ++i) {
			engineNameLowercase[i] = tolower(engineNameLowercase[i]);
		}
	#ifndef __APPLE__
		// Append the .. as a prefix if Windows or Linux, not needed for Mac OS X
                char prefix[] = "..";
        #else
                char prefix[] = "";
	#endif
		char buf[512];
		sprintf( buf, "%s/media/%s_logo.png", prefix,
                         engineNameLowercase.c_str() );
		//----------------------------------------------------------------------
		
		g_gui->clear();
		ITexture *tex = g_driver->getTexture(buf);
		const core::dimension2d<s32>& size = tex->getSize();
		const core::dimension2d<s32>& ss = g_driver->getScreenSize();
		
		g_gui->addImage(tex, core::position2d<s32>(ss.Width - size.Width ,ss.Height - size.Height));
	}
	virtual void Update() {
		if (g_SceneFinished)
			return;
		
		//update physics
		if (pp) pp->Update(step_size);
		if (pp)	counter += (int)(pp->GetTime()/max_time);
		
		SaveData();

		//update gfx state
		for (unsigned int i=0;i<vbo.size();i++) {
			vbo[i]->Update();
		}

		//do inner loop
		doInnerUpdateLoop();
	
		if (counter!=last_counter) {
				StoreData();
				int selected = counter % (g_engines.size());
				if (selected==0) {
					g_SceneFinished = true;
					return;
				}
				PF->Cleanup();
				pp = 0;
				printf("now using %s [%d %d]\n",g_engines[counter].c_str(),counter,last_counter);
				UpdateGUI(g_engines[counter]);
				PF->SelectEngine(g_engines[counter]);
				CreatePhysics();
			}
		last_counter = counter;
	};
	
//	void ApplyMaterialToNode(IAnimatedMeshSceneNode* node) {

	void ErasevBO() {
		for (unsigned int i=0;i<vbo.size();i++) {
			vbo[i]->node->remove();
			delete vbo[i];
		}
		vbo.clear();
	}

	void BuildGraphics(palTerrain *pt) {
		//IAnimatedMesh* mesh = 0;
		ISceneNode* node = 0;
		switch (pt->GetType()) {
			case PAL_TERRAIN_PLANE:
				{
					//palTerrainPlane *ptp = dynamic_cast<palTerrainPlane *>(pt);
					//ptp->m_fSize
//					mesh = g_smgr->getMesh("cube.x");
//					node = g_smgr->addAnimatedMeshSceneNode( mesh );
					node = g_smgr->addCubeSceneNode(2);
					node->setScale(vector3df(20,0.01f,20));
				}
				break;
				/*
				PAL_TERRAIN_HEIGHTMAP = 2, //!< Heightmap terrain type
				PAL_TERRAIN_MESH = 3, //!< Mesh terrain type - functionality determind by implementation
				*/
		}
		ApplyMaterialToNode(node);
		if (pt->GetType() == PAL_TERRAIN_PLANE) {
			node->setMaterialFlag(EMF_LIGHTING, false);
			node->setMaterialTexture( 0, g_driver->getTexture( spotImageTexture.c_str() ) );
		}
	}

	void BuildGraphics(palBody *pb) {
        if (!g_smgr) {
            return;
        }
		palGeometry *pg = pb->m_Geometries[0];
		IAnimatedMesh* mesh = 0;
		IAnimatedMeshSceneNode* node = 0;
		switch (pg->m_Type) {
			case PAL_GEOM_BOX:
				{
				palBoxGeometry *pBoxG;
				pBoxG=dynamic_cast<palBoxGeometry *>(pg);
				mesh = g_smgr->getMesh( cubeMeshFile.c_str() );
				node = g_smgr->addAnimatedMeshSceneNode( mesh );
				node->setScale(vector3df( (float)0.5*pBoxG->m_fWidth, (float)0.5*pBoxG->m_fHeight, (float)0.5*pBoxG->m_fDepth ));
				break;
				}
			case PAL_GEOM_SPHERE:
				{
				palSphereGeometry *pSphereG;
				pSphereG=dynamic_cast<palSphereGeometry *>(pg);
				mesh = g_smgr->getMesh( sphereMeshFile.c_str() );
				node = g_smgr->addAnimatedMeshSceneNode( mesh );
				node->setScale(vector3df(pSphereG->m_fRadius,pSphereG->m_fRadius,pSphereG->m_fRadius));
				break;
				}
			case PAL_GEOM_CAPSULE:
				{
				palCapsuleGeometry *pCylG;
				pCylG=dynamic_cast<palCapsuleGeometry *>(pg);
				mesh = g_smgr->getMesh( capsuleMeshFile.c_str() );
				node = g_smgr->addAnimatedMeshSceneNode( mesh );
				aabbox3d<f32> bb = node->getBoundingBox();
				vector3df size = bb.getExtent();
				node->setScale(vector3df(2*pCylG->m_fRadius/size.X,2*pCylG->m_fLength/size.Y,2*pCylG->m_fRadius/size.Z));
				}
		}

		ApplyMaterialToNode(node);
		BindObject *bo = new BindObject;
		bo->pb = pb;
		bo->node = node;
		vbo.push_back(bo);
	}

	int counter;
	int last_counter;
	ICameraSceneNode *camera;
	std::vector<BindObject *> vbo;
};

#endif
