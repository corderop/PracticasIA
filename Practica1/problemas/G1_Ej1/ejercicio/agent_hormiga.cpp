#include "agent_hormiga.h"
#include "environment.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <utility>

using namespace std;

// -----------------------------------------------------------
Agent::ActionType Agent::Think()
{
	ActionType accion = Agent::actIDLE;
	
	/* ESCRIBA AQUI LAS REGLAS */
	// 1. Ver si tiene una feromona. Si ta tiene avanza
	if( FEROMONA_ ){
		accion = Agent::actFORWARD;
		prohibirgiro = false;
	}
	else{
		if(!prohibirgiro){
			accion = Agent::actTURN_R;
			prohibirgiro = true;
		}
		else{
			accion = Agent::actTURN_L;
		}
	}
	
	return accion;

}
// -----------------------------------------------------------
void Agent::Perceive(const Environment &env)
{
	FEROMONA_ = env.isFeromona();
}
// -----------------------------------------------------------
string ActionStr(Agent::ActionType accion)
{
	switch (accion)
	{
	case Agent::actFORWARD: return "FORWARD";
	case Agent::actTURN_L: return "TURN LEFT";
	case Agent::actTURN_R: return "TURN RIGHT";
	case Agent::actIDLE: return "IDLE";
	default: return "????";
	}
}
