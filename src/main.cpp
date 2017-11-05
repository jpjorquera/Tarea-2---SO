#include<iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <string>
#include <stdlib.h>
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
        //num = mas4;
        //col = negro;
        //**********************************//
        //**********************************//
        //**********************************//

        // Actualizar ultima jugada
        last_card->setNum(num);
        last_card->setColor(col);

        

        // Comunicar jugada codificada
        // En orden: "Estados de UNO x jugador +Sentido + 
        //            N_jugadores a saltar + Color cambiado + Más cartas acumuladas"
        string play;
        if (num == reversa) {
            play = "000012000";
            cout << "Revirtiendo...\n" << endl;
        }
        else if (num == salto) {
            play = "000000902";
        }
        else if (num == mas2) {
            play = "000000902";
        }
        else if (num == mas4) {
            play = "000000904";
        }
        else if (num == colores) {
            play = "000000900";
        }
        else {
            play = "000000900";
        }
        // Escribir jugada
        close(pipeJugadaWrite[0]);
        write(pipeJugadaWrite[1], play.c_str(), 9);

        // Comunicar turno inicial
        close(pipeDtoA[0]);
        write(pipeDtoA[1] ,"2", 1);
    }

    int estado_turno;
    int color, sentido, n_reversa;
    int estado_jugada;
    string play;
    int p1, p2, p3, p4;
    while (!salir) {
        //sleep(1);

        // Leer del pipe correspondiente
        close(pipeRead[1]);
        read(pipeRead[0], turno, 1);
        estado_turno = atoi(turno);

        // Estado de término
        if (estado_turno == 3) {
            break;
        }

        // Si le toca al jugador
        if (estado_turno == 1 || estado_turno == 2) {
            // Leer última jugada
            int col = last_card->getColor();
            int num = last_card->getNum();

            // Verificar estado de última jugada
            string play;
            char play_aux[9];
            close(pipeJugadaRead[1]);
            read(pipeJugadaRead[0], play_aux, 9);
            play = play_aux;
            // Estado de UNOs
            p1 = stoi(play.substr(0,1));
            p2 = stoi(play.substr(1,1));
            p3 = stoi(play.substr(2,1));
            p4 = stoi(play.substr(3,1));
            // Estado de jugada
            sentido = stoi(play.substr(4, 1));
            n_reversa = stoi(play.substr(5, 1));
            color = stoi(play.substr(6, 1));
            estado_jugada = stoi(play.substr(7, 2));
            // Llevar cuenta de tamaño
            int tam = shared->getSize();
            // Avanzar al jugador
            if (estado_turno != 2 && sentido==1 && n_reversa > 0) {
                n_reversa--;
                play = to_string(p1)+to_string(p2)+to_string(p3)+to_string(p4)+
                    to_string(sentido)+to_string(n_reversa)+to_string(color)+to_string(0)+to_string(estado_jugada);

                // Pasar jugada
                close(pipeJugadaWrite[0]);
                write(pipeJugadaWrite[1], play.c_str(), 9);
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
                    cout << "--- Saltando al jugador " << n_jugador <<" ---\n" << endl;
                    // Actualizar jugada
                    play = to_string(p1)+to_string(p2)+to_string(p3)+to_string(p4)+
                            to_string(sentido)+to_string(n_reversa)+to_string(color)+to_string(0)+to_string(0);
                    close(pipeJugadaWrite[0]);
                    write(pipeJugadaWrite[1], play.c_str(), 9);
                    // Pasar jugador
                    close(pipeWrite[0]);
                    write(pipeWrite[1], "1", 1);
                    continue;
                }
            }
            else if (num == mas2) {
                estado_jugada--;
                if (estado_jugada == 1) {
                    // Robar cartas
                    cout << "Lo siento jugador " << n_jugador << ", la última carta fue +2.\n";
                    cout << "Robando 2 cartas... \n";
                    Carta robada = shared->sacar();
                    if (tam-1 == 0) {
                        cout << "\nSe acabaron las cartas.\n";
                        cout << "Juego terminado en empate!\n";
                        salir = 1;
                        break;
                    }
                    hand.insertar(robada);
                    robada = shared->sacar();
                    if (tam-1 == 0) {
                        cout << "\nSe acabaron las cartas.\n";
                        cout << "Juego terminado en empate!\n";
                        salir = 1;
                        break;
                    }
                    // Devolver estado de UNO
                    if (!p1 && n_jugador==1) { p1 = 0; }
                    if (!p2 && n_jugador==2) { p2 = 0; }
                    if (!p3 && n_jugador==3) { p3 = 0; }
                    if (!p4 && n_jugador==4) { p4 = 0; }
                    hand.insertar(robada);
                    cout << "--- Saltando el turno ---\n\n";
                    // Actualizar jugada
                    play = to_string(p1)+to_string(p2)+to_string(p3)+to_string(p4)+
                            to_string(sentido)+to_string(n_reversa)+to_string(color)+to_string(0)+to_string(0);
                    close(pipeJugadaWrite[0]);
                    write(pipeJugadaWrite[1], play.c_str(), 9);
                    // Pasar jugador
                    close(pipeWrite[0]);
                    write(pipeWrite[1], "1", 1);
                    continue;
                }
            }
            else if (num == mas4) {
                estado_jugada--;
                if (estado_jugada == 1 || estado_turno==2) {
                    // Pasar cualquier color a siguiente jugador
                    if (estado_turno==2) {
                        color = 8;
                    }
                    // Robar cartas
                    cout << "Lo siento jugador " << n_jugador << ", la última carta fue +4.\n";
                    cout << "Robando 4 cartas... \n";
                    Carta robada = shared->sacar();
                    if (tam-1 == 0) {
                        cout << "\nSe acabaron las cartas.\n";
                        cout << "Juego terminado en empate!\n";
                        salir = 1;
                        break;
                    }
                    hand.insertar(robada);
                    robada = shared->sacar();
                    if (tam-1 == 0) {
                        cout << "\nSe acabaron las cartas.\n";
                        cout << "Juego terminado en empate!\n";
                        salir = 1;
                        break;
                    }
                    hand.insertar(robada);
                    robada = shared->sacar();
                    if (tam-1 == 0) {
                        cout << "\nSe acabaron las cartas.\n";
                        cout << "Juego terminado en empate!\n";
                        salir = 1;
                        break;
                    }
                    hand.insertar(robada);
                    robada = shared->sacar();
                    if (tam-1 == 0) {
                        cout << "\nSe acabaron las cartas.\n";
                        cout << "Juego terminado en empate!\n";
                        salir = 1;
                        break;
                    }
                    hand.insertar(robada);
                    // Devolver estado de UNO
                    if (!p1 && n_jugador==1) { p1 = 0; }
                    if (!p2 && n_jugador==2) { p2 = 0; }
                    if (!p3 && n_jugador==3) { p3 = 0; }
                    if (!p4 && n_jugador==4) { p4 = 0; }

                    cout << "--- Saltando el turno ---\n\n";
                    // Actualizar jugada
                    play = to_string(p1)+to_string(p2)+to_string(p3)+to_string(p4)+
                        to_string(sentido)+to_string(n_reversa)+to_string(color)+to_string(0)+to_string(0);
                    close(pipeJugadaWrite[0]);
                    write(pipeJugadaWrite[1], play.c_str(), 9);
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
            // Imprimir estados de UNOs
            int uno = 0;
            if (p1 && n_jugador!=1) {
                cout << "Al jugador 1 le queda una carta.\n";
                uno = 1;
            }
            if (p2 && n_jugador!=2) {
                cout << "Al jugador 2 le queda una carta.\n";
                uno = 1;
            }
            if (p3 && n_jugador!=3) {
                cout << "Al jugador 3 le queda una carta.\n";
                uno = 1;
            }
            if (p4 && n_jugador!=4) {
                cout << "Al jugador 4 le queda una carta.\n";
                uno = 1;
            }
            if (uno) { cout << endl; }
            // Imprimir cartas restantes
            cout << "Quedan "<<tam<<" carta(s).\n\n";

            // Imprimir última jugada al jugador actual
            cout << "Última carta jugada: \n";
            showCards(col, num);
            cout << "\n";
            // Verificar seleccion anterior de color
            if (num == colores || num == mas4) {
                // Si no es turno inicial
                if (estado_turno != 2 && color != 8) {
                    col = color;
                    cout << "El color actual es: ";
                    if (color == azul) {
                        cout << "[Azul]\n";
                    }
                    if (color == amarillo) {
                        cout << "[Amarillo]\n";
                    }
                    if (color == rojo) {
                        cout << "[Rojo]\n";
                    }
                    if (color == verde) {
                        cout << "[Verde]\n";
                    }
                }
                // Turno inicial
                else {
                    cout << "Para elegir color, simplemente juegue la carta deseada.\n";
                } 
            }
            cout <<  endl;

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
            if (!checkAvailable(hand, col, num) && color != 8) {
                cout << "(" << i << ") Robar carta\n"; 
                tieneCartas = 0;
            }
            else {
                i--;
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
                if (!tieneCartas && opcion == i) {
                    // Sacar carta
                    cout << "Robando carta... \n";
                    Carta robada = shared->sacar();
                    if (tam-1 == 0) {
                        cout << "\nSe acabaron las cartas.\n";
                        cout << "Juego terminado en empate!\n";
                        salir = 1;
                        break;
                    }
                    // Devolver estado de UNO
                    if (!p1 && n_jugador==1) { p1 = 0;}
                    if (!p2 && n_jugador==2) { p2 = 0;}
                    if (!p3 && n_jugador==3) { p3 = 0;}
                    if (!p4 && n_jugador==4) { p4 = 0;}
                    
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
                                if (hand.largo()==1){
                                    if (n_jugador == 1) {
                                        p1 = 1;
                                    }
                                    else if (n_jugador == 2) {
                                        p2 = 1;
                                    }
                                    else if (n_jugador == 3) {
                                        p3 = 1;
                                    }
                                    else if (n_jugador == 4) {
                                        p4 = 1;
                                    }
                                    cout << "|||   Jugador "<<n_jugador<<": UNO!   |||\n\n";
                                }
                                break;
                            }
                            else if (opcion == 2){
                                cout << "Poniéndola en la mano...\n" << endl;
                                cout << "\n";
                                hand.insertar(robada);
                                estado_robo = 1;
                                break;
                            }
                            else {
                                cout << "Opción errónea, escoja nuevamente" << endl;
                            }
                        }
                    }
                    else{
                        hand.insertar(robada);
                    }
                    if (!estado_robo) {
                        // Actualizar carta jugada
                        cout << "\nUsted jugó: \n";
                        showCards(col_aux, num_aux);
                        cout << "\n" << endl;
                        // Actualizar con cartas especiales
                        if (num_aux == mas2) {
                            estado_jugada = 2;
                        }
                        else if (num_aux == mas4) {
                            estado_jugada = 2;
                            while (true) {
                                cout << "Elija color para continuar: \n";
                                cout << "(1) Azul\n";
                                cout << "(2) Rojo\n";
                                cout << "(3) Verde\n";
                                cout << "(4) Amarillo" << endl;
                                string respuesta;
                                getline(cin, respuesta);
                                if (respuesta.length()>3) {
                                    cout << "Opción inválida, escoja nuevamente. \n\n";
                                    continue;
                                }
                                opcion = stoi(respuesta);
                                if (opcion > 4 || opcion < 1) {
                                    cout << "Opción inválida, escoja nuevamente. \n\n";
                                    continue;
                                }
                                else {
                                    cout << endl;
                                    color = opcion-1;
                                    break;
                                }
                            }
                        }
                        else if (num_aux == salto) {
                            estado_jugada = 2;
                        }
                        else if (num_aux == reversa) {
                            cout << "Revirtiendo...\n" << endl;
                            sentido = (sentido+1)%2;
                            n_reversa = 2;
                        }
                        else if (num_aux == colores) {
                            while (true) {
                                cout << "Elija color para continuar: \n";
                                cout << "(1) Azul\n";
                                cout << "(2) Rojo\n";
                                cout << "(3) Verde\n";
                                cout << "(4) Amarillo" << endl;
                                string respuesta;
                                getline(cin, respuesta);
                                if (respuesta.length()>3) {
                                    cout << "Opción inválida, escoja nuevamente. \n\n";
                                    continue;
                                }
                                opcion = stoi(respuesta);
                                if (opcion > 4 || opcion < 1) {
                                    cout << "Opción inválida, escoja nuevamente. \n\n";
                                    continue;
                                }
                                else {
                                    cout << endl;
                                    color = opcion-1;
                                    break;
                                }
                            }
                        }
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
                    if (!checkRight(col, num, col_aux, num_aux) && color!=8){
                        // Carta errónea, devolviendo
                        cout << "-X-X-X- Carta equivocada, debe robar -X-X-X-\n\n";
                        // Devolver carta
                        cout << "Devolviéndola a la mano. \n";
                        hand.insertar(elegida);
                        // Robar adicional
                        cout << "Robando carta... \n";
                        Carta robada = shared->sacar();
                        if (tam-1 == 0) {
                            cout << "\nSe acabaron las cartas.\n";
                            cout << "Juego terminado en empate!\n";
                            salir = 1;
                        break;
                        }
                        // Devolver estado de UNO
                        if (!p1 && n_jugador==1) { p1 = 0;}
                        if (!p2 && n_jugador==2) { p2 = 0;}
                        if (!p3 && n_jugador==3) { p3 = 0;}
                        if (!p4 && n_jugador==4) { p4 = 0;}
                        // Mostrar robada
                        int col_aux = robada.getColor();
                        int num_aux = robada.getNum();
                        hand.insertar(robada);
                        showCards(col_aux, num_aux);
                        cout << "\n" << endl;
                    }
                    else {
                        // Verificar si ganó
                        int ganador = hand.largo();
                        if (!ganador) {
                            cout << "\n" << endl;
                            cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::\n";
                            cout << ":::::::::::  Felicitaciones Jugador "<<n_jugador<<"!  :::::::::::\n";
                            cout << ":::::::::::    Ha ganado la partida.    :::::::::::\n";
                            cout << ":::::::::::::::::::::::::::::::::::::::::::::::::::\n";
                            cout << endl;
                            salir = 1;
                            break;
                        }

                        // Actualizar carta jugada
                        last_card->setNum(num_aux);
                        last_card->setColor(col_aux);
                        // Actualizar con cartas especiales
                        if (num_aux == mas2) {
                            estado_jugada = 2;
                        }
                        else if (num_aux == mas4) {
                            estado_jugada = 2;
                            while (true) {
                                cout << "Elija color para continuar: \n";
                                cout << "(1) Azul\n";
                                cout << "(2) Rojo\n";
                                cout << "(3) Verde\n";
                                cout << "(4) Amarillo" << endl;
                                string respuesta;
                                getline(cin, respuesta);
                                if (respuesta.length()>3) {
                                    cout << "Opción inválida, escoja nuevamente. \n\n";
                                    continue;
                                }
                                opcion = stoi(respuesta);
                                if (opcion > 4 || opcion < 1) {
                                    cout << "Opción inválida, escoja nuevamente. \n\n";
                                    continue;
                                }
                                else {
                                    cout << endl;
                                    color = opcion-1;
                                    break;
                                }
                            }
                        }
                        else if (num_aux == salto) {
                            estado_jugada = 2;
                        }
                        else if (num_aux == reversa) {
                            cout << "Revirtiendo...\n" << endl;
                            sentido = (sentido+1)%2;
                            n_reversa = 2;
                        }
                        else if (num_aux == colores) {
                            while (true) {
                                cout << "Elija color para continuar: \n";
                                cout << "(1) Azul\n";
                                cout << "(2) Rojo\n";
                                cout << "(3) Verde\n";
                                cout << "(4) Amarillo" << endl;
                                string respuesta;
                                getline(cin, respuesta);
                                if (respuesta.length()>3) {
                                    cout << "Opción inválida, escoja nuevamente. \n\n";
                                    continue;
                                }
                                opcion = stoi(respuesta);
                                if (opcion > 4 || opcion < 1) {
                                    cout << "Opción inválida, escoja nuevamente. \n\n";
                                    continue;
                                }
                                else {
                                    cout << endl;
                                    color = opcion-1;
                                    break;
                                }
                            }
                        }
                        // Actualizar estado UNO
                        if (hand.largo()==1){
                            if (n_jugador == 1) {
                                p1 = 1;
                            }
                            else if (n_jugador == 2) {
                                p2 = 1;
                            }
                            else if (n_jugador == 3) {
                                p3 = 1;
                            }
                            else if (n_jugador == 4) {
                                p4 = 1;
                            }
                            cout << "|||   Jugador "<<n_jugador<<": UNO!   |||\n\n";
                        }
                    }
                }
                // Pasar estado de jugada
                if (estado_jugada < 0){
                    estado_jugada = 0;
                }
                play = to_string(p1)+to_string(p2)+to_string(p3)+to_string(p4)+
                        to_string(sentido)+to_string(n_reversa)+to_string(color)+to_string(0)+to_string(estado_jugada);
                close(pipeJugadaWrite[0]);
                write(pipeJugadaWrite[1], play.c_str(), 9);
                // Avanzar jugador
                close(pipeWrite[0]);
                write(pipeWrite[1], "1", 1);
                break;
            }
        }
    }
    // Comunicar término
    close(pipeWrite[0]);
    write(pipeWrite[1], "3", 1);
    
    // Liberar memoria
    /*cout << "Liberando en jugador "<<n_jugador << endl;
    free(pipeRead);
    cout << "pipeReadLiberado" << endl;
    free(pipeWrite);
    cout << "pipeWriteLiberado" << endl;
    free(pipeJugadaRead);
    cout << "pipeJugadaRead" << endl;
    free(pipeJugadaWrite);
    cout << "pipeJugadaWrite" << endl;
    hand.borrar();
    cout << "mano borrada" << endl;
    // Liberar memoria compartida*/
    if (pid != 0) {
        cout << endl;
        munmap(shared, sizeof(Carta)*108+sizeof(Mazo));
        munmap(last_card, sizeof(Carta));
    }

    return 0;
}