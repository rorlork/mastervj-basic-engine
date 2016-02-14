#include "Application.h"

#include <Base/Math/Math.h>

#include <Graphics/Context/ContextManager.h>
#include <Graphics/Renderer/RenderManager.h>

#include <Core/Input/InputManager.h>
#include <Core/Input/InputManagerImplementation.h>
#include <Core/Debug/DebugHelper.h>

#include <XML/XMLTreeNode.h>
#include <Graphics/Cinematics/Cinematic.h>

#include <PhysX/PhysXManager.h>

#include <Base/Scripting/ScriptManager.h>

#include <Engine/Engine.h>


static float s_mouseSpeed = 1;

CScriptManager *s_sm = nullptr;
static CPhysXManager* phMgr = nullptr;

static void __stdcall SwitchCameraCallback( void* _app )
{
	((CApplication*)_app)->m_RenderManager->SwitchCamera();
}

static void __stdcall ReloadSceneCommands(void* _app)
{
	CEngine::GetSingleton().getSceneRendererCommandManager()->Reload();
}

static void __stdcall CreateScene(void* a)
{
	CPhysXManager::ShapeDesc desc;
	desc.shape = CPhysXManager::ShapeDesc::Shape::Box;
	desc.density = 1;
	desc.material = "box";
	desc.size = Vect3f(1, 1, 1);
	desc.position = Vect3f(0, 5, 0);
	phMgr->createActor("b1", CPhysXManager::ActorType::Dynamic, desc);
	desc.position = Vect3f(0, 8, 0);
	phMgr->createActor("b2", CPhysXManager::ActorType::Dynamic, desc);
	desc.position = Vect3f(0, 16, 0);
	phMgr->createActor("b3", CPhysXManager::ActorType::Dynamic, desc);
}

static void __stdcall CreateChar(void* a)
{
	
}

CApplication::CApplication( CContextManager *_ContextManager, CRenderManager *_renderManager )
	: m_RenderManager( _renderManager )
	, m_ContextManager( _ContextManager )
{
	CDebugHelper::GetDebugHelper()->Log( "CApplication::CApplication" );

	CDebugHelper::SDebugBar bar;
	bar.name = "CApplication";
	{
		CDebugHelper::SDebugVariable var = {};
		var.name = "background";
		var.type = CDebugHelper::COLOR;
		var.mode = CDebugHelper::READ_WRITE;
		var.pColor = &_ContextManager->m_BackgroundColor;

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
	{
		CDebugHelper::SDebugVariable var = {};
		var.name = "PhysX: Create scene";
		var.type = CDebugHelper::BUTTON;
		var.callback = CreateScene;
		var.data = this;
		
		bar.variables.push_back(var);
	}
	{
		CDebugHelper::SDebugVariable var = {};
		var.name = "PhysX: Create char";
		var.type = CDebugHelper::BUTTON;
		var.callback = CreateChar;
		var.data = this;

		bar.variables.push_back(var);
	}
	{
		CDebugHelper::SDebugVariable var = {};
		var.name = "Reload Options";
		var.type = CDebugHelper::STRING;
		var.pString = "";

		bar.variables.push_back(var);
	}
	{
		CDebugHelper::SDebugVariable var = {};
		var.name = "  - Commands";
		var.type = CDebugHelper::BUTTON;
		var.callback = ReloadSceneCommands;
		var.data = this;

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
	phMgr = CPhysXManager::CreatePhysXManager();
	phMgr->registerMaterial("ground", 1, 0.9, 0.1);
	phMgr->registerMaterial("box", 1, 0.9, 0.8);
	phMgr->registerMaterial("controller_material", 10, 2, 0.5);
	phMgr->createPlane("ground", "ground", Vect4f(0, 1, 0, 0));
	phMgr->createController(2, 0.5f, 10, Vect3f(0, 4, 0), "main");
}


void CApplication::Update( float _ElapsedTime )
{
	phMgr->update(_ElapsedTime);
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

				
				
				m_RenderManager->getSphericalCamera()->Update(cameraMovement);
			}
			break;
		case 1:
		{

			m_RenderManager->getFPSCamera()->AddYaw(-CInputManager::GetInputManager()->GetAxis("X_AXIS") * 0.0005f);
			m_RenderManager->getFPSCamera()->AddPitch(CInputManager::GetInputManager()->GetAxis("Y_AXIS") * 0.005f);
			Vect3f cameraMovement(0, -9.8f, 0);
			cameraMovement.z = CInputManager::GetInputManager()->GetAxis("MOVE_FWD");
			cameraMovement.x = CInputManager::GetInputManager()->GetAxis("STRAFE");

			cameraMovement = phMgr->moveCharacterController(cameraMovement, _ElapsedTime);
			//m_RenderManager->getFPSCamera()->Move(CInputManager::GetInputManager()->GetAxis("STRAFE"), CInputManager::GetInputManager()->GetAxis("MOVE_FWD"), false, _ElapsedTime);
			m_RenderManager->getFPSCamera()->Move(cameraMovement.x, cameraMovement.z, false, _ElapsedTime);
		}
		break;
	}
}


void CApplication::Render()
{
	CEngine::GetSingleton().getSceneRendererCommandManager()->Execute(*m_ContextManager);
}
