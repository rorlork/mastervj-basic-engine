#ifndef LIGHT_EFFECT_PARAMETERS_H
#define LIGHT_EFFECT_PARAMETERS_H

#include "Math\Vector4.h"
#include "Math\Matrix44.h"

#define MAX_LIGHTS_BY_SHADER 4

class CLightEffectParameters
{
public:
	Vect4f m_LightAmbient;
	float m_LightEnabled[MAX_LIGHTS_BY_SHADER];
	float m_LightType[MAX_LIGHTS_BY_SHADER];
	Vect4f m_LightPosition[MAX_LIGHTS_BY_SHADER];
	Vect4f m_LightDirection[MAX_LIGHTS_BY_SHADER];
	float m_LightAngle[MAX_LIGHTS_BY_SHADER];
	float m_LightFallOffAngle[MAX_LIGHTS_BY_SHADER];
	float m_LightAttenuationStartRange[MAX_LIGHTS_BY_SHADER];
	float m_LightAttenuationEndRange[MAX_LIGHTS_BY_SHADER];
	float m_LightIntensity[MAX_LIGHTS_BY_SHADER];
	Vect4f m_LightColor[MAX_LIGHTS_BY_SHADER];
	float m_UseShadowMap[MAX_LIGHTS_BY_SHADER];
	float m_UseShadowMask[MAX_LIGHTS_BY_SHADER];
	Mat44f m_LightView[MAX_LIGHTS_BY_SHADER];
	Mat44f m_LightProjection[MAX_LIGHTS_BY_SHADER];
};

#endif