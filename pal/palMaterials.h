#ifndef PALMATERIALS_H
#define PALMATERIALS_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)

/** The Material definition structure.
   The use of the friction information is dependent on the phyisics implementation used.
   Friction is represented by the Coulomb model of friction.
   Static friction represents the ammount of friction in a material when an object first begins to move.
   Kinetic friction is the friction of a material for an object already in motion.
   Resitution is used to represent an elastic collision (ie: bounce), it is the height an object will bounce when droped onto a material.

   \f[ c = \frac{s_2-v_2}{v_1-s_1} \f]
   Where,
   c = coefficient of restitution
   \f$v_1\f$ = linear velocity of bodyA mass center before impact
   \f$s_1\f$ = linear velocity of bodyB before impact (will be negative according to our convention that away from the player is positive)
   \f$v_2\f$ = linear velocity of bodyA mass center after impact
   \f$s_2\f$ = linear velocity of bodyB ball after impact

   or, in the case of a falling object bouncing off the ground:
   \f[ c = \sqrt{\frac{h}{H} } \f]
   Where,
   h = bounce height
   H = drop height
   */
struct palMaterialDesc {
   palMaterialDesc();
   Float m_fStatic; //!< Static friction coefficient, defaults to 0.0
   Float m_fKinetic; //!< Kinetic friction coefficient for isotropic or inline with the direction of anisotropy, defaults to 0.0
   Float m_fRestitution; //!< Restitution coefficient defaults to 0.5
   palVector3 m_vStaticAnisotropic; //!< Anistropic friction vector 0-1 based on the friction coefficient.  Defaults to 1,1,1.
   palVector3 m_vKineticAnisotropic; //!< Anistropic friction vector 0-1 based on the friction coefficient. Defaults to 1,1,1.
   palVector3 m_vDirAnisotropy; //!< A direction vector defining the main direction of anisotropy, which allows rotating the anistrophic vector. Defaults 1,0,0.
   bool m_bEnableAnisotropicFriction; //!< defaults to false;
   bool m_bDisableStrongFriction; //!<Some engines accumulate left over friction across frames, this will disable it if the engine supports it. Defaults to false.
};

/*! \file palBase.h
	\brief
		PAL - Physics Abstraction Layer. 
		Materials
	\author
		Adrian Boeing
	\version
	<pre>
		Version 0.1   : 11/12/07 - Original
	</pre>
	\todo
*/

/** This is the base material class.
	This class is only neccessary when constructing a new PAL physics implementation.
	To obtain a pointer to a Material you need to access it via palMaterials::GetMaterial().
*/
class palMaterial : public palFactoryObject, public palMaterialDesc {
public:
	/*
	 Sets the member variables.
	*/
	virtual void SetParameters(const palMaterialDesc& matDesc);
};


/** This class represents unique material interactions (eg: wood/wood). 
	This class is only used internally by palMaterials, and should not be manually created.
 */
class palMaterialUnique : virtual public palMaterial {
public:
	palMaterialUnique();
	//api version 1 have only static_friction (maybe restitution depending on)
	//virtual void Init(Float static_friction);
	//version two:
	/*
	Initializes the material
	\param name The materials name
	\param desc the material description
	*/
   virtual void Init(PAL_STRING name, const palMaterialDesc& desc); //api version 3

	PAL_STRING m_Name;//!< The name for this material. (eg:"wood")	
protected:
	FACTORY_CLASS(palMaterialUnique,palMaterialUnique,*,1);
};

/** This class represents two material interactions (eg: wood/metal).  
	This class is only used internally by palMaterials, and should not be manually created 
 */
class palMaterialInteraction : virtual public palMaterial {
public:
	palMaterialInteraction();
	/*
	Initializes the material
	\param pM1 a pointer to a unique material
	\param pM2 a pointer to a unique material
   \param desc the material description
	*/
	virtual void Init(palMaterialUnique *pM1, palMaterialUnique *pM2, const palMaterialDesc& matDesc); //api version 2
	palMaterial *m_pMaterial1;	//!< Pointers to the unique materials which interact
	palMaterial *m_pMaterial2;	//!< Pointers to the unique materials which interact
protected:
	FACTORY_CLASS(palMaterialInteraction,palMaterialInteraction,*,1);
};


/** The materials management class.
	This class allows you to add materials into the physics engine. The class maintains a library of all materials created and generates the appropriate underlying data structures. 
	A Material can be extracted from the Materials library using the GetMaterial() function.

	<br>
	Friction is represented by the Coulomb model of friction.
	Static friction represents the ammount of friction in a material when an object first begins to move.
	Kinetic friction is the friction of a material for an object already in motion.
	Resitution (elasticity) is used to represent an elastic collision (ie: bounce), it is the height an object will bounce when droped onto a material.

	\f[ c = \frac{s_2-v_2}{v_1-s_1} \f]
	Where,
	c = coefficient of restitution
	\f$v_1\f$ = linear velocity of bodyA mass center before impact
	\f$s_1\f$ = linear velocity of bodyB before impact (will be negative according to our convention that away from the player is positive)
	\f$v_2\f$ = linear velocity of bodyA mass center after impact
	\f$s_2\f$ = linear velocity of bodyB ball after impact

	or, in the case of a falling object bouncing off the ground:
	\f[ c = \sqrt{\frac{h}{H} } \f]
	Where,
	h = bounce height
	H = drop height

	<br>
	Developer Notes:
	The materials management class employes the palMaterialInteraction and palMaterialUnique classes behind the scenes. 
	You should only need to implement the aforementioned classes. 
*/

#endif
