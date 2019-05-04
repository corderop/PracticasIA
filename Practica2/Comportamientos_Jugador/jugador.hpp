#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <set>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

struct nodoConCoste{
	estado st;
	list<Action> secuencia;
	int cost;

	bool operator<(const nodoConCoste& b) const{
    if(cost == b.cost){
      return secuencia.size() < b.secuencia.size();
    }
    else
      return cost < b.cost;
  }
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      hayPlan = false;
      conozcoMiPosicion = false;
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      hayPlan = false;
      conozcoMiPosicion = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    int fil, col, brujula;
    estado actual, destino;
    list<Action> plan;

    // Nuevas variables de estado
    Action ultimaAccion;
    bool hayPlan;
    bool conozcoMiPosicion;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_CostoUniforme(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Nivel2(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_PK(const estado &origen, const estado &destino, list<Action> &plan);
    void calcularCoste(nodoConCoste &a);
    int hayPK(Sensores sensores);
    estado buscarPK(const estado &actual, int pos);

    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);
    bool HayObstaculoDelanteNivel2(estado &st);
    void actualizarMapa(const Sensores &sensores);
    multiset<nodoConCoste>::iterator buscarIgual(const nodoConCoste &a, const multiset<nodoConCoste> &b);

};

#endif
