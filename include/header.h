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

// Mazo
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

class Mano {

public:
	list<Carta> cartas;
    Mano(){};
    void insertar(Carta card, int posicion=0);
    Carta sacar();
    Carta sacar(int pos);
    int largo();
    void borrar();
};

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
    void insertar(Carta card, int posicion = -1);
    Carta sacar();
    int getSize();
    void randomizar();
    void resetPos();
    void primera();
};

// Funciones en functions
void showCards(int color, int numero);
int checkAvailable(Mano hand, int color_actual, int numero_actual);
int checkRight(int color_actual, int numero_actual, int color_deseado, int num_deseado);


#endif