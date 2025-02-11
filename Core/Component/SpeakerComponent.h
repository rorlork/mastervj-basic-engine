#ifndef SPEAKER_COMPONENT_H
#define SPEAKER_COMPONENT_H

#include "Component.h"
#include <Graphics/Renderer/3DElement.h>

class CSpeakerComponent : public CComponent
{

protected:

	virtual void Init();

public:
	CSpeakerComponent(CXMLTreeNode& node, CElement* Owner);
	CSpeakerComponent(const CSpeakerComponent& name, CElement* Owner);
	virtual ~CSpeakerComponent();

	virtual void Update(double ElapsedTime);

	virtual void Destroy();

	void Play( const std::string audioClipName);
	bool Finished(const std::string EventName);
	void CSpeakerComponent::SetSwitch(const std::string SwitchValue, const std::string SwitchName);
	void Stop();
	void CSpeakerComponent::SetVolume(const std::string VolumeName, float volumeValue);

	Vect3f m_Position;
	Vect3f m_Orientation;
	float m_Volume;
	C3DElement m_Speaker;

	static const std::string COMPONENT_TYPE;
	virtual std::string GetComponentType() { return COMPONENT_TYPE; }

	virtual CComponent* Clone(CElement* Owner) const { return new CSpeakerComponent( *this, Owner ); }
};

#endif
