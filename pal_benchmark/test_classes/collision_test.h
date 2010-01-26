#ifndef PAL_COLLISION_TEST_H
#define PAL_COLLISION_TEST_H

#include "palFactory.h" //PAL physics
#include "pal_test.h"

/*
	PAL Test Collection
    Copyright (C) 2007  Adrian Boeing

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

template <typename T = PALTest> class PAL_Collision_Test : public T
{
protected:
	virtual void doInnerUpdateLoop() { ; }

	float verts[3*5];					//pyramid verts
	int	  inds[3*4];					//pyramid inds
	std::vector<palSphere *> vSpheres;	//sphere bodies
	palVector3 plane_normals[4];		//pyramid face plane
	float plane_ds[4];
	float radius;

	int doCreatePhysics()
	{	
		radius = 0.04f;
		float lverts[] = {	0,0,1,					//top
							-1,0,0,	0,-1,0, 1,0,0,	//middle
							0,0,-1 };				//bottom

		int linds[] = {	2,1,0, 2,0,3,
						1,2,4, 2,3,4};

		memcpy(verts,lverts,3*5*sizeof(float));
		memcpy(inds,linds,3*4*sizeof(int));
		
		this->pp = PF->CreatePhysics();
		
		if (this->pp == NULL) {
		#ifdef _WIN32
			MessageBox(NULL,"Could not start physics!","Error",MB_OK);
		#else
			perror("Error: Could not start physics!");
		#endif
			return -1;
		}
		//initialize gravity
		palPhysicsDesc desc; // -9.8f gravity, remember to set BW
		desc.m_nUpAxis = 2;
		this->pp->Init(desc); //initialize it, set the main gravity vector

		//initialize terrain
		palTerrainMesh *ptm = 0;
		ptm = PF->CreateTerrainMesh();
		ptm->Init(0,0,0,verts,5,inds,3*4);

		palSphere *ps;
		for (int j=0;j<8;j++) {
			for (int i=0;i<8;i++) {
				ps = PF->CreateSphere();
				ps->Init(i/10.0f - 0.4f,0.2f,j/10.0f - 0.4f,radius,1);
#ifdef NO_IDEA_WHERE_THIS_SHOULD_COME_FROM
				BuildGraphics(ps);
#endif
				vSpheres.push_back(ps);
			}
		}
		//=================================================================
		//create data for verification	

			//calcualte plane normals
			for (int i=0;i<4;i++) {
				palVector3 v1,v2,v3;
				int ix;
				ix = inds[i*3+0];
				vec_set(&v1,verts[ix*3+0],verts[ix*3+1],verts[ix*3+2]);
				ix = inds[i*3+1];
				vec_set(&v2,verts[ix*3+0],verts[ix*3+1],verts[ix*3+2]);
				ix = inds[i*3+2];
				vec_set(&v3,verts[ix*3+0],verts[ix*3+1],verts[ix*3+2]);
				palVector3 v21,v31;
				vec_sub(&v21,&v2,&v1);
				vec_sub(&v31,&v3,&v1);

				vec_cross(&plane_normals[i],&v21,&v31);
				plane_ds[i] = -vec_dot(&plane_normals[i],&v1);
				//printf("n:%f %f %f\n",plane_normals[i].x,plane_normals[i].y,plane_normals[i].z);
			}

			return 0;
	}

	float distance_from_plane(palVector3 point, palVector3 norm, float d) {
		float mag = vec_mag(&norm);
		float top = norm.x * point.x + norm.y * point.y + norm.z * point.z + d;
		return fabs(top)/mag;
	}

	virtual void Reset() {
		data.clear();
	}
	
	void SaveData() {
			float esum = 0;
			for (unsigned int i=0;i<vSpheres.size();i++) {
				palVector3 pos;
				vSpheres[i]->GetPosition(pos);
				if (pos.y<-1) {
//					printf("FAILED!!! DROPPED THROUGH!");
					esum = -9999999;
				}
				for (int j=0;j<4;j++) { 
					float dist = distance_from_plane(pos,plane_normals[j],plane_ds[j]);
					if (dist<radius) {
//						printf("Sphere %d is below plane %d %f\n",i,j,radius-dist);
						esum+=radius-dist;
					}
				}
			}
			data.push_back(esum);

	}
public:
	std::vector<float> data;
};

#endif
