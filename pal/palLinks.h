#ifndef PALLINKS_H
#define PALLINKS_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/**	\file palLinks.h	
	\brief
		PAL - Physics Abstraction Layer. 
		Links
		
	\author
		Adrian Boeing
	\version
	<pre>
	Revision History:
		Version 0.3.24: 14/10/08 - Generic link frame matrix
		Version 0.3.23: 01/10/08 - Generic link enum and init
		Version 0.3.22: 11/07/08 - Get angle epislon bugfix, get revolute position
		Version 0.3.21: 10/07/08 - Get angle bugfix
		Version 0.3.2 : 11/08/04 - Revolute link, torque&velocity
		Version 0.3.1 : 28/07/04 - Doxygen
		Version 0.3   : 04/07/04 - Split from pal.h 
	</pre>
	\todo
*/
#include "palBodies.h"

/** The type of link
*/
typedef enum {
	PAL_LINK_NONE = 0,
	PAL_LINK_SPHERICAL = 1, //!< Spherical link, (ball&socket) 3d rotation
	PAL_LINK_REVOLUTE = 2, //!< Revolute link, (hinge) 1d rotation
	PAL_LINK_PRISMATIC = 3, //!< Prismatic link, (slider) 1d translation
	PAL_LINK_GENERIC = 4 //!< Generic 6DOF link
} palLinkType;

//corkscrew?
//universal ( 2d)

/** The base link class.
	Connects two bodies together via a given constraint.
	All links coordinates are specified in world space unless otherwise indicated.
	Although the direction of link connections does not matter for most physics engine implementaions, it is often optimal to specify connections steming from one central body out to all the ending body links.
*/
class palLink : public palFactoryObject {
public:
	//
	Float m_fPosX;
	Float m_fPosY;
	Float m_fPosZ;
	palLinkType m_Type;
	palBodyBase *m_pParent;
	palBodyBase *m_pChild;

//	virtual void SetTorque(Float tx, Float ty, Float tz);
//	virtual void GetTorque(palVector3& torque) = 0;

//	virtual void GenericInit(palBody *pb0, palBody *pb1, void *paramarray) = 0;
protected:
};

/** A Spherical link.
	A spherical link, (also know as a ball and socket link) provides 3 degrees of rotational freedom for the constraint. That is, it can twist about its axis, as well as rotate freely about its central point.
	The link connects two bodies, at a given position.
	<img src="../pictures/shericallink2.jpg">
	The diagram indicates the central point of the spherical link.
*/
class palSphericalLink: virtual public palLink {
public:
	/** Initializes the spherical link.
	\param parent The "parent" body to connect
	\param child The "child" body to connect
	\param x The x position of the link's center
	\param y The y position of the link's center
	\param z The z position of the link's center
	*/
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z);
	//link coordinates specified in WORLD coordinates
	//virtual void SetLimits(Float axis_x, Float axis_y, Float axis_z, Float limit_rad);

	//perhaps modify these to only support one 'range'?
	//and only one function. ala newton
	//api version 1:
	/** Constrains the movement of the spherical link.
	This limits the ammount of movement of the link.
	\param cone_limit_rad Limits the rotational movement of the joint. Specifies the maximum movement in radians.
	\param twist_limit_rad Limits the twist movement of the joint. Specifies the maximum movement in radians.
	????DIAGRAM
	*/
	virtual void SetLimits(Float cone_limit_rad, Float twist_limit_rad);

//	void GenericInit(palBody *pb0, palBody *pb1, void *paramarray); 
	Float m_fConeLimit; //!< The maximum limit of rotational movement (radians)
	Float m_fTwistLimit; //!< The maximum limit of twisting movement (radians)
/*	//api version 2:
//	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad); //radians
//	virtual void SetTwistLimits(Float lower_limit_rad, Float upper_limit_rad);
	//
	Float m_fLowerLimit;
	Float m_fUpperLimit;
	//
	Float m_fLowerTwistLimit;
	Float m_fUpperTwistLimit;*/
};

/** A Revolute Link
	A revolute link (also known as a hinge) provides one degree of rotational freedom for the constraint.
	The link connects two bodies, at a given position, and rotates around a specified axis.
	<img src="../pictures/hinge.jpg">
	The diagram illustrates two geometries central positions, and the central pivot point of the revolute link.
	The arrow in the diagram illustrates the axis about which the link can rotate.
*/
class palRevoluteLink: virtual public palLink {
public:
	/** Initializes the revolute link.
	\param parent The "parent" body to connect
	\param child The "child" body to connect
	\param x The x position of the link's center
	\param y The y position of the link's center
	\param z The z position of the link's center
	\param axis_x The vector about which the link rotates. (x)
	\param axis_y The vector about which the link rotates. (y)
	\param axis_z The vector about which the link rotates. (z)
	*/
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z);
	/** Constrains the movement of the revolute link.
	This limits the ammount of movement of the link.
	\param lower_limit_rad The lower angular limit of movement. (raidans)
	\param upper_limit_rad The upper angular limit of movement. (raidans)
	*/
	virtual void SetLimits(Float lower_limit_rad, Float upper_limit_rad); //radians

//	virtual void GenericInit(palBody *pb0, palBody *pb1, void *paramarray); 
	
	/** Retrieves the position of the link as a 3 dimensional vector.
	\param pos A three dimensional vector representing the links position
	*/
	virtual void GetPosition(palVector3& pos);

	/** Gets the current angle (in radians) between the two connected bodies.
	\return Angle (radians) between the two connected bodies.
	*/
	virtual Float GetAngle(); //current rotation angle 

	/** Gets the current angular velocity
	*/
	virtual Float GetAngularVelocity(); 
	
	/** Applies a torque to act on the link
	*/
	virtual void ApplyTorque(Float torque);
	
	/** Applies a torque to act on the link
	*/
	virtual void ApplyAngularImpulse(Float torque);

	Float m_fRelativePosX;
	Float m_fRelativePosY;
	Float m_fRelativePosZ;

	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;
	Float m_fLowerLimit;
	Float m_fUpperLimit;

	palVector3 m_pivotA;
	palVector3 m_pivotB;
	
	palMatrix4x4 m_frameA;
	palMatrix4x4 m_frameB;
};

/** A Prismatic Link
	A prismatic link (also known as a slider) provides one degree of translational freedom for the constraint.
	The link connects two bodies, at a given position, and extends along a specified axis.
	<img src="../pictures/prismatic.jpg">
	The diagram indicates the central point of two geometries.	The arrow indicates the axis about which the link extends. The point the arrow extends from indicates the starting position for the slider.
*/
class palPrismaticLink: virtual public palLink {
public:
	/** Initializes the primsatic link.
	\param parent The "parent" body to connect
	\param child The "child" body to connect
	\param x The x position of the link's center
	\param y The y position of the link's center
	\param z The z position of the link's center
	\param axis_x The axis vector which the link exteneds from. (x)
	\param axis_y The axis vector which the link exteneds from. (y)
	\param axis_z The axis vector which the link exteneds from. (z)
	*/
	virtual void Init(palBodyBase *parent, palBodyBase *child, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z); //axis is direction of sliding

//	void GenericInit(palBody *pb0, palBody *pb1, void *paramarray); 
	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;
};


class palGenericLink  : virtual public palLink {
public:
	virtual void Init(palBodyBase *parent, palBodyBase *child, palMatrix4x4& parentFrame, palMatrix4x4& childFrame,
		palVector3 linearLowerLimits,
		palVector3 linearUpperLimits,
		palVector3 angularLowerLimits,
		palVector3 angularUpperLimits);

	palMatrix4x4 m_frameA;
	palMatrix4x4 m_frameB;
//	void GenericInit(palBodyBase *pb0, palBodyBase *pb1, void *paramarray) {;};
};
#endif
