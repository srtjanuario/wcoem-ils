/*
 * polygon.cc
 *
 *  Created on: Oct 20, 2012
 *      Author: januario
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <queue>

#ifdef THREADLIMIT
#include <thread>
#endif

using namespace std;

#include "polygon.h"

Polygon::Polygon(const int& vertices) {
	nodes = vertices;

	found = false;

	this->poligono.resize(vertices);

	poligono[0] = poligono.size();
	for (unsigned i = 1; i < poligono.size(); i++)
		poligono[i] = i;

	// The matrix of matches
	table.resize(nodes, vector<unsigned>(nodes));

	//The matrix of opponents
	opponent.resize(nodes, vector<int>(nodes - 1));

	edges = (nodes * (nodes - 1)) / 2;
	colors = nodes - 1;

	matrizIncidencia.resize(nodes, vector<unsigned>(edges));
	my_hash.resize(nodes, vector<unsigned>(nodes));

	//Inicializa a Matriz de Incidência com Zeros
	for (unsigned i = 0; i < nodes; i++)
		fill(matrizIncidencia[i].begin(), matrizIncidencia[i].end(), 0);

	//Executa a leitura dos dados de entrada
	unsigned aresta = 0;
	for (unsigned i = 0; i < nodes; i++) {
		for (unsigned j = i + 1; j < nodes; j++) {
			matrizIncidencia[i][aresta] = 1;
			matrizIncidencia[j][aresta] = 1;
			my_hash[i][j] = aresta;
			my_hash[j][i] = aresta;
			aresta++;
		}
	}
}

Polygon::~Polygon() {
	poligono.clear();
}

void Polygon::RodaPoligonoSentidoAntiHorario() {
	for (unsigned i = 1; i < poligono.size() - 1; i++) {
		swap(poligono[i], poligono[i + 1]);
	}
}

void Polygon::RodaPoligonoSentidoHorario() {
	for (int i = poligono.size() - 1; i > 1; i--) {
		swap(poligono[i], poligono[i - 1]);
	}
}

void Polygon::RunRoundSwap(unsigned t, unsigned c1, unsigned c2) {

	vector<unsigned> et;
	unsigned t1 = t;
	unsigned count = 0;
	do {
		t1 = opponent[t1][c2];
		et.push_back(t1);
		t1 = opponent[t1][c1];
		et.push_back(t1);
		count += 2;
	} while (t1 != t);

	for (unsigned i = 0; i < et.size(); i++) {
		swap(opponent[et[i]][c1], opponent[et[i]][c2]);
	}
	RebuildTable();
}

void Polygon::Check() {

	for (unsigned i = 0; i < opponent.size(); i++)
		for (unsigned j = 0; j < opponent[i].size(); j++) {
			for (unsigned k = j + 1; k < opponent[i].size(); k++) {
				if (opponent[i][j] == opponent[i][k])
					cout << "opponent[" << i << "][" << j << "]" << "="
							<< "opponent[" << i << "][" << k << "]" << endl;
			}
		}
}

unsigned Polygon::Cost() {

	unsigned t1;
	unsigned count = 0;
	unsigned cost = 0;

	found = true;

	for (unsigned t = 0; t < nodes; t++) {
		for (unsigned c1 = 0; c1 < colors; c1++) {
			for (unsigned c2 = c1 + 1; c2 < colors; c2++) {
				t1 = t;
				count = 0;
				do {
					t1 = opponent[t1][c2];
					t1 = opponent[t1][c1];
					count += 2;
				} while (t1 != t);
				if (count == nodes)
					cost++;
				else
					found = false;
			}
		}
	}

	return cost;

}

bool Polygon::RoundSwap(unsigned t, unsigned c1, unsigned c2) {

	unsigned t1 = t;
	unsigned count = 0;
	do {
		t1 = opponent[t1][c2];
		t1 = opponent[t1][c1];
		count += 2;
	} while (t1 != t);

	if (count == nodes)
		return true;
	else
		return false;
}

Polygon& Polygon::operator=(const Polygon& p) {

	for (unsigned i = 0; i < p.opponent.size(); i++) {
		for (unsigned j = 0; j < p.opponent.size(); j++) {
			this->opponent[i][j] = p.opponent[i][j];
		}
	}

	for (unsigned i = 0; i < p.table.size(); i++) {
		for (unsigned j = 0; j < p.table.size(); j++) {
			this->table[i][j] = p.table[i][j];
		}
	}
	return *this;
}

ostream& operator<<(ostream& os, const Polygon& p) {
	for (unsigned i = 0; i < p.nodes; ++i) {
		for (unsigned j = 0; j < p.nodes; ++j) {
			os << setw(3) << p.table[i][j];
		}
		os << endl;
	}
	os << endl;
	for (unsigned i = 0; i < p.nodes; ++i) {
		for (unsigned j = 0; j < p.nodes - 1; ++j) {
			os << setw(3) << p.opponent[i][j];
		}
		os << endl;
	}
	return os;
}

bool Polygon::RunTeamSwap(unsigned c, unsigned t1, unsigned t2) {

	vector<unsigned> er;
	unsigned r = c;
	unsigned t_stop = opponent[t1][r];
	unsigned t = opponent[t2][r];

	er.push_back(r);

	while (t != t_stop) {
		r = table[t1][t] - 1;
		t = opponent[t2][r];
		er.push_back(r);
	}
	for (unsigned i = 0; i < er.size(); i++) {
		opponent[opponent[t1][er[i]]][er[i]] = t2;
		opponent[opponent[t2][er[i]]][er[i]] = t1;
		swap(opponent[t1][er[i]], opponent[t2][er[i]]);
	}
	RebuildTable();
	return true;
}

void Polygon::RebuildTable() {
	for (unsigned i = 0; i < nodes; i++) {
		for (unsigned j = 0; j < nodes - 1; j++) {
			table[i][opponent[i][j]] = j + 1;
			table[opponent[i][j]][i] = j + 1;
		}
	}
}

bool Polygon::TeamSwap(unsigned c, unsigned t1, unsigned t2) {

	unsigned count = 1;
	unsigned r = c;
	unsigned t_stop = opponent[t1][r];
	unsigned t = opponent[t2][r];

	while (t != t_stop) {
		r = table[t1][t] - 1;
		t = opponent[t2][r];
		count++;
	}
	if (count == nodes - 2)
		return true;
	else {
		return false;
	}
}

bool Polygon::RunRoundRotation(unsigned c, unsigned t1, unsigned t2) {

	bool stop = false;
	vector<unsigned> er;
	unsigned t3;
	unsigned r = c;
	unsigned t_stop = opponent[t1][r];
	unsigned r_close1;
	unsigned r_close2;
	unsigned t = opponent[t2][r];
	er.push_back(r);

	while (t != t_stop && !stop) {
		r_close1 = table[t1][t] - 1;
		r_close2 = table[t2][t_stop] - 1;
		if (opponent[t_stop][r_close1] == opponent[t][r_close2]) {
			t3 = opponent[t_stop][r_close1];
			stop = true;
		} else {
			r = table[t1][t] - 1;
			t = opponent[t2][r];
			er.push_back(r);
		}
	}

	if (stop) {
		unsigned first = opponent[t1][er[0]];
		unsigned last = opponent[t2][er[er.size() - 1]];
		unsigned tm;

		opponent[first][r_close1] = t1;
		opponent[first][r_close2] = t3;
		opponent[first][er[0]] = t2;

		opponent[last][r_close1] = t3;
		opponent[last][r_close2] = t2;
		opponent[last][er[er.size() - 1]] = t1;

		for (unsigned i = 1; i < er.size(); i++) {
			tm = opponent[t1][er[i]];
			swap(opponent[tm][er[i]], opponent[tm][er[i - 1]]);
		}

		for (unsigned i = er.size() - 1; i > 0; i--) {
			opponent[t2][er[i]] = opponent[t2][er[i - 1]];
		}
		opponent[t2][r_close2] = last;
		opponent[t2][er[0]] = first;

		for (unsigned i = 0; i < er.size() - 1; i++) {
			opponent[t1][er[i]] = opponent[t1][er[i + 1]];
		}
		opponent[t1][r_close1] = first;
		opponent[t1][er[er.size() - 1]] = last;

		opponent[t3][r_close1] = last;
		opponent[t3][r_close2] = first;

		RebuildTable();
		return true;
	} else
		return false;

}

bool Polygon::RunERoundRotation(unsigned c, unsigned t1, unsigned t2) {

	bool long_chain = false;
	bool stop = false;
	vector<unsigned> er;
	vector<unsigned> et;
	unsigned t3;
	unsigned r = c;
	unsigned t_stop = opponent[t1][r];
	unsigned r_close1;
	unsigned r_close2;
	unsigned c_swap;
	unsigned c_current;
	unsigned t = opponent[t2][r];
	er.push_back(r);
	unsigned t_one;

	while (t != t_stop && !stop) {
		r_close1 = table[t1][t] - 1;
		r_close2 = table[t2][t_stop] - 1;
		if (opponent[t_stop][r_close1] == opponent[t][r_close2]) {
			t3 = opponent[t_stop][r_close1];
			stop = true;
		} else {
			long_chain = true;
			c_swap = r_close1;
			c_current = r_close2;
			t_one = opponent[t_stop][c_swap];
			et.push_back(t_one);
			do {
				t_one = opponent[t_one][c_current];
				if (t_one == t_stop || t_one == t1 || t_one == t2) {
					long_chain = false;
					break;
				} else if (t_one == t) {
					long_chain = true;
					break;
				} else {
					et.push_back(t_one);
					swap(c_swap, c_current);
				}
			} while (t_one != t);
			if (long_chain)
				stop = true;
			else {
				et.clear();
				r = table[t1][t] - 1;
				t = opponent[t2][r];
				er.push_back(r);
			}
		}
	}
	if (stop) {
		unsigned first = opponent[t1][er[0]];
		unsigned last = opponent[t2][er[er.size() - 1]];
		unsigned tm;

		//first
		opponent[first][r_close1] = t1;
		opponent[first][er[0]] = t2;

		//last
		opponent[last][r_close2] = t2;
		opponent[last][er[er.size() - 1]] = t1;

		//intermediarios
		for (unsigned i = 1; i < er.size(); i++) {
			tm = opponent[t1][er[i]];
			swap(opponent[tm][er[i]], opponent[tm][er[i - 1]]);
		}

		for (unsigned i = er.size() - 1; i > 0; i--) {
			opponent[t2][er[i]] = opponent[t2][er[i - 1]];
		}
		opponent[t2][r_close2] = last;
		opponent[t2][er[0]] = first;

		for (unsigned i = 0; i < er.size() - 1; i++) {
			opponent[t1][er[i]] = opponent[t1][er[i + 1]];
		}
		opponent[t1][r_close1] = first;
		opponent[t1][er[er.size() - 1]] = last;

		if (long_chain) {

			opponent[first][r_close2] = et[0];
			opponent[last][r_close1] = et[et.size() - 1];

			for (unsigned i = 0; i < et.size(); i++) {
				swap(opponent[et[i]][r_close1], opponent[et[i]][r_close2]);
			};
		} else {
			opponent[t3][r_close1] = last;
			opponent[t3][r_close2] = first;

			opponent[first][r_close2] = t3;
			opponent[last][r_close1] = t3;
		}
		RebuildTable();

		return true;
	} else
		return false;

}

bool Polygon::RoundRotation(unsigned c, unsigned t1, unsigned t2) {

//cout << *this << endl;
	unsigned r = c;
	unsigned t_stop = opponent[t1][r];
	unsigned r_close1;
	unsigned r_close2;
	unsigned t = opponent[t2][r];

	while (t != t_stop) {

		r_close1 = table[t1][t] - 1;
		r_close2 = table[t2][t_stop] - 1;

		if (opponent[t_stop][r_close1] == opponent[t][r_close2]) {
			return true;
		} else {
			r = table[t1][t] - 1;
			t = opponent[t2][r];
		}
	}
	return false;
}

int Polygon::Generic3TeamSwap(unsigned c, unsigned t1, unsigned t2,
		unsigned t3) {

	unsigned i;
	unsigned round;
	unsigned pivots_capacity = 3;
	queue<unsigned> qrounds;
	queue<unsigned> qteams;
	vector<unsigned> rs_;
	vector<unsigned> pivot_;

	pivot_.push_back(t1);
	pivot_.push_back(t2);
	pivot_.push_back(t3);
	rs_.push_back(c);

	qrounds.push(c);

	while (!qrounds.empty()) {
		while (!qrounds.empty()) {
			for (i = 1; i < pivots_capacity; i++) {
				qteams.push(opponent[pivot_[i]][qrounds.front()]);
			}
			qrounds.pop();
		}
		while (!qteams.empty()) {
			round = table[pivot_[0]][qteams.front()] - 1;
			if (table[pivot_[1]][pivot_[2]] - 1 == round) {
				return -1;
			}
			if (find(rs_.begin(), rs_.end(), round) == rs_.end()) {
				qrounds.push(round);
				rs_.push_back(round);
			}
			qteams.pop();
		}
	}

	if (rs_.size() == this->nodes - 4)
		return 1;
	else
		return 0;
}

int Polygon::Generic4TeamSwap(unsigned c, unsigned t1, unsigned t2, unsigned t3,
		unsigned t4) {

	unsigned i;
	unsigned round;
	unsigned pivots_capacity = 4;
	queue<unsigned> qrounds;
	queue<unsigned> qteams;
	vector<unsigned> rs_;
	vector<unsigned> pivot_;

	pivot_.push_back(t1);
	pivot_.push_back(t2);
	pivot_.push_back(t3);
	pivot_.push_back(t4);
	rs_.push_back(c);

	qrounds.push(c);

	while (!qrounds.empty()) {
		while (!qrounds.empty()) {
			for (i = 1; i < pivots_capacity; i++) {
				qteams.push(opponent[pivot_[i]][qrounds.front()]);
			}
			qrounds.pop();
		}
		while (!qteams.empty()) {
			round = table[pivot_[0]][qteams.front()] - 1;
			if (table[pivot_[1]][pivot_[2]] - 1 == round
					|| table[pivot_[1]][pivot_[3]] - 1 == round
					|| table[pivot_[2]][pivot_[3]] - 1 == round) {

				return -1;
			}
			if (find(rs_.begin(), rs_.end(), round) == rs_.end()) {
				qrounds.push(round);
				rs_.push_back(round);
			}
			qteams.pop();
		}
	}

	if (rs_.size() == this->nodes - 4)
		return 1;
	else
		return 0;
}

bool Polygon::BuscaArquivo(unsigned n) {
	ostringstream oss;
	int trash;
	oss << n;
	string name("P1F_" + oss.str() + ".txt");
	ifstream arquivo;

	arquivo.open(name.c_str(), ios::in);
	if (!arquivo.is_open())
		return false;

	arquivo >> trash;

	for (unsigned k = 0; k < nodes; k++)
		for (unsigned l = 0; l < nodes; l++) {
			arquivo >> table[k][l];
		}

	BuildOpponentMatrix();

	arquivo.close();
	return true;
}

void Polygon::CriaUmQuadradoLatino() {

	unsigned n = nodes - 1;
	vector<int> a;
	a.push_back(0);
	for (unsigned i = 1; i < n; i++)
		a.push_back((a[i - 1] + (n - 2)) % n);

	for (unsigned i = 0; i < n; i++) {
		for (unsigned j = 0; j < n; j++) {
			table[i][j] = a[(i + j) % n] + 1;
			if (i == j) {
				table[i][n] = table[i][j];
				table[n][j] = table[i][j];
				table[i][j] = 0;
			}
		}
	}

	BuildOpponentMatrix();

}

void Polygon::CriaUmaFatoracao() {

	int factor = 1;
	for (unsigned k = 0; k < nodes - 1; k++) {
		table[poligono[0] - 1][poligono[1] - 1] = factor;
		table[poligono[1] - 1][poligono[0] - 1] = factor;
		for (unsigned l = 2; l <= nodes / 2; l++) {
			table[poligono[nodes + 1 - l] - 1][poligono[l] - 1] = factor;
			table[poligono[l] - 1][poligono[nodes + 1 - l] - 1] = factor;
		}
		RodaPoligonoSentidoHorario();
		factor++;
	}

	BuildOpponentMatrix();
}

void Polygon::write(string name) {
	ofstream saida(name.c_str(), ios::out);
	saida << nodes << endl;
	for (unsigned i = 0; i < nodes; i++) {
		for (unsigned j = 0; j < nodes; j++) {
			saida << setw(3) << table[i][j];
		}
		saida << endl;
	}
}

void Polygon::BuildOpponentMatrix() {
//Build opponent matrix
	for (unsigned i = 0; i < nodes; i++) {
		for (unsigned j = i + 1; j < nodes; j++) {
			opponent[i][table[i][j] - 1] = j;
			opponent[j][table[j][i] - 1] = i;
		}
	}
}
