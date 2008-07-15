#ifndef PALFACTORY_H
#define PALFACTORY_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
#include "pal.h"

/** \file palFactory.h
	\brief
		PAL Factory -	Physics Abstraction Layer. 
						The factory required to create all objects
	
	\author
		Adrian Boeing
	\version
	<pre>
	Revision History:
		Version 0.2.11: 15/07/08 - Convex object create method
		Version 0.2.1 : 05/07/08 - Notification added.
		Version 0.2.01: 13/12/07 - lib fix
		Version 0.2   : 06/12/07 - DLL singleton cleanup
		Version 0.1.33: 28/06/07 - DLL support
		Version 0.1.32: 28/08/04 - Diagrams
		Version 0.1.31: 07/08/04 - Materials fix
		Version 0.1.3 : 04/08/04 - Internal changes (parenting)
		Version 0.1.2 : 27/07/04 - Doxygen documentation
		Version 0.1.1 : 25/07/04 - Physics state management
		Version 0.1.0 : 05/07/04 - Compound body (geom), velocimeter
		Version 0.0.99: 28/06/04 - Gyroscope create
		Version 0.0.98: 24/06/04 - Singleton, contact sensor create
		Version 0.0.95: 12/06/04 - safe memory cleanup
		Version 0.0.9 : 11/04/04 - terrain functions, prismatic link, and generic 'create object'
		Version 0.0.8 : 06/04/04 
	</pre>
	\todo
*/

#define PF palFactory::GetInstance()

/**	The PAL factory class.
	This singelton class is responsible for the construction, and removal of all objects in PAL.

	The factory allows you to select any existing physics implementation system at runtime, and create whichever objects you require.
	Custom objects and extended implementations are automatically imported by the factory.
*/
#ifndef INTERNAL_DEBUG
//class palFactory : private myFactory {
class palFactory : public myFactory {
#else
class palFactory : public myFactory {
#endif
public:
	palFactory();

	/**	Selects the underlying physics engine to be used when construction objects. (eg: ODE).
	This function must be called before any objects are created.
	
	If the call was succesfull then subsequent create calls should succeed, else, they will return null.

	\param name The name of the physics engine to be used
	*/
	void SelectEngine(PAL_STRING name); 

	/**
	Removes all the objects created - regardless of which engine they were constructed with.
	*/
	void Cleanup();
	//
	// i might remove this
	palMaterials *CreateMaterials();
	/** Creates the physics class.
	This should be created and initialized before any other objects are created for the current physics engine
	\return A newly constructed physics class, specified by the select method
	*/
	palPhysics *CreatePhysics();
	//
	/** Creates a static ground plane
	\return A newly constructed terrain plane class, specified by the select method
	*/
	palTerrainPlane *CreateTerrainPlane();
	/** Creates a static ground height map
	\return A newly constructed terrain heightmap class, specified by the select method
	*/
	palTerrainHeightmap *CreateTerrainHeightmap();
	/** Creates a static environment
	\return A newly constructed terrain mesh class, specified by the select method
	*/
	palTerrainMesh *CreateTerrainMesh();
	//
	/** Creates a box.
	<img src="../pictures/cube.jpg" alt="box">
	\return A newly constructed box class, specified by the select method
	*/
	palBox *CreateBox();
	/** Creates a sphere.
	<img src="../pictures/sphere.jpg" alt="sphere">
	\return A newly constructed sphere class, specified by the select method
	*/
	palSphere *CreateSphere();
	/** Creates a convex object.
	\return A newly constructed convex object class, specified by the select method
	*/
	palConvex *CreateConvex();
	/** Creates a capped cylinder.
	<img src="../pictures/capsule.jpg" alt="cylinder">
	\return A newly constructed capped cylinder class, specified by the select method
	*/
	palCapsule *CreateCapsule();
	/** Creates a compound body.
	<img src="../pictures/compoundbody.jpg" alt="compound">
	\return A newly constructed compound body class, specified by the select method
	*/
	palCompoundBody *CreateCompoundBody();
	//
	/** Creates a spherical link
	A spherical link has three degress of freedom. It is also known as a ball-and-socket joint. (example: hip-leg joint)
	<img src="../pictures/shericallink2.jpg">
	\return A newly constructed spherical link class, specified by the select method
	*/
	palSphericalLink *CreateSphericalLink();
	/** Creates a revolute link
	A revolute link has one degree of rotational freedom. It is also know as a hinge joint. (example: door)
	<img src="../pictures/hinge.jpg">
	\return A newly constructed revolute link class, specified by the select method
	*/
	palRevoluteLink	*CreateRevoluteLink();
	/** Creates a prismatic link
	A prismatic link has one degree of translational freedom. It is also know as a slider joint. (example: slide rule, hydrolic ram)
	<img src="../pictures/prismatic.jpg">
	\return A newly constructed prismatic link class, specified by the select method
	*/
	palPrismaticLink *CreatePrismaticLink();
	//
	/** Creates a PSD sensor
	This sensor tells you the distance from one object to another. This is also called raycasting.
	<img src="../pictures/psdsensor.jpg">
	\return A newly constructed PSD sensor class, specified by the select method
	*/
	palPSDSensor *CreatePSDSensor();

	/** Creates a contact sensor
	This sensor tells you whether the object has collided with another object. This is sometimes called collision detection/querying.
	<img src="../pictures/contact.jpg">
	\return A newly constructed contact sensor class, specified by the select method
	*/
	palContactSensor *CreateContactSensor();

	/** Creates an inclinometer sensor
	This sensor tells you the angle between its starting orientation, and the bodies current orientation. (Angle sensor)
	\return A newly constructed inclinometer sensor class.
	*/
	palInclinometerSensor *CreateInclinometerSensor();

	/** Creates a compass sensor
	This sensor tells you the angle between its starting orientation, and the bodies current orientation. (Angle sensor)
	\return A newly constructed compass sensor class.
	*/
	palCompassSensor *CreateCompassSensor();

	//remove this sensor to seperate DLL?
	/** Creates an gyroscope sensor
	This sensor tells you the change in the angular velocity of a body.
	\return A newly constructed gyroscope sensor class.
	*/
	palGyroscopeSensor *CreateGyroscopeSensor();

	//remove this sensor to seperate DLL?
	palVelocimeterSensor *CreateVelocimeterSensor();

	//remove this sensor to seperate DLL?
	palGPSSensor *CreateGPSSensor();
	//
	//low-level creations, standard user shouldn't use these:
	/** Creates any PAL object
	This will return the most suitable class that matches the currently selected engine, and name. This function can be used to construct objects which are not part of the standard PAL implementation. (eg: custom plug-ins)
	\return A newly constructed PAL object
	*/
	palFactoryObject *CreateObject(PAL_STRING name); //this is only to be used for user add-on functionality

	palPhysics *GetActivePhysics();
	void SetActivePhysics(palPhysics *physics);
	void LoadPALfromDLL(char *szPath = NULL);
private:
	palPhysics *m_active;
public:
	static palFactory *GetInstance();
	static void SetInstance(palFactory *pf);
};

#endif
