#include<iostream>
#include <unistd.h>
#include "./mazo.cpp"
using namespace std;

enum colores {azul, rojo, verde, amarillo, negro};
enum tipo {mas2=10, reversa=11, salto=12, colores=13, mas4=14};

int main(int argc, char const *argv[]) {
    Lista_cartas mazo = Lista_cartas();
    for (int color=0; color<5; color++) {
        Carta card;
        int cant_cartas = 0;
        if (color == 4){
            for (int i=3; i<5; i++) {
                card = Carta(color, 10+i);
                cant_cartas = 4;
                int j=0;
                while (j<cant_cartas) {
                    mazo.insertar(card);
                    j++;
                }
            }

        }
        else {
            for (int i=0; i<13; i++) {
                if (i>=10){
                    card = Carta(color, i);
                    cant_cartas = 2;
                }
                else {
                    card = Carta(color, i);
                    if (i>0) {
                        cant_cartas = 2;
                    }
                    else {
                        cant_cartas=1;
                    }
                }
                int j=0;
                while (j<cant_cartas) {
                    mazo.insertar(card);
                    j++;
                }
            }
        }
    }

    /*int pid = getpid();
    unsigned short n_jugador;
    for (n_jugador = 4; n_jugador > 1; n_jugador--) {
        pid = fork();
        if (pid == 0) {
            break;
        }
    }

    cout << "Soy el proceso: " << pid << " y el jugador n: " << n_jugador << "\n";*/
    //printf("Soy el proceso: %d\n", pid);

    mazo.primera();
    cout << "La primera carta es de color: " << mazo.sacar().getColor() << " y numero: " << mazo.sacar().getNum() << "\n";
    mazo.primera();
    cout << "La segunda carta es de color: " << mazo.sacar().getColor() << " y numero: " << mazo.sacar().getNum() << "\n";
    mazo.primera();
    cout << "La tercera carta es de color: " << mazo.sacar().getColor() << " y numero: " << mazo.sacar().getNum() << "\n";
    mazo.primera();


    return 0;
}