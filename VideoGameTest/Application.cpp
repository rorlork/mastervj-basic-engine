#include "Application.h"

#include <Base/Math/Math.h>

#include <Core/Engine/Engine.h>

#include <Graphics/Scene/SceneRendererCommandManager.h>
#include <Graphics/Effect/EffectManager.h>
#include <Graphics/Camera/CameraManager.h>
#include <Graphics/Renderer/Renderer.h>
#include <PhysX/PhysXManager.h>
#include <Graphics/Cinematics/CinematicManager.h>
#include <Sound/SoundManager.h>

#include <Graphics/Camera/FPSCameraController.h>

#include <Core/Input/InputManager.h>
#include <Core/Input/InputManagerImplementation.h>
#include <Core/Debug/DebugHelper.h>

#include <Core/Scene/SceneManager.h>

#include <Core/Component/ComponentManager.h>

#include <XML/XMLTreeNode.h>



CApplication::CApplication(CContextManager *_ContextManager)
	: m_ContextManager(_ContextManager)
	, m_FixedTimer(0)
	, m_Timer(0)
{
	CDebugHelper::GetDebugHelper()->Log("CApplication::CApplication");
	CDebugHelper::GetDebugHelper()->CreateMainBar();
	m_FixedCamera = false;
}


CApplication::~CApplication()
{
	CDebugHelper::GetDebugHelper()->Log("CApplication::~CApplication");
}

void CApplication::Update(double _ElapsedTime)
{
	CEngine& engine = CEngine::GetSingleton();
	m_Timer += _ElapsedTime;

	m_FixedTimer += _ElapsedTime;

	bool isFixedUpdate = false;
	if ( m_FixedTimer >= PHYSX_UPDATE_STEP )
	{
		isFixedUpdate = true;
		m_FixedTimer = fmod( m_FixedTimer, PHYSX_UPDATE_STEP );
	}

	engine.getEffectsManager()->m_SceneParameters.m_Time = m_Timer;

	engine.getSceneManager()->Update();

	CEngine::GetSingleton().getCinematicManager()->Update(_ElapsedTime);

	CEngine::GetSingleton().getComponentManager()->PhysxUpdate();

	CEngine::GetSingleton().getCameraManager()->GetCurrentCameraController();
	m_CurrentCamera = { };
	auto camController = CEngine::GetSingleton().getCameraManager()->GetCurrentCameraController();
	if ( camController )
	{
		camController->UpdateCameraValues( &m_CurrentCamera );
	}


	if (CInputManager::GetInputManager()->IsActionActive("FIXCAMERA"))
	{
		m_FixedCamera = !m_FixedCamera;
	}

	if(isFixedUpdate)
	{
		CEngine::GetSingleton().getPhysXManager()->update( PHYSX_UPDATE_STEP );
		CEngine::GetSingleton().getComponentManager()->FixedUpdate( PHYSX_UPDATE_STEP );
	}

	CEngine::GetSingleton().getComponentManager()->Update(_ElapsedTime);


	if ( isFixedUpdate )
	{
		CEngine::GetSingleton().getSceneManager()->FixedUpdate();
	}

	CEngine::GetSingleton().getCameraManager()->Update(_ElapsedTime);

}


void CApplication::Render()
{
	if ( CEngine::GetSingleton().getCameraManager()->GetCurrentCameraController() != nullptr )
	{
		auto renderer = CEngine::GetSingleton().getRenderer();

		renderer->BeginRender();

		CEngine::GetSingleton().getComponentManager()->Render( *m_ContextManager );

		CEngine::GetSingleton().getSceneRendererCommandManager()->Execute( *m_ContextManager );

		renderer->EndRender();
	}
}

void CApplication::PostRender( double _ElapsedTime )
{
	CEngine::GetSingleton().getSoundManager()->Update(&m_CurrentCamera);
}
