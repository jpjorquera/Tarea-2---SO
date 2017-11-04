#include<iostream>
using namespace std;

// Definicion de tipos de cartas
enum colores {azul, rojo, verde, amarillo, negro};
enum tipo {mas2=10, reversa=11, salto=12, colores=13, mas4=14};

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