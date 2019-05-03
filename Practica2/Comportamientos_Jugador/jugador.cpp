#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>


// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
	//Capturar valores de filas y columnas
	if(sensores.mensajeF != -1 && !conozcoMiPosicion){
		fil = sensores.mensajeF;
		col = sensores.mensajeC;
		actualizarMapa(sensores);
		conozcoMiPosicion = true;
		hayPlan = false;
	}
	else if(!conozcoMiPosicion){
		int pos = hayPK(sensores);
		if(pos!=0 && !hayPlan){
			actual.fila = fil;
			actual.columna = col;
			actual.orientacion = brujula;
			estado stPk = buscarPK(actual, pos); 
		  hayPlan = pathFinding_PK(actual, stPk, plan);
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
		// if(mapaResultado[destino.fila][destino.columna]!='?'){
			actual.fila = fil;
			actual.columna = col;
			actual.orientacion = brujula;
			hayPlan = pathFinding(sensores.nivel, actual, destino, plan);
		// }
		// else if( sensores.nivel == 4 && conozcoMiPosicion){
		// 	actual.fila = fil;
		// 	actual.columna = col;
		// 	actual.orientacion = brujula;
		// 	hayPlan = pathFinding_CostoUniforme(actual, destino, plan);
		// }
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
		if(sensores.terreno[2]=='P' or sensores.terreno[2]=='M' or
		   sensores.terreno[2]=='D' or sensores.superficie[2]=='a' ){
			sigAccion = actTURN_R;
		}
		else{
			sigAccion = actFORWARD;
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


bool EsObstaculoNivel2(unsigned char casilla){
	if (casilla=='P' or casilla=='M' or casilla =='D' or casilla == '?' )
		return true;
	else
	  return false;
}


// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelanteNivel2(estado &st){
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
	if (!EsObstaculoNivel2(mapaResultado[fil][col])){
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

// Implementación de la búsqueda de costo uniforme.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	priority_queue<nodoConCoste> cola;    // Lista de Abiertos

    nodoConCoste current;
	current.st = origen;
	current.secuencia.empty();
	current.cost = 0;

	cola.push(current);

    while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de avanzar
		nodoConCoste hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				calcularCoste(hijoForward);
				cola.push(hijoForward);
			}
		}

		// Generar descendiente de girar a la derecha
		nodoConCoste hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			hijoTurnR.cost += 1;
			cola.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodoConCoste hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			hijoTurnL.cost += 1;
			cola.push(hijoTurnL);
		}

		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = cola.top();
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

// Implementación del nivel 2.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Nivel2(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	priority_queue<nodoConCoste> cola;    // Lista de Abiertos

  nodoConCoste current;
	current.st = origen;
	current.secuencia.empty();

	cola.push(current);

    while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de avanzar
		nodoConCoste hijoForward = current;
		if (!HayObstaculoDelanteNivel2(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				calcularCoste(hijoForward);
				cola.push(hijoForward);
			}
		}

		// Generar descendiente de girar a la derecha
		nodoConCoste hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			hijoTurnR.cost = 1;
			cola.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodoConCoste hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			hijoTurnL.cost = 1;
			cola.push(hijoTurnL);
		}

		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = cola.top();
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

bool ComportamientoJugador::pathFinding_PK(const estado &origen, const estado &destino, list<Action> &plan){
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> cola;    // Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	cola.push(current);

    while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){

		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		switch (hijoForward.st.orientacion) {
			case 0: hijoForward.st.fila--; break;
			case 1: hijoForward.st.columna++; break;
			case 2: hijoForward.st.fila++; break;
			case 3: hijoForward.st.columna--; break;
		}
		if (generados.find(hijoForward.st) == generados.end()){
			hijoForward.secuencia.push_back(actFORWARD);
			cola.push(hijoForward);
		}

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

estado ComportamientoJugador::buscarPK(const estado &actual, int pos){
	estado salida;
	salida.orientacion = 0;

	switch(actual.orientacion){
		case 0: switch(pos){
							case 1: salida.fila = actual.fila-1;
											salida.columna = actual.columna-1;
											break;
							case 2: salida.fila = actual.fila-1;
											salida.columna = actual.columna;
											break;
							case 3: salida.fila = actual.fila-1;
											salida.columna = actual.columna+1;
											break;
							case 4: salida.fila = actual.fila-2;
											salida.columna = actual.columna-2;
											break;
							case 5: salida.fila = actual.fila-2;
											salida.columna = actual.columna-1;
											break;
							case 6: salida.fila = actual.fila-2;
											salida.columna = actual.columna;
											break;
							case 7: salida.fila = actual.fila-2;
											salida.columna = actual.columna+1;
											break;
							case 8: salida.fila = actual.fila-2;
											salida.columna = actual.columna+2;
											break;
							case 9: salida.fila = actual.fila-3;
											salida.columna = actual.columna-3;
											break;
							case 10: salida.fila = actual.fila-3;
											salida.columna = actual.columna-2;
											break;
							case 11: salida.fila = actual.fila-3;
											salida.columna = actual.columna-1;
											break;
							case 12: salida.fila = actual.fila-3;
											salida.columna = actual.columna;
											break;
							case 13: salida.fila = actual.fila-3;
											salida.columna = actual.columna+1;
											break;
							case 14: salida.fila = actual.fila-3;
											salida.columna = actual.columna+2;
											break;
							case 15: salida.fila = actual.fila-3;
											salida.columna = actual.columna+3;
											break;
						}
						break;
		case 1: switch(pos){
							case 1: salida.fila = actual.fila-1;
											salida.columna = actual.columna+1;
											break;
							case 2: salida.fila = actual.fila;
											salida.columna = actual.columna+1;
											break;
							case 3: salida.fila = actual.fila+1;
											salida.columna = actual.columna+1;
											break;
							case 4: salida.fila = actual.fila-2;
											salida.columna = actual.columna+2;
											break;
							case 5: salida.fila = actual.fila-1;
											salida.columna = actual.columna+2;
											break;
							case 6: salida.fila = actual.fila;
											salida.columna = actual.columna+2;
											break;
							case 7: salida.fila = actual.fila+1;
											salida.columna = actual.columna+2;
											break;
							case 8: salida.fila = actual.fila+2;
											salida.columna = actual.columna+2;
											break;
							case 9: salida.fila = actual.fila-3;
											salida.columna = actual.columna+3;
											break;
							case 10: salida.fila = actual.fila-2;
											salida.columna = actual.columna+3;
											break;
							case 11: salida.fila = actual.fila-1;
											salida.columna = actual.columna+3;
											break;
							case 12: salida.fila = actual.fila;
											salida.columna = actual.columna+3;
											break;
							case 13: salida.fila = actual.fila+1;
											salida.columna = actual.columna+3;
											break;
							case 14: salida.fila = actual.fila+2;
											salida.columna = actual.columna+3;
											break;
							case 15: salida.fila = actual.fila+3;
											salida.columna = actual.columna+3;
											break;
						}
						break;
		case 2: switch(pos){
							case 1: salida.fila = actual.fila+1;
											salida.columna = actual.columna+1;
											break;
							case 2: salida.fila = actual.fila+1;
											salida.columna = actual.columna;
											break;
							case 3: salida.fila = actual.fila+1;
											salida.columna = actual.columna-1;
											break;
							case 4: salida.fila = actual.fila+2;
											salida.columna = actual.columna+2;
											break;
							case 5: salida.fila = actual.fila+2;
											salida.columna = actual.columna+1;
											break;
							case 6: salida.fila = actual.fila+2;
											salida.columna = actual.columna;
											break;
							case 7: salida.fila = actual.fila+2;
											salida.columna = actual.columna-1;
											break;
							case 8: salida.fila = actual.fila+2;
											salida.columna = actual.columna-2;
											break;
							case 9: salida.fila = actual.fila+3;
											salida.columna = actual.columna+3;
											break;
							case 10: salida.fila = actual.fila+3;
											salida.columna = actual.columna+2;
											break;
							case 11: salida.fila = actual.fila+3;
											salida.columna = actual.columna+1;
											break;
							case 12: salida.fila = actual.fila+3;
											salida.columna = actual.columna;
											break;
							case 13: salida.fila = actual.fila+3;
											salida.columna = actual.columna-1;
											break;
							case 14: salida.fila = actual.fila+3;
											salida.columna = actual.columna-2;
											break;
							case 15: salida.fila = actual.fila+3;
											salida.columna = actual.columna-3;
											break;
						}
						break;
		case 3: switch(pos){
							case 1: salida.fila = actual.fila+1;
											salida.columna = actual.columna-1;
											break;
							case 2: salida.fila = actual.fila;
											salida.columna = actual.columna-1;
											break;
							case 3: salida.fila = actual.fila-1;
											salida.columna = actual.columna-1;
											break;
							case 4: salida.fila = actual.fila+2;
											salida.columna = actual.columna-2;
											break;
							case 5: salida.fila = actual.fila+1;
											salida.columna = actual.columna-2;
											break;
							case 6: salida.fila = actual.fila;
											salida.columna = actual.columna-2;
											break;
							case 7: salida.fila = actual.fila-1;
											salida.columna = actual.columna-2;
											break;
							case 8: salida.fila = actual.fila-2;
											salida.columna = actual.columna-2;
											break;
							case 9: salida.fila = actual.fila+3;
											salida.columna = actual.columna-3;
											break;
							case 10: salida.fila = actual.fila+2;
											salida.columna = actual.columna-3;
											break;
							case 11: salida.fila = actual.fila+1;
											salida.columna = actual.columna-3;
											break;
							case 12: salida.fila = actual.fila;
											salida.columna = actual.columna-3;
											break;
							case 13: salida.fila = actual.fila-1;
											salida.columna = actual.columna-3;
											break;
							case 14: salida.fila = actual.fila-2;
											salida.columna = actual.columna-3;
											break;
							case 15: salida.fila = actual.fila-3;
											salida.columna = actual.columna-3;
											break;
						}
						break;
	}

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
