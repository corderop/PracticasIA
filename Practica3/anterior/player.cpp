#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}
 
// ------------------- Los tres metodos anteriores no se pueden modificar

// int numeroAlr(const Environment &estado, int jugador, int row, int col){
//     int salida = 0;

//     if(row>0){
//         (estado.See_Casilla(row-1, col) == jugador) ? salida++ : salida ;
//         if(col>0){
//             (estado.See_Casilla(row-1, col-1) == jugador) ? salida++ : salida;
//         }
//         if(col<6){
//             (estado.See_Casilla(row-1, col+1) == jugador) ? salida++ : salida;
//         }
//     }
//     if(row<6){
//         (estado.See_Casilla(row+1, col) == jugador) ? salida++ : salida ;
//         if(col>0){
//             (estado.See_Casilla(row+1, col-1) == jugador) ? salida++ : salida;
//         }
//         if(col<6){
//             (estado.See_Casilla(row+1, col+1) == jugador) ? salida++ : salida;
//         }
//     }
//     if(col>0){
//         (estado.See_Casilla(row, col-1) == jugador) ? salida++ : salida;
//     }
//     if(col<6){
//         (estado.See_Casilla(row, col+1) == jugador) ? salida++ : salida;
//     }

//     return salida;
// };

// int puntAlr(const Environment &estado, int jugador){
//     int salida = 0,
//         ocupadas = 7*7 - estado.Get_Casillas_Libres(),
//         vistos = 0;

//     for(int i=0; i<7 && vistos<ocupadas; i++)
//         for(int j=0; j<7 && vistos<ocupadas; j++)
//             if(estado.See_Casilla(i, j) == jugador){
//                 salida += numeroAlr(estado, jugador, i, j);
//                 vistos++;
//             }

//     return salida;
// }

double posibilidadFilas(const Environment &estado, int jugador){
    int contador = 0,
        posibilidades = 0,
        numeroPosibilidades = 0,
        sumaPosibilidades = 0,
        vacios = 0;
    const int MAX_RES = 4;

    for(int i=0; i<7 && vacios!=7; i++){
        contador = 0;
        posibilidades = 0;
        vacios = 0;
        for(int j=0; j<7 && vacios!=7; j++){
            int casilla = estado.See_Casilla(i,j);
            if(casilla == jugador || casilla == jugador+2 || casilla == 0){
                contador++;
                (casilla == 0) ? vacios++ : vacios;
            }
            else{
                contador = 0;
            }
            if(contador>=4)
                posibilidades++;
        }
        
        sumaPosibilidades += posibilidades/MAX_RES;
        numeroPosibilidades++;
    }

    return sumaPosibilidades/numeroPosibilidades;
}

double posibilidadColumnas(const Environment &estado, int jugador){
    int contador = 0,
        posibilidades = 0,
        numeroPosibilidades = 0,
        sumaPosibilidades = 0,
        vacios = 0;
    const int MAX_RES = 4;

    for(int i=0; i<7; i++){
        contador = 0;
        posibilidades = 0;
        vacios = 0;
        for(int j=0; j<7; j++){
            int casilla = estado.See_Casilla(j,i);
            if(casilla == jugador || casilla == jugador+2 || casilla == 0){
                contador++;
            }
            else{
                contador = 0;
            }
            if(contador>=4)
                posibilidades++;
        }
        
        sumaPosibilidades += posibilidades/MAX_RES;
        numeroPosibilidades++;
    }

    return sumaPosibilidades/numeroPosibilidades;
}

double Player::Heuristica(const Environment &estado, int jugador){
    int valor=0;

    // if(jugador_ == 1){
        // if(estado.JuegoTerminado()){
        //     switch(estado.RevisarTablero()){
        //         case 0: return -5;
        //         case 1: return 60;
        //         case 2: return -10;
        //     }
        // }
        // else{
        int prob1 = (posibilidadColumnas(estado, jugador_)+posibilidadFilas(estado, jugador_))/2;
        int prob2 = (jugador_ == 1) ? (posibilidadColumnas(estado, 2)+posibilidadFilas(estado, 2))/2 : (posibilidadColumnas(estado, 1)+posibilidadFilas(estado, 1))/2;

        valor -= prob1*10000;
        valor += prob2*10000;

        return valor;
        // }
    // }
    // else{
        // if(estado.JuegoTerminado()){
        //     switch(estado.RevisarTablero()){
        //         case 0: return -5;
        //         case 1: return -10;
        //         case 2: return 60;
        //     }
        // }
        // else{
            // int prob1 = (posibilidadColumnas(estado, 1)+posibilidadFilas(estado, 1))/2;
            // int prob2 = (posibilidadColumnas(estado, 2)+posibilidadFilas(estado, 2))/2;

            // valor += prob1*5000;
            // valor -= prob2*5000;

            // return valor;
    // }

    // return valor;
}

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Player::Valoracion(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador_)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Heuristica(estado, jugador);
}





// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}


double Player::Poda_AlfaBeta (const Environment &env, int jug, int prof, Environment::ActionType &accion, double alpha, double beta){
    if(prof == 0 || env.JuegoTerminado()){
        if(jug==1)
            return ValoracionTest(env, jug);
        else
            return -ValoracionTest(env, jug);
    }

    if(jug == 1){
        bool vect[8];
        double value = -999999999.0;
        int num = env.possible_actions(vect);
        int anterior = -1;
        Environment::ActionType act;

        for(int i=0; i<num; i++){
            Environment hijo = env.GenerateNextMove(anterior);
            double MM = Poda_AlfaBeta(hijo, 2, prof-1, act, alpha, beta);
            if(MM>value){
                value = MM;
                accion = static_cast<Environment::ActionType>(anterior); 
            }
            alpha = (value>alpha) ? value : alpha;
            if(alpha>=beta)
                break;
        }

        return value;
    }
    else{
        bool vect[8];
        double value = +999999999.0;
        int num = env.possible_actions(vect);
        int anterior = -1;
        Environment::ActionType act;

        for(int i=0; i<num; i++){
            Environment hijo = env.GenerateNextMove(anterior);
            double MM = Poda_AlfaBeta(hijo, 1, prof-1, act, alpha, beta);
            if(MM<value){
                value = MM;
                accion = static_cast<Environment::ActionType>(anterior); 
            }
            beta = (value<beta) ? value : beta;
            if(alpha>=beta)
                break;
        }

        return value;
    }
}

double Player::minimax(const Environment &env, int jug, int prof, Environment::ActionType &accion){
    if(prof == 0 || env.JuegoTerminado()){
        if(jug==1)
            return ValoracionTest(env, jug);
        else
            return -ValoracionTest(env, jug);
    }

    if(jug == 1){
        bool vect[8];
        double value = -999999999.0;
        int num = env.possible_actions(vect);
        int anterior = -1;
        Environment::ActionType act;

        for(int i=0; i<num; i++){
            Environment hijo = env.GenerateNextMove(anterior);
            double MM = minimax(hijo, 2, prof-1, act);
            if(MM>value){
                value = MM;
                accion = static_cast<Environment::ActionType>(anterior); 
            }
        }

        return value;
    }
    else{
        bool vect[8];
        double value = +999999999.0;
        int num = env.possible_actions(vect);
        int anterior = -1;
        Environment::ActionType act;

        for(int i=0; i<num; i++){
            Environment hijo = env.GenerateNextMove(anterior);
            double MM = minimax(hijo, 1, prof-1, act);
            if(MM<value){
                value = MM;
                accion = static_cast<Environment::ActionType>(anterior); 
            }
        }

        return value;
    }
}



// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acci�n que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = -999999999.0, 
           beta = +999999999.0; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;


    //--------------------- COMENTAR Desde aqui
   //  cout << "\n\t";
   //  int n_opciones=0;
   //  JuegoAleatorio(aplicables, opciones, n_opciones);

   //  if (n_act==0){
   //    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
   //    cout << " No puede realizar ninguna accion!!!\n";
   //    //accion = Environment::actIDLE;
   //  }
   //  else if (n_act==1){
   //         (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
   //          cout << " Solo se puede realizar la accion "
   //               << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
   //          accion = static_cast< Environment::ActionType > (opciones[0]);

   //       }
   //       else { // Hay que elegir entre varias posibles acciones
   //          int aleatorio = rand()%n_opciones;
   //          cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
   //          accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
   //       }

    //--------------------- COMENTAR Hasta aqui


    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    valor = Poda_AlfaBeta(actual_, jugador_, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
    // valor = minimax(actual_, jugador_, PROFUNDIDAD_MINIMAX, accion);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    return accion;
}

