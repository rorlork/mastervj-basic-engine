#pragma once

#include <Windows.h>

#include <unordered_map>

#include <AntTweakBar.h>

#include "DebugHelper.h"

class CDebugHelperImplementation : public CDebugHelper
{
public:
	CDebugHelperImplementation(void *device);
	~CDebugHelperImplementation();


	bool Update(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void Render();

	virtual void Log(const std::string& text) const;

	virtual void RegisterBar(const SDebugBar& bar);
	void RemoveBar(const std::string &bar);

	void CreateMainBar();

	std::unordered_map<std::string, TwBar*> m_Bars;
	SPositionOrientation * m_posRot;
	TwType m_PosRotType;
};


