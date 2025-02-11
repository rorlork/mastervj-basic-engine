#ifndef EFFECT_TECHNIQUE_H
#define EFFECT_TECHNIQUE_H


#include <Base/Utils/Named.h>
class CEffectVertexShader;
class CEffectPixelShader;
class CEffectGeometryShader;

class CEffectTechnique : public CNamed
{
private:
	CEffectVertexShader *m_VertexShader;
	CEffectPixelShader *m_PixelShader;
	CEffectGeometryShader *m_GeometryShader;
	std::string m_VertexShaderName;
	std::string m_PixelShaderName;
	std::string m_GeometryShaderName;
public:
	CEffectTechnique(CXMLTreeNode &TreeNode);
	virtual ~CEffectTechnique();
	CEffectVertexShader * GetVertexShader();
	CEffectPixelShader * GetPixelShader();
	CEffectGeometryShader * GetGeometryShader();

	void SetConstantBuffer(unsigned int IdBuffer, void *ConstantBuffer);
	void destroy();
	void Refresh();
};

#endif
