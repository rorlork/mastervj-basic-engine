#include "ScriptedComponent.h"

#include <Core/Engine/Engine.h>
#include <Base/Scripting/ScriptManager.h>
#include <Base/XML/XMLTreeNode.h>
#include "Scene/Element.h"

#include <selene.h>

#include <sstream>
#include <fstream>

#include <Base/Scripting/LuaErrorCapture.h>

const std::string CScriptedComponent::COMPONENT_TYPE = "Script";

unsigned CScriptedComponent::s_nextComponentStateId = 0;

CScriptedComponent::CScriptedComponent(const CScriptedComponent& base,
									   CElement* Owner)
	: CComponent(base, Owner)
	, m_scriptMgr(CEngine::GetSingleton().getScriptManager())
	, m_componentStateId(s_nextComponentStateId++)
{
	m_scriptClass = base.m_scriptClass;
	std::string n = Owner->getName() + "_" + m_scriptClass;
	SetNameFromParentName(n);
}

CScriptedComponent::CScriptedComponent(CXMLTreeNode& node,
									   CElement* Owner)
	: CComponent(node, Owner)
	, m_scriptMgr(CEngine::GetSingleton().getScriptManager())
	, m_componentStateId(s_nextComponentStateId++)
{
	std::string name = node.GetPszProperty("class", "");
	DEBUG_ASSERT(name != "");
	m_scriptClass = name;
	name = Owner->getName() + "_" + name;
	SetNameFromParentName(name);
}

CScriptedComponent::~CScriptedComponent()
{}

void CScriptedComponent::Init()
{
	m_scriptMgr->RunCode( "if (_componentStates == nil) then _componentStates = {}; end" );

	std::string script = m_scriptMgr->GetScript(m_scriptClass);
	DEBUG_ASSERT(script != "");

	m_scriptMgr->AddScriptReferenceComponent( m_scriptClass, getName() );

	sel::State* state = m_scriptMgr->getLuaState();

	(*state)["_currentComponent"]
		.SetObj(*this,
			"GetGameObject", &CComponent::GetOwner,
			"SendMessage", &CComponent::SendMsg,
			"SetEnabled", &CComponent::SetEnabled,
			"GetEnabled", &CComponent::GetEnabled);


	std::stringstream ss;
	ss << m_scriptClass << " = _currentComponent;"
		<< script
		<< "_componentStates[" << m_componentStateId << "] = _currentComponent;";
	for (auto const& prop : m_properties)
	{
		std::string val = prop.value;
		if (prop.type == "string")
		{
			val = "\"" + val + "\"";
		}
		else if ( prop.type == "array" )
		{
			val = val.substr( 1, val.size() - 2 );
			val = "{" + val + "}";
		}
		ss << m_scriptClass << "." << prop.name << " = " << val << ";";
	}
	ss << "if (_currentComponent.OnCreate ~= nil) then _currentComponent:OnCreate(); end\n";
	//OutputDebugStringA(ss.str().c_str());
	m_scriptMgr->RunCode(ss.str());
}

void CScriptedComponent::OnObjectInitialized()
{
	if ( m_Destroyed ) return;
	SetComponent();

	std::stringstream ss;
	ss << "if (_currentComponent.OnObjectInitialized ~= nil) then _currentComponent:OnObjectInitialized(); end";
	m_scriptMgr->RunCode( ss.str() );
}

void CScriptedComponent::Destroy()
{
	if ( m_Destroyed ) return;
	m_Destroyed = true;
	std::stringstream ss;
	ss << "_currentComponent = _componentStates[" << m_componentStateId << "];"
		<< "if (_currentComponent.OnDestroy ~= nil) then _currentComponent:OnDestroy(); end\n"
		<< "_currentComponent = nil;"
		<< "_componentStates[" << m_componentStateId << "] = nil;";
	m_scriptMgr->RunCode(ss.str());

	m_scriptMgr->RemoveScriptReferenceComponent( m_scriptClass, getName() );
}

void CScriptedComponent::Reload()
{
	OutputDebugStringA( ( "Reloading Component " + getName() +"\n" ).c_str() );
}


void CScriptedComponent::SetComponent()
{
	if ( m_Destroyed ) return;
	std::stringstream ss;
	ss << "_currentComponent = _componentStates[" << m_componentStateId << "];";

	m_scriptMgr->RunCode(ss.str());
}

void CScriptedComponent::Update(double ElapsedTime)
{
	if ( m_Destroyed ) return;
	if (!GetOwner()->GetEnabled())
	{
		return;
	}

	SetComponent();

	std::stringstream ss;
	ss << "if (_currentComponent.OnUpdate ~= nil) then _currentComponent:OnUpdate(" << ElapsedTime << "); end";
	m_scriptMgr->RunCode( ss.str() );
}

void CScriptedComponent::FixedUpdate(double ElapsedTime)
{
	if ( m_Destroyed ) return;
	if (!GetOwner()->GetEnabled())
	{
		return;
	}

	SetComponent();

	std::stringstream ss;
	ss << "if (_currentComponent.OnFixedUpdate ~= nil) then _currentComponent:OnFixedUpdate(" << ElapsedTime << "); end";
	m_scriptMgr->RunCode(ss.str());
}

void CScriptedComponent::Render(CContextManager&  _context)
{
	if ( m_Destroyed ) return;
	if ( ! GetOwner()->GetEnabled() )
	{
		return;
	}

	SetComponent();

	std::stringstream ss;
	ss << "if (_currentComponent.OnRender ~= nil) then _currentComponent:OnRender(); end";
	m_scriptMgr->RunCode(ss.str());
}

void CScriptedComponent::RenderDebug(CContextManager&  _context)
{
	if ( m_Destroyed ) return;
	if ( ! GetOwner()->GetEnabled() )
	{
		return;
	}

	SetComponent();

	std::stringstream ss;
	ss << "if (_currentComponent.OnRenderDebug ~= nil) then _currentComponent:OnRenderDebug(); end";
	m_scriptMgr->RunCode(ss.str());
}

void CScriptedComponent::SendMsg(const std::string& msg)
{
	if ( m_Destroyed ) return;
	SetComponent();

	std::stringstream ss;
	ss << "if (_currentComponent." << msg << " ~= nil) then _currentComponent:" << msg << "(); end";
	m_scriptMgr->RunCode(ss.str());
}

void CScriptedComponent::SendMsg(const std::string& msg, const std::string& arg1)
{
	if ( m_Destroyed ) return;
	LuaErrorCapturedStdout errorCapture;
	SetComponent();

	sel::State &state = *m_scriptMgr->getLuaState();

	state(("_r = (_currentComponent." + msg + " ~= nil);").c_str());
	if (state["_r"])
	{
		//state["_currentComponent"][msg.c_str()](arg1);
		state( ("_currentComponent:" + msg + "(\"" + arg1 + "\");").c_str() );
	}
}

void CScriptedComponent::SendMsg(const std::string& msg, float arg1)
{
	if ( m_Destroyed ) return;
	LuaErrorCapturedStdout errorCapture;
	SetComponent();

	sel::State &state = *m_scriptMgr->getLuaState();

	state(("_r = (_currentComponent." + msg + " ~= nil);").c_str());
	if (state["_r"])
	{
		//state["_currentComponent"][msg.c_str()](arg1);
		state( ("_currentComponent:" + msg + "(" + std::to_string(arg1) + ");").c_str() );
	}
}

void CScriptedComponent::SendMsg(const std::string& msg, int arg1)
{
	if ( m_Destroyed ) return;
	LuaErrorCapturedStdout errorCapture;
	SetComponent();

	sel::State &state = *m_scriptMgr->getLuaState();

	state(("_r = (_currentComponent." + msg + " ~= nil);").c_str());
	if (state["_r"])
	{
		//state["_currentComponent"][msg.c_str()](arg1);
		state( ("_currentComponent:" + msg + "(" + std::to_string(arg1) + ");").c_str() );
	}
}

void CScriptedComponent::SendMsg(const std::string& msg, int arg1, float arg2)
{
	if ( m_Destroyed ) return;
	LuaErrorCapturedStdout errorCapture;
	SetComponent();

	sel::State &state = *m_scriptMgr->getLuaState();

	state(("_r = (_currentComponent." + msg + " ~= nil);").c_str());
	if (state["_r"])
	{
		//state["_currentComponent"][msg.c_str()](arg1);
		state( ("_currentComponent:" + msg + "(" + std::to_string(arg1) + ", " + std::to_string(arg2) + ");").c_str() );
	}
}

void CScriptedComponent::SendMsg(const std::string &msg, int arg1, const std::string &arg2)
{
	if ( m_Destroyed ) return;
	LuaErrorCapturedStdout errorCapture;
	SetComponent();

	sel::State &state = *m_scriptMgr->getLuaState();

	state(("_r = (_currentComponent." + msg + " ~= nil);").c_str());
	if (state["_r"])
	{
		//state["_currentComponent"][msg.c_str()](arg1);
		state( ("_currentComponent:" + msg + "(" + std::to_string(arg1) + ", \"" + arg2 + "\");").c_str() );
	}
}

class CElemHolder
{
public:
	CElemHolder( CElement* elem ) : Element( elem ) {}
	CElement* Element;
	CElement* GetElement() { return Element; }
};

void CScriptedComponent::SendMsg(const std::string& msg, CElement* arg1)
{
	if ( m_Destroyed ) return;
	LuaErrorCapturedStdout errorCapture;
	SetComponent();

	sel::State &state = *m_scriptMgr->getLuaState();

	state(("_r = (_currentComponent." + msg + " ~= nil);").c_str());
	if (state["_r"])
	{
		CElemHolder elemHolder( arg1 );
		state["_elemHolder"].SetObj( elemHolder, "GetElement", &CElemHolder::GetElement );
		//state["_currentComponent"][msg.c_str()](arg1);
		state( ("_currentComponent:" + msg + "(_elemHolder:GetElement());").c_str() );
	}
}
