#ifndef PAL_H
#define PAL_H

// don't include this file if using scons to build (it is specific to CMake)
#ifndef SCONS_BUILD
#include <pal/Config.h>
#endif

//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*! \file pal.h
	\brief
		PAL - Physics Abstraction Layer.

	\author
		Adrian Boeing
	\version
	<pre>
	Revision History:
		Version 0.4.01: 28/02/08 - Physics get gravity, additional init for palOrientatedPlane.
		Version 0.4   : 30/09/08 - PAL Versioning
		Version 0.3.16: 26/05/08 - Collision groups
		Version 0.3.15: 06/12/07 - Update for GCC 4 compatibility
		Version 0.3.14: 19/10/07 - Version and Timestep query
		Version 0.3.13: 28/06/07 - PAL DLL support
		Version 0.3.12: 06/12/04 - GetTime
		Version 0.3.11: 28/07/04 - Doxygen documentation update
		Version 0.3.1 : 27/07/04 - Doxygen documentation
		Version 0.3   : 04/07/04 - Split pal header
		Version 0.2.11: 28/06/04 - Set position with rotation, psd sensor distance
		Version 0.2.1 : 25/06/04 - Generic intialization
		Version 0.2.0 : 24/06/04 - Sensors: psd, gyroscope, inclinometer, started contact, material controller, modified spherical limits, added get veloctiy body functions
		Version 0.1.2 : 23/06/04 - Experimental branch, multiple versioned API
		Version 0.1.1 : 21/06/04 - Started limits (revolute, spherical)
		Version 0.1.0 : 11/06/04 - Redesigned terrain abstractions, added terrain, plane, heightmap, terrain mesh, prismatic link, and body mesh
		Version 0.0.9 : 03/06/04
		Version 0.0.8 : 21/05/04
	</pre>
	\todo
		- performance tweeking (double,float,accuracy level specifications)
		- possibly include multiple simulator worlds? not sure about this.
		- possibly include support for different centers of masses?
		- option for 'safe' and 'unsafe' - makes copies of the terrain meshes,etc.
*/

#include "palBase.h"
#include "palDebugDraw.h"
#include "palMaterials.h"

//forward decl
class palGeometry;
class palBodyBase;
class palCollisionDetection;
class palSolver;
class palAction;

typedef enum {
	PAL_X_AXIS = 0,
	PAL_Y_AXIS = 1,
	PAL_Z_AXIS = 2,
	PAL_AXIS_COUNT = 3
} palAxis;

struct palPhysicsDesc {
	static const FLOAT DEFAULT_GRAVITY_X;
	static const FLOAT DEFAULT_GRAVITY_Y; //!< Standard gravity, according to NIST Special Publication 330, p. 39
	static const FLOAT DEFAULT_GRAVITY_Z;

	palPhysicsDesc();

	palVector3 m_vGravity;
	palAxis m_nUpAxis;
	PAL_MAP<PAL_STRING, PAL_STRING> m_Properties;
};

/** The main physics class.
	This class controls the underlying physics engine.

	NOTE: The current version of PAL does not allow multiple instances of physics, for the same underlying physics engine.- 27/07/04
*/
class palPhysics : public palFactoryObject {
	friend class palFactory;
public:
	palPhysics();
	/**
	Initializes the physics engine.
	\param desc The description object defining the settings for this physics instance.
	*/
	virtual void Init(const palPhysicsDesc& desc);
	/**
	This advances the physics simulation by the specified ammount of time.
	The best usage of this parameter is determined by the physics engine implementation. Consult the implementation documentation, or the physics engine documentation.
	The safest method is to treat the timestep as a constant.
	\param timestep The increment in time since the last update
	*/
	virtual void Update(Float timestep);
	/**
	This removes all objects and frees all memory used by the physics engine. Any further uses of existing phyiscics object references will have an undefined effect.
	*/
	virtual void Cleanup() = 0;

	/**
	This returns the PAL version information.
	*/
	virtual const char* GetPALVersion() const = 0;

	/**
	This returns the physics engine name and physics engine version.
	*/
	virtual const char* GetVersion() const = 0;
	/**
	Returns the current simulation time
	*/
	virtual Float GetTime() const;

	/**
	Returns the last timestep
	*/
	virtual Float GetLastTimestep() const;

	/**
	Sets the interactions between collision groups to enabled/disabled
	*/
	virtual void SetGroupCollision(palGroup a, palGroup b, bool enabled);

	/**
	Returns the current direction of gravity.
	*/
	virtual void GetGravity(palVector3 &g) const;

	//virtual void Finalize(); //for dynamechs. possibly others
	virtual void SetFactoryInstance(palFactory *pfInstance = 0); //for dll usage

	/// Return the index, i.e. x (0), y (1), or z(2), to use for up.
	palAxis GetUpAxis() const { return m_nUpAxis; }

	/**
	 * Adds a new action to the physics system
	 * @see palAction
	 */
	virtual void AddAction(palAction *action);
	/// Removes an action from the physics system
	virtual void RemoveAction(palAction *action);

	/// Assigns the debug draw instance.
	virtual void SetDebugDraw(palDebugDraw* debugDraw);
	/// @return the debug draw instance.
	virtual palDebugDraw* GetDebugDraw();

	virtual palCollisionDetection* asCollisionDetection() { return 0; }
protected:
	bool m_bListen; //!< If set to true, notify functions are called.
	palMaterials *m_pMaterials;
	virtual void Iterate(Float timestep) = 0;

	///Call all actions.
	virtual void CallActions(Float timestep);
	Float m_fGravityX; //!< The gravity vector (x)
	Float m_fGravityY; //!< The gravity vector (y)
	Float m_fGravityZ; //!< The gravity vector (z)
	Float m_fLastTimestep;
	Float m_fTime; //dodgy?
	palAxis m_nUpAxis;
	PAL_MAP<PAL_STRING, PAL_STRING> m_Properties;

	virtual void NotifyGeometryAdded(palGeometry *pGeom);
	virtual void NotifyBodyAdded(palBodyBase *pBody);
//	PAL_LIST<palGeometry*> m_Geometries;//!< Internal list of all geometries
//	PAL_LIST<palBodyBase*> m_Bodies;//!< Internal list of all bodies
//	palMaterial *m_pDefaultMaterial;
//	palCollisionDetection *m_pCollision;
//	palSolver *m_pSolver;

	PAL_LIST<palAction*> m_Actions;
private:
	palDebugDraw *m_pDebugDraw;
	palPhysics(const palPhysics& pp) {}
	palPhysics& operator=(palPhysics& pp) { return *this; }
};

/*!
 * Actions are custom objects that can be added to the physics system and they will be called during each physics step.
 * The default behavior is to call these once each time palPhysics::Update is called, but some engines support calling this every
 * substep.  In either case, the correct time step will be passed in.
 *
 * @note Unless you are certain the engine you are using will only call this once per call to Update(), you should
 *       not call AddForce because the force could continue to be applied for multiple time steps, giving undesired
 *       results.  Instead, you should call ApplyImpulse giving the force you want multiplied by the timestep.
 */
class palAction {
public:
	virtual ~palAction() {}
	virtual void operator()(Float timeStep) = 0;
};

#include "palBodyBase.h"
#include "palBodies.h"
#include "palLinks.h"
#include "palStatic.h"
#include "palTerrain.h"
#include "palSensors.h"

#include "palActuators.h"

/*! \mainpage pal - Physics Abstraction Layer
	Your best "pal" when push comes to shove....

	\section intro_sec Introduction
	PAL is a C++ physics abstraction system.
	This allows you to rapidly develop applications that support various physics engines.

	\subsection intro_goal_sec Goal:
	Provide a single, flexible API to access a variety of physics engines.

	\subsection intro_imp_sec Available Implementations:
	PAL is available for: (alphabetical listing)
		- AGEIA PhysX http://www.ageia.com/ [* - same as NovodeX]
		- Box2D http://www.box2d.org/ [* - experimental implementation]
		- Bullet http://www.continuousphysics.com/Bullet/
		- Dynamechs http://dynamechs.sourceforge.net/ [* - implementation permanently disabled]
		- Havok http://www.havok.com/
		- IBDS http://www.impulse-based.de/ [* - experimental implementation]
		- Jiggle http://www.rowlhouse.co.uk/jiglib/
		- Meqon http://www.meqon.com/ [* - implementation permanently disabled]
		- Newton http://www.physicsengine.com/
		- NovodeX http://www.novodex.com/ [* - same as AGEIA,nVidia]
		- nVidia PhysX [* - same as NovodeX]
		- ODE http://www.ode.org/
		- OpenTissue http://www.opentissue.org/ [* - experimental implementation]
		- Simple Physics Engine http://spehome.com/
		- Tokamak http://www.tokamakphysics.com/
		- True Axis http://trueaxis.com/


	\section install_sec Installation
	- Install framework
	- Install PAL

	If you wish to use all of PAL's extended functionality:
	- Install tinyXML
	- Install libSDL
	- Install boost

	\subsection install_requirements_sec Requirements
	The following compilers are currently supported:

	Microsoft:
		- Microsoft Visual C++ 6.0
		- Microsoft Visual C++ 7.0, 7.1 (.NET)
		- Microsoft Visual C++ 8.0

	GNU
		- GCC 3.2 and above

	\section pal_info_sec PAL Information
	- Coordinates used for any functions are specified in world coordinates.
	- Angular measurements are in radians.

	\section example_sec Example
	Just starting?

	Try the simple examples. Then take a look at palFactory and follow the documentation links!
	\subsection example_1_sec Tiny example of using PAL directly
	- Make sure all the PAL & framework files are correctly included in your project

	\code
	PF->SelectEngine(szEngine); //szEngine is the name of the engine you wish to use. eg:"ODE"
	palPhysics *pp = PF->CreatePhysics(); //create the main physics class
	pp->Init(0,-9.8f,0); //initialize it, set the main gravity vector
	palTerrainPlane *pt= PF->CreateTerrainPlane(); //create the ground
	pt->Init(0,0,0,50.0f); //initialize it, set its location to 0,0,0 and minimum size to 50
	palBox *pb = PF->CreateBox(); //create a box
	pb->Init(0,5,0, 1,1,1, 1); //initialize it, set its location to 0,5,0 (five units up in the air), set dimensions to 1x1x1 and its mass to 1
	while (!quit) { //loop until done
		pp->Update(0.02f); //update the physics engine. advance the simulation time by 0.02

  		palMatrix4x4 m;
		m=pb->GetLocationMatrix(); //get the location of the box (transformation matrix)
		//display the box
	}

  	PF->Cleanup(); //we are done with the physics. clean up.
	\endcode

	\subsection example_2_sec Example of using PAL via XML

	NOTE:
	** PAL XML is now unsupported, try PAL COLLADA and PAL Scythe instead **

	- Make sure all the PAL & framework files are correctly included in your project
	- Make sure tinyXML and the xml files are correctly included in your project

	\code
	palXMLFactory px;
	px.LoadXML(szFilename); //eg:"physics.xml"
	palPhysics *pp = palGetObject<palPhysics>("Physics"); //get the physics engine described in the XML file
	palBox *pb = palGetObject<palBox>("box0"); //get the box described in the XML file - all objects can also be accessed via a public vector array in the XML factory
  	while (!quit) { //loop until done
		pp->Update(0.02f); //update the physics engine. advance the simulation time by 0.02

  		palMatrix4x4 m;
		m=pb->GetLocationMatrix(); //get the location of the box (transformation matrix)
		//display the box
	}

	 PF->Cleanup(); //we are done with the physics. clean up.
	\endcode
	\section credits_sec Credits
	PAL created by Adrian Boeing.

	Diagrams created by Martin Sawtell.

	Some code based from ODE and MESA libraries.
*/

#endif
