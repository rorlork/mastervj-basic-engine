#include "Scene/DeferredShadingSceneRendererCommand.h"
#include <Core/Engine/Engine.h>
#include "Light/Light.h"

CDeferredShadingSceneRendererCommand::CDeferredShadingSceneRendererCommand(CXMLTreeNode &TreeNode)
	: CStagedTexturedSceneRendererCommand(TreeNode)
{
	auto mm = CEngine::GetSingleton().getMaterialManager();
	auto mat = mm->get(TreeNode.GetPszProperty("material"));
	m_RenderableObjectTechnique = mat->getRenderableObjectTechique();

	for (int i = 0; i < TreeNode.GetNumChildren(); i++)
	{
		CXMLTreeNode texChild = TreeNode(i);
		if ( texChild.GetName() != std::string( "texture" ) )
		{
			continue;
		}
		CTexture *tex = nullptr;
		tex = CEngine::GetSingleton().getTextureManager()->GetTexture(texChild.GetPszProperty("file"));
		AddStageTexture(texChild.GetIntProperty("stage_id"), tex);
	}

	D3D11_BLEND_DESC l_AlphablendDesc;
	ZeroMemory(&l_AlphablendDesc, sizeof(D3D11_BLEND_DESC));
	l_AlphablendDesc.RenderTarget[0].BlendEnable = true;
	l_AlphablendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	l_AlphablendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	l_AlphablendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	l_AlphablendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	l_AlphablendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	l_AlphablendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	l_AlphablendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT hr = CEngine::GetSingleton().getContextManager()->GetDevice()->CreateBlendState(&l_AlphablendDesc, &m_EnabledAlphaBlendState);
	assert(!FAILED(hr));
}

CDeferredShadingSceneRendererCommand::~CDeferredShadingSceneRendererCommand()
{
	m_EnabledAlphaBlendState->Release();
}

void CDeferredShadingSceneRendererCommand::Execute(CContextManager &_context)
{
	CEngine::GetSingleton().getContextManager()->GetDeviceContext()->OMSetBlendState(m_EnabledAlphaBlendState, nullptr, 0xffffffff);

	auto lm = CEngine::GetSingleton().getLightManager();
	auto em = CEngine::GetSingleton().getEffectsManager();

	ActivateTextures();
	for (int i = 0; i < lm->count(); i++)
	{
		CLight& light = lm->iterate(i);
		// Set light to the buffer
		em->SetLightConstants(0, &light);
		m_RenderableObjectTechnique->GetEffectTechnique()->SetConstantBuffer(1, &CEffectManager::m_LightParameters);
		// Render scene with that light

		// TODO: Paint only enabled lights
		_context.DrawScreenQuad(m_RenderableObjectTechnique->GetEffectTechnique(),
								nullptr, 0, 0, 1, 1, CColor(1, 1, 1, 1));
	}
	DeactivateTextures();

	CEngine::GetSingleton().getContextManager()->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xffffffff);
}
