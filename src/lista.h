#ifndef LISTA_H_
#define LISTA_H_

#include <iostream>

using namespace std;



//class Lista;


template <class T>
class Lista {
public:
    class Celula {
    public:
        friend class Lista<T>;
        T item;
        Celula *prox, *ant;
    };
    Celula *primeiro,*ultimo,*atual;
    int tamanho;
    Lista ();
    void insere(T item);
    void insere_ant(Celula *pos,T item);
    void insere_ordem(T item);
    T* procura(T item);
    //remove o elemento atual
    void remove();
    void remove(T item);

    void remove(Celula *atual) {
        if (tamanho>0 && atual!=0 ) {
            atual->prox->ant = atual->ant;
            atual->ant->prox = atual->prox;
            tamanho--;
            delete atual;
        }
    }

    void removePrimeiro() {
        if (tamanho>0) {
            remove(primeiro->prox);
        }
    }
    void removeUltimo() {
        if (tamanho>0) {
            remove(ultimo->prox);
        }
    }


    void imprimir();
    Celula* inicio() {
        return primeiro->prox;
    }
    Celula* fim() {
        return ultimo->ant;
    }

    int ehUltimo(Celula* item) {
        return item==ultimo;
    }

    int ehPrimeiro(Celula* item) {
        return item==primeiro;
    }

    Celula* proximoCircular(Celula *atual) {
        if (atual==0 || ehUltimo(atual->prox) ) {
            return inicio();
        } else {
            return atual->prox;
        }
    }
    Celula* anteriorCircular(Celula *atual) {
        if (atual==0 || ehPrimeiro(atual->ant) ) {
            return fim();
        } else {
            return atual->ant;
        }
    }



    void desalocar() {
        Celula *latual = this->primeiro->prox,*p;
        while (latual!=ultimo) {

            p =  latual;
            latual=latual->prox;
            remove(p);
        }
    }




};



#include "lista.cpp"



#endif /*CAIXEIRO_H_*/
