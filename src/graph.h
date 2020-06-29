#ifndef GRAPH_H
#define GRAPH_H


#include <iostream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdlib>

#include "edge.h"
#include "color.h"
#include "node.h"

using namespace std;

class Graph{
	friend class Vizing;
	friend class MTTP;
public:

	Graph(int times);
	~Graph();

	//! Aloca a matriz de adjacencia e a lista de arestas do grafo
	void Initialize( int vertices );

	//! Limpa as informações de lixo contidas no grafo
	void clean( );

	//! Procura em 'vértice' uma cor não incidente que seja diferente de corB
	int ProcuraCorFaltandoEmVertice(int vertice);

	// Procura no grafo a proxima aresta sem cor para ser colorida pelo metodo de Vizing
	int ProcuraProximaArestaSemCor();

	//! Apaga a cor da 'aresta' e atualiza todos os campos
	void ApagaCorAresta(int aresta, int cor );

	//! Insere a cor 'color' a arestas que contem os vértives v e w1
	void InsereCorNaAresta(int v, int w1, int aresta, int color );

	//! Procura a cor que falta simultaneamente em v e w1
	int corFaltandoEmDoisVertices(int v1, int w1, int aresta);

	int corFaltandoEmDoisVertices_vAleatoria(int v1, int w1, int aresta );
	int getArestas() const;
	int delta() const;
	int indice_cromatico_do_grafo() const;
	int proxima_aresta_sem_cor() const;
	int getVertices() const;
	void setArestas(int arestas);
	void setDelta(int delta);
	void setIndiceCromatico(int indiceCromatico);
	void setProx(int prox);
	void set_num_vertices_do_grafo(int vertices);

	friend ostream& operator<< (ostream& os, Graph &p);

	// color é uma matriz n por (delta+1) que armazena a cor atribuida a cada vertice
	  vector< vector<Cor> > color;

private:
	// Total de vertices do grafo
	int num_vertices_do_grafo_;

	// Total de arestas do grafo
	int num_arestas_no_grafo_;

	//Incide cromativo do grafo
	int indice_cromatico_do_grafo_;

	// Grau maximo do grafo, ou maior grau de qualquer vertice do grafo
	int delta_;

	Node *listaCorDisponivel;

	// Estrutura principal do grafo - o vetor de arestas
	vector<Edge> edgeVector;

	//! Uma matriz de tamanho n x Delta+1 matrix que armazena as cores disponíveis em cada vértice
	int **availableColor;

	//! proxima aresta ainda não colorida
	int proxima_aresta_sem_cor_;

};
#endif 
