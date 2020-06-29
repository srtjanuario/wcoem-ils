//#include "lista.h"

#ifdef LISTA_H_

template <class T>
Lista<T>::Lista() {
    primeiro = new Celula;
    ultimo = new Celula;
    primeiro->prox =  ultimo;
    ultimo->ant = primeiro;
    primeiro->ant = 0;
    ultimo->prox = 0;
    tamanho = 0;
}

template <class T>
void  Lista<T>::insere(T item) {
    tamanho++;
    Celula *cel = new Celula;
    cel->prox = 0;
    cel->ant = ultimo;
    ultimo->prox = cel;
    ultimo->item = item;
    ultimo = cel;
}

template <class T>
void  Lista<T>::insere_ant(Celula *pos, T item) {
    tamanho++;
    Celula *cel = new Celula;
    cel->item = item;
    cel->prox = pos;
    cel->ant = pos->ant;
    cel->ant->prox =cel;
    cel->prox->ant = cel;
}

template <class T>
void  Lista<T>::insere_ordem(T item) {

    atual = this->primeiro->prox;
    while (atual!=ultimo ) {
        if (atual->item > item) {
            break;
        }
        atual = atual->prox;
    }
    tamanho++;
    Celula *cel = new Celula;
    cel->item = item;
    cel->prox = atual;
    cel->ant = atual->ant;
    cel->ant->prox =cel;
    cel->prox->ant = cel;
}



template <class T>
void   Lista<T>::imprimir() {
    Celula *latual = this->primeiro->prox;
    while (latual!=ultimo) {
        if (latual==0) {
            cout<<"erro\n";
            exit(0);
        }
        cout<<latual->item;

        if (latual->prox!=ultimo) {
            cout<<" ";
        }
        latual=latual->prox;
    }

}


template <class T>
T*  Lista<T>::procura(T item) {
    atual = this->primeiro->prox;
    while (atual!=ultimo ) {
        if (atual->item==item) {
            return &(atual->item);
        }
        atual = atual->prox;
    }
    atual = 0;
    return 0;
}


template <class T>
void  Lista<T>::remove() {
    if (tamanho>0 && atual!=0 ) {
        atual->prox->ant = atual->ant;
        atual->ant->prox = atual->prox;
        tamanho--;
        delete atual;
    }
}

template <class T>
void  Lista<T>::remove
(T item) {
    if ( procura(item) ) {
        remove
        ();
    }
}


#endif
