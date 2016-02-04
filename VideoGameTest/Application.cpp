#include "Application.h"

#include <Base/Math/Math.h>

#include <Graphics/Context/ContextManager.h>
#include <Graphics/Debug/DebugRender.h>
#include <Graphics/Renderer/RenderManager.h>

#include <Core/Input/InputManager.h>
#include <Core/Input/InputManagerImplementation.h>
#include <Core/Debug/DebugHelper.h>

#include <XML/XMLTreeNode.h>
#include <Graphics/Cinematics/Cinematic.h>

#include <PhysX/PhysXManager.h>


static float s_mouseSpeed = 1;


static void __stdcall SwitchCameraCallback( void* _app )
{
	((CApplication*)_app)->m_RenderManager->SwitchCamera();
}

CApplication::CApplication( CContextManager *_ContextManager, CRenderManager *_renderManager )
	: m_RenderManager( _renderManager )
	, m_ContextManager( _ContextManager )
	, m_BackgroundColor( .2f, .1f, .4f )
	
{
	CDebugHelper::GetDebugHelper()->Log( "CApplication::CApplication" );

	CDebugHelper::SDebugBar bar;
	bar.name = "CApplication";
	{
		CDebugHelper::SDebugVariable var = {};
		var.name = "background";
		var.type = CDebugHelper::COLOR;
		var.mode = CDebugHelper::READ_WRITE;
		var.pColor = &m_BackgroundColor;

		bar.variables.push_back(var);
	}
	/*
	{
		CDebugHelper::SDebugVariable var = {};
		var.name = "cube";
		var.type = CDebugHelper::POSITION_ORIENTATION;
		var.mode = CDebugHelper::READ_WRITE;
		var.pPositionOrientation = m_Cube.GetPtrTransform();

		bar.variables.push_back(var);
	}
	*/
	{
		CDebugHelper::SDebugVariable var = {};
		var.name = "switch camera";
		var.type = CDebugHelper::BUTTON;
		var.callback = SwitchCameraCallback;
		var.data = this;

		bar.variables.push_back(var);
	}
	{
		CDebugHelper::SDebugVariable var = {};
		var.name = "mouse speed";
		var.type = CDebugHelper::FLOAT;
		var.mode = CDebugHelper::READ_WRITE;
		var.pFloat = &s_mouseSpeed;
		var.params = " min=0.1 max=10 step=0.1 precision=1 ";

		bar.variables.push_back(var);
	}

	CDebugHelper::GetDebugHelper()->RegisterBar(bar);
}


CApplication::~CApplication()
{
	CDebugHelper::GetDebugHelper()->Log( "CApplication::~CApplication" );
}

void CApplication::Init()
{

}



void CApplication::Update( float _ElapsedTime )
{
	CEngine::GetSingleton().getLayerManager()->Update(_ElapsedTime);

	( (CInputManagerImplementation*)CInputManager::GetInputManager() )->SetMouseSpeed( s_mouseSpeed );

	switch (m_RenderManager->getCurrentCameraNum())
	{
		case 0:
			if ( CInputManager::GetInputManager()->IsActionActive( "MOVE_CAMERA" ) )
			{
				Vect3f cameraMovement( 0, 0, 0 );

				cameraMovement.x = CInputManager::GetInputManager()->GetAxis( "X_AXIS" ) * 0.0005f;
				cameraMovement.y = CInputManager::GetInputManager()->GetAxis( "Y_AXIS" ) * 0.005f;

				m_RenderManager->getSphericalCamera().Update(cameraMovement);
			}
			break;
		case 1:
		{
			m_RenderManager->getFPSCamera().AddYaw(-CInputManager::GetInputManager()->GetAxis("X_AXIS") * 0.0005f);
			m_RenderManager->getFPSCamera().AddPitch(CInputManager::GetInputManager()->GetAxis("Y_AXIS") * 0.005f);

			m_RenderManager->getFPSCamera().Move(CInputManager::GetInputManager()->GetAxis("STRAFE"), CInputManager::GetInputManager()->GetAxis("MOVE_FWD"), false, _ElapsedTime);
		}
		break;
	}
}

void CApplication::Render()
{
	m_RenderManager->SetCamerasMatrix(m_ContextManager);
	m_ContextManager->BeginRender( m_BackgroundColor );

	// a�adir todos los objetos que se quiere pintar
	//m_RenderManager.AddRenderableObjectToRenderList(&m_Cube);

	m_RenderManager->Render( m_ContextManager );

	//Mat44f world;

	//world.SetIdentity();
	//m_ContextManager->SetWorldMatrix(world);
	//m_ContextManager->Draw(m_DebugRender->GetAxis());

	//world.SetIdentity();
	//world.SetFromPos(10, 0, 0);
	//m_ContextManager->SetWorldMatrix(world);
	//m_ContextManager->Draw(m_DebugRender->GetClassicBlendTriangle(), CContextManager::RS_SOLID, CContextManager::DSS_OFF, CContextManager::BLEND_CLASSIC);

	//world.SetIdentity();
	//world.SetFromPos(0, 0, -10);
	//m_ContextManager->SetWorldMatrix(world);
	//m_ContextManager->Draw(m_DebugRender->GetPremultBlendTriangle(), CContextManager::RS_SOLID, CContextManager::DSS_OFF, CContextManager::BLEND_PREMULT);

	//ANTTWEAK
	//CDebugHelper::GetDebugHelper()->Render();

	m_ContextManager->EndRender();
}
