#include<iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "./mazo.cpp"
using namespace std;

// Definicion de tipos de cartas
enum colores {azul, rojo, verde, amarillo, negro};
enum tipo {mas2=10, reversa=11, salto=12, colores=13, mas4=14};

int main(int argc, char const *argv[]) {
    // Crear mazo
    Mazo mazo = Mazo(108);
    // Asignar cartas ordenadas
    for (int color=0; color<5; color++) {
        Carta card;
        int cant_cartas = 0;
        // Cartas especiales
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
        // Cartas de colores
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

    // Desordenar mazo
    mazo.randomizar();

    // Enviar mazo a memoria compartida
    Mazo * shared = (Mazo *)mmap(NULL, sizeof(Carta)*108+sizeof(Mazo), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    (* shared) = mazo;
    
    // Manejo de pipes
    char turno[1];
    int pipeAtoB[2], pipeBtoC[2], pipeCtoD[2], pipeDtoA[2], pipeJugada[2];
    pipe(pipeAtoB);
    pipe(pipeBtoC);
    pipe(pipeCtoD);
    pipe(pipeDtoA);
    pipe(pipeJugada);

    // Crear hijos
    int pid = getpid();
    unsigned short n_jugador;
    for (n_jugador = 4; n_jugador > 1; n_jugador--) {
        pid = fork();
        if (pid == 0) {
            break;
        }
    }

    cout << "Soy el proceso: " << pid << " y el jugador n: " << n_jugador << "\n";
    printf("Soy el proceso: %d\n\n", pid);

    // Crear manos
    Mano hand = Mano();
    for (int i=0; i<7; i++){
        Carta carta_actual = shared->sacar();
        hand.insertar(carta_actual);
    }


    // Condiciones iniciales
    if (n_jugador == 4) {
        close(pipeDtoA[0]);
        write(pipeDtoA[1] ,"1", 1); 
    }

    int estado_turno;
    cout << "***n_jugador:*** " << n_jugador << "\n";
    while (true) {
        sleep(1);

        // Leer del pipe correspondiente
        if (n_jugador == 1) {
            close(pipeDtoA[1]);
            read(pipeDtoA[0], turno, 1);

        }
        else if (n_jugador == 2) {
            close(pipeAtoB[1]);
            read(pipeAtoB[0], turno, 1);
        }
        else if (n_jugador == 3) {
            close(pipeBtoC[1]);
            read(pipeBtoC[0], turno, 1);
        }
        else if (n_jugador == 4){
            close(pipeCtoD[1]);
            read(pipeCtoD[0], turno, 1);
        }
        else {
            cout << "Error seleccionando al hijo \n" << endl;
            break;
        }
        
        estado_turno = atoi(turno);

        cout << "n_jugador: " << n_jugador << "\n";
        
        
    }
    
    

    hand.borrar();

    return 0;
}