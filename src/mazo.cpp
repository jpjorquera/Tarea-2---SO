#include<iostream>
#include <time.h>
#include <list>
using namespace std;

class Carta {
    int numero;
    int color;
public:
    Carta(int col = -1, int num = -1) {
        color = col;
        numero = num;
    };
    int getNum() {
        return numero;
    }
    int getColor() {
        return color;
    }
};

class Mazo {
	int size;
	int size_actual;
	int pos_actual;
	Carta cartas[108];

public:
    Mazo(int tam = 108){
    	size = size_actual = tam;
    	pos_actual = 0;
    };
    void insertar(Carta card, int posicion = -1) {
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
    Carta sacar() {
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
    int getSize() {
        return size_actual;
    };
    void randomizar() {
    	this->resetPos();
    	srand (time(NULL));
    	Mazo mazo_aux = Mazo(108);
    	for (int i = 0; i < 108; i++) {
    		Carta card_actual = this->sacar();
    		int random_pos = rand() % size;
    		mazo_aux.insertar(card_actual, random_pos);
    	}
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
    void resetPos() {
    	pos_actual = 0;
    }
    void primera() {
    	int i;
    	for ( i=0; i<size; i++) {
    		Carta aux = cartas[i];
    		cout << "Carta: \n" << "Color: " << aux.getColor() << "\n" << "Num: " << aux.getNum() << "\n" << endl;
    	}
    	cout << "Tamaño total: " << i << "\n" << endl;
    };
};

class Mano {

public:
	list<Carta> cartas;
    Mano(){};
    void insertar(Carta card, int posicion = 0) {
        if (posicion == 0) {
            cartas.push_back(card);
        }
        else {
            list<Carta>::iterator it = cartas.begin();
            advance(it, posicion);
            cartas.insert(it, card);
        }
    };
    Carta sacar() {
    	Carta card = cartas.back();
        cartas.pop_back();
        return card;
    };
    int largo() {
        return cartas.size();
    };
    void borrar() {
        cartas.clear();
    };
    /*void primera() {
        int i=0;
        for (auto v : cartas) {
            cout << "Color: " << v.getColor() << "\n";
            cout << "Numero: " << v.getNum() << "\n\n";
            i++;
        }
        cout << "Total " << i << "\n";
    };*/
};