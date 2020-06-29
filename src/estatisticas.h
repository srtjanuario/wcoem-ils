#ifndef ESTATISTICAS_H_
#define ESTATISTICAS_H_

#include <string>

#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class dado_tab {
public:
	double num;
	string texto;
	int tipo;
	dado_tab(double vv) {
		tipo = 0;
		num = vv;
		texto = "";
	}
	dado_tab(string ss) {

		tipo = 1;
		texto = ss;
		num = -100.0;
	}

	friend ostream& operator<<(ostream& co, const dado_tab &f) {
		//co<<setw(10);
		if (f.tipo == 0) {
			//co<<setprecision(4);
			printf("%.6g", f.num);

			//stringstream oss;
			//oss <<fixed<< f.num;
			//string s = oss.str();
			//for (unsigned i = 0; i < s.size(); i++) {
			//	if (s[i] == '.') {
			//		co << ',';
			//	} else {
			//		co << s[i];
			//	}
			//}

		} else if (f.tipo == 1) {
			co << f.texto;
		}
		return co;
	}

};

class coluna: public vector<dado_tab> {
public:
	string nome;
	void insere(double valor) {
		push_back(dado_tab(valor));
	}

	void insere(string valor) {
		push_back(dado_tab(valor));
	}

	void insere(dado_tab valor) {
		push_back(valor);
	}

	double media;
	double maior;
	double menor;
	double dst;
};

class nomeApelido {
public:
	string nome;
	string apelido;
	nomeApelido(string nome_, string apelido_ = "") {
		nome = nome_;
		if (apelido_ == "") {
			apelido = nome;
		} else {
			apelido = apelido_;
		}
	}
	friend ostream& operator<<(ostream& co, const nomeApelido &f) {
		cout << "teste\n";

		return co;
	}
};

class tabela: public map<string, coluna> {
public:
	vector<nomeApelido> nome_cols;
	void print() {
		if (begin() == end()) {
			return;
		}
//		for (tabela::iterator iter = begin(); iter != end(); iter++) {
//			cout << iter->first << "; "<<flush;
//			//<< "; " << (*iter).second << " years old" << endl;
//		}
//		cout << "\n";
		int n = (*this)[begin()->first].size();
		for (int i = 0; i < n; i++) {
			cout << "{";
			for (tabela::iterator iter = begin(); iter != end();) {
				cout << " \"" << iter->first << "\": " << flush;
				cout << fixed << ((*this)[iter->first][i]) << flush;
				iter++;
				if (iter != end())
					cout << ", ";
			}
			cout << " }";
		}
		//  for(int i=0;i<nome_cols.size();i++){
		//	  cout<<nome_cols[i]<<"; ";
		// }

	}

	void print2() {
		//for (int i=0;i<nome_cols.size();i++) {
		//   cout<<nome_cols[i].apelido<<"; ";
		// }
		//cout<<"\n";
		int n = (*this)[begin()->first].size();
		for (int i = 0; i < n; i++) {
			cout << "{ ";
			for (unsigned j = 0; j < nome_cols.size(); j++) {
				cout << ((*this)[nome_cols[j].nome][i]) << "; ";
			}
			cout << "}\n";
		}
	}

	void imprime_tabela(int cabecalho = 1, int dados = 1, ostream &co = cout) {

		string sep = "; ";
		if (cabecalho) {
			for (unsigned i = 0; i < nome_cols.size(); i++) {
				co << nome_cols[i].apelido << sep;
			}
			co << "\n";
		}

		if (dados) {
			int n = (*this)[begin()->first].size();
			for (int i = 0; i < n; i++) {
				for (unsigned j = 0; j < nome_cols.size(); j++) {
					co << ((*this)[nome_cols[j].nome][i]) << sep;
				}
				co << "\n";
			}
		}
	}

	void insere(string coluna, double valor) {
		(*this)[coluna].insere(valor);
	}

	void insere(string coluna, string valor) {
		(*this)[coluna].insere(valor);
	}

	friend tabela operator+(tabela &A, tabela &B) {
		tabela r;
		r = A;
		int n = B[B.begin()->first].size();
		for (int i = 0; i < n; i++) {
			for (tabela::iterator iter = B.begin(); iter != B.end(); iter++) {
				r[iter->first].insere(B[iter->first][i]);
			}
		}

		return r;
	}

};

#endif /*ESTATISTICAS_H_*/

