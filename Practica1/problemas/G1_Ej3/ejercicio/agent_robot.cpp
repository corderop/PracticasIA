#include "agent_robot.h"
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
	
	if(!BUMPER_){
		if(!distanciaCalc){
			if(girando){
				accion = Agent::actTURN_L;
				girando = false;
			}
			else{
				if(!CNY70_){
					accion = Agent::actFORWARD;
					npasos++;
				}
				else{
					if(fVisitada){
						cout << npasos <<endl;
						accion = Agent::actIDLE;
						distanciaCalc = true;
					}
					else{
						accion = Agent::actTURN_L;
						npasos = 1;
						fVisitada = true;
						girando = true;
					}
				}
			}
		}
		else{
			if(CNY70_ ){
				switch (giro){
					case 0:
						accion = Agent::actTURN_L;
						cout<< giro++ <<endl;
						break;
					case 1:
						accion = Agent::actTURN_L;
						cout<< giro++ <<endl;
						break;
					case 2:
						accion = Agent::actTURN_L;
						cout<< giro++ <<endl;
						break;
					case 4:
						accion = Agent::actTURN_L;
						cout<< giro++ <<endl;
						break;
					case 5:
						accion = Agent::actTURN_R;
						cout<< giro++ <<endl;
						break;
					case 7:
						accion = Agent::actTURN_R;
						cout<< giro <<endl;
						giro = 2;
						break;
					default:
						accion = Agent::actTURN_L;
						break;
				}
			}
			else{
				switch (giro){
					case 3:
						accion = Agent::actFORWARD;
						cout<< giro++ <<endl;
						break;
					case 4:
						accion = Agent::actTURN_L;
						cout<< giro++ <<endl;
						break;
					case 6:
						accion = Agent::actFORWARD;
						cout<< giro++ <<endl;
						break;
					case 7:
						accion = Agent::actTURN_R;
						cout<< giro <<endl;
						giro = 2;
						break;
					default:
						accion = Agent::actFORWARD;
						break;
				}
			}
		}
	}
	else{
			
	}

	return accion;

}
// -----------------------------------------------------------
void Agent::Perceive(const Environment &env)
{
	CNY70_ = env.isFrontier();
	BUMPER_ = env.isBump();
}
// -----------------------------------------------------------
string ActionStr(Agent::ActionType accion)
{
	switch (accion)
	{
	case Agent::actFORWARD: return "FORWARD";
	case Agent::actTURN_L: return "TURN LEFT";
	case Agent::actTURN_R: return "TURN RIGHT";
	case Agent::actBACKWARD: return "BACKWARD";
	case Agent::actPUSH: return "PUSH";
	case Agent::actIDLE: return "IDLE";
	default: return "???";
	}
}
