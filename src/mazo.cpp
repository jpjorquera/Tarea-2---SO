#include<iostream>
#include <list>
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
    list<Carta> cartas;
public:
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
            cout << "Color: " << v.getColor() << "\n";
            cout << "Numero: " << v.getNum() << "\n\n";
            i++;
        }
        cout << "Total " << i << "\n";
    };
};