#ifndef PLAYER_H
#define PLAYER_H

#include "environment.h"

class Player{
    public:
      Player(int jug);
      Environment::ActionType Think();
      void Perceive(const Environment &env);
    private:
      int jugador_;
      Environment actual_;
      double Poda_AlfaBeta(const Environment &env, int jug, int profundidad, Environment::ActionType &accion, double alpha, double beta);
      double minimax(const Environment &env, int jug, int prof, Environment::ActionType &accion);
      double Valoracion(const Environment &estado, int jugador);
      double Heuristica(const Environment &estado, int jugador);
};
#endif
