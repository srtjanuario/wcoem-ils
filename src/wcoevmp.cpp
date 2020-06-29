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

#include "wcoevmp.h"
#include <climits>
#include <stdlib.h>
#define VERIFICA_INDICE_MOVIMENTOS 0

//#define DEBUG
#define canonica 0
#define vizing 1
#define kidd 2
#define RRSWAP 0
#define RRFAN 1
#define LAST 2

ptvef::~ptvef() {
	for (int i = 0; i < nequipes; i++) {
		delete[] jogos[i];
		delete[] coeMatrix[i];
		delete[] jogoRodada[i];
		delete[] weight[i];
	}
	delete[] coeMatrix;
	delete[] jogoRodada;
	delete[] jogos;
	delete[] weight;
	delete[] aux1;
	delete[] aux2;
	delete[] aux3;
	delete[] er;
	delete[] et;
}

void ptvef::fatoracaoCanonica() {
	solucaoInicial(0);
}

void ptvef::fatoracaoBinaria() {
	solucaoInicial(1);
}

void ptvef::fatoracaoRecursiva() {
	solucaoInicial(2);
}

void ptvef::solucaoinicialvizing() {
	Polygon sol(nequipes);
	Vizing v;
	v.Initialize(nequipes);
	v.CriaColoracaoVizing(sol);

	for (int e1 = 0; e1 < nequipes; e1++) {
		for (int e2 = e1 + 1; e2 < nequipes; e2++) {
			setjogo(sol.table[e1][e2] - 1, e1, e2);
		}
	}
	recalcula();
}

void ptvef::solucaoInicial(int indice, int embaralhada) {

	fatoracao f(nequipes);

	if (embaralhada) {
		sorteiaEquipesRodadas();
	} else {
		for (int i = 0; i < nrodadas; i++) {
			embequipes[i] = i;
			embrodadas[i] = i;
		}
		embequipes[nrodadas] = nrodadas;
	}

	indice = indice % fatoracoes_iniciais.fatoracoes[nequipes].size();
	carryover = false;

	for (int e = 0; e < nequipes; e++) {
		for (int r = 0; r < nequipes - 1; r++) {
			setjogo(embrodadas[r], embequipes[e],
					embequipes[fatoracoes_iniciais.fatoracoes[nequipes][indice].jogo(
							e, r)]);
		}
	}
	recalcula();
}

void ptvef::sorteiaEquipesRodadas() {
	int t1, t2, temp;
#if 1
	for (int i = 0; i < nequipes; i++) {
		t1 = aleatorioEntre(0, nequipes - 1);
		t2 = aleatorioEntre(0, nequipes - 1);
		temp = embequipes[t1];
		embequipes[t1] = embequipes[t2];
		embequipes[t2] = temp;
	}
#endif
#if 1
	for (int i = 0; i < nequipes - 1; i++) {
		t1 = aleatorioEntre(0, nequipes - 2);
		t2 = aleatorioEntre(0, nequipes - 2);
		temp = embrodadas[t1];
		embrodadas[t1] = embrodadas[t2];
		embrodadas[t2] = temp;
	}
#endif
}

void ptvef::recalcula() {

	coe_total = 0;

//#pragma omp parallel for collapse(2)
	for (int t1 = 0; t1 < nequipes; t1++) {
		for (int t2 = 0; t2 < nequipes; t2++) {
			coeMatrix[t1][t2] = 0;
		}
	}

//#pragma omp parallel for collapse(2)
	for (int t = 0; t < nequipes; t++) {
		for (int r = 0; r < nrodadas; r++) {
			coeMatrix[jogo(t, r)][jogo(t, (r + 1) % nrodadas)]++;
//			coe_total += 2 * coeMatrix[jogo(t, r)][jogo(t, (r + 1) % nrodadas)] - 1;
		}
	}

//#pragma omp parallel for collapse(2)
	for (int t1 = 0; t1 < nequipes; t1++) {
		for (int t2 = 0; t2 < nequipes; t2++) {
			coe_total += weight[t1][t2] * coeMatrix[t1][t2] * coeMatrix[t1][t2];
		}
	}

	carryover = true;

}

int ptvef::coeV(const int& t1, const int& t2) {
	if (t1 != t2)
		return weight[t1][t2] * (coeMatrix[t1][t2] + coeMatrix[t1][t2] - 1);
	else
		return 0;
}

void ptvef::upcoeV(const int& t1, const int& t2) {
	if (t1 != t2)
		coeMatrix[t1][t2]++;
}

void ptvef::downcoeV(const int& t1, const int& t2) {
	if (t1 != t2)
		coeMatrix[t1][t2]--;
}

inline int ptvef::avaliaDistanciaTrocaAdv(int t, int r1, int r2) {

	int deltacoe = 0;

	if (r1 > r2) {
		swap(r1, r2);
	}

	// Calcular oldcoe
	if (r1 + 1 == r2 || (r1 == 0 && r2 == nrodadas - 1)) {

		if (r1 == 0 && r2 == nrodadas - 1)
			swap(r1, r2);
		// r1-1 com r1
		deltacoe -= coeV(jogo(t, r1 - 1), jogo(t, r1));
		downcoeV(jogo(t, r1 - 1), jogo(t, r1));
		upcoeV(jogo(t, r1 - 1), jogo(t, r2));
		deltacoe += coeV(jogo(t, r1 - 1), jogo(t, r2));

		// r1 com r2
		deltacoe -= coeV(jogo(t, r1), jogo(t, r2));
		downcoeV(jogo(t, r1), jogo(t, r2));
		upcoeV(jogo(t, r2), jogo(t, r1));
		deltacoe += coeV(jogo(t, r2), jogo(t, r1));

		// r2 com r2+1
		deltacoe -= coeV(jogo(t, r2), jogo(t, r2 + 1));
		downcoeV(jogo(t, r2), jogo(t, r2 + 1));
		upcoeV(jogo(t, r1), jogo(t, r2 + 1));
		deltacoe += coeV(jogo(t, r1), jogo(t, r2 + 1));

	} else {

		// r1- 1 com r1
		deltacoe -= coeV(jogo(t, r1 - 1), jogo(t, r1));
		downcoeV(jogo(t, r1 - 1), jogo(t, r1));
		upcoeV(jogo(t, r1 - 1), jogo(t, r2));
		deltacoe += coeV(jogo(t, r1 - 1), jogo(t, r2));

		// r1 com r1+1
		deltacoe -= coeV(jogo(t, r1), jogo(t, r1 + 1));
		downcoeV(jogo(t, r1), jogo(t, r1 + 1));
		upcoeV(jogo(t, r2), jogo(t, r1 + 1));
		deltacoe += coeV(jogo(t, r2), jogo(t, r1 + 1));

		// r2-1 com r2
		deltacoe -= coeV(jogo(t, r2 - 1), jogo(t, r2));
		downcoeV(jogo(t, r2 - 1), jogo(t, r2));
		upcoeV(jogo(t, r2 - 1), jogo(t, r1));
		deltacoe += coeV(jogo(t, r2 - 1), jogo(t, r1));

		// r2 com r2+1
		deltacoe -= coeV(jogo(t, r2), jogo(t, r2 + 1));
		downcoeV(jogo(t, r2), jogo(t, r2 + 1));
		upcoeV(jogo(t, r1), jogo(t, r2 + 1));
		deltacoe += coeV(jogo(t, r1), jogo(t, r2 + 1));
	}

	return deltacoe;

}

inline int ptvef::restaura(const int& t, const int& s1, const int& s2) {

	int deltacoe = 0;

	int r1 = s1;
	int r2 = s2;

	if (r1 > r2) {
		swap(r1, r2);
	}

	if (r1 + 1 == r2 || (r1 == 0 && r2 == nrodadas - 1)) {

		if (r1 == 0 && r2 == nrodadas - 1)
			swap(r1, r2);
		// r1- 1 com r1
		deltacoe -= coeV(jogo(t, r1 - 1), jogo(t, r2));
		downcoeV(jogo(t, r1 - 1), jogo(t, r2));
		upcoeV(jogo(t, r1 - 1), jogo(t, r1));
		deltacoe += coeV(jogo(t, r1 - 1), jogo(t, r1));

		// r1 com r2
		deltacoe -= coeV(jogo(t, r2), jogo(t, r1));
		downcoeV(jogo(t, r2), jogo(t, r1));
		upcoeV(jogo(t, r1), jogo(t, r2));
		deltacoe += coeV(jogo(t, r1), jogo(t, r2));

		// r2 com r2+1
		deltacoe -= coeV(jogo(t, r1), jogo(t, r2 + 1));
		downcoeV(jogo(t, r1), jogo(t, r2 + 1));
		upcoeV(jogo(t, r2), jogo(t, r2 + 1));
		deltacoe += coeV(jogo(t, r2), jogo(t, r2 + 1));

	} else {
		// r1- 1 com r1
		deltacoe -= coeV(jogo(t, r1 - 1), jogo(t, r2));
		downcoeV(jogo(t, r1 - 1), jogo(t, r2));
		upcoeV(jogo(t, r1 - 1), jogo(t, r1));
		deltacoe += coeV(jogo(t, r1 - 1), jogo(t, r1));

		// r1 com r1+1
		deltacoe -= coeV(jogo(t, r2), jogo(t, r1 + 1));
		downcoeV(jogo(t, r2), jogo(t, r1 + 1));
		upcoeV(jogo(t, r1), jogo(t, r1 + 1));
		deltacoe += coeV(jogo(t, r1), jogo(t, r1 + 1));

		// r2-1 com r2
		deltacoe -= coeV(jogo(t, r2 - 1), jogo(t, r1));
		downcoeV(jogo(t, r2 - 1), jogo(t, r1));
		upcoeV(jogo(t, r2 - 1), jogo(t, r2));
		deltacoe += coeV(jogo(t, r2 - 1), jogo(t, r2));

		// r2 com r2+1
		deltacoe -= coeV(jogo(t, r1), jogo(t, r2 + 1));
		downcoeV(jogo(t, r1), jogo(t, r2 + 1));
		upcoeV(jogo(t, r2), jogo(t, r2 + 1));
		deltacoe += coeV(jogo(t, r2), jogo(t, r2 + 1));
	}

	return deltacoe;

}

void ptvef::avmovN3(const int& round, const int& t1, const int& t2,
		int &deltaDist) {

	es1++;
	if (es1 == 0) {
		es2++;
		if (es2 == 0)
			es3++;
	}

	// The opponent of team t1
	int at1;

	// The opponent of team t2
	int at2;

	// Auxiliar round
	int ra;

	// Last round of the move
	int last_round = round;

	// Number of changes
	int alteracoes = -1;

	deltaDist = 0;

	int r = round;

	do {
		alteracoes++;
		at1 = jogo(t1, r);
		at2 = jogo(t2, r);
		ra = jRodada(t1, at2);

		aux1[alteracoes] = r;
		aux2[alteracoes] = at1;
		aux3[alteracoes] = at2;

		grupo_r[r] = 1;

		r = ra;

	} while (r != last_round);

	if (alteracoes > nequipes / 2) {
		deltaDist = INT_MAX;
	} else {

		for (int i = 0; i <= alteracoes; i++) {
			setjogo(aux1[i], t1, aux3[i]);
			setjogo(aux1[i], t2, aux2[i]);
		}
		deltaDist = coe_total;
		for (int i = 0; i <= alteracoes; i++) {
			setjogo(aux1[i], t1, aux2[i]);
			setjogo(aux1[i], t2, aux3[i]);
		}
		deltaDist = deltaDist - coe_total;
	}

}

void ptvef::avmovN6(const int& t, const int& r1, const int& r2, int &deltaDist,
		int& kind, int& depth) {

	es1++;
	if (es1 == 0) {
		es2++;
		if (es2 == 0)
			es3++;
	}

	int c = r1;
	int d = r2;
	int bestDelta = INT_MAX;
	int cdpathsize = 0;
	int dcpathsize = 0;
	int alteracoes;
	int v1;
	int v2;
	int w1 = 0;
	int wk = 0;
	int wlast;
	int clast;
	int oppc = jogo(t, c);
	int oppd = jogo(t, d);
	aux3[cdpathsize] = oppc;
	et[cdpathsize] = oppd;
	cdpathsize++;

	if (depth == 1) {
		avmovN3(c, oppc, oppd, deltaDist);
		kind = RRSWAP;
		return;
	} else {
		while (cdpathsize < depth && oppc != oppd) {
			swap(c, d);
			oppc = jogo(oppc, c);
			oppd = jogo(oppd, d);
			aux3[cdpathsize] = oppc;
			et[cdpathsize] = oppd;
			cdpathsize++;
		}

		v1 = oppc;
		v2 = oppd;

		if (v1 != v2) {
			w1 = jogo(v2, c);
			wk = jogo(v1, d);
		}

		if (w1 != wk) {
			wlast = w1;
			alteracoes = -1;
			do {
				clast = jRodada(v1, wlast);
				if (clast == r1 || clast == r2) {
					deltaDist = INT_MAX;
					return;
				}
				alteracoes++;
				er[alteracoes] = clast;
				aux1[alteracoes] = jogo(v1, clast);
				aux2[alteracoes] = jogo(v2, clast);
				wlast = jogo(v2, clast);
			} while (wlast != wk);
		}

		if (v1 == v2 || w1 == wk) {
			avmovN4(t, r1, r2, deltaDist);
			kind = LAST;
			return;
		} else {
			/**
			 * RRSWAP
			 */
			//cd-path swap
			c = r1;
			d = r2;
			setjogo(d, t, aux3[0]);
			setjogo(c, t, et[0]);
			for (int i = 0; i < cdpathsize - 1; i++) {
				swap(c, d);
				setjogo(d, aux3[i], aux3[i + 1]);
				setjogo(c, et[i], et[i + 1]);
			}

			if (cdpathsize % 2) {
				c = r1;
				d = r2;
			} else {
				c = r2;
				d = r1;
			}

			//Swap Part
			for (int i = 0; i <= alteracoes; i++) {
				setjogo(er[i], v1, aux2[i]);
				setjogo(er[i], v2, aux1[i]);
			}
			setjogo(c, v1, w1);
			setjogo(d, v2, wk);
			deltaDist = coe_total;

			//Restoring SWAP
			c = r2;
			d = r1;
			setjogo(d, t, aux3[0]);
			setjogo(c, t, et[0]);
			for (int i = 0; i < cdpathsize - 1; i++) {
				swap(c, d);
				setjogo(d, aux3[i], aux3[i + 1]);
				setjogo(c, et[i], et[i + 1]);
			}

			if (cdpathsize % 2) {
				c = r1;
				d = r2;
			} else {
				c = r2;
				d = r1;
			}

			for (int i = 0; i <= alteracoes; i++) {
				setjogo(er[i], v1, aux1[i]);
				setjogo(er[i], v2, aux2[i]);
			}
			setjogo(d, v1, wk);
			setjogo(c, v2, w1);
			deltaDist = deltaDist - coe_total;
			if (deltaDist < bestDelta) {
				kind = RRSWAP;
				bestDelta = deltaDist;
			}

			if (alteracoes > 0) {
				/*********
				 * RRFAN *
				 *********/

				//Build cd-path from w1 to wk
				if (cdpathsize % 2) {
					c = r1;
					d = r2;
				} else {
					c = r2;
					d = r1;
				}
				dcpathsize = 0;
				aux3[dcpathsize] = w1;
				dcpathsize++;
				do {
					aux3[dcpathsize] = jogo(aux3[dcpathsize - 1], d);
					dcpathsize++;
					swap(c, d);
				} while (aux3[dcpathsize - 1] != wk);

				/**
				 * EXCHANGING THE DC-PATH
				 */
				if (cdpathsize % 2) {
					c = r1;
					d = r2;
				} else {
					c = r2;
					d = r1;
				}
				for (int i = 0; i < dcpathsize - 1; i++) {
					setjogo(c, aux3[i], aux3[i + 1]);
					swap(c, d);
				}

				if (cdpathsize % 2) {
					c = r1;
					d = r2;
				} else {
					c = r2;
					d = r1;
				}

				//ROTATING THE FAN
				for (int i = 0; i <= alteracoes; i++) {
					setjogo(er[i], v1, aux2[i]);
					setjogo(er[i], v2, aux1[i]);
				}
				setjogo(c, v2, wk);
				setjogo(d, v1, w1);
				deltaDist = coe_total;

				//RESTORING THE DC-PATH
				for (int i = 0; i < dcpathsize - 1; i++) {
					setjogo(d, aux3[i], aux3[i + 1]);
					swap(c, d);
				}

				if (cdpathsize % 2) {
					c = r1;
					d = r2;
				} else {
					c = r2;
					d = r1;
				}

				//RESTORING THE FAN
				for (int i = 0; i <= alteracoes; i++) {
					setjogo(er[i], v1, aux1[i]);
					setjogo(er[i], v2, aux2[i]);
				}
				setjogo(c, v2, w1);
				setjogo(d, v1, wk);
				deltaDist = deltaDist - coe_total;
				if (deltaDist < bestDelta) {
					kind = RRFAN;
					bestDelta = deltaDist;
				}

			}
		}
	}

	deltaDist = bestDelta;
}

void ptvef::avmovN4(const int& team, const int& r1, const int& r2,
		int &deltaDist) {

	es1++;
	if (es1 == 0) {
		es2++;
		if (es2 == 0)
			es3++;
	}

	int controle = 0;

	int t = team;
	int t1, t2;
	int alteracoes = -1;
	deltaDist = 0;
	int tp = t;
	int count = 0;
	do {
		alteracoes++;
		t1 = jogo(t, r1);
		t2 = jogo(t, r2);
		grupo_e[t] = 1;
		grupo_e[t1] = 1;

		aux1[alteracoes] = t;
		aux2[alteracoes] = t2;

		t = jogo(t2, r1);
		count++;
	} while (t != tp);

	if (alteracoes == 1) {
		deltaDist = INT_MAX;
		return;
	}

	for (int i = 0; i <= alteracoes; i++) {
		deltaDist += avaliaDistanciaTrocaAdv(aux1[i], r1, r2)
				+ avaliaDistanciaTrocaAdv(aux2[i], r1, r2);
	}

	for (int i = 0; i <= alteracoes; i++) {
		controle += restaura(aux1[i], r1, r2) + restaura(aux2[i], r1, r2);
	}
	if (deltaDist + controle)
		cout << "error" << endl;

}

void ptvef::movN3(const int& round, const int& t1, const int& t2) {
// Opponent of team t1
	int at1;

// Opponent of team t2
	int at2;

// Auxiliary round
	int ra;

	int r = round;

	int r_stop = r;

	do {
		at1 = jogo(t1, r);
		at2 = jogo(t2, r);
		ra = jRodada(t1, at2);

		setjogo(r, t1, at2);
		setjogo(r, t2, at1);
		r = ra;
	} while (r != r_stop);

}

void ptvef::movN4(const int& team, const int& r1, const int& r2) {

	int t = team;
	int t1, t2;
	int tp = t;
	int i;

	do {
		t1 = jogo(t, r1);
		t2 = jogo(t, r2);
		i = jogo(t2, r1);
		setjogo(r1, t, t2);
		setjogo(r2, t, t1);
		t = i;
	} while (t != tp);

}

void ptvef::movN6(const int& t, const int& r1, const int& r2, const int& kind,
		int& depth) {

	if (kind == LAST)
		movN4(t, r1, r2);

	int c = r1;
	int d = r2;
	int cdpathsize = 0;
	int dcpathsize = 0;
	int alteracoes = -1;
	int v1;
	int v2;
	int w1 = 0;
	int wk = 0;
	int wlast;
	int clast;
	int oppc = jogo(t, c);
	int oppd = jogo(t, d);
	aux3[cdpathsize] = oppc;
	et[cdpathsize] = oppd;
	cdpathsize++;
	bool PRS = false;

	if (depth == 1) {
		movN3(c, oppc, oppd);
		return;
	} else {

		while (cdpathsize < depth && oppc != oppd) {
			swap(c, d);
			oppc = jogo(oppc, c);
			oppd = jogo(oppd, d);
			aux3[cdpathsize] = oppc;
			et[cdpathsize] = oppd;
			cdpathsize++;
		}

		v1 = oppc;
		v2 = oppd;

		if (v1 != v2) {
			w1 = jogo(v2, c);
			wk = jogo(v1, d);
		}

		if (w1 != wk) {
			wlast = w1;
			alteracoes = -1;
			do {
				clast = jRodada(v1, wlast);
				if (clast == r1 || clast == r2) {
					return;
				}
				alteracoes++;
				er[alteracoes] = clast;
				aux1[alteracoes] = jogo(v1, clast);
				aux2[alteracoes] = jogo(v2, clast);
				wlast = jogo(v2, clast);
			} while (wlast != wk);
		}

		if (kind == RRSWAP) {
			if (v1 == v2 || w1 == wk) {
				movN4(t, r1, r2);
				return;
			} else {
				/**
				 * RRSWAP
				 */
				//cd-path swap
				c = r1;
				d = r2;
				setjogo(d, t, aux3[0]);
				setjogo(c, t, et[0]);
				for (int i = 0; i < cdpathsize - 1; i++) {
					swap(c, d);
					setjogo(d, aux3[i], aux3[i + 1]);
					setjogo(c, et[i], et[i + 1]);
				}

				if (cdpathsize % 2) {
					c = r1;
					d = r2;
				} else {
					c = r2;
					d = r1;
				}

				//Swap Part
				for (int i = 0; i <= alteracoes; i++) {
					setjogo(er[i], v1, aux2[i]);
					setjogo(er[i], v2, aux1[i]);
				}
				setjogo(c, v1, w1);
				setjogo(d, v2, wk);
			}
		} else if (kind == RRFAN && cdpathsize > 1 && !PRS && alteracoes > 0) {
			/*********
			 * RRFAN *
			 *********/
			//Build cd-path from w1 to wk
			if (cdpathsize % 2) {
				c = r1;
				d = r2;
			} else {
				c = r2;
				d = r1;
			}
			dcpathsize = 0;
			aux3[dcpathsize] = w1;
			dcpathsize++;
			do {
				aux3[dcpathsize] = jogo(aux3[dcpathsize - 1], d);
				dcpathsize++;
				swap(c, d);
			} while (aux3[dcpathsize - 1] != wk);

			/**
			 * EXCHANGING THE DC-PATH
			 */
			if (cdpathsize % 2) {
				c = r1;
				d = r2;
			} else {
				c = r2;
				d = r1;
			}
			for (int i = 0; i < dcpathsize - 1; i++) {
				setjogo(c, aux3[i], aux3[i + 1]);
				swap(c, d);
			}

			if (cdpathsize % 2) {
				c = r1;
				d = r2;
			} else {
				c = r2;
				d = r1;
			}

			//ROTATING THE FAN
			for (int i = 0; i <= alteracoes; i++) {
				setjogo(er[i], v1, aux2[i]);
				setjogo(er[i], v2, aux1[i]);
			}
			setjogo(c, v2, wk);
			setjogo(d, v1, w1);
		}
	}
}

void ptvef::buscaLocal() {

	if (tbl == "n3Un4")
		buscaLocalN3UN4();
	else if (tbl == "n6")
		buscaLocalN6();
	else {
		cout << "I can't recognize the neighborhood " << tbl << endl;
		exit(1);
	}
}

/**
 * Neigborhood: Partial Round Swap
 */
void ptvef::neighborhoodN4() {

// Delta variation computed by evMovN4
	int delta;

// Best ceov (carry-over effect value) found
	int bestDelta;

// Team parameter
	int t;

// round 1 parameter
	int round1;

// round 2 parameter
	int round2;

// Best neighborhood. In this part of the code there is only one neighborhood
	int bestNeigborhood;

	do {
		bestNeigborhood = 0;
		bestDelta = 0;

		for (int r1 = 0; r1 < nrodadas; r1++) {
			for (int r2 = r1 + 1; r2 < nrodadas; r2++) {
				// used to avoid check the same group of teams twice
				fill(grupo_e.begin(), grupo_e.end(), -1);
				for (int team = 0; team < nequipes; team++) {
					if (grupo_e[team] == -1) {
						avmovN4(team, r1, r2, delta);
						if (delta < bestDelta) {
							t = team;
							round1 = r1;
							round2 = r2;
							bestNeigborhood = 4;
							bestDelta = delta;
						}

					}
				}
			}
		}

		if (bestNeigborhood) {
			movN4(t, round1, round2);
		}

	} while (bestNeigborhood);

	saveImprovement();

}

void ptvef::buscaLocalN6() {

#ifdef DEBUG
	int improving = 0;
	int nmovimentos = 0;
#endif

	int deltaDist;
	int n;
	int depth;
	int kind;
	int mDist;

	vector<int> mKind;
	vector<int> mDepthDist;
	vector<int> mT;
	vector<int> mR1;
	vector<int> mR2;
	int mDvizinhanca;

	do {
#ifdef DEBUG
		improving = 0;
#endif
		mDvizinhanca = 0;
		mDist = 0;

		for (int r1 = 0; r1 < nrodadas; r1++) {
			for (int r2 = r1 + 1; r2 < nrodadas; r2++) {

				for (int t = 0; t < nequipes; t++) {

					for (depth = 1; depth < nequipes / 2; depth++) {

						avmovN6(t, r1, r2, deltaDist, kind, depth);

						if (deltaDist < 0){
							if( deltaDist < mDist) {
								mT.resize(0);
								mR1.resize(0);
								mR2.resize(0);
								mKind.resize(0);
								mDepthDist.resize(0);

								mKind.push_back(kind);
								mDepthDist.push_back(depth);
								mT.push_back(t);
								mR1.push_back(r1);
								mR2.push_back(r2);
								mDvizinhanca = 5;
								mDist = deltaDist;
							}
							if( deltaDist == mDist){
								// printf("%u ",mT.size());
								mT.push_back(t);
								mR1.push_back(r1);
								mR2.push_back(r2);
								mKind.push_back(kind);
								mDepthDist.push_back(depth);
								mDvizinhanca = 5;
								mDist = deltaDist;
							}
							// printf("oi\n");
						}
					}
				}
			}
		}

		if (mDvizinhanca) {
			n = rand()%mT.size();
			movN6(mT[n], mR1[n], mR2[n], mKind[n], mDepthDist[n]);
		}

#ifdef DEBUG
		cout << ++nmovimentos << "\t" << improving << "\t" << coe_total << endl;
#endif

	} while (mDvizinhanca);

	saveImprovement();

}

int ptvef::buscaLocalN3UN4() {
//int ptvef::buscaLocalN3UN4() {

#ifdef DEBUG
	int nMovimentos = 0;
	int improving = 0;
#endif

	int n;
	int deltaDist;
	vector<int> m3Di;
	vector<int> m3Dj;
	vector<int> m3Dk;
	int mDist;				//melhor dist�ncia com vio<=0
	vector<int> m4Di;
	vector<int> m4Dj;
	vector<int> m4Dk;
	int mDvizinhanca;

	do {
#ifdef DEBUG
		improving = 0;
#endif

		mDvizinhanca = 0;
		mDist = 0;

		for (int j = 0; j < nequipes; j++) {
			for (int k = j + 1; k < nequipes; k++) {

				fill(grupo_r.begin(), grupo_r.end(), -1);
				fill(grupo_e.begin(), grupo_e.end(), -1);

				for (int i = 0; i < nequipes; i++) {
					if (i < nrodadas && j != jogo(k, i) && grupo_r[i] == -1) {
						avmovN3(i, j, k, deltaDist);
						if (deltaDist < 0){
							if( deltaDist < mDist) {
								m3Di.resize(0);
								m3Dj.resize(0);
								m3Dk.resize(0);

								m3Di.push_back(i);
								m3Dj.push_back(j);
								m3Dk.push_back(k);
								mDvizinhanca = 3;
								mDist = deltaDist;
							}
							if( deltaDist == mDist) {
								m3Di.push_back(i);
								m3Dj.push_back(j);
								m3Dk.push_back(k);
								mDvizinhanca = 3;
								mDist = deltaDist;
							}
						}
					}
					if (j < nrodadas && k < nrodadas && grupo_e[i] == -1) {
						avmovN4(i, j, k, deltaDist);
						if (deltaDist < 0){
							if( deltaDist < mDist) {
								m4Di.resize(0);
								m4Dj.resize(0);
								m4Dk.resize(0);

								m4Di.push_back(i);
								m4Dj.push_back(j);
								m4Dk.push_back(k);
								mDvizinhanca = 4;
								mDist = deltaDist;
							}
							if( deltaDist == mDist) {
								m4Di.push_back(i);
								m4Dj.push_back(j);
								m4Dk.push_back(k);
								mDvizinhanca = 4;
								mDist = deltaDist;
							}
						}

					}

				}

			}
		}

		if (mDvizinhanca==3) {
			n = rand()%m3Di.size();
			movN3(m3Di[n], m3Dj[n], m3Dk[n]);
			
		}

		else if (mDvizinhanca==4) {
			n = rand()%m4Di.size();
			movN4(m4Di[n], m4Dj[n], m4Dk[n]);
		} 

	} while (mDvizinhanca);
	saveImprovement();

	return 0;

}

ptvef::solucao::solucao(int nequipes) {
	coe = INT_MAX;
//	ifat = 0;
	jogos.resize(nequipes, vector<int>(nequipes - 1));
	coeM.resize(nequipes, vector<int>(nequipes));
	jogoRodada.resize(nequipes, vector<int>(nequipes));
	prodadas.resize(nequipes - 1);
}

void ptvef::printdiff(const ptvef::solucao &pt1, const ptvef::solucao &pt2) {
	for (int t = 0; t < nequipes; t++) {
		for (int r = 0; r < nequipes - 1; r++) {
			if (pt1.jogos[t][r] == pt2.jogos[t][r])
				cout << setw(3) << " ";
			else
				cout << setw(3) << pt2.jogos[t][r];
		}
		cout << endl;
	}

}

void ptvef::solucao::cola(ptvef &pt) {
	for (int r = 0; r < pt.nequipes - 1; r++) {
		for (int t = 0; t < pt.nequipes; t++) {
			pt.jogos[t][r] = jogos[t][r];
		}
	}

	for (int i = 0; i < pt.nequipes; i++) {
		for (int j = 0; j < pt.nequipes; j++) {
			pt.jogoRodada[i][j] = jogoRodada[i][j];
			pt.coeMatrix[i][j] = coeM[i][j];
		}
	}
	pt.coe_total = coe;
//	pt.ifat = ifat;
}

//bool ptvef::solucao::operator<=(const solucao& a,const solucao& b) {
//    return a.distancia <= b.distancia;
//}

void ptvef::solucao::copia(const ptvef &pt) {
	for (int r = 0; r < pt.nequipes - 1; r++) {
		for (int t = 0; t < pt.nequipes; t++) {
			jogos[t][r] = pt.jogos[t][r];
		}
	}
	for (int i = 0; i < pt.nequipes; i++) {
		for (int j = 0; j < pt.nequipes; j++) {
			jogoRodada[i][j] = pt.jogoRodada[i][j];
			coeM[i][j] = pt.coeMatrix[i][j];
		}
	}
	coe = pt.coe_total;
//	ifat = pt.ifat;

}

void ptvef::printSolution() {

	cout << "opponents" << endl;
	for (int i = 0; i < nequipes; i++) {
		for (int rod = 0; rod < nrodadas; rod++) {
			cout << setw(3) << jogo(i, rod);
		}
		cout << endl;
	}
	cout << "matches" << endl;
	for (int i = 0; i < nequipes; i++) {
		for (int rod = 0; rod < nequipes; rod++) {
			if (i != rod)
				cout << setw(3) << jRodada(i, rod);
			else
				cout << setw(3) << 0;
		}
		cout << endl;
	}
	cout << "wcoev" << endl;
	int coecalculado = 0;
	for (int i = 0; i < nequipes; i++) {
		for (int j = 0; j < nequipes; j++) {
			cout << setw(3) << coeMatrix[i][j];
			coecalculado += weight[i][j] * coeMatrix[i][j] * coeMatrix[i][j];
		}
		cout << endl;
	}
	cout << "coe_total: " << coe_total << endl;
	cout << "coe_calcu: " << coecalculado << endl;
}

int ptvef::parser(string arquivo) {
	ifstream file_instancias(arquivo.c_str());
	if (!file_instancias.is_open()) {
		cout << "file " << arquivo << " not found\n" << flush;
		exit(0);
	}

	file_instancias >> nequipes;

	weight = new int*[nequipes];
	for (int i = 0; i < nequipes; i++)
		weight[i] = new int[nequipes];

	for (int i = 0; i < nequipes; i++) {
		for (int j = 0; j < nequipes; j++) {
			file_instancias >> weight[i][j];
		}
	}

	carryover = false;
//	melhorSolucaoConhecidaViolacoes = 0;
	melhorSolucaoConhecidaDistancia = 0;

	iddleiterations = 100;
	maxrandommoves = 2;

//	aux1.resize(nequipes);
//	aux2.resize(nequipes);
//	aux3.resize(nequipes);
//	et.resize(nequipes);
//	er.resize(nequipes);

	aux1 = new int[3 * nequipes];				//.resize(nequipes);
	aux2 = new int[3 * nequipes];				//.resize(nequipes);
	aux3 = new int[3 * nequipes];				//.resize(nequipes);
	et = new int[nequipes];				//.resize(nequipes);
	er = new int[nequipes];				//.resize(nequipes);

	grupo_r.resize(nequipes);
	grupo_e.resize(nequipes);

	fatoracoes_iniciais.geraFatoracoes(nequipes);

	nrodadas = nequipes - 1;
	dtime.resize(nequipes);

//	coeMatrix.resize(nequipes, vector<int>(nequipes));
	coeMatrix = new int*[nequipes];
	for (int i = 0; i < nequipes; i++)
		coeMatrix[i] = new int[nequipes];

//jogoRodada.resize(nequipes, vector<int>(nequipes));
	jogoRodada = new int*[nequipes];
	for (int i = 0; i < nequipes; i++)
		jogoRodada[i] = new int[nequipes];

//jogos.resize(nequipes, vector<int>(nequipes - 1));
	jogos = new int*[nequipes];
	for (int i = 0; i < nequipes; i++)
		jogos[i] = new int[nequipes - 1];

	embequipes.resize(nequipes);
	embrodadas.resize(nequipes - 1);

	for (int i = 0; i < nequipes - 1; i++) {
		embrodadas[i] = i;
	}
	for (int i = 0; i < nequipes; i++) {
		embequipes[i] = i;
	}

	solucaoInicial(0);
//	inviavel = 0;

	return 1;
}

int ptvef::movimentoAleatorio(const int& vizinhanca) {
	int t1 = 0;
	int t2 = 0, r1 = 0, r2 = 0, r = 0, t = 0;
	switch (vizinhanca) {
	case 3:
		while (t1 == t2 || jogo(t1, r) == t2) {
			t1 = aleatorioEntre(0, nequipes - 1);
			t2 = aleatorioEntre(0, nequipes - 1);
			r = aleatorioEntre(0, nequipes - 2);
		}
		movN3(r, t1, t2);
		break;
	case 4:    //PartidaEntreRodadas
		while (r1 == r2) {
			r1 = aleatorioEntre(0, nequipes - 2);
			r2 = aleatorioEntre(0, nequipes - 2);
		}
		t = aleatorioEntre(0, nequipes - 1);
		movN4(t, r1, r2);
		break;
	}

	return 0;
}

inline int ptvef::jogo(const int& equipe, const int& rodada) {
	if (rodada < 0)
		return jogos[equipe][nrodadas - 1];
	else if (rodada > nrodadas - 1)
		return jogos[equipe][0];
	else
		return jogos[equipe][rodada];
}

void ptvef::setjogo(const int& r, const int& t1, const int& t2) {

	if (carryover != false) {
		coe_total -= coeV(jogo(t1, r - 1), jogo(t1, r));
		downcoeV(jogo(t1, r - 1), jogo(t1, r));
		upcoeV(jogo(t1, r - 1), t2);
		coe_total += coeV(jogo(t1, r - 1), t2);

		coe_total -= coeV(jogo(t1, r), jogo(t1, r + 1));
		downcoeV(jogo(t1, r), jogo(t1, r + 1));
		upcoeV(t2, jogo(t1, r + 1));
		coe_total += coeV(t2, jogo(t1, r + 1));

		coe_total -= coeV(jogo(t2, r - 1), jogo(t2, r));
		downcoeV(jogo(t2, r - 1), jogo(t2, r));
		upcoeV(jogo(t2, r - 1), t1);
		coe_total += coeV(jogo(t2, r - 1), t1);

		coe_total -= coeV(jogo(t2, r), jogo(t2, r + 1));
		downcoeV(jogo(t2, r), jogo(t2, r + 1));
		upcoeV(t1, jogo(t2, r + 1));
		coe_total += coeV(t1, jogo(t2, r + 1));

	}

	jogos[t1][r] = t2;
	jogos[t2][r] = t1;

	if (t1 < t2) {
		jogoRodada[t1][t2] = r;
	} else {
		jogoRodada[t2][t1] = r;
	}
}

int ptvef::jRodada(const int& i, const int& j) {
	if (i < j) {
		return jogoRodada[i][j];
	} else {
		return jogoRodada[j][i];
	}
}

void ptvef::imprime_jRodadas() {
	for (int i = 0; i < nequipes; i++) {
		cout << i << ": ";
		for (int j = 0; j < nequipes; j++) {
			if (i != j) {
				cout << (int) jRodada(i, j) << " ";
			} else {
				cout << "_ ";
			}
		}
		cout << "\n";
	}
}

void ptvef::saveImprovement() {

	if (melhorSolucaoConhecidaDistancia == 0
			|| coe_total < melhorSolucaoConhecidaDistancia) {

		melhorSolucaoConhecidaDistancia = coe_total;

		salvar();

	}

}

int ptvef::ils(double segundos, int bl) {

	int melhordistancia;
	int d;
	int iteracoes;
//	int iteracoesM;
	int iteracoesTotal = 0;
	int troca;

	tempo ti, tf;
	ti.getTime();

	ptvef::solucao atual(nequipes);
	ptvef::solucao melhor(nequipes);

	buscaLocal();

	atual.copia(*this);
	melhor.copia(*this);
	melhordistancia = d = coe_total;

	iteracoes = 0;
//	iteracoesM = 0;

	for (int i = 0; i < aleatorioEntre(3, maxrandommoves); i++) {
		movimentoAleatorio(aleatorioEntre(4, 5));
	}

	while (1) {
		iteracoesTotal++;
//		iteracoesM++;

		//printSolution();
		atual.cola(*this);
		//recalcula();
		//printSolution();

		tf.getTime();
		if ((tf - ti).usuario >= segundos) {
			melhor.cola(*this);
			//recalcula();
			return iteracoesTotal;
		}
		iteracoes++;
		//cout<<"@"<<iteracoes<<endl;

		troca = 0;

		for (int i = 0; i < aleatorioEntre(3, maxrandommoves); i++) {
			movimentoAleatorio(aleatorioEntre(4, 5));
		}

		buscaLocal();

		if (coe_total != melhordistancia && coe_total < d) {
			troca = 1;
		} else {
//			if (coe_total != d && coe_total != melhordistancia)
			//{
			if (iteracoes > iddleiterations && coe_total < d * 1.01) {
				iddleiterations = min(iddleiterations + 100, 1000);
				maxrandommoves = min(maxrandommoves + 2, 10);
				troca = 1;
			}
			//}
		}

		if (troca) {

			d = coe_total;
			tf.getTime();

			if (d < melhordistancia) {
//				iteracoesM = 0;
				iddleiterations = max(iddleiterations - 100, 100);
//				cout<<"-"<<iddleiterations<<endl;
				maxrandommoves = max(maxrandommoves - 2, 5);
//				cout<<"-"<<maxrandommoves<<endl;
				melhordistancia = d;
				melhor.copia(*this);
			}

			atual.copia(*this);

			iteracoes = 0;
		}

	}

	return 0;

}

tabela ptvef::ils(int seg) {

	tempo t0, t1;
	tabela media;
	t0.getTime();

	if (tsi == canonica) {
		//solucaoInicial(aleatorioEntre(1, nfat - 1), 1);
		solucaoInicial(0, 0);
	} else {
		solucaoinicialvizing();
	}

	ils(seg, 0);

	t1.getTime();

	media.insere("time", (t1 - t0).usuario);
	media.insere("cost", melhorSolucaoConhecidaDistancia);

	return media;

}

tabela ptvef::state() {
	tempo t0, t1;
	tabela media;
	t0.getTime();
	int dedo;

	ptvef::solucao base(nequipes);
	ptvef::solucao vizinho1(nequipes);
	ptvef::solucao vizinho2(nequipes);

	/**
	 * Build a starting fixture;
	 */
	if (tsi == canonica) {
		//solucaoInicial(aleatorioEntre(1, nfat - 1), 1);
		solucaoInicial(0, 0);
	} else if (tsi == vizing) {
		solucaoinicialvizing();
	} else {
		cout << "tsi desconhecido" << endl;
	}

	dedo = 4;
	base.copia(*this);
	printSolution();
	movN6(5, 0, 1, 1, dedo);
	printSolution();
	base.cola(*this);
	dedo = 2;
	movN6(2, 0, 1, 0, dedo);
	printSolution();
	base.cola(*this);

//	for (int r1 = 0; r1 < nrodadas; r1++) {
//		for (int r2 = r1 + 1; r2 < nrodadas; r2++) {
//			for (int t = 0; t < nequipes; t++) {
//				for (int depth = 1; depth < nequipes / 2; depth++) {
//					for (int kind = 0; kind <= 1; kind++) {
//						base.cola(*this);
//						if (movN6(t, r1, r2, kind, depth)) {
//							vizinho1.copia(*this);
//							for (int s1 = r1; s1 < nrodadas; s1++) {
//								for (int s2 = s1 + 1; s2 < nrodadas; s2++) {
//									for (int e = t; e < nequipes; e++) {
//										for (int d = 1; d < nequipes / 2; d++) {
//											for (int k = 0; k <= 1; k++) {
//												base.cola(*this);
//												if (!(r1 == s1 && r2 == s2
//														&& t == e && depth == d
//														&& kind == k))
//													if (movN6(e, s1, s2, k,
//															d)) {
//														vizinho2.copia(*this);
//														if (vizinho1
//																== vizinho2) {
//															cout << "r1: "
//																	<< r1;
//															cout << " ,r2: "
//																	<< r2;
//															cout << " ,t: "
//																	<< t;
//															cout << " ,depth: "
//																	<< depth;
//															cout << " ,kind: "
//																	<< kind;
//															cout << " ,cost: "
//																	<< vizinho1.coe
//																	<< endl;
//															printdiff(base,
//																	vizinho1);
//															cout << "r1: "
//																	<< s1;
//															cout << " ,r2: "
//																	<< s2;
//															cout << " ,t: "
//																	<< e;
//															cout << " ,depth: "
//																	<< d;
//															cout << " ,kind: "
//																	<< k;
//															cout << " ,cost: "
//																	<< vizinho2.coe
//																	<< endl;
//															printdiff(base,
//																	vizinho2);
//														}
//													}
//											}
//										}
//									}
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//	}

	media.insere("time", (t1 - t0).usuario);
	return media;
}

void ptvef::abrir() {

	int dado;
	string arq = to_string(nequipes-1);
	ifstream arquivo(arq);

	if (!arquivo.is_open()) {
		cout<<"falha ao abrir "<<nequipes-1<<endl;;
	}

	arquivo >> dado;

	for (int i = 0; i < nequipes; i++) {
		for (int rod = 0; rod < nequipes - 1; rod++) {
			arquivo >> dado;
			setjogo(rod, i, dado);
		}
	}
	arquivo.close();

	recalcula();
}

void ptvef::salvar() {

	string arq = to_string(nequipes);

	arq.append("_");
	arq.append(to_string(tsi));
	arq.append("_");
	arq.append(tbl);

	fstream arquivo(arq.c_str(), ios::out);

	arquivo << nequipes << endl;

	for (int i = 0; i < nequipes; i++) {
		for (int rod = 0; rod < nequipes - 1; rod++) {
			arquivo << setw(2) << (int) jogo(i, rod) << "  ";
		}
		arquivo << "\n";
	}
	arquivo << coe_total << endl;
	arquivo.close();

}

tabela ptvef::ilsGuedes() {


	fill(grupo_e.begin(), grupo_e.end(), -1);
	tempo t0, t1;
	tabela media;
	t0.getTime();
//	int iteracoes;
//	int badMove = 0;
//	int iddleIteration = 0;
//	double beta = b;
	ptvef::solucao atual(nequipes);
	ptvef::solucao melhor(nequipes);
	tempo ti, tf;
	ti.getTime();
	//int runtime = nequipes*nequipes*nequipes/2;
//	es1 = 0;
//	es2 = 0;
//	es3 = 0;
//	unsigned long long ilsiterations = 0;
//	unsigned long long innerloop = 0;
	//for (int i = 0; i < 10 * nequipes; i++) {
//	while ((tf - ti).usuario < runtime) {
		atual.coe = INT_MAX;
//		ilsiterations++;

		/**
		 * Build a starting fixture;
		 */

		if (tsi == canonica) {
			//solucaoInicial(aleatorioEntre(1, nfat - 1), 1);
			solucaoInicial(0, 1);
		} else if (tsi == vizing) {
			solucaoinicialvizing();
		} else if (tsi == kidd) {
			abrir();
		}

		if (coe_total < atual.coe) {
			atual.copia(*this);
			if (coe_total < melhor.coe)
				melhor.copia(*this);
		}


#ifdef DEBUG
		buscaLocal();
		exit(1);
#endif

		/**
		 * Obtain a new solution by applying a perturbation;
		 */
		// for (int i = 0; i < 5; i++) {
			// movimentoAleatorio(3);
		// }

		/**
		 * Improve the initial solution by local search;
		 */
		buscaLocal();
		tf.getTime();
//		if ((tf - ti).usuario >= runtime)
//			break;
		/**
		 * Update the best initial solution;
		 */
		if (coe_total < atual.coe) {
			atual.copia(*this);
			if (coe_total < melhor.coe)
				melhor.copia(*this);
		}


//		iteracoes = 0;
//		do {
//			innerloop++;
//			atual.cola(*this);
//			iteracoes++;
//			/**
//			 * Obtain a new solution S′ by applying a perturbation to S;
//			 */
//			for (int i = 0; i < aleatorioEntre(1, maxrandommoves); i++) {
//				movimentoAleatorio(aleatorioEntre(3, 4));
//			}
//
//			/**
//			 * Apply local search to solution S′;
//			 */
//			buscaLocal();
//			tf.getTime();
//			if ((tf - ti).usuario >= runtime)
//				break;
//			/**
//			 * Replace the current solution S by S′ using an acceptance criterion;
//			 */
//			if (coe_total <= (atual.coe * (1 + beta))) {
//				beta = b;
//				iddleIteration = 0;
//
//				if (coe_total >= atual.coe)
//					badMove++;
//
//				atual.copia(*this);
//				/**
//				 * Update the best known solution S∗;
//				 */
//				if (coe_total < melhor.coe) {
//					melhor.copia(*this);
//					badMove = 0;
//				}
//			} else {
//				iddleIteration++;
//				if (iddleIteration >= 2 * nequipes)
//					beta *= 2;
//			}
//
//			/**
//			 * until a stopping criterion is reached
//			 */
//		} while (badMove < maxBadMoves);

//		tf.getTime();
//		if ((tf - ti).usuario >= runtime)
//			break;
//	}
	t1.getTime();
	melhor.cola(*this);
	salvar();
	media.insere("time", (t1 - t0).usuario);
	media.insere("cost", melhorSolucaoConhecidaDistancia);
//	media.insere("es3", es3);
//	media.insere("es2", es2);
//	media.insere("es1", es1);
//	media.insere("ils_iter", ilsiterations);
//	media.insere("inner_iter", innerloop);
	return media;


}
