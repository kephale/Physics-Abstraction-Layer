#include "palSensors.h"
#include "palFactory.h"
#include <memory.h>

FACTORY_CLASS_IMPLEMENTATION(palInclinometerSensor);
FACTORY_CLASS_IMPLEMENTATION(palGyroscopeSensor);
FACTORY_CLASS_IMPLEMENTATION(palVelocimeterSensor);
FACTORY_CLASS_IMPLEMENTATION(palCompassSensor);
FACTORY_CLASS_IMPLEMENTATION(palGPSSensor);

FACTORY_CLASS_IMPLEMENTATION(palTransponderSender);
FACTORY_CLASS_IMPLEMENTATION(palTransponderReciever);


palPSDSensor::palPSDSensor() {
	m_pBody = NULL;
	m_Type = PAL_SENSOR_PSD;
}

void palPSDSensor::Init(palBody *body, Float x, Float y, Float z, Float dx, Float dy, Float dz, Float range) {
	m_pBody=body;
	m_fPosX = x;
	m_fPosY = y;
	m_fPosZ = z;
	m_fAxisX=dx;
	m_fAxisY=dy;
	m_fAxisZ=dz;
	m_fRange=range;
}

palCompassSensor::palCompassSensor() 
{
	m_pBody = NULL;
	m_Type = PAL_SENSOR_COMPASS;
}

void palCompassSensor::Init(palBody *body, 
								 Float n_x, Float n_y, Float n_z) 
{
	m_pBody = body;
	
	vec_set(&m_fNorth, n_x, n_y, n_z);
}

Float iGetAngle(palVector3 &fwd, palMatrix4x4 &M) {
	//normailze the input
	vec_norm(&fwd);
	palVector3 rotated_fwd;
	vec_mat_mul(&rotated_fwd,&M,&fwd); 
	vec_norm(&rotated_fwd);
	Float m = 1; //m is one, since two unit vectors
	Float dot = vec_dot(&rotated_fwd,&fwd);
	Float a = acos( dot/m ); //problem, a is always positive!

	//calcualte right vector, so we can get the correct positive or negative value
	palVector3 right;
	palVector3 rotated_right;

	palMatrix4x4 rightMatrix; //so we can set the right direction automatically
	mat_identity(&rightMatrix);
	roty(&rightMatrix,(Float)M_PI/2);

	vec_mat_mul(&right,&rightMatrix,&fwd); //build the right vector
	//now we have the 'right' vector
	
	vec_mat_mul(&rotated_right,&M,&right); 
	vec_norm(&rotated_right);
	
	//dot product between fowards and rotated right
	Float right_dot = vec_dot(&rotated_right,&fwd);
	if (right_dot < 0 ) a = -a; //change sign if we need to.
	
	return a;
}

Float palCompassSensor::GetAngle() 
{
	palMatrix4x4 M = m_pBody->GetLocationMatrix();
//#error todo: use getangle here and inclino, and make lowlevel example of submarine diving with a sin() on the props or something
	palVector3 fwd = m_fNorth;
	vec_norm(&fwd);
	palVector3 rotated_fwd;
	vec_mat_mul(&rotated_fwd,&M,&fwd); 
	vec_norm(&rotated_fwd);
	Float a = atan2(rotated_fwd.z,rotated_fwd.x);
	return a;

	//return iGetAngle(fwd,M);*/
}

palInclinometerSensor::palInclinometerSensor() {
	m_pBody = NULL;
	m_Type = PAL_SENSOR_INCLINOMETER;
	//STATIC_SET_ERROR("palInclinometerSensor Cons\n"); 
}

void palInclinometerSensor::Init(palBody *body, 
								 Float axis_x, Float axis_y, Float axis_z,
								 Float up_x, Float up_y, Float up_z,
								 Float g_x, Float g_y, Float g_z) 
{
	m_pBody = body;
	
	vec_set(&m_fAxis, axis_x, axis_y, axis_z);
	vec_set(&m_fUp, up_x, up_y, up_z);
	vec_set(&m_fGravity, g_x, g_y, g_z);
	
	vec_norm(&m_fAxis);
	vec_norm(&m_fUp);
	vec_norm(&m_fGravity);
}


Float palInclinometerSensor::GetAngle() 
{
	palMatrix4x4 M = m_pBody->GetLocationMatrix();

	palVector3 fwd;
	fwd = m_fAxis;
	return iGetAngle(fwd,M);
}

palGyroscopeSensor::palGyroscopeSensor() {
	m_pBody = NULL;
	m_Type = PAL_SENSOR_INCLINOMETER;
}

void palGyroscopeSensor::Init(palBody *body, Float axis_x, Float axis_y, Float axis_z) {
	m_pBody = body;
	palVector3 axis;
	axis.x=axis_x;
	axis.y=axis_y;
	axis.z=axis_z;
	vec_norm(&axis);
	m_fAxisX=axis.x;
	m_fAxisY=axis.y;
	m_fAxisZ=axis.z;
}

Float palGyroscopeSensor::GetAngle() {
	palVector3 angular_vel; 
	m_pBody->GetAngularVelocity(angular_vel);
	palVector3 pos;
	pos.x=m_fAxisX;
	pos.y=m_fAxisY;
	pos.z=m_fAxisZ;
	Float dot = vec_dot(&pos,&angular_vel); //find the dot product (multiply the two vectors together)
	return dot;
}

palVelocimeterSensor::palVelocimeterSensor() {
	m_pBody = NULL;
	m_Type = PAL_SENSOR_VELOCIMETER;
}

void palVelocimeterSensor::Init(palBody *body, Float axis_x, Float axis_y, Float axis_z) {
	m_pBody = body;
	palVector3 axis;
	axis.x=axis_x;
	axis.y=axis_y;
	axis.z=axis_z;
	vec_norm(&axis);
	m_fAxisX=axis.x;
	m_fAxisY=axis.y;
	m_fAxisZ=axis.z;
}

Float palVelocimeterSensor::GetVelocity() {
	palVector3 linear_vel; 
	m_pBody->GetLinearVelocity(linear_vel);
	palMatrix4x4 m = m_pBody->GetLocationMatrix();
	palVector3 old_pos;
	palVector3 new_pos;
	old_pos.x=m_fAxisX;
	old_pos.y=m_fAxisY;
	old_pos.z=m_fAxisZ;
	vec_mat_mul(&new_pos,&m,&old_pos); //transform to new pos
	Float dot = vec_dot(&new_pos,&linear_vel); //find the dot product (multiply the two vectors together)
	return dot;
}

///////////////////////////////////////
palContactSensor::palContactSensor() {
	m_pBody = NULL;
	m_Type = PAL_SENSOR_CONTACT;
}

void palContactSensor::Init(palBody *body) {
	m_pBody = body;
}


///////////////////

palGPSSensor::palGPSSensor() {
	m_pBody = NULL;
	m_Type = PAL_SENSOR_GPS;
	initialLong=0;
	initialLat=0;
	initialUTC = 0;
}

void palGPSSensor::Init(palBody *body, int UTCseconds, float latitude, float longitude) {
	m_pBody = body;
	initialLong = longitude;
	initialLat = latitude;
	initialUTC = UTCseconds;
}

Float palGPSSensor::Rad2Deg(Float rad) {
	return rad*180/(Float)M_PI;
}
Float palGPSSensor::frac(Float input) {
	int i = (int)input;
	return input-i;
}

void palGPSSensor::GetGPSString(char *string) {
	int utc,latDeg,longDeg;
	Float latMf,longMf;

	const float metersPerSecond = 30.9f;
	const float meterPStoknot = 1.94384449f;

	palVector3 linear_vel; 
	m_pBody->GetLinearVelocity(linear_vel);

	palVector3 position; 
	m_pBody->GetPosition(position);

	utc = (int)(PF->GetActivePhysics()->GetTime() + initialUTC);

	Float rLat = initialLat + ((position.x / metersPerSecond) / 3600);
	Float rLong = initialLong + ((position.z / metersPerSecond) / 3600);

	latDeg = (int)Rad2Deg(rLat);
	latMf = frac(Rad2Deg(rLat)) * 60; 

	longDeg = (int)Rad2Deg(rLong);
	longMf = frac(Rad2Deg(rLong)) * 60; 

	Float velocity = vec_mag(&linear_vel);

	float track = 0;
	int date = 0;
	int magnetic_variation=0;
	char buffer[4096];
	memset(buffer,0,sizeof(char)*4096);
	sprintf(buffer,"GPRMC,%06d,A,%02d%09.6f,S,%02d%09.6f,E,%06.2f,%05.1f,%06d,%d,X,A*",utc,latDeg,latMf,longDeg,longMf,velocity*meterPStoknot,track,date,magnetic_variation);
	int i=0;
	int checksum=0;
	while (buffer[i]!='*') {
		checksum^=buffer[i];
		i++;
	}
	sprintf(string,"$%s%02X",buffer,checksum);
}

//////////////////////////
PAL_VECTOR<palTransponderSender *> g_TransponderSenders;

palTransponderSender::palTransponderSender() {
	m_pBody = NULL;
	m_Type = PAL_SENSOR_TRANSPONDER_SENDER;
}

void palTransponderSender::Init(palBody *body, Float range) {
	m_pBody=body;
	m_fRange = range;
	g_TransponderSenders.push_back(this);
}

palTransponderReciever::palTransponderReciever() {
	m_pBody = NULL;
	m_Type = PAL_SENSOR_TRANSPONDER_RECIEVER;
}

void palTransponderReciever::Init(palBody *body) {
	m_pBody = body;
}

Float palTransponderReciever::GetDistance(int transponder) {
	if (transponder<0) return -1;
	if ((unsigned int)transponder>m_Distances.size()) return -1;
	return m_Distances[transponder];
}

Float DistanceBetweenTwoBodies(palBody *p1, palBody *p2) {
	palVector3 pos1;
	palVector3 pos2;
	p1->GetPosition(pos1);
	p2->GetPosition(pos2);
	palVector3 diff;
	vec_sub(&diff,&pos1,&pos2);
	return vec_mag(&diff);
}

int palTransponderReciever::GetNumTransponders(void) {
	m_Distances.clear();
	for (unsigned int i=0;i<g_TransponderSenders.size();i++) {
		Float d=DistanceBetweenTwoBodies(m_pBody,g_TransponderSenders[i]->m_pBody);
		if (d<g_TransponderSenders[i]->m_fRange)
			m_Distances.push_back(d);
	}
	return (int)m_Distances.size();
}
