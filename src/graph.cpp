#include "graph.h"

//Procura a cor nao atribuida ao 'vertice'
int Graph::ProcuraCorFaltandoEmVertice(int vertice) {

    if (listaCorDisponivel[vertice].size > 0) {
        return listaCorDisponivel[vertice].corDisponivel[rand()
                % listaCorDisponivel[vertice].size];
    }

    return -1;
}

int Graph::corFaltandoEmDoisVertices(int v, int w1, int aresta) {

    if (listaCorDisponivel[v].size < listaCorDisponivel[w1].size) {
        for (int i = rand() % listaCorDisponivel[v].size, j = 0;
                j < listaCorDisponivel[v].size; j++) {
            if (color[w1][listaCorDisponivel[v].corDisponivel[i]].status
                    == false
                    && edgeVector[aresta].taboo
                            != listaCorDisponivel[v].corDisponivel[i])
                return listaCorDisponivel[v].corDisponivel[i];
            i = (i + 1) % listaCorDisponivel[v].size;
        }
    } else {
        for (int i = rand() % listaCorDisponivel[w1].size, j = 0;
                j < listaCorDisponivel[w1].size; j++) {
            if (color[v][listaCorDisponivel[w1].corDisponivel[i]].status
                    == false
                    && edgeVector[aresta].taboo
                            != listaCorDisponivel[w1].corDisponivel[i])
                return listaCorDisponivel[w1].corDisponivel[i];
            i = (i + 1) % listaCorDisponivel[w1].size;
        }
    }
    //! retorna -1 caso nao encontre uma cor para colorir a aresta
    return -1;
}

// Insere a cor 'color' a arestas que contem os vertives v e w1
void Graph::InsereCorNaAresta(int v, int w1, int aresta, int cor) {

    int index_v, index_w1, final_v, final_w1;

    //! Insere a cor 'color' na lista das cores de v
    color[v][cor].status = true;

    //! o vertice conectado a v pela cor color e w1
    color[v][cor].adj = w1;

    //! Insere a cor 'color' na lista das cores de w1
    color[w1][cor].status = true;

    //! o vertice conectado a w1 pela cor color e v
    color[w1][cor].adj = v;

    //! a aresta que conecta estes dois vertices e aresta
    color[v][cor].e = aresta;

    color[w1][cor].e = aresta;

    edgeVector[aresta].taboo = -1;

    edgeVector[aresta].color = cor;

    //! ----------------------- !//

    //! Faça com que a cor color nao esteja mais disponivel no vertice v, buscando-a atraves do seu index
    index_v = color[v][cor].disponivel_index;

    final_v = listaCorDisponivel[v].size - 1;

    listaCorDisponivel[v].corDisponivel[index_v] =
            listaCorDisponivel[v].corDisponivel[final_v];

    //! Faça com que a cor color nao esteja mais disponivel no vertice w1, buscando-a atraves do seu index
    index_w1 = color[w1][cor].disponivel_index;
    final_w1 = listaCorDisponivel[w1].size - 1;

    listaCorDisponivel[w1].corDisponivel[index_w1] =
            listaCorDisponivel[w1].corDisponivel[final_w1];

    //! ----------------------- !//

    //! Atualiza o ponteiro para a cor que mudou de lugar devido ? eliminaçao da cor disponivel
    color[v][listaCorDisponivel[v].corDisponivel[index_v]].disponivel_index =
            index_v;
    color[w1][listaCorDisponivel[w1].corDisponivel[index_w1]].disponivel_index =
            index_w1;

    //! ----------------------- !//

    //! Encolhe o tamanho da lista de cores disponiveis
    listaCorDisponivel[w1].size--;
    listaCorDisponivel[v].size--;

}

// Apaga a cor da 'aresta' e atualiza todos os campos
void Graph::ApagaCorAresta(int aresta, int cor) {

    const int empty = -1;

    //! Torna a removida disponivel para os vertices
    listaCorDisponivel[edgeVector[aresta].start].corDisponivel[listaCorDisponivel[edgeVector[aresta].start].size] =
            cor;
    listaCorDisponivel[edgeVector[aresta].end].corDisponivel[listaCorDisponivel[edgeVector[aresta].end].size] =
            cor;

    //! Atualiza o ponteiro para a cor disponivel da posiçao size para a nova posiçao
    color[edgeVector[aresta].start][cor].disponivel_index =
            listaCorDisponivel[edgeVector[aresta].start].size;
    color[edgeVector[aresta].end][cor].disponivel_index =
            listaCorDisponivel[edgeVector[aresta].end].size;

    //! Aumenta o tamanho da lista de cores disponiveis
    listaCorDisponivel[edgeVector[aresta].start].size++;
    listaCorDisponivel[edgeVector[aresta].end].size++;

    //! Apaga a cor 'color' na lista das cores de start
    color[edgeVector[aresta].start][cor].status = false;
    color[edgeVector[aresta].start][cor].adj = empty;
    color[edgeVector[aresta].start][cor].e = empty;

    //! Apaga a cor 'color' na lista das cores de end
    color[edgeVector[aresta].end][cor].status = false;
    color[edgeVector[aresta].end][cor].adj = empty;
    color[edgeVector[aresta].end][cor].e = empty;

    //! Apaga a cor da aresta
    edgeVector[aresta].color = empty;

}

// Libera o espaço de memória utilizado para alocar o grafo
Graph::~Graph() {

    for (int i = 0; i < getVertices(); i++) {
        delete[] availableColor[i];
        delete[] listaCorDisponivel[i].corDisponivel;
    }
    delete[] availableColor;
    delete[] listaCorDisponivel;

}
/*
 //! Embaralha a sequencia de arestas do grafo
 void embaralhaAresta(graph *g){
 //! contador
 int i;

 aresta = new int[g->e];
 for(i = 0; i < g->e; i++)
 aresta[i] = i;

 //! Aresta a ser permutada
 int s;

 //! Constroi uma sequencia de arestas totalmente aleatoria
 for(i=0 ; i < g->e-1; i++)
 {
 //! Seleciona uma aresta da sequencia de arestas a ser permutada
 s = rand() % g->e;
 swap(aresta[i],aresta[s]);
 }
 }
 */

Graph::Graph(int times) {

    setProx(0);
    Initialize(times - 1);
    clean();
}

void Graph::Initialize(int n) {
    int empty = -1;

    /////////////
    // ATENCAO!//
    /////////////

    //O grafo a ser colorido tem n-1 vertices
    num_vertices_do_grafo_ = n;

    // O grafo em questao eh completo
    num_arestas_no_grafo_ = num_vertices_do_grafo_
            * (num_vertices_do_grafo_ - 1) / 2;

    delta_ = num_vertices_do_grafo_ - 1;

    indice_cromatico_do_grafo_ = delta_ + 1;

    //! Aloca e posições para o array de arestas
    edgeVector.resize(num_arestas_no_grafo_);

    //! Aloca um array de cores para cada vertice
    //! O tamanho do array de cores para cada vertice deve ser igual a (delta+1)
    color.resize(num_vertices_do_grafo_,vector<Cor>(indice_cromatico_do_grafo_));

    //! This stores the missingColor matrix rows
    availableColor = new int*[num_vertices_do_grafo_];

    listaCorDisponivel = new Node[num_vertices_do_grafo_];

    for (int i = 0; i < num_vertices_do_grafo_; ++i) {
        listaCorDisponivel[i].corDisponivel =
                new int[indice_cromatico_do_grafo_];
        availableColor[i] = new int[indice_cromatico_do_grafo_];
    }

    int k = 0;
    for (int i = 0; i < num_vertices_do_grafo_; ++i) {
        for (int j = i + 1; j < num_vertices_do_grafo_; ++j) {
            edgeVector[k].start = i;
            edgeVector[k].end = j;
            edgeVector[k].color = empty;
            k++;
        }
    }

    //! Inicializa com false pra indicar que aquela cor nao foi atribuida aquele vertice.
    for (int i = 0; i < num_vertices_do_grafo_; ++i) {
        for (int j = 0; j < indice_cromatico_do_grafo_; ++j) {
            color[i][j].status = false;
            color[i][j].disponivel_index = j;
            color[i][j].adj = empty;
            color[i][j].e = empty;
        }
    }

    //! Inicializa a lista de cores disponiveis de cada vertice
    for (int i = 0; i < num_vertices_do_grafo_; ++i) {
        listaCorDisponivel[i].size = indice_cromatico_do_grafo_;
        for (int j = 0; j < indice_cromatico_do_grafo_; ++j) {
            listaCorDisponivel[i].corDisponivel[j] = j;
        }
    }
}

void Graph::clean() {

    int empty = -1;

    setProx(0);

    int k = 0;
    for (int i = 0; i < num_vertices_do_grafo_; ++i) {
        for (int j = i + 1; j < num_vertices_do_grafo_; ++j) {
            edgeVector[k].start = i;
            edgeVector[k].end = j;
            edgeVector[k].color = empty;
            k++;
        }
    }

    //! Inicializa com false pra indicar que aquela cor nao foi atribuida aquele vertice.
    for (int i = 0; i < num_vertices_do_grafo_; ++i) {
        for (int j = 0; j < indice_cromatico_do_grafo_; ++j) {
            color[i][j].status = false;
            color[i][j].disponivel_index = j;
            color[i][j].adj = empty;
            color[i][j].e = empty;
        }
    }

    //! Inicializa a lista de cores disponiveis de cada vertice
    for (int i = 0; i < num_vertices_do_grafo_; ++i) {
        listaCorDisponivel[i].size = indice_cromatico_do_grafo_;
        for (int j = 0; j < indice_cromatico_do_grafo_; ++j) {
            listaCorDisponivel[i].corDisponivel[j] = j;
        }
    }
}

ostream& operator<<(ostream& os, Graph &g) {
    os << endl;
    os << "Vertices = " << g.getVertices() << endl;
    os << "Arestas = " << g.getArestas() << endl;
    for (int i = 0; i < g.getArestas(); i++)
        os << g.edgeVector[i].start << "\t";
    os << endl;

    for (int i = 0; i < g.getArestas(); i++)
        os << g.edgeVector[i].end << "\t";
    os << endl;

    for (int i = 0; i < g.getArestas(); i++)
        os << g.edgeVector[i].color << "\t";
    os << endl;

    return os;
}

int Graph::ProcuraProximaArestaSemCor() {

    if (proxima_aresta_sem_cor_ >= num_arestas_no_grafo_)
        return -1;
    return proxima_aresta_sem_cor_++;

}

int Graph::getArestas() const {
    return num_arestas_no_grafo_;
}

int Graph::delta() const {
    return delta_;
}

int Graph::indice_cromatico_do_grafo() const {
    return indice_cromatico_do_grafo_;
}

int Graph::proxima_aresta_sem_cor() const {
    return proxima_aresta_sem_cor_;
}

int Graph::getVertices() const {
    return num_vertices_do_grafo_;
}

void Graph::setArestas(int arestas) {
    this->num_arestas_no_grafo_ = arestas;
}

void Graph::setDelta(int delta) {
    this->delta_ = delta;
}

void Graph::setIndiceCromatico(int indiceCromatico) {
    this->indice_cromatico_do_grafo_ = indiceCromatico;
}

void Graph::setProx(int prox) {
    this->proxima_aresta_sem_cor_ = prox;
}

void Graph::set_num_vertices_do_grafo(int vertices) {
    this->num_vertices_do_grafo_ = vertices;
}

