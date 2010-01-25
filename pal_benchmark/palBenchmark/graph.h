#include "main.h"

#ifndef GRAPH_H
#define GRAPH_H

class Graph : public DemoScene {
public:
	//input : text nodes
	virtual void MakeGraphArea(std::vector<irr::scene::ISceneNode *> nodes, bool colored = false) {
		g_gui->clear();
		g_smgr->addCameraSceneNode(0, vector3df(0,0,-10), vector3df(0,0,0));

		g_smgr->addLightSceneNode(0, core::vector3df(0,0,-10),
			video::SColorf(0.6f, 0.6f, 0.6f), 7.0f);

		size_t num = nodes.size();
		for (unsigned int i=0;i<num;i++) {
			nodes[i]->setPosition(vector3df(16*(i/(float)num) - 8, -6.5 ,0));
			nodes[i]->setRotation(core::vector3df(-90,0,45));
			ApplyMaterialToNode(nodes[i]);
			if (colored) {
				nodes[i]->setMaterialType(EMT_SOLID);
				for (unsigned int ii=0;ii<nodes[i]->getMaterialCount();ii++) {
					nodes[i]->getMaterial(ii).DiffuseColor = g_colors[i];
					nodes[i]->getMaterial(ii).AmbientColor = g_colors[i];
					nodes[i]->getMaterial(ii).EmissiveColor= g_colors[i];
					nodes[i]->getMaterial(ii).SpecularColor= g_colors[i];
				}
			}
			//nodes[i]->setScale(core::vector3df(0.5,1,1));
		}
	}

virtual void Render() {
//		g_driver->beginScene(true, true, SColor(255,100,101,140));
//		g_driver->beginScene(true, true, SColor(255,0,0,0));
		g_driver->beginScene(true, true, SColor(255,250,250,250));

		g_smgr->drawAll();
		g_gui->drawAll();

		matrix4 m;
		m.makeIdentity();
		g_driver->setTransform(video::ETS_WORLD,m);

		SMaterial mat;
		mat.Lighting= true;
		mat.DiffuseColor = SColor(255,0,0,0);
		mat.AmbientColor = SColor(255,0,0,0);
		mat.EmissiveColor =SColor(255,0,0,0);
		g_driver->setMaterial(mat);
		float h_y = -3;
		g_driver->draw3DLine(vector3df(-8,h_y,0),vector3df(8,h_y,0));
		g_driver->draw3DLine(vector3df(-8,h_y,0),vector3df(-8,7,0));

		g_driver->endScene();
}

};

class PlotGraph : public Graph {
public:
	void MakeGraphPlot(std::vector<std::vector<float> > plot_data) {
		if (plot_data.size()<1)
			return;
		
		float max;
		std::vector<float> vmax;
		for (unsigned j=0;j<plot_data.size();j++) {
			vmax.push_back(* std::max_element(plot_data[j].begin(),plot_data[j].end()));
		}
		max = * std::max_element(vmax.begin(),vmax.end());

		for (unsigned j=0;j<plot_data.size();j++) {
			size_t num=plot_data[j].size();
			for (unsigned i=0;i<num;i++) {
				plot_data[j][i] /= max;
				float h = plot_data[j][i] * 10;
				scene::ISceneNode *node = 
					g_smgr->addBillboardSceneNode( 0, core::dimension2df(0.1f,0.1f), core::vector3df(16.0f * (irr::f32) i / num - 8,h-3,0));

				node->setMaterialType(EMT_SOLID);
				for (unsigned int ii=0;ii<node->getMaterialCount();ii++) {
					node->getMaterial(ii).DiffuseColor = g_colors[j];
					node->getMaterial(ii).AmbientColor = g_colors[j];
					node->getMaterial(ii).EmissiveColor= g_colors[j];
					node->getMaterial(ii).SpecularColor= g_colors[j];
				}
			}
		}
	}


};

class BarGraph : public Graph {
public:
	/*float vmax(std::vector<float> x) {
		float max = -99999999999;
		for (int i=0;i<x.size();i++) {
			if (x[i]>max) max=x[i];
		}
		return max;
	}*/
void MakeGraphBars(std::vector<irr::scene::ISceneNode *> nodes, float offset, float width, std::vector<float> heights, float setmax = -1) {
	
	if (heights.size()<1)
		return;
	
	IAnimatedMesh* mesh = g_smgr->getMesh( cubeMeshFile.c_str() );
	float max;
	if (setmax<0)
		max = * std::max_element(heights.begin(),heights.end(),std::less<float>()); //WTF!?
	else
		max=setmax;
	//float max = vmax(heights);
	
	unsigned int i = 0;
//	printf("max:%f\n",max);
	for (i=0;i<heights.size();i++) {
		heights[i]/=max;
//		printf("hi:%f\n",heights[i]);
	}
	//system("pause");

	size_t num=nodes.size();
	for ( i=0;i<num;i++) {
	IAnimatedMeshSceneNode* node = g_smgr->addAnimatedMeshSceneNode( mesh );
	
	node->setMaterialFlag(EMF_LIGHTING, true);
	node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->setMaterialType(EMT_SOLID);
	for (unsigned int ii=0;ii<node->getMaterialCount();ii++) {
		node->getMaterial(ii).DiffuseColor = g_colors[i];
		node->getMaterial(ii).AmbientColor = g_colors[i];
		node->getMaterial(ii).EmissiveColor= g_colors[i];
		node->getMaterial(ii).SpecularColor= g_colors[i];
	}


	float y=heights[i]*4.5f;
	node->setScale(vector3df(width,y,0.5));
	float offmod = 0.5f* 16.f*(1.f/num);
	node->setPosition(vector3df(16*(i/(float)num) - 8 + offset + offmod, y - 3 ,0));
	}
}

};

#endif