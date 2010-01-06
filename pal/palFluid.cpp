#include "palFluid.h"
#include "palFactory.h"
#include "palCollision.h"
#include <memory.h>

FACTORY_CLASS_IMPLEMENTATION(palDampendShallowFluid);

#define READBUFFER(x,i,j) x[(i)+(j)*m_DimX]
#define SETBUFFER(x,i,j,v) x[(i)+(j)*m_DimX]=v

//todo: try multi-level fluid
//todo: translate

palDampendShallowFluid::palDampendShallowFluid() {
	m_DimX = m_DimY = 0;
	m_Waterbuf0 = 0;
	m_Waterbuf1 = 0;
	m_Vertices = 0;
	m_ReadBuffer = m_Waterbuf0;
	m_WriteBuffer= m_Waterbuf1;
	m_VertexCount = 0;
}

palDampendShallowFluid::~palDampendShallowFluid() {
        delete m_Vertices;
        delete m_Waterbuf0;
        delete m_Waterbuf1;
}

void palDampendShallowFluid::Init(int dimX, int dimY, float cellSize, float density, float dampingFluid, float dampingBody_linear, float dampingBody_angular) {
		m_Waterbuf0 = new float [dimX*dimY];
		m_Waterbuf1 = new float [dimX*dimY];
		memset(m_Waterbuf0,0,sizeof(float)*dimX*dimY);
		memset(m_Waterbuf1,0,sizeof(float)*dimX*dimY);
		m_CellSize = cellSize;
		m_Density = density;
		m_DimX = dimX;
		m_DimY = dimY;
		m_FluidDampingFactor = dampingFluid;
		m_BodyDampingFactor_Linear = dampingBody_linear;
		m_BodyDampingFactor_Angular = dampingBody_angular;
		m_ReadBuffer = m_Waterbuf0;
		m_WriteBuffer= m_Waterbuf1;
		m_Vertices = new palVector3[dimX*dimY];
		m_VertexCount = 0;
	}
void palDampendShallowFluid::SwitchBuffers() {
		float *tmp;
		tmp = m_ReadBuffer;
		m_ReadBuffer = m_WriteBuffer;
		m_WriteBuffer = tmp;
	}
void palDampendShallowFluid::UpdateFluid() {
		int i,j;
		//switch buffers
		SwitchBuffers();
		//update the fluid with a blur
		for (j=2;j<m_DimY-2;j++)
		for (i=2;i<m_DimX-2;i++) {
			//int pos = i+j*m_DimX;
			float value = (
				READBUFFER(m_ReadBuffer,i-2,j) +
				READBUFFER(m_ReadBuffer,i+2,j) +
				READBUFFER(m_ReadBuffer,i,j-2) +
				READBUFFER(m_ReadBuffer,i,j+2) +
				READBUFFER(m_ReadBuffer,i-1,j) +
				READBUFFER(m_ReadBuffer,i+1,j) +
				READBUFFER(m_ReadBuffer,i,j-1) +
				READBUFFER(m_ReadBuffer,i,j+1) +
				READBUFFER(m_ReadBuffer,i-1,j-1) +
				READBUFFER(m_ReadBuffer,i+1,j-1) +
				READBUFFER(m_ReadBuffer,i-1,j+1) +
				READBUFFER(m_ReadBuffer,i+1,j+1));
			value /= 6.0f;		// Average * 2
			value -= (float)READBUFFER(m_WriteBuffer,i,j);
			//values for damping from 0.04 - 0.0001 are pretty good
			value -= (value * m_FluidDampingFactor);
			SETBUFFER(m_WriteBuffer,i,j,value);
		}
	}


palVector3* palDampendShallowFluid::GetFluidVertices() {
	float k = m_CellSize;
	m_VertexCount = 0;
	for (int j=0;j<m_DimY;j++)
		for (int i=0;i<m_DimX;i++) {
			vec_set(m_Vertices+m_VertexCount,(i-m_DimX*0.5f)*k,READBUFFER(m_ReadBuffer,i,j),(j-m_DimY*0.5f)*k);
			m_VertexCount++;
		}
	return m_Vertices;
}
int palDampendShallowFluid::GetNumVertices() {
	return m_VertexCount;
}

//#include <GL/gl.h>
void palDampendShallowFluid::UpdateInteraction(int step, float WaterDepth) {
		palCollisionDetection *pcd = dynamic_cast<palCollisionDetection *>( PF->GetActivePhysics());
		for (int j=0;j<m_DimY;j+=step)
			for (int i=0;i<m_DimX;i+=step) {
				palRayHit hit;
				float x = (i-m_DimX*0.5f)*m_CellSize;
				float z = (j-m_DimY*0.5f)*m_CellSize;
				//lets cast a ray up from the 'bottom' of the water
				pcd->RayCast(x,-WaterDepth,z,0,1,0,WaterDepth,hit);
				//did we hit an object, and the body?
				if ((hit.m_bHitPosition)&&(hit.m_pBody)) {
					//where is the water level right now?
					float water_height = READBUFFER(m_ReadBuffer,i,j);

					if (hit.m_vHitPosition.y>water_height) //we are above the water, so still falling, so break!
						continue;
					//if (water_height<0)
					//	water_height = 0;

					//who did we hit, and where?
					palVector3 bottomHit;
					bottomHit = hit.m_vHitPosition;
					palBody *pb = dynamic_cast<palBody *>(hit.m_pBody);
					if (!pb)  //we won't interact with static objects, right?
						continue;
					float y = bottomHit.y;

					bool immersed = false;
					//are we completely immersed?
					pcd->RayCast(x,0,z,0,-1,0,WaterDepth,hit);
					palVector3 topHit;
					//bool topFail = !hit.m_bHit;
					topHit = hit.m_vHitPosition;
					if (!hit.m_bHitPosition)
						topHit.y = water_height;
					else
						immersed = true;

#if 0
			//underwater body
			glColor3f(0,0.5,0);
			glBegin(GL_LINES);
			glVertex3f(x,-WaterDepth,z);
			glVertex3fv(bottomHit._vec);
			glEnd();


			glColor3f(0.5,0,0);
			glBegin(GL_LINES);
			glVertex3f(x,0,z);
			glVertex3fv(topHit._vec);
			glEnd();


			//water volume
			glColor3f(0,0,0.5);
			glBegin(GL_LINES);
			glVertex3f(x,READBUFFER(m_ReadBuffer,i,j),z);
			glVertex3fv(bottomHit._vec);
			glEnd();
#endif

					//where is the top of the displaced water? either the water height, OR , the top of the raycast
					float top = (topHit.y < water_height) ? topHit.y: water_height;
					//if there is little difference between top and bottom, it means the raycast has not hit the other side of the object
					if (fabs(topHit.y - bottomHit.y)<0.001) {
						top = water_height;
						immersed = false;
					}
					//the displaced water. top - bottom. (because were in -'ves)
					float disp = fabs(top - bottomHit.y);

					//volume of displaced water?
					float vDisplaced =  disp * m_CellSize * step * m_CellSize * step;
					float bouyancy = 9.8f * m_Density* vDisplaced; //whats the buoyancy force? (gpV)

					//lets apply the bouyancy force.
					pb->ApplyForceAtPosition(x,y,z,0,bouyancy,0);

					//lets dampen the system.
					palVector3 v;
					pb->GetLinearVelocity(v);
					vec_mul(&v,m_BodyDampingFactor_Linear);
					pb->ApplyImpulse(-v.x,-v.y,-v.z);

					//now lets update the water.
					//float cur_water = READBUFFER(m_WriteBuffer,i,j);
					//float lerp=0.5;
#define LERP(x,y,a) ((x)*a+(y)*(1-a))
				if (!immersed) {
					float displaced = 0.5f;

					SETBUFFER(m_WriteBuffer,i,j,READBUFFER(m_WriteBuffer,i,j)-displaced);

					SETBUFFER(m_ReadBuffer,i+1,j,READBUFFER(m_ReadBuffer,i+1,j)+displaced*0.25f);
					SETBUFFER(m_ReadBuffer,i-1,j,READBUFFER(m_ReadBuffer,i-1,j)+displaced*0.25f);

					SETBUFFER(m_ReadBuffer,i,j+1,READBUFFER(m_ReadBuffer,i,j+1)+displaced*0.25f);
					SETBUFFER(m_ReadBuffer,i,j-1,READBUFFER(m_ReadBuffer,i,j-1)+displaced*0.25f);
				}/*

				if (v.y>0) //are we already moving up? lets not over-emphasise this!
					SETBUFFER(m_WriteBuffer,i,j,LERP(cur_water,  y,lerp));
				else
					SETBUFFER(m_WriteBuffer,i,j,LERP(cur_water,0-y,lerp)); //okay, make a buldge where we go down.

				if (immersed) //flatten the fluid now! (we are totally under water)
					SETBUFFER(m_WriteBuffer,i,j,LERP(cur_water,  0,lerp));
*/
				pb->GetAngularVelocity(v);
				vec_mul(&v,m_BodyDampingFactor_Angular);
				//pb->SetAngularVelocity(v);
				pb->ApplyAngularImpulse(-v.x,-v.y,-v.z);

				}
			}
	}
	void palDampendShallowFluid::Update() {
		UpdateFluid();
		UpdateInteraction();
		m_Count++;
	}
