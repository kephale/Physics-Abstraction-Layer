//(c) Adrian Boeing 2008, see liscence.txt (BSD liscence)
/*! \file palCollision.h
	\brief
		PAL - Physics Abstraction Layer. 
		Collision Detection Subsystem
	\author
		Adrian Boeing
	\version
	<pre>
	Revision History:
		Version 0.0.2: 05/07/08 - Collision design implementation pass
		Version 0.0.1: 26/05/08 - Collision planning
*/
#include "palBase.h"
#include "palBodyBase.h"

class palContactPoint {
public:
	palContactPoint();
	palBodyBase *m_pBody1; //!< A body involved in the collision
	palBodyBase *m_pBody2; //!< Another body involved in the collision
	palVector3 m_vContactPosition; //!< The contact position
	palVector3 m_vContactNormal; //!< The contact normal
	Float m_fDistance; //!< The distance between closest points. Negative distance indicates interpenetrations
};

class palContact {
public:
	palContact();
	VECTOR<palContactPoint> m_ContactPoints;
};

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
	If 

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

	/** Returns the contact points.
	A collision notification must be set up before any contact points can be returned.
	*/
	virtual void GetContacts(palBodyBase *a, palBodyBase *b, palContact& contact) = 0;
};