#ifndef DYNAMIC_TEXTURE_H
#define DYNAMIC_TEXTURE_H

#include "Texture\Texture.h"
#include <D3D11.h>
#include "Utils\Utils.h"

class CDynamicTexture : public CTexture
{
protected:
	ID3D11Texture2D *m_RenderTargetTexture;
	ID3D11RenderTargetView *m_RenderTargetView;
	ID3D11Texture2D *m_DepthStencilBuffer;
	ID3D11DepthStencilView *m_DepthStencilView;

	UINT m_Width;
	UINT m_Height;

	bool m_CreateDepthStencilBuffer;

	DXGI_FORMAT m_format;

	void Unload();
	void Init();

public:
	CDynamicTexture(const CXMLTreeNode &TreeNode);
	CDynamicTexture(const std::string &Name, int Width, int Height, bool
		CreateDepthStencilBuffer, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	~CDynamicTexture();
	bool Reload();

	UINT GetWidth(){ return m_Width; }
	UINT GetHeight(){ return m_Height; }
	ID3D11RenderTargetView * GetRenderTargetView(){ return m_RenderTargetView; }
	ID3D11DepthStencilView * GetDepthStencilView(){ return m_DepthStencilView; }
};

#endif