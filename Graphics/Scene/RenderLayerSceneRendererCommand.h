#ifndef RENDER_LAYER_SCENE_RENDERER_COMMAND_H
#define RENDER_LAYER_SCENE_RENDERER_COMMAND_H

#include "Scene/SceneRendererCommand.h"
#include "Engine/Engine.h"

class CRenderLayerSceneRendererCommand : public CSceneRendererCommand
{
private:
	std::string m_layerName;
	bool m_layer_zsort = false;
public:
	CRenderLayerSceneRendererCommand(CXMLTreeNode &TreeNode);
	void Execute(CContextManager &_context);
};

#endif