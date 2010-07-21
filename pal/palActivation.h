#ifndef PAL_ACTIVATION_H
#define PAL_ACTIVATION_H

/*
 * Classes related to body activation (aka sleeping).
 */

#include <bitset>
#include "pal/palMath.h"

/**
 * This is a pure virtual class that a palBody implements if it
 * supports automatic (de)activation.
 */
class palActivationSettings {
public:	
	typedef enum {
		LINEAR_VELOCITY_THRESHOLD,
		ANGULAR_VELOCITY_THRESHOLD,
		TIME_THRESHOLD,
		DUMMY_ACTIVATION_SETTING_TYPE // must be last
	} ActivationSettingType;
	virtual ~palActivationSettings() {}

	virtual Float GetActivationLinearVelocityThreshold() const = 0;
	virtual void SetActivationLinearVelocityThreshold(Float) = 0;

	virtual Float GetActivationAngularVelocityThreshold() const = 0;
	virtual void SetActivationAngularVelocityThreshold(Float) = 0;

	virtual Float GetActivationTimeThreshold() const = 0;
	virtual void SetActivationTimeThreshold(Float) = 0;

	virtual const std::bitset<DUMMY_ACTIVATION_SETTING_TYPE>& GetSupportedActivationSettings() const = 0;
};

#endif
