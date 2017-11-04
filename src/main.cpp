#include<iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <string>
#include "./mazo.cpp"
#include "./functions.cpp"
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


    // Seleccionas pipes
    int salir = 0;
    int * pipeRead = (int *)malloc(sizeof(int)*2);
    int * pipeWrite = (int *)malloc(sizeof(int)*2);

    if (n_jugador == 1) {
        pipeRead = pipeDtoA;
        pipeWrite = pipeAtoB;
    }
    else if (n_jugador == 2) {
        pipeRead = pipeAtoB;
        pipeWrite = pipeBtoC;
    }
    else if (n_jugador == 3) {
        pipeRead = pipeBtoC;
        pipeWrite = pipeCtoD;
    }
    else if (n_jugador == 4){
        pipeRead = pipeCtoD;
        pipeWrite = pipeDtoA;
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

        // Actualizar ultima jugada
        last_card->setNum(num);
        last_card->setColor(col);

        // Comunicar jugada
        /*string play;
        if (num>=10){
            play = to_string(col)+to_string(num);
        }
        else {
            play = to_string(col)+"0"+to_string(num);
        }
        close(pipeJugada[0]);
        write(pipeJugada[1], play.c_str(), 3);*/

        // Comunicar turno
        close(pipeDtoA[0]);
        write(pipeDtoA[1] ,"1", 1);
    }

    int estado_turno;
    char last_play[2];
    int estado_jugada;
    while (!salir) {
        sleep(1);
        // Leer del pipe correspondiente
        close(pipeRead[1]);
        read(pipeRead[0], turno, 1);
        estado_turno = atoi(turno);

        // Si le toca al jugador
        if (estado_turno == 1) {
            // Leer jugada
            /*string play;
            char play_aux[3];
            close(pipeJugada[1]);
            read(pipeJugada[0], play_aux, 3);
            play = play_aux;
            cout << "The play is: " << play << "\n" << endl;
            int col = stoi(play.substr(0, 1));
            int num = stoi(play.substr(1, 3));*/
            int col = last_card->getColor();
            int num = last_card->getNum();

            // Verificar estado de última jugada
            /*if (num == reversa) {
                close(pipeJugada[1]);
                read(pipeJugada[0], last_play, 2);
                estado_jugada = atoi(last_play);
                estado_jugada--;
                // Si no me toca, saltar
                if (estado_jugada) {
                    if (estado_jugada >= 10) {
                        *last_play = estado_jugada+'0';
                    }
                    else {
                        last_play[0] = '0';
                        last_play[1] = estado_jugada+'0';
                    }
                    // Actualizar jugador
                    close(pipeJugada[0]);
                    write(pipeJugada[1], last_play, 2);
                    // Informar al siguiente
                    close(pipeWrite[0]);
                    write(pipeWrite[1], "1", 1);
                }
            }
            else if (num == salto) {
                close(pipeJugada[1]);
                read(pipeJugada[0], last_play, 2);
                estado_jugada = atoi(last_play);
                estado_jugada--;
                if (estado_jugada) {
                    // Actualizar jugador
                    close(pipeJugada[0]);
                    write(pipeJugada[1], "01", 2);
                    // Informar al siguiente
                    close(pipeWrite[0]);
                    write(pipeWrite[1], "1", 1);
                }
            }
            else if (num == mas2 || num == mas4) {
                close(pipeJugada[1]);
                read(pipeJugada[0], last_play, 2);
                estado_jugada = atoi(last_play);
            }*/



            // Imprimir última jugada al jugador actual
            showCards(col, num);
            cout << "\n" << endl;



            // Avanzar al jugador actual
            cout << "*** Turno del jugador número " << n_jugador << " ***\n\n";

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
            cout << "(" << i << ") Robar carta \n"; 
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
    hand.borrar();
    if (pid != 0) {
        munmap(shared, sizeof(Carta)*108+sizeof(Mazo));
        munmap(last_card, sizeof(Carta));
    }

    return 0;
}