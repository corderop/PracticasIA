#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include<stdlib.h>
#include<time.h>


// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
	//Capturar valores de filas y columnas
	if(sensores.mensajeF != -1 && !conozcoMiPosicion){
		fil = sensores.mensajeF;
		col = sensores.mensajeC;
		conozcoMiPosicion = true;
		hayPlan = false;
	}
	else if(!conozcoMiPosicion && !hayPlan){
		int pos = hayPK(sensores);
		if(pos!=0){
		  hayPlan = pathFinding_PK(sensores, pos, plan);
		}
	}

	if(conozcoMiPosicion){
		actualizarMapa(sensores);
	}

	// Mirar si ha cambiado el destino
	if(sensores.destinoF != destino.fila or sensores.destinoC!=destino.columna){
		destino.fila = sensores.destinoF;
		destino.columna = sensores.destinoC;
		hayPlan=false;
	}

	// Calcular un camino hasta el destino
	if(!hayPlan && conozcoMiPosicion){
		actual.fila = fil;
		actual.columna = col;
		actual.orientacion = brujula;
		hayPlan = pathFinding(sensores.nivel, actual, destino, plan);
	}
	

	Action sigAccion = actIDLE;

	if(hayPlan and plan.size()>0){
		if(sensores.superficie[2]!='a'){
			if(plan.front()==actFORWARD && (sensores.terreno[2]=='P' or sensores.terreno[2]=='M' or
		     sensores.terreno[2]=='D')){
				hayPlan=false;
			}
			else{
				sigAccion = plan.front();
				plan.erase(plan.begin());
			}		
		}
	}
	else if(!hayPlan){
		srand(time(NULL));
		int num;
		static int num2 = 0;
		if(sensores.terreno[2]=='P' or sensores.terreno[2]=='M' or
		   sensores.terreno[2]=='D' or sensores.superficie[2]=='a' ){
			sigAccion = actTURN_L;
		}
		else{
			sigAccion = actFORWARD;
			num2++;
			if(num2==30){
				num=rand()%2;
				switch (num){
					case 0: sigAccion = actTURN_L; break;
					case 1: sigAccion = actTURN_R; break;
				}
				num2=0;
			}
		}

		
	}

	ultimaAccion = sigAccion;

	// Actualizar estado de la última acción
	switch(ultimaAccion){
		case actTURN_R: brujula = (brujula+1)%4; break;
		case actTURN_L: brujula = (brujula+3)%4; break;
		case actFORWARD:
			switch (brujula){
				case 0: fil--; break;
				case 1: col++; break;
				case 2: fil++; break;
				case 3: col--; break;
			}
			break;
	}

  return sigAccion;
}


// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan){
	switch (level){
		case 1: cout << "Busqueda en profundad\n";
			      return pathFinding_Profundidad(origen,destino,plan);
						break;
		case 2: cout << "Busqueda en Anchura\n";
				  return pathFinding_Anchura(origen, destino, plan);
						break;
		case 3: cout << "Busqueda Costo Uniforme\n";
				  return pathFinding_CostoUniforme(origen, destino, plan);
						break;
		case 4: cout << "Busqueda para el reto\n";
					return pathFinding_CostoUniforme(origen, destino, plan);
						break;
	}
	cout << "Comportamiento sin implementar\n";
	return false;
}


//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M' or casilla =='D')
		return true;
	else
	  return false;
}


// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}

struct nodo{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};


// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

    nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

    while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		pila.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

    cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}

// Implementación de la búsqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> cola;											// Lista de Abiertos

    nodo current;
	current.st = origen;
	current.secuencia.empty();

	cola.push(current);

    while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			cola.push(hijoTurnR);

		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			cola.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				cola.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = cola.front();
		}
	}

    cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}

// Sobrecarga para la priority_queue de costo uniforme
void ComportamientoJugador::calcularCoste(nodoConCoste &a){
	switch(mapaResultado[a.st.fila][a.st.columna]){
		case 'T': a.cost += 2;
				  break;
		case 'B': a.cost += 5;
				  break;
		case 'A': a.cost += 10;
				  break;
		case '?': a.cost += 4;
					break;
		default:  a.cost += 1;
				  break;
	}
}

multiset<nodoConCoste>::iterator ComportamientoJugador::buscarIgual(const nodoConCoste &a, const multiset<nodoConCoste> &b){
	multiset<nodoConCoste>::iterator salida = b.end();
	multiset<nodoConCoste>::iterator it;

	for(it=b.begin(); it!=b.end() && salida==b.end(); it++)
		if(a.st.fila == it->st.fila && 
		   a.st.columna == it->st.columna &&
			 a.st.orientacion == it->st.orientacion)
			 salida = it;

	return salida;
}

// Implementación de la búsqueda de costo uniforme.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	multiset<nodoConCoste> cola;    // Lista de Abiertos

  nodoConCoste current;
	current.st = origen;
	current.secuencia.empty();
	current.cost = 0;

	cola.insert(current);

    while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.erase(cola.begin());
		generados.insert(current.st);

		// Generar descendiente de avanzar
		nodoConCoste hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				calcularCoste(hijoForward);
				multiset<nodoConCoste>::iterator it = buscarIgual(hijoForward, cola);
				if(it!=cola.end()){
					if(hijoForward < (*it)){
						cola.erase(it);
						cola.insert(hijoForward);
					}
				}
				else
					cola.insert(hijoForward);	
			}
		}

		// Generar descendiente de girar a la derecha
		nodoConCoste hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			hijoTurnR.cost += 1;
			multiset<nodoConCoste>::iterator it = buscarIgual(hijoTurnR, cola);
			if(it!=cola.end()){
				if(hijoTurnR < (*it)){
					cola.erase(it);
					cola.insert(hijoTurnR);
				}
			}
			else
				cola.insert(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodoConCoste hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			hijoTurnL.cost += 1;
			multiset<nodoConCoste>::iterator it = buscarIgual(hijoTurnL, cola);
			if(it!=cola.end()){
				if(hijoTurnL < (*it)){
					cola.erase(it);
					cola.insert(hijoTurnL);
				}
			}
			else
				cola.insert(hijoTurnL);
		}

		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = (*(cola.begin()));
		}
	}

    cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}

void ComportamientoJugador::actualizarMapa(const Sensores &sensores){
	mapaResultado[fil][col] = sensores.terreno[0];

	switch(brujula){
		case 0:
			mapaResultado[fil-1][col-1] = sensores.terreno[1];
			mapaResultado[fil-1][col] = sensores.terreno[2];
			mapaResultado[fil-1][col+1] = sensores.terreno[3];
			mapaResultado[fil-2][col-2] = sensores.terreno[4];
			mapaResultado[fil-2][col-1] = sensores.terreno[5];
			mapaResultado[fil-2][col] = sensores.terreno[6];
			mapaResultado[fil-2][col+1] = sensores.terreno[7];
			mapaResultado[fil-2][col+2] = sensores.terreno[8];
			mapaResultado[fil-3][col-3] = sensores.terreno[9];
			mapaResultado[fil-3][col-2] = sensores.terreno[10];
			mapaResultado[fil-3][col-1] = sensores.terreno[11];
			mapaResultado[fil-3][col] = sensores.terreno[12];
			mapaResultado[fil-3][col+1] = sensores.terreno[13];
			mapaResultado[fil-3][col+2] = sensores.terreno[14];
			mapaResultado[fil-3][col+3] = sensores.terreno[15];
			break;
		case 1:
			mapaResultado[fil-1][col+1] = sensores.terreno[1];
			mapaResultado[fil][col+1] = sensores.terreno[2];
			mapaResultado[fil+1][col+1] = sensores.terreno[3];
			mapaResultado[fil-2][col+2] = sensores.terreno[4];
			mapaResultado[fil-1][col+2] = sensores.terreno[5];
			mapaResultado[fil][col+2] = sensores.terreno[6];
			mapaResultado[fil+1][col+2] = sensores.terreno[7];
			mapaResultado[fil+2][col+2] = sensores.terreno[8];
			mapaResultado[fil-3][col+3] = sensores.terreno[9];
			mapaResultado[fil-2][col+3] = sensores.terreno[10];
			mapaResultado[fil-1][col+3] = sensores.terreno[11];
			mapaResultado[fil][col+3] = sensores.terreno[12];
			mapaResultado[fil+1][col+3] = sensores.terreno[13];
			mapaResultado[fil+2][col+3] = sensores.terreno[14];
			mapaResultado[fil+3][col+3] = sensores.terreno[15];
			break;
		case 2:
			mapaResultado[fil+1][col+1] = sensores.terreno[1];
			mapaResultado[fil+1][col] = sensores.terreno[2];
			mapaResultado[fil+1][col-1] = sensores.terreno[3];
			mapaResultado[fil+2][col+2] = sensores.terreno[4];
			mapaResultado[fil+2][col+1] = sensores.terreno[5];
			mapaResultado[fil+2][col] = sensores.terreno[6];
			mapaResultado[fil+2][col-1] = sensores.terreno[7];
			mapaResultado[fil+2][col-2] = sensores.terreno[8];
			mapaResultado[fil+3][col+3] = sensores.terreno[9];
			mapaResultado[fil+3][col+2] = sensores.terreno[10];
			mapaResultado[fil+3][col+1] = sensores.terreno[11];
			mapaResultado[fil+3][col] = sensores.terreno[12];
			mapaResultado[fil+3][col-1] = sensores.terreno[13];
			mapaResultado[fil+3][col-2] = sensores.terreno[14];
			mapaResultado[fil+3][col-3] = sensores.terreno[15];
			break;
		case 3:
			mapaResultado[fil+1][col-1] = sensores.terreno[1];
			mapaResultado[fil][col-1] = sensores.terreno[2];
			mapaResultado[fil-1][col-1] = sensores.terreno[3];
			mapaResultado[fil+2][col-2] = sensores.terreno[4];
			mapaResultado[fil+1][col-2] = sensores.terreno[5];
			mapaResultado[fil][col-2] = sensores.terreno[6];
			mapaResultado[fil-1][col-2] = sensores.terreno[7];
			mapaResultado[fil-2][col-2] = sensores.terreno[8];
			mapaResultado[fil+3][col-3] = sensores.terreno[9];
			mapaResultado[fil+2][col-3] = sensores.terreno[10];
			mapaResultado[fil+1][col-3] = sensores.terreno[11];
			mapaResultado[fil][col-3] = sensores.terreno[12];
			mapaResultado[fil-1][col-3] = sensores.terreno[13];
			mapaResultado[fil-2][col-3] = sensores.terreno[14];
			mapaResultado[fil-3][col-3] = sensores.terreno[15];
			break;
	}
}

bool ComportamientoJugador::pathFinding_PK(const Sensores &sensores, int destino, list<Action> &plan){
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estadoPK,ComparaEstadosPK> generados; // Lista de Cerrados
	queue<nodoPK> cola;    // Lista de Abiertos

  nodoPK current;
	current.st.pos = 0;
	current.st.orientacion = 0;
	current.secuencia.empty();

	cola.push(current);

  while (!cola.empty() and current.st.pos!=destino){

		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de avanzar
		nodoPK hijoForward = current;
		int aux = hijoForward.st.pos;
		switch (hijoForward.st.orientacion) {
			case 0: switch(hijoForward.st.pos){
								case 0: hijoForward.st.pos += 2; break;
								case 1:
								case 2:
								case 3:  hijoForward.st.pos += 4 ; break;
								case 4: 
								case 5: 
								case 6:
								case 7:
								case 8:  hijoForward.st.pos += 6 ; break;
							}
							break;
			case 1: switch(hijoForward.st.pos){
								case 1:  hijoForward.st.pos += 1 ; break;
								case 2:  hijoForward.st.pos += 1 ; break;
								case 4:  hijoForward.st.pos += 1 ; break;
								case 5:  hijoForward.st.pos += 1 ; break;
								case 6:  hijoForward.st.pos += 1 ; break;
								case 7:  hijoForward.st.pos += 1 ; break;
								case 9:  hijoForward.st.pos += 1 ; break;
								case 10:  hijoForward.st.pos += 1 ; break;
								case 11:  hijoForward.st.pos += 1 ; break;
								case 12:  hijoForward.st.pos += 1 ; break;
								case 13:  hijoForward.st.pos += 1 ; break;
								case 14:  hijoForward.st.pos += 1 ; break;
							}
							break;
			case 2: switch(hijoForward.st.pos){
								case 2:  hijoForward.st.pos -= 2 ; break;
								case 5: 
								case 6: 
								case 7:  hijoForward.st.pos -= 4 ; break;
								case 10: 
								case 11:
								case 12: 
								case 13: 
								case 14:  hijoForward.st.pos -= 6 ; break;
							}
							break;
			case 3: switch(hijoForward.st.pos){
								case 2:  hijoForward.st.pos -= 1 ; break;
								case 3:  hijoForward.st.pos -= 1 ; break;
								case 5:  hijoForward.st.pos -= 1 ; break;
								case 6:  hijoForward.st.pos -= 1 ; break;
								case 7:  hijoForward.st.pos -= 1 ; break;
								case 8:  hijoForward.st.pos -= 1 ; break;
								case 10:  hijoForward.st.pos -= 1 ; break;
								case 11:  hijoForward.st.pos -= 1 ; break;
								case 12:  hijoForward.st.pos -= 1 ; break;
								case 13:  hijoForward.st.pos -= 1 ; break;
								case 14:  hijoForward.st.pos -= 1 ; break;
								case 15:  hijoForward.st.pos -= 1 ; break;
							}
							break;
		}
		if(!EsObstaculo(sensores.terreno[hijoForward.st.pos])){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				cola.push(hijoForward);
			}
		}

		// Generar descendiente de girar a la derecha
		nodoPK hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			cola.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodoPK hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			cola.push(hijoTurnL);
		}

		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = cola.front();
		}
	}

    cout << "Terminada la busqueda\n";

	if (current.st.pos == destino){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		// PintaPlan(plan);
		// // ver el plan en el mapa
		// VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}


	return false;
}


int ComportamientoJugador::hayPK(Sensores sensores){
	int salida=0;

	for(int i=1; i<sensores.terreno.size() && salida == 0; i++)
		if(sensores.terreno[i]=='K')
			salida = i;

	return salida;
}

// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}



void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}


// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
