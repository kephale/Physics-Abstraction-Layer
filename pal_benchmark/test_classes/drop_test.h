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
template <typename T = PALTest> class PAL_Drop_Test : public T 
{
protected:

	virtual void doInnerUpdateLoop() { ; }
	
	int doCreatePhysics()
	{
		pp = PF->CreatePhysics();
		if (!pp) {
			MessageBox(NULL,"Could not start physics!", "Error", MB_OK);
			return -1;
		}
		palPhysicsDesc desc; // -9.8f gravity, remember to set 
		pp->Init(desc); //initialize it, set the main gravity vector

		ps = PF->CreateSphere();
		if (!ps) {
			MessageBox(NULL,"Could not make sphere!","Error",MB_OK);
			return -1;
		}
		ps->Init(0,0,0,1,1);
		BuildGraphics(ps);		
		return 0;
	}

	virtual void SaveData() 
	{
		palVector3 p;
		ps->GetPosition(p);
		data.push_back(p.y);
	}

public:
	palSphere *ps;
	VECTOR<float> data;
};