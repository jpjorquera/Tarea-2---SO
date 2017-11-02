#include<iostream>
#include <list>
#include <time.h>
using namespace std;

class Carta {
    int numero;
    int color;
public:
    Carta(){};
    Carta(int col, int num) {
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

class Lista_cartas {

public:
	list<Carta> cartas;
    Lista_cartas(){};
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
    void primera() {
        int i=0;
        for (auto v : cartas) {
            //cout << "Color: " << v.getColor() << "\n";
            //cout << "Numero: " << v.getNum() << "\n\n";
            i++;
        }
        cout << "Total " << i << "\n";
    };
    void randomizar(Lista_cartas * mazo) {
    	srand (time(NULL));
    	//Lista_cartas aux = Lista_cartas();
    	int i = 0;
    	int largo = 108;
    	do {
	    	int random_pos = rand() % (i+1);
	    	Carta carta_actual = this->sacar();
	    	mazo->insertar(carta_actual, random_pos);
	    	i++;
    	} while (i<largo);
    	//return aux;
    };
    /*Lista_cartas copy() {
    	Carta aux_card;
    	int largo = this->largo();
    	int i = 0;
    	while (i < largo) {
    		aux_card = this->sacar
    	}
    }*/
};