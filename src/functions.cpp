#include<iostream>
using namespace std;
#include "../include/header.h"

// Imprimir Color y Número de Carta
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

// Verificar cartas jugables en la mano
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

// Verificar jugada correcta
int checkRight(int color_actual, int numero_actual, int color_deseado, int num_deseado) {
    if (color_deseado == negro) {
        return 1;
    }
    else if (color_deseado == color_actual) {
        return 1;
    }
    else if (num_deseado == numero_actual) {
        return 1;
    }
    else {
        return 0;
    }
}