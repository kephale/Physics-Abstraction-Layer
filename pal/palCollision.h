#ifndef PALCOLLISION_H
#define PALCOLLISION_H
//(c) Adrian Boeing 2008, see liscence.txt (BSD liscence)
/** \file palCollision.h
	\brief
		PAL - Physics Abstraction Layer.
		Collision Detection Subsystem
	\author
		Adrian Boeing
	\version
	<pre>
	Revision History:
		Version 0.0.21:05/09/08 - Doxygen support
		Version 0.0.2: 05/07/08 - Collision design implementation pass
		Version 0.0.1: 26/05/08 - Collision planning
*/
#include <ostream>
#include "palBase.h"
#include "palBodyBase.h"

/** The contact point
This describes the information available from a single contact point.
This information is engine-specific.
The contact position may represent the closest point between bodies, and need not be located on either bodies geometry.
Consult the engine-specific documentation for more information.
*/
class palContactPoint {
public:
	palContactPoint();
	palBodyBase *m_pBody1; //!< A body involved in the collision
	palBodyBase *m_pBody2; //!< Another body involved in the collision
	palVector3 m_vContactPosition; //!< The contact position.
	palVector3 m_vContactNormal; //!< The contact normal
	Float m_fDistance; //!< The distance between closest points. Negative distance indicates interpenetrations
	Float m_fImpulse; //!< The impulse magnitude used to resolve the constraints on the bodies along the normal.
	palVector3 m_vImpulseLateral1; //!< Impulse vector applied to the body based on friction in the direction of travel along the second body.
	palVector3 m_vImpulseLateral2; //!< Impulse vector applied to the body based on friction in the direction normal to both the contact normal and the laterl1 vector

    friend std::ostream& operator<<(std::ostream &os, const palContactPoint& cp);
};

/** A contact
A contact represents the collection of contact positions where two objects are in contact.
The number of contact points generated in a collision is engine-specific.
You can not assume a fixed number of returned contact points.
*/
class palContact {
public:
	palContact();
	PAL_VECTOR<palContactPoint> m_ContactPoints; //!< A vector of Contact Points
};

/** The ray hit information.
The ray hit contains the information from the result of a ray casting operation.
This includes the body and geometry that terminated the raycast, as well as the position and normal of the hit location, and the distance from the origin of the initial ray cast operation.
Each engine may not support the full set of raycast information, so each variable should be checked for validity.
This can be done via the m_bHit, m_bHitPosition, and m_bHitNormal flags.
Example:
<pre>
palRayHit *prh;
if (prh->m_bHit)
	prh->m_vHitPosition.x //do a calculation
</pre>
Raycasting allows the implementation of AI functionality and the simulation of distance sensors (eg: PSD or Sonar)
*/
class palRayHit {
public:
	palRayHit();
	void Clear();
	void SetHitPosition(Float x, Float y, Float z);
	void SetHitNormal(Float x, Float y, Float z);
	bool m_bHit; //!< The ray succesfully hit an object
	bool m_bHitPosition; //!< The ray hit position is available
	bool m_bHitNormal; //!< The ray hit normal is available
	palBodyBase *m_pBody;  //!< The body that was hit (if applicable)
	palGeometry *m_pGeom;  //!< The geometry that was hit (if applicable)
	palVector3 m_vHitPosition; //!< The world position where the ray hit location (if applicable)
	palVector3 m_vHitNormal;   //!< The surface normal at the point where the ray hit (if applicable)
	Float m_fDistance; //!< The distance between the ray origin and hit position
};


/** Raycasting callback.
 * This will be called for each ray hit.
 */
class palRayHitCallback {
public:
   palRayHitCallback();

   /** Notifies the code of a hit.
    * @param hit the information about the hit.
    * @return the max ray length that should accepted and passed to add hit.
    *         This means if you wanted just the closest hit, you could always pass the fraction of
    *         of the hit passed in.  This could also be used to ignore certain physics objects and get
    *         the closest not counting those certain ones.  If you want to stop, return 0.  If you want
    *         all hits, return the max range.
    */
   virtual Float AddHit(palRayHit& hit) = 0;
};

/** The collision detection subsystem.
The collision detection sub system is the portion of the main physics engine responsible for determining whether and where objects are colliding.
It enables querying the geometry of the world, for example ray casting.
If you wish PAL to keep track of collision that occur you must call the 'Notify' functions for each body.
Colliding objects can be set to be in different collision groups to allow non-interacting collisions.
Not all engines support a seperate collision detection query system, so collision support for a physics engine must be tested before use.
Example:
<pre>
	palPhysics*pp = PF->GetActivePhysics();
	palCollisionDetection *pcd = dynamic_cast<palCollisionDetection *>(pp);
	if (!pcd)
		//error.
</pre>
*/
class palCollisionDetection {
public:

	palCollisionDetection();
	/**	Sets the accuracy of the collision detection system
	\param fAccuracy Ranges from 0..1, 0 indicates fast and inaccurate, 1 indicates accurate and slow.
	*/
	virtual void SetCollisionAccuracy(Float fAccuracy) = 0;//0 - fast, 1 - accurate

	/**	Sets the interactions between collision groups to enabled/disabled
	*/
	virtual void SetGroupCollision(palGroup a, palGroup b, bool enabled) = 0;

	/** Queries the collision system for a ray interesection. Requires the origin and heading of the ray.

	If a ray has hit, the palRayHit is filled with the body that has been hit, and if available, the geometry that was hit.
	\param x The position (x) of the ray
	\param y The position (y) of the ray
	\param z The position (z) of the ray
	\param dx The direction vector (x) of the ray
	\param dy The direction vector (y) of the ray
	\param dz The direction vector (z) of the ray
	\param range The maximum range to test
	\param hit The ray hit information
	*/
	virtual void RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range, palRayHit& hit) = 0;

	/** Enables listening for a collision between two bodies.
	\param a The first body
	\param b The second body
	\param enabled Enable/disable listening for a collision
	*/
	virtual void NotifyCollision(palBodyBase *a, palBodyBase *b, bool enabled) = 0;

	/**	Enables listening for a collision involving a body.
	\param pBody The body which listens for all collisions
	\param enabled Enable/disable listening for a collision
	*/
	virtual void NotifyCollision(palBodyBase *pBody, bool enabled) = 0;

	/** Returns the contact points.
	A collision notification must be set up before any contact points can be returned.
	*/
	virtual void GetContacts(palBodyBase *pBody, palContact& contact) = 0;

	/** Returns the contact points.�
	A collision notification must be set up before any contact points can be returned.
	*/
	virtual void GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact) = 0;
};

class palCollisionDetectionExtended: public palCollisionDetection {
public:
	palCollisionDetectionExtended();
	/**
	 * An extended raycast that will callback with every hit.
	 * @param x starting pos x
	 * @param y starting pos y
	 * @param z starting pos z
	 * @param dx normalized direction vector x
	 * @param dy normalized direction vector y
	 * @param dz normalized direction vector z
	 * @param range Length of the ray.
	 * @param callback The call back.
	 * @param groups An optional set of groups to use as a filter.
	 */
	virtual void RayCast(Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range,
	         palRayHitCallback& callback, palGroupFlags groupFilter = ~0) = 0;
};
#endif

