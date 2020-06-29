/*
 * polygon.h
 * "Copyright 2012 <Tiago Januario>"
 *  Created on: Oct 20, 2012
 *      Author: januario
 */

#ifndef POLYGON_H_
#define POLYGON_H_

#include <vector>
#include <iostream>
#include <sstream>
//#include "gurobi_c++.h"
using namespace std;

class Polygon {
public:
	//Construtores
	Polygon(const int& vertices);

	//Destrutores
	~Polygon();

	void Check();

	unsigned Cost();

	bool RunTeamSwap(unsigned c, unsigned t1, unsigned t2);

	void RebuildTable();

	bool BuscaArquivo(unsigned n);

	void CriaUmaFatoracao();

	//void CriaUmaFatoracaoExataArestaCor();

	//void CriaUmaFatoracaoExataVerticeVertice();

	void BuildOpponentMatrix();

	void write(string name);

	void CriaUmQuadradoLatino();

	void AlocarVariaveisVV();

	bool RoundSwap(unsigned t, unsigned c1, unsigned c2);
	void RunRoundSwap(unsigned t, unsigned c1, unsigned c2);

	bool TeamSwap(unsigned c, unsigned t1, unsigned t2);

	bool RoundRotation(unsigned c, unsigned t1, unsigned t2);
	bool RunRoundRotation(unsigned c, unsigned t1, unsigned t2);
	bool RunERoundRotation(unsigned c, unsigned t1, unsigned t2);

	int Generic3TeamSwap(unsigned c, unsigned t1, unsigned t2, unsigned t3);
	int Generic4TeamSwap(unsigned c, unsigned t1, unsigned t2, unsigned t3,
			unsigned t4);

	vector<vector<unsigned> > table;

	bool found;

	// Print the patter in Di Gaspero/Schaerf format
	Polygon& operator=(const Polygon& p);
	friend ostream& operator<<(ostream& os, const Polygon& p);
	//private:

//  GRBVar *** xvv;
//  GRBVar ** xec;
//  GRBEnv *env;
//  GRBModel *model;

	ostringstream graph_name;
	ostringstream lp_name;

	void RodaPoligonoSentidoAntiHorario();

	void RodaPoligonoSentidoHorario();

	vector<int> poligono;

	unsigned nodes;
	unsigned edges;
	unsigned colors;

	//Alocação do Grafo para leitura na Matriz de Incidência
	vector<vector<unsigned> > matrizIncidencia;
	vector<vector<unsigned> > my_hash;
	vector<vector<int> > opponent;

};

#endif // POLYGON_H_
