#ifndef MATERIAL_H
#define MATERIAL_H

#include <Base/Utils/Named.h>
#include <Base\Utils\Utils.h>
#include <Base/Math/Color.h>
#include <vector>

class CXMLTreeNode;
class CEffectTechnique;
class CTexture;
class CMaterialParameter;
class CRenderableObjectTechnique;
class CMaterialParameter;

class CMaterial : public CNamed
{
private:
	std::vector<CTexture *> m_textures;
	CEffectTechnique *m_effectTechnique;

	std::vector<CMaterialParameter *> m_Parameters;
	CRenderableObjectTechnique *m_RenderableObjectTechnique;
	unsigned int m_CurrentParameterData;
	void destroy();
public:
	CMaterial(CXMLTreeNode &TreeNode);
	virtual ~CMaterial();
	virtual void apply(CRenderableObjectTechnique *RenderableObjectTechnique = NULL);
	CEffectTechnique* getEffectTechnique() const;
	CRenderableObjectTechnique * getRenderableObjectTechique(){ return m_RenderableObjectTechnique; }
	std::vector<CMaterialParameter *> getParameters(){ return m_Parameters;  }
	void * GetNextParameterAddress(unsigned int NumBytes);
};

#endif