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
        cout << "\nCarta inicial: \n";
        Carta inicial = shared->sacar();
        int col = inicial.getColor();
        int num = inicial.getNum();
        showCards(col, num);
        cout << "\n" << endl;

        //**********************************/
        //**********************************//
        //**********************************//
        // Testeo
        //num = reversa;
        //col = azul;
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
            play = "12000";
            cout << "Revirtiendo...\n" << endl;
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
        else {
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
    int color, sentido, n_reversa;
    int estado_jugada;
    string play;
    while (!salir) {
        sleep(1);

        // Leer del pipe correspondiente
        close(pipeRead[1]);
        read(pipeRead[0], turno, 1);
        estado_turno = atoi(turno);

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
            //cout << "sentido: " << sentido << endl;
            n_reversa = stoi(play.substr(1, 1));
            //cout << "n_reversa actual: "<< n_reversa << endl;
            color = stoi(play.substr(2, 1));
            //cout << "color actual: "<< color << endl;
            estado_jugada = stoi(play.substr(3, 2));
            //cout << "estado jugada: "<< estado_jugada << endl;
            // Avanzar al jugador
            if (estado_turno != 2 && sentido==1 && n_reversa > 0) {
                n_reversa--;
                play = to_string(sentido)+to_string(n_reversa)+to_string(color)+"0"+to_string(estado_jugada);

                // Pasar jugada
                close(pipeJugadaWrite[0]);
                write(pipeJugadaWrite[1], play.c_str(), 5);
                // Avanzar jugador
                close(pipeWrite[0]);
                write(pipeWrite[1], "1", 1);
                continue;
            }
            else {
                // Resetear cantidad de saltos en reversa
                if (estado_turno != 2 && sentido==1) {
                    n_reversa = 2;
                }
            }
            // Verificar jugada específica
            // Corresponde saltar
            if (num == salto) {
                estado_jugada--;
                if (estado_jugada == 1) {
                    cout << "Saltando al jugador " << n_jugador <<"\n" << endl;
                    // Actualizar jugada
                    play = to_string(sentido)+to_string(n_reversa)+to_string(color)+"00";
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
            if (num == mas2) {
                estado_jugada--;
                if (estado_jugada == 1) {
                    // Robar cartas
                    cout << "Lo siento jugador " << n_jugador << ", la última carta fue +2.\n";
                    cout << "Robando 2 carta(s)... \n";
                    Carta robada = shared->sacar();
                    hand.insertar(robada);
                    robada = shared->sacar();
                    hand.insertar(robada);
                    cout << "Saltando el turno.\n\n";
                    // Actualizar jugada
                    play = to_string(sentido)+to_string(n_reversa)+to_string(color)+"00";
                    close(pipeJugadaWrite[0]);
                    write(pipeJugadaWrite[1], play.c_str(), 5);
                    // Pasar jugador
                    close(pipeWrite[0]);
                    write(pipeWrite[1], "1", 1);
                    continue;
                }
            }
            else if (num == mas4) {
                estado_jugada--;
                if (estado_jugada == 1) {
                    // Robar cartas
                    cout << "Lo siento jugador " << n_jugador << ", la última carta fue +4.\n";
                    cout << "Robando 4 carta(s)... \n";
                    Carta robada = shared->sacar();
                    hand.insertar(robada);
                    robada = shared->sacar();
                    hand.insertar(robada);
                    robada = shared->sacar();
                    hand.insertar(robada);
                    robada = shared->sacar();
                    hand.insertar(robada);

                    cout << "Saltando el turno.\n\n";
                    // Actualizar jugada
                    play = to_string(sentido)+to_string(n_reversa)+to_string(color)+"00";
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
            // Verificar mano por cartas jugables
            int tieneCartas = 1;
            if (!checkAvailable(hand, col, num)) {
                cout << "(" << i << ") Robar carta"; 
                tieneCartas = 0;
            }
            else {
                //i--;
            }
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
                    cout << "Opción inválida, escoja nuevamente. \n\n";
                    continue;
                }

                // Caso de robar carta
                int estado_robo = 0;
                if (opcion == i) {
                    cout << "Robando carta... \n";
                    Carta robada = shared->sacar();
                    int col_aux = robada.getColor();
                    int num_aux = robada.getNum();
                    showCards(col_aux, num_aux);
                    cout << "\n" << endl;
                    estado_robo = 1;
                    // Dar opción
                    if (checkRight(col, num, col_aux, num_aux)) {
                        cout << "¿Desea jugarla ahora?\n(1) Si\n(2) No\n";
                        while (true) {
                            string respuesta;
                            getline(cin, respuesta);
                            opcion = stoi(respuesta);
                            if (opcion == 1){
                                estado_robo = 0;
                                last_card->setColor(col_aux);
                                last_card->setNum(num_aux);
                                break;
                            }
                            else if (opcion == 2){
                                hand.insertar(robada);
                                estado_robo = 1;
                                break;
                            }
                            else {
                                cout << "Opción errónea, escoja nuevamente" << endl;
                            }
                        }
                    }
                    if (!estado_robo) {
                        cout << "\nUsted jugó: \n";
                        showCards(col_aux, num_aux);
                        cout << "\n" << endl;
                    }

                }
                // Caso de jugar carta
                else {
                    // Sacar carta de la mano
                    cout << "Usted jugó: \n";
                    Carta elegida = hand.sacar(opcion-1);
                    int col_aux = elegida.getColor();
                    int num_aux = elegida.getNum();
                    showCards(col_aux, num_aux);
                    cout << "\n" << endl;

                    // Verificar carta jugada
                    if (!checkRight(col, num, col_aux, num_aux)){
                        // Carta errónea, devolviendo
                        cout << "-X-X-X- Carta equivocada, debe robar -X-X-X-\n";
                        hand.insertar(elegida);
                        cout << "Robando carta... \n";
                        Carta robada = shared->sacar();
                        int col_aux = robada.getColor();
                        int num_aux = robada.getNum();
                        showCards(col_aux, num_aux);
                        cout << "\n" << endl;
                    }
                    else {
                        // Actualizar carta jugada
                        last_card->setNum(num_aux);
                        last_card->setColor(col_aux);
                        color = col_aux;
                        if (num_aux == mas2) {
                            estado_jugada = 2;
                        }
                        else if (num_aux == mas4) {
                            estado_jugada = 2;
                        }
                    }
                }
                // Pasar estado de jugada
                play = to_string(sentido)+to_string(n_reversa)+to_string(color)+'0'+to_string(estado_jugada);
                //cout << "escribiendo play al final: " << play << endl;
                close(pipeJugadaWrite[0]);
                write(pipeJugadaWrite[1], play.c_str(), 5);
                // Avanzar jugador
                close(pipeWrite[0]);
                write(pipeWrite[1], "1", 1);
                break;
            }
        }
    }
    
    // Liberar memoria
    free(pipeRead);
    free(pipeWrite);
    free(pipeJugadaRead);
    free(pipeJugadaWrite);
    hand.borrar();
    // Liberar memoria compartida
    if (pid != 0) {
        munmap(shared, sizeof(Carta)*108+sizeof(Mazo));
        munmap(last_card, sizeof(Carta));
    }

    return 0;
}