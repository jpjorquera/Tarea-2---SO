#include "../include/header.h"

// Métodos de Carta
int Carta::getNum() {
    return numero;
};
int Carta::getColor() {
    return color;
};
void Carta::setNum(int num) {
	numero = num;
};
void Carta::setColor(int col){
	color = col;
};

// Métodos de Mazo

// Insertar en posición arbitraria
void Mazo::insertar(Carta card, int posicion) {
	if (posicion == -1) {
		posicion = pos_actual;
	}
	do {
    	Carta aux = cartas[posicion];
    	if (aux.getColor() == -1) {
    		cartas[posicion] = card;
    		pos_actual++;
    		break;
    	}
    	else {
    		posicion += 13;
    		if (posicion >= size) {
    			posicion = posicion % size;
    		}
    	}
	} while(true);
};
// Sacar siguiente carta
Carta Mazo::sacar() {
	Carta aux;
	do {
    	aux = cartas[pos_actual];
    	if (aux.getColor() != -1) {
    		cartas[pos_actual] = Carta();
    		pos_actual++;
    		break;
    	}
    	pos_actual++;
	} while (true);
	size_actual--;
	return aux;
};
// Obtener tamaño
int Mazo::getSize() {
    return size_actual;
};
// Desordenar el Mazo
void Mazo::randomizar() {
	this->resetPos();
	srand (time(NULL));
    // Ir pasando a mazo auxiliar
	Mazo mazo_aux = Mazo(108);
	for (int i = 0; i < 108; i++) {
		Carta card_actual = this->sacar();
		int random_pos = rand() % size;
        // Insertar en posición al azar
		mazo_aux.insertar(card_actual, random_pos);
	}
    // Devolver al mazo original
	pos_actual = 0;
	for (int i = 0; i < 108; i++) {
		mazo_aux.resetPos();
		Carta card_actual = mazo_aux.sacar();
		this->insertar(card_actual);
		pos_actual++;
	}
	this->resetPos();
	size_actual = 108;
};
// Devolver al principio
void Mazo::resetPos() {
	pos_actual = 0;
}

// Métodos de Mano
void Mano::insertar(Carta card, int posicion) {
    if (posicion == 0) {
        cartas.push_back(card);
    }
    else {
        list<Carta>::iterator it = cartas.begin();
        advance(it, posicion);
        cartas.insert(it, card);
    }
};
// Sacar última carta
Carta Mano::sacar() {
	Carta card = cartas.back();
    cartas.pop_back();
    return card;
};
// Sacar carta en posición
Carta Mano::sacar(int pos) {
	list<Carta>::iterator it = cartas.begin();
	for (int i = 0; i<pos; i++) {
		it++;
	}
	list<Carta> aux_carta;
	aux_carta.splice(aux_carta.begin(), cartas, it);
    return aux_carta.back();
};
// Obtener tamaño
int Mano::largo() {
    return cartas.size();
};
// Liberar memoria
void Mano::borrar() {
    cartas.clear();
};