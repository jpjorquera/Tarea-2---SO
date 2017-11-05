#include<iostream>
//#include "./mazo.cpp"
using namespace std;
#include "../include/header.h"

/*// Definicion de tipos de cartas
enum colores {azul, rojo, verde, amarillo, negro};
enum tipo {mas2=10, reversa=11, salto=12, colores=13, mas4=14};*/

void showCards(int color, int numero) {
	switch (color) {
                case azul:
                    cout << "[Azul] " ;
                    break;
                case rojo:
                    cout << "[Rojo] ";
                    break;
                case verde:
                    cout << "[Verde] ";
                    break;
                case amarillo:
                    cout << "[Amarillo] ";
                    break;
                case negro:
                    cout << "[Negro] ";
            }
            switch (numero) {
                case mas2:
                    cout << "+2 ";
                    break;
                case reversa:
                    cout << "Reversa ";
                    break;
                case salto:
                    cout << "Salto ";
                    break;
                case colores:
                    cout << "Cambio color ";
                    break;
                case mas4:
                    cout << "+4 ";
                    break;
                default:
                    cout << numero;
            }
}

int checkAvailable(Mano hand, int color_actual, int numero_actual) {
	list<Carta>::iterator it;
	for (it = hand.cartas.begin(); it != hand.cartas.end(); it++) {
                Carta carta_actual = *it;
                int col_aux = carta_actual.getColor();
                int num_aux = carta_actual.getNum();
                if (col_aux == negro) {
                	return 1;
                }
                if (num_aux == numero_actual) {
                	return 1;
                }
                if (col_aux == color_actual) {
                	return 1;
                }
    }
	return 0;
}