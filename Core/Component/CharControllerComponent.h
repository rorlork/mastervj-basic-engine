#ifndef CHARACTER_CONTROLLER_COMPONENT_H
#define CHARACTER_CONTROLLER_COMPONENT_H

#include "Component.h"

#include <vector>

class CCharacterControllerComponent : public CComponent
{
	float m_height;
	float m_radius;
	float m_density;
	Vect3f m_offset;

protected:
	virtual void Init();

public:
	CCharacterControllerComponent(CXMLTreeNode& node, CRenderableObject* Owner);
	CCharacterControllerComponent(CRenderableObject* Owner);
	virtual ~CCharacterControllerComponent();

	virtual void FixedUpdate( float ElapsedTime );

	virtual void Destroy();

	bool IsGrounded();

	void Move(const Vect3f& velocity, const Vect3f& up, float elapsedTime);

	void SetPosition(const Vect3f& pos);
};

#endif
