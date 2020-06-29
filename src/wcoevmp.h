/**
 Copyright (C) 2012  Fabricio Nunes da Costa
 Copyright (C) 2014  Tiago de Oliveira Januario

 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.

 * Neither the name of the Universidade Federal de Minas Gerais nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL TIAGO JANUARIO BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PTVEF_H_
#define PTVEF_H_

#include "estatisticas.h"
#include "vizing.h"
#include "tempo.h"
#include "1-fatoracoes.h"

using namespace std;

#define DIR_MELHOR_SOL  "../../../MelhoresSolucoes/"
#define DIR_MELHOR_DUAL "../../../MelhoresLimitesDuais/"

#define ARQ_MELHOR_SOLUCAO (char*) (string(DIR_MELHOR_SOL) + nome_instancia + string(".sol")).c_str()
#define ARQ_MELHOR_SOLUCAO_P (char*) (string(DIR_MELHOR_SOL) + p.nome_instancia + string(".sol")).c_str()

class ptvef {
public:

	/**
	 * DATA
	 */
	// Number of teams
	int nequipes;

	double b;

	// Number of rounds
	int nrodadas;

	// Group of rounds
	vector<int> grupo_r;

	// Group of Teams
	vector<int> grupo_e;

	//Auxiliar vector used to update the coevalue
	int* aux1;

	//Auxiliar vector used to update the coevalue
	int* aux2;

	//Auxiliar vector used to update the coevalue
	int* aux3;

	// Envolved round in movn5
	int* er;

	// involved teams in movN5
	int* et;

	// Carry-over effect matrix
	int** coeMatrix;

	// Weight matrix
	int** weight;

	// Carry-over effect total
	int coe_total;

	int maxBadMoves;
	unsigned long long es1;
	unsigned long long es2;
	unsigned long long es3;

	// Used to control the access of the function
	// that recomputes the coev - carry over effect value
	bool carryover;

	// Kind of local search
	string tbl;

	// Kind of initial solution method
	int tsi;

	int iddleiterations;
	int maxrandommoves;

	/**
	 * FUNCTIONS
	 */

	// Save the information in a file
	void salvar();

	// Open a file with information about initial instance
	void abrir();

	// Neighborhood: Partial Round Swap
	void neighborhoodN4();

	// Used to restore the coe-matrix
	int restaura(const int& t, const int& r1, const int& r2);

	// Evaluates a neighbor in Neighborhood N3
	void avmovN3(const int& round, const int& t1, const int& t2, int &delta);

	// Evaluates a neighbor in Neighborhood N4
	void avmovN4(const int& team, const int& r1, const int& r2, int &delta);

	// Create a new instance of size " n"
	int parser(string arquivo);

	// Executes a move in Neighborhood N4
	void movN4(const int& t, const int& r1, const int& r2);

	~ptvef();

	///////////////////////////

	vector<int> dtime;

//	int inviavel;

	int aleatorioEntre(const int& lo, const int& hi) {
		return (rand() % (hi - lo + 1)) + lo;
	}

	fatoracao fatoracoes_iniciais;
//	int melhorSolucaoConhecidaViolacoes;
	int melhorSolucaoConhecidaDistancia;
//	int ifat;

	int** jogos;

	int** jogoRodada;
	double tempo_melhor;
	vector<int> embequipes; //define permuta��o das rodadas
	vector<int> embrodadas; //define permuta��o das rodadas

	class solucao {
	public:
		int coe;
//		int ifat;
		vector<vector<int> > coeM;
		vector<int> prodadas; //define permuta��o das rodadas
		vector<vector<int> > jogos;
		vector<vector<int> > jogoRodada;
		solucao(int nequipes);
		void copia(const ptvef &pt);
		void cola(ptvef &pt);
		friend bool operator<(const solucao& a, const solucao& b) {
			return a.coe < b.coe;
		}
		friend bool operator==(const solucao& a, const solucao& b) {
			if (a.coe != b.coe)
				return false;
			for (int t = 0; t < a.jogos.size(); t++)
				for (int r = 0; r < a.jogos[t].size(); r++)
					if (a.jogos[t][r] != b.jogos[t][r])
						return false;
			return true;
		}
	};

	void printdiff(const ptvef::solucao &pt1, const ptvef::solucao &pt2);

	inline int jogo(const int& equipe, const int& rodada);
	void setjogo(const int& rodada, const int& equipe1, const int& equipe2);
	int jRodada(const int& i, const int& j);

	int avaliaDistanciaTrocaAdv(int t, int r1, int r2);

	int coeV(const int& t1, const int& t2);
	void downcoeV(const int& t1, const int& t2);
	void upcoeV(const int& t1, const int& t2);

	void avmovN6(const int& t, const int& r1, const int& r2, int &deltaDist,
			int& kind, int& depth);

	void movN3(const int& r, const int& t1, const int& t2);

	void movN6(const int& t, const int& r1, const int& r2, const int& kind,
			 int& depth);

	void buscaLocal();
	int buscaLocalN3UN4();
	void buscaLocalN6();

	void recalcula();

	string nome_instancia;

	int movimentoAleatorio(const int& vizinhanca);

	tabela ils(const int tempo);
	tabela ilsGuedes();
	tabela state();
	int ils(double segundos = 60.0, int bl = 0);

	void saveImprovement();

	void printSolution();
	bool verifica_tabela();
	void imprime_jRodadas();
	void imprime_valor_solucao();

	//--------------------------------
	// Constru��o inicial

	void fatoracaoBinaria();
	void fatoracaoCanonica();
	void solucaoinicialvizing();
	void fatoracaoRecursiva();

	void solucaoInicial(int indice, int embaralhada = 1);

	void sorteiaEquipesRodadas();
};

#endif /*PTVEF_H_*/
