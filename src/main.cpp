#include<iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <string>
//#include "./mazo.cpp"
//#include "./functions.cpp"
#include "../include/header.h"
using namespace std;

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
    // Carta actual en memoria compartida
    Carta * last_card = (Carta *)mmap(NULL, sizeof(Carta), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    last_card->setNum(-1);
    last_card->setColor(-1);
    
    // Manejo de pipes
    char turno[1];
    int pipeAtoB[2], pipeBtoC[2], pipeCtoD[2], pipeDtoA[2];
    int pipeJugadaAtoB[2], pipeJugadaBtoC[2], pipeJugadaCtoD[2], pipeJugadaDtoA[2];
    pipe(pipeAtoB);
    pipe(pipeBtoC);
    pipe(pipeCtoD);
    pipe(pipeDtoA);
    // Pipes de jugada
    pipe(pipeJugadaAtoB);
    pipe(pipeJugadaBtoC);
    pipe(pipeJugadaCtoD);
    pipe(pipeJugadaDtoA);

    // Crear hijos
    int pid = getpid();
    unsigned short n_jugador;
    for (n_jugador = 4; n_jugador > 1; n_jugador--) {
        pid = fork();
        if (pid == 0) {
            break;
        }
    }

    /*cout << "Soy el proceso: " << pid << " y el jugador n: " << n_jugador << "\n";
    printf("Soy el proceso: %d\n\n", pid);*/

    // Crear manos
    Mano hand = Mano();
    for (int i=0; i<7; i++){
        Carta carta_actual = shared->sacar();
        hand.insertar(carta_actual);
    }


    // Seleccionas pipes para verificar turno y escribir jugada
    int salir = 0;
    int * pipeRead = (int *)malloc(sizeof(int)*2);
    int * pipeWrite = (int *)malloc(sizeof(int)*2);
    int * pipeJugadaRead = (int *)malloc(sizeof(int)*2);
    int * pipeJugadaWrite = (int *)malloc(sizeof(int)*2);

    if (n_jugador == 1) {
        pipeRead = pipeDtoA;
        pipeWrite = pipeAtoB;
        pipeJugadaRead = pipeJugadaDtoA;
        pipeJugadaWrite = pipeJugadaAtoB;
    }
    else if (n_jugador == 2) {
        pipeRead = pipeAtoB;
        pipeWrite = pipeBtoC;
        pipeJugadaRead = pipeJugadaAtoB;
        pipeJugadaWrite = pipeJugadaBtoC;
    }
    else if (n_jugador == 3) {
        pipeRead = pipeBtoC;
        pipeWrite = pipeCtoD;
        pipeJugadaRead = pipeJugadaBtoC;
        pipeJugadaWrite = pipeJugadaCtoD;
    }
    else if (n_jugador == 4){
        pipeRead = pipeCtoD;
        pipeWrite = pipeDtoA;
        pipeJugadaRead = pipeJugadaCtoD;
        pipeJugadaWrite = pipeJugadaDtoA;
    }
    else {
        cout << "Error seleccionando al hijo \n" << endl;
        salir = 1;
    }

    // Condiciones iniciales
    if (n_jugador == 4) {
        sleep(1);
        // Primera carta
        cout << "Carta inicial: \n";
        Carta inicial = shared->sacar();
        int col = inicial.getColor();
        int num = inicial.getNum();
        showCards(col, num);
        cout << "\n" << endl;

        //**********************************/
        //**********************************//
        //**********************************//
        // Testeo
        num = reversa;
        col = azul;
        //**********************************//
        //**********************************//
        //**********************************//

        // Actualizar ultima jugada
        last_card->setNum(num);
        last_card->setColor(col);

        

        // Comunicar jugada codificada
        // En orden: "Sentido + N_jugadores a saltar + Color cambiado + Más cartas acumuladas"
        string play;
        if (num == reversa) {
            play = "10003";
            cout << "Revirtiendo..." << endl;
        }
        else if (num == salto) {
            play = "00002";
        }
        else if (num == mas2) {
            play = "00002";
        }
        else if (num == mas4) {
            play = "00904";
        }
        else if (num == colores) {
            play = "00000";
        }
        // Escribir jugada
        cout << "Escribi play inicial: " << play.c_str() << endl;
        close(pipeJugadaWrite[0]);
        write(pipeJugadaWrite[1], play.c_str(), 5);

        // Comunicar turno inicial
        close(pipeDtoA[0]);
        write(pipeDtoA[1] ,"2", 1);
    }

    int estado_turno;
    int color_mas4, sentido, n_reversa;
    int estado_jugada;
    while (!salir) {
        //sleep(1);

        // Leer del pipe correspondiente
        close(pipeRead[1]);
        read(pipeRead[0], turno, 1);
        estado_turno = atoi(turno);

        cout << "Lei mi turno y soy el jugador " << n_jugador << endl;
        cout << " Lei estado de turno: " <<estado_turno<<endl;

        // Si le toca al jugador
        if (estado_turno == 1 || estado_turno == 2) {
            // Leer última jugada
            int col = last_card->getColor();
            int num = last_card->getNum();

            // Verificar estado de última jugada
            string play;
            char play_aux[5];
            close(pipeJugadaRead[1]);
            read(pipeJugadaRead[0], play_aux, 5);
            play = play_aux;
            // Estado de jugada
            sentido = stoi(play.substr(0, 1));
            n_reversa = stoi(play.substr(1, 2));
            color_mas4 = stoi(play.substr(2, 3));
            estado_jugada = stoi(play.substr(3, 5));
            // Avanzar al jugador
            if (estado_turno != 2 && sentido==1 && n_reversa > 0) {
                n_reversa--;
                play = to_string(sentido)+to_string(n_reversa)+to_string(color_mas4)+to_string(estado_jugada);
                // Pasar jugada
                close(pipeJugadaWrite[0]);
                write(pipeJugadaWrite[1], play.c_str(), 5);
                // Avanzar jugador
                close(pipeWrite[0]);
                write(pipeWrite[1], "1", 1);
                continue;
            }
            // Verificar jugada específica
            // Corresponde saltar
            if (num == salto) {
                estado_jugada--;
                if (estado_jugada == 1) {
                    cout << "Saltando al jugador " << n_jugador <<"\n" << endl;
                    // Actualizar jugada
                    play = to_string(sentido)+"0001";
                    close(pipeJugadaWrite[0]);
                    write(pipeJugadaWrite[1], play.c_str(), 5);
                    // Pasar jugador
                    close(pipeWrite[0]);
                    write(pipeWrite[1], "1", 1);
                    continue;
                }
            }
            // Cambiar sentido
            else if (num == reversa && estado_turno!=2) {
                estado_jugada--;
                if (estado_jugada>0) {
                    sentido = (sentido+1)%2;
                    n_jugador = 4;
                    play = to_string(sentido)+to_string(n_reversa)+ "00" + to_string(estado_jugada);
                    // Actualizar jugada
                    close(pipeJugadaWrite[0]);
                    write(pipeJugadaWrite[1], play.c_str(), 5);
                    // Pasar jugador
                    close(pipeWrite[0]);
                    write(pipeWrite[1], "1", 1);
                    continue;
                }
            }



            



            // Avanzar al jugador actual
            cout << "****************************************\n";
            cout << "****** Turno del jugador número " << n_jugador << " ******\n";
            cout << "****************************************\n\n";

            // Imprimir última jugada al jugador actual
            cout << "Última carta jugada: \n";
            showCards(col, num);
            cout << "\n" << endl;

            // Mostrar mano
            cout << "Sus cartas actuales son: \n" << endl;
            list<Carta>::iterator it;
            int i = 1;
            for (it = hand.cartas.begin(); it != hand.cartas.end(); it++) {
                Carta carta_actual = *it;
                cout << "(" << i << ") ";
                int col_aux = carta_actual.getColor();
                int num_aux = carta_actual.getNum();
                showCards(col_aux, num_aux);
                cout << "\n";
                i++;
            }
            cout << "(" << i << ") Robar carta(s) \n"; 
            cout << endl;

            // Pedir jugada
            while (true) {
                int validar_jugada = 1;
                cout << "Elija el número de la opción a jugar:\n";
                string respuesta;
                int opcion;
                getline(cin, respuesta);
                // Verificar tamaño
                if (respuesta.length() > 3) {
                    validar_jugada = 0;
                }
                else {
                    // Verificar que sea número
                    try {
                        opcion = stoi(respuesta);
                    }
                    catch (...) {
                        validar_jugada = 0;
                    }
                    // Verificar rango
                    if (opcion <= 0 || opcion > i) {
                        validar_jugada = 0;
                    }
                }
                if (!validar_jugada) {
                    cout << "Opción inválida, escoja nuevamente \n\n";
                    continue;
                }
                // Caso de robar carta
                if (opcion == i) {
                    cout << "Robando carta... \n";
                    Carta robada = shared->sacar();
                    int col_aux = robada.getColor();
                    int num_aux = robada.getNum();
                    showCards(col_aux, num_aux);
                    cout << "\n" << endl;


                }

                // Verificar mano
                
                // Caso de jugar carta
                else {
                    // Sacar carta de la mano
                    cout << "Usted jugó: \n";
                    Carta elegida = hand.sacar(opcion-1);
                    int col_aux = elegida.getColor();
                    int num_aux = elegida.getNum();
                    showCards(col_aux, num_aux);
                    cout << "\n" << endl;
                }
            }
        }
            close(pipeWrite[0]);
            write(pipeWrite[1], "1", 1);
        
        
        
    }
    
    
    free(pipeRead);
    free(pipeWrite);
    free(pipeJugadaRead);
    free(pipeJugadaWrite);
    hand.borrar();
    if (pid != 0) {
        munmap(shared, sizeof(Carta)*108+sizeof(Mazo));
        munmap(last_card, sizeof(Carta));
    }

    return 0;
}