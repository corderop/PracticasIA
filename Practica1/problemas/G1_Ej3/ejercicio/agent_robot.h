#ifndef AGENT__
#define AGENT__

#include <string>
#include <iostream>
using namespace std;

// -----------------------------------------------------------
//				class Agent
// -----------------------------------------------------------
class Environment;
class Agent
{
public:
	Agent(){
		CNY70_=false;
		BUMPER_=false;
		fVisitada = false;
		npasos= 0;
		girando = false;
		giro = 0;
		distanciaCalc = false;
		recorriendo = false;
	}

	enum ActionType
	{
	    actFORWARD,
	    actTURN_L,
	    actTURN_R,
		actBACKWARD,
		actPUSH,
		actIDLE
	};

	void Perceive(const Environment &env);
	ActionType Think();

private:
	bool CNY70_;
	bool BUMPER_;
	bool fVisitada;
	bool girando; 
	int npasos;
	int giro;
	bool recorriendo;
	bool distanciaCalc;
};

string ActionStr(Agent::ActionType);

#endif
