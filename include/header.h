#ifndef HEADER_H_
#define HEADER_H_

#include<iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <string>
#include <time.h>
#include <list>
using namespace std;

// Definicion de tipos de cartas
enum colores {azul, rojo, verde, amarillo, negro};
enum tipo {mas2=10, reversa=11, salto=12, colores=13, mas4=14};

// Clase Carta para identificarlas con color y número
class Carta {
private:
    int numero;
    int color;
 
public:
    Carta(int col=-1, int num=-1) {
    	numero = num;
    	color = col;
    };
    int getNum();
    int getColor();
    void setNum(int num);
    void setColor(int col);
};

// Clase Mano como lista de C++
class Mano {

public:
	list<Carta> cartas;
    Mano(){};
    // Insertar carta, en posición
    void insertar(Carta card, int posicion=0);
    // Sacar última carta || Según posición
    Carta sacar();
    Carta sacar(int pos);
    // Devolver el tamaño
    int largo();
    // Liberar memoria
    void borrar();
};

// Clase Mazo como array de tamaño definido
class Mazo {
	int size;
	int size_actual;
	int pos_actual;
	Carta cartas[108];

public:
    Mazo(int tam = 108){
    	size = size_actual = tam;
    	pos_actual = 0;
    };
    // Insertar / Sacar Carta
    void insertar(Carta card, int posicion = -1);
    Carta sacar();
    // Obtener tamaño
    int getSize();
    // Desordenar el Mazo
    void randomizar();
    // Devolver al principio
    void resetPos();
};

// Funciones en functions

// Imprimir la carta según color y número
void showCards(int color, int numero);
// Revisar si hay cartas jugables en la mano
int checkAvailable(Mano hand, int color_actual, int numero_actual);
// Revisar jugada correcta
int checkRight(int color_actual, int numero_actual, int color_deseado, int num_deseado);


#endif