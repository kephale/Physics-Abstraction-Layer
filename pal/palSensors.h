#ifndef PALSENSORS_H
#define PALSENSORS_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/*! \file palSensors.h
	\brief
		PAL - Physics Abstraction Layer. 
		Sensors
		
	\author
		Adrian Boeing
	\version
	<pre>
	Revision History:
		Version 0.4.2 : 10/10/06 - Added transponder
		Version 0.4.1 : 08/10/06 - Fixed compass bug, redefined compass inputs
		Version 0.4	  : 19/09/06 - Revisision from lost verion, added GPS and Compass
		Version 0.3.21: 20/12/04 - Documentation update, bugfixes (vel,inc,gyro)
		Version 0.3.2 : 28/07/04 - Doxygen
		Version 0.3.1 : 05/07/04 - Velocimeter
		Version 0.3   : 04/07/04 - Split from pal.h 
	</pre>
	\todo
		- accelerometer
		- improve contact sensor to return contact points (or subset)
*/

#include "pal.h"

/** The type of sensor
*/
typedef enum {
	PAL_SENSOR_NONE = 0, //!< No/undefined sensor
	PAL_SENSOR_INCLINOMETER = 1, //!< Inclinometer (angle sensor)
	PAL_SENSOR_GYROSCOPE = 2, //!< Gyrsocope (angular velocity sensor)
	PAL_SENSOR_ACCELEROMETER = 3, //!< Accelerometer (linear acceleration sensor)
	PAL_SENSOR_PSD = 4, //!< Positional Sensative Device (raycast distance sensor)
	PAL_SENSOR_CONTACT = 5, //!< Contact Sensor (collision detection query sensor)
	PAL_SENSOR_VELOCIMETER = 6, //!< Velocimeter (linear velocity sensor)
	PAL_SENSOR_GPS = 7, //!< GPS (global positioning sensor)
	PAL_SENSOR_COMPASS = 8, //!<< Compass (angle sensor)
	PAL_SENSOR_TRANSPONDER_SENDER = 9, //!<< Transponder sender (distance sensor)
	PAL_SENSOR_TRANSPONDER_RECIEVER = 10 //!<< Transponder reciever (distance sensor)
} palSensorType;


/** The base sensor class.
	Every sensor is attached to a body.
	All coordinates are specified in world space, unless otherwise indicated.
*/
class palSensor : public palFactoryObject {
public:
	palSensor()
	: m_pBody(0), m_Type(PAL_SENSOR_NONE) {}
	palBody *m_pBody;
	palSensorType m_Type;
private:
	palSensor(const palSensor& obj) {}
	palSensor& operator=(palSensor& obj) { return *this; }
};

//this doesnt need to be virtual, but you never know right?
/** A PSD (Position Sensitive Device) Sensor.
	A PSD sensor returns the distance between its location and direction and the nearest object.
	The process of doing so usually involves casting a ray and determining the intersection point to calculate the distance.
	The PSD sensor is therefore sometimes refered to as raycasting or raytraceing.

	<img src="../pictures/psdsensor.jpg">
	The diagram indicates the body to which the sensor is attached (the cube), the position at which the PSD sensor is located (the bottom of the arrow), and the direction which the PSD is facing (the tip of the arrow).
	The diagram illustrates the operation of the PSD, showing the PSD ray intersecting a neighbouring object. The length of the arrow is the distance returned when the PSD is queried.
*/
class palPSDSensor : virtual public palSensor {
public:
	palPSDSensor();
	/** Initializes the PSD sensor.
	Attaches a PSD sensor to a given body, at specified position, and pointed in a given direction, with a maximum range.
	\param body The body which the sensor is attached to.
	\param x The position (x)
	\param y The position (y)
	\param z The position (z)
	\param axis_x The direction vector the PSD is facing (x)
	\param axis_y The direction vector the PSD is facing (y)
	\param axis_z The direction vector the PSD is facing (z)
	\param range The maximum range the PSD has
	*/
	virtual void Init(palBody *body, Float x, Float y, Float z, Float axis_x, Float axis_y, Float axis_z, Float range); //position, direction
	/** Returns the distance
	Casts a ray from the sensors current location and orientation to determine the nearest intersection point.
	\return The distance to the closest object
	*/
	virtual Float GetDistance() = 0;
	Float m_fPosX;
	Float m_fPosY;
	Float m_fPosZ;
	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;
	Float m_fRange;
};

/** A Compass Sensor
*/
class palCompassSensor : public palSensor 
{
public:
	palCompassSensor();
	/** Initializes the compass sensor.
	Attaches a compass to a given body, with a given initial direction for north.
	\param body The body which the sensor is attached to.
	\param north_x The axis which the angle is measured about w.r.t. the body (x)
	\param north_y The axis which the angle is measured about w.r.t. the body (y)
	\param north_z The axis which the angle is measured about w.r.t. the body (z)
	*/
	virtual void Init(palBody *body, 
					  Float north_x, Float north_y, Float north_z);

	/** Returns the angle (radians) between compass direction and world north measured in the plane normal to sensor axis
	\return The angle (radians)
	*/
	virtual Float GetAngle();

	palVector3 m_fNorth;
	FACTORY_CLASS(palCompassSensor,palCompassSensor,*,1);
};

//todo: check if i should store the inital orientation matrix of the bodies its attached to.
/** A Inclinometer Sensor
	An Inclinometer sensor returns the angle between its initial orientation and its current orientation.
*/
class palInclinometerSensor : public palSensor {
public:
	palInclinometerSensor();
	/** Initializes the Inclinometer sensor.
	Attaches an inclinometer to a given body, with a given initial orientation.
	\param body The body which the sensor is attached to.
	\param axis_x The axis which the angle is measured about w.r.t. the body (x)
	\param axis_y The axis which the angle is measured about w.r.t. the body (y)
	\param axis_z The axis which the angle is measured about w.r.t. the body (z)
	\param up_x The up axis w.r.t the body (x)
	\param up_y The up axis w.r.t the body (y)
	\param up_z The up axis w.r.t the body (z)
	\param g_x The gravity axis w.r.t the world (x)
	\param g_y The gravity axis w.r.t the world (y)
	\param g_z The gravity axis w.r.t the world (z)
	*/
	virtual void Init(palBody *body, 
					  Float axis_x, Float axis_y, Float axis_z,
					  Float up_x, Float up_y, Float up_z,
					  Float g_x, Float g_y, Float g_z);

	/** Returns the angle (radians) between initial and current orientation measured in the plane normal to sensor axis
	\return The angle (radians)
	*/
	/*
	virtual int GetData ( void* data ) {
		float *dp = (float *)data;
		*dp = GetAngle ();
		return 0;
	}*/
	virtual Float GetAngle();

	palVector3 m_fAxis;
	palVector3 m_fUp;
	palVector3 m_fGravity;
	FACTORY_CLASS(palInclinometerSensor,palInclinometerSensor,*,1);
};
//need to get the velocities to do this?
/** A Gyroscope Sensor
	A gyroscope sensor returns the angualar velocity in a given direction.
*/
class palGyroscopeSensor : public palSensor {
public:
	palGyroscopeSensor();
	/**Initializes the Gyroscope sensor.
	Attaches a gyroscope to a given body, with a given sense direction.
	\param body The body which the sensor is attached to.
	\param axis_x The direction vector (x)
	\param axis_y The direction vector (y)
	\param axis_z The direction vector (z)
	*/
	virtual void Init(palBody *body, Float axis_x, Float axis_y, Float axis_z);
	/**Return the anglular velocity (radians)
	\return The angular velcoity (radians)
	*/
	virtual Float GetAngle();
	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;
	FACTORY_CLASS(palGyroscopeSensor,palGyroscopeSensor,*,1);
};

/** A Velocimeter Sensor
	A velocimeter sensor returns the velocity in a given direction
*/
class palVelocimeterSensor : public palSensor {
public:
	palVelocimeterSensor();
	/** Initializes the Velocimeter sensor.
	Attaches a velocimeter to a given body, with a given heading.
	\param body The body which the sensor is attached to.
	\param axis_x The direction vector (x)
	\param axis_y The direction vector (y)
	\param axis_z The direction vector (z)
	*/
	virtual void Init(palBody *body, Float axis_x, Float axis_y, Float axis_z);
	/** Returns the linear velocity
	\return The linear velocity
	*/
	virtual Float GetVelocity();
	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;
	FACTORY_CLASS(palVelocimeterSensor,palVelocimeterSensor,*,1);
};

/*
//accelerometer: linear, or angular?
class palAccelerometerSensor : virtual public palSensor {
public:
	palAccelerometerSensor();
	virtual void Init(palBody *body, Float axis_x, Float axis_y, Float axis_z) = 0;
	virtual Float GetAcceleration() = 0;
	Float m_fAxisX;
	Float m_fAxisY;
	Float m_fAxisZ;
};
*/
//this is closer to the physics engine:
//virtual void Init(palBody *body);
//and the (subsim) 'sensor' can have a location etc.
/** A Contact Sensor
	A contact sensor returns the location where a collision between the given body and any other object occurs.
	<img src="../pictures/contact.jpg">
	The diagram indicates the contact position returned when two objects collide.
*/
class palContactSensor : virtual public palSensor {
public:
	palContactSensor();
	/** Initializes the Contact sensor.
	Attaches a contact sensor to a given body.
	\param body The body which the sensor is attached to.
	*/
	virtual void Init(palBody *body); //location and size?
	/** Gets the position where a collision occured.
	\param contact A vector representing the location where the collision last occured.
	*/
	virtual void GetContactPosition(palVector3& contact) = 0;
};

/** A GPS Sensor
	A GPS sensor returns the location of the center of the body as a GPS string, eg:

*/
class palGPSSensor : public palSensor {
public:
	palGPSSensor();
	/** Initializes the GPS sensor.
	Attaches a GPS sensor to a given body.
	NOTE: Magnetic variation, Track/Course Made Good, date, are not calculated, and movement is assumed to occur on the equator!
	\param body The body which the sensor is attached to.
	\param UTCseconds The initial UTC time total (hours*60*60 + mins*60 + seconds)
	\param latitude The initial latitude position of the sensor (radians, ie: rad(degrees,minutes,seconds))
	\param longitude The initial longitude position of the sensor (radians, ie: rad(degrees,minutes,seconds))
	*/
	virtual void Init(palBody *body, int UTCseconds, float latitude,
					  float longitude); 
	/** Gets the GPS string
	\param string A pointer to a valid character buffer
	*/
	virtual void GetGPSString(char *string);
private:
	Float Rad2Deg(Float rad);
	Float frac(Float input);

private:
	float initialLong,initialLat;
	int initialUTC;

	FACTORY_CLASS(palGPSSensor,palGPSSensor,*,1);
};

/** A Transponder Sensor - Sender
	A transponder sensor returns the distance between two objects
	It composes of two parts: a sender, and a receiver.

	The transponder sender broadcasts its location to the world. 
*/
class palTransponderSender : public palSensor {
	friend class palTransponderReciever;
public:
	palTransponderSender();
	/** Initializes the Transponder sender.
	Attaches a transponder sender to a given body.
	\param body The body which the sendor is attached to
	\param range The maximum range the transponders signal can send
	*/
	virtual void Init(palBody *body, Float range); 
private:
	Float m_fRange;
	FACTORY_CLASS(palTransponderSender,palTransponderSender,*,1);
};

/** A Transponder Sensor - Receiver
	A transponder sensor returns the distance between two objects
	It composes of two parts: a sender, and a receiver.

	The reciever returns the distance from the senders
*/
class palTransponderReciever : public palSensor {
public:
	palTransponderReciever();
	/** Initializes the Transponder receiver.
	Attaches a transponder receiver to a given body.
	\param body The body which the sendor is attached to
	*/
	virtual void Init(palBody *body); 

	/** Gets the distance to a transponder
	Note: You MUST call GetNumTransponders before you can call GetDistance
	\param transponder The integer handle of the transponder to measure the distance from (ranges from 0 to max transponder - from GetNumTransponders)
	\return The distance to the closest object, or -1 if error
	*/
	virtual Float GetDistance(int transponder);

	/** Gets the number of transponders in range
	\return The number of transponders in range.
	*/
	virtual int GetNumTransponders(void);
private:
	PAL_VECTOR<Float> m_Distances;
	FACTORY_CLASS(palTransponderReciever,palTransponderReciever,*,1);
};


#endif
