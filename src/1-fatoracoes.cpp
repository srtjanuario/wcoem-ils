/**
 Copyright (C) 2011  Dilson Lucas Pereira

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
 DISCLAIMED. IN NO EVENT SHALL DILSON LUCAS PEREIRA BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "1-fatoracoes.h"
#include <math.h>

int fatoracao::valida_solucao() {
	int jogos_equipe[nequipest];
	for (int e = 0; e < nequipest; e++) {
		for (int r = 0; r < nequipest; r++) {
			jogos_equipe[r] = 0;
		}
		for (int r = 0; r < nequipest - 1; r++) {
			jogos_equipe[jogo(e, r)]++;
		}
		for (int r = 0; r < nequipest; r++) {
			if (r == e) {
				if (jogos_equipe[r] > 0) {
					return 0;
				}
			} else {
				if (jogos_equipe[r] != 1) {
					return 0;
				}
			}
		}
	}
	int vezes_por_rodada[nequipest];
	for (int r = 0; r < nequipest - 1; r++) {
		for (int e = 0; e < nequipest; e++) {
			vezes_por_rodada[e] = 0;
		}
		for (int e = 0; e < nequipest; e++) {
			vezes_por_rodada[jogo(e, r)]++;
		}
		for (int e = 0; e < nequipest; e++) {
			if (vezes_por_rodada[e] != 1) {
				return 0;
			}
		}
	}
	return 1;
}

fatoracao::fatoracao() {
	nequipest = 0;
	nequipes = 0;
	nrodadas = 0;
	//tabela = 0;
}

fatoracao::fatoracao(int _n) {
	nequipes = _n;
	if (nequipes % 2) {
		nrodadas = nequipes;
		nequipest = nequipes + 1;
	} else {
		nrodadas = nequipes - 1;
		nequipest = nequipes;
	}

	tabela.resize(nequipest, vector<int>(nrodadas));

//	tabela = new int*[nequipest];
//	for (int i = 0; i < nequipest; i++) {
//		tabela[i] = new int[nrodadas];
//	}

}

fatoracao::fatoracao(fatoracao &a, fatoracao &b, bipartido &bip) {
	*this = combina(a, b, bip);
}

void fatoracao::setjogo(int rodada, int equipe1, int equipe2) {
	tabela[equipe1][rodada] = equipe2;
	tabela[equipe2][rodada] = equipe1;
}

int fatoracao::jogo(int equipe, int rodada) {
	return tabela[equipe][rodada];
}

ostream & operator <<(ostream &co, fatoracao & f) {
	for (int e = 0; e < f.nequipest; e++) {
		cout << setw(2) << e << ": ";
		for (int r = 0; r < f.nrodadas; r++) {
			if (f.jogo(e, r) == f.nequipes || e == f.nequipes) {
				cout << setw(3) << "*" << " ";
			} else {
				cout << setw(3) << f.jogo(e, r) << " ";
			}
		}
		cout << "\n";
	}
	return co;
}

void fatoracao::canonica() {
	int p1;
	int p2;
	for (int i = 0; i < nequipest - 1; i++) {
		setjogo(i, i, nequipest - 1);
		p1 = i;
		p2 = i;
		for (int j = 0; j < (nequipest - 1) / 2; j++) {
			p1 = (p1 - 1) % (nequipest - 1);
			p2 = (p2 + 1) % (nequipest - 1);
			if (p1 < 0) {
				p1 += (nequipest - 1);
			}
			setjogo(i, p1, p2);
		}
	}
	stringstream ss(stringstream::in | stringstream::out);
	ss << "c_" << nequipes;
	nome = ss.str();

}

fatoracao fatoracao::combina(fatoracao &a, fatoracao &b, bipartido &bip,
		int decequipes) {

	fatoracao f(bip.nequipes);

	if (a.nequipes == b.nequipes && 2 * a.nequipes == bip.nequipes) {

		for (int e = 0; e < a.nequipest; e++) {
			for (int r = 0; r < a.nrodadas; r++) {
				f.setjogo(r, e, a.jogo(e, r));
				f.setjogo(r, e + a.nequipest, b.jogo(e, r) + a.nequipest);
			}
		}

		for (int e = 0; e < bip.nrodadas; e++) {
			for (int r = 0; r < bip.nrodadas; r++) {
				f.setjogo(r + a.nrodadas, e, bip.jogo(e, r));
			}
		}

	} else if (a.nequipes == b.nequipes && 2 * a.nequipes - 2 == bip.nequipes) {
		for (int r = 0; r < a.nrodadas; r++) {
			for (int e = 0; e < a.nequipest; e++) {
				if (a.jogo(e, r) == a.nequipes - 1 || e == a.nequipes - 1) {
				} else {
					f.setjogo(r, e, a.jogo(e, r));
				}

				if (b.jogo(e, r) == b.nequipes - 1 || e == b.nequipes - 1) {
				} else {
					f.setjogo(r, e + a.nequipes - 1,
							b.jogo(e, r) + a.nequipes - 1);
				}
			}
			f.setjogo(r, a.jogo(a.nequipes - 1, r),
					b.jogo(b.nequipes - 1, r) + a.nequipes - 1);
		}

		int eq[f.nequipest];

		for (int r = 0; r < a.nrodadas; r++) {
			int t1 = r;
			int t2 = bip.jogo(r, 0);
			int t3 = a.jogo(a.nequipes - 1, r);
			int t4 = b.jogo(b.nequipes - 1, r) + a.nequipes - 1;
			eq[t1] = t3;
			eq[t2] = t4;
		}

		for (int e = 0; e < bip.nrodadas; e++) {
			for (int r = 1; r < bip.nrodadas; r++) {
				f.setjogo(r - 1 + a.nrodadas, eq[e], eq[bip.jogo(e, r)]);
			}
		}

	} else {
		cout << "Erro: combina fatoracao\n";
	}

	stringstream ss(stringstream::in | stringstream::out);
	ss << f.nequipes;
	f.nome = string("(") + a.nome + " " + b.nome + " " + string(",") + bip.nome
			+ string(")_") + ss.str();

	return f;

}

void fatoracao::geraFatoracoes(int n) {
	int m;
	unsigned int j, k;

	bipartido bip;

	bip.geraFatoracoes(n + 1);

	//cout<<"bipartidos:"<< bip.bipartidos[n].size()<<"\n";
	fatoracoes.resize(n + 1);
	for (int i = 2; i <= n; i += 2) {
		fatoracoes[i].push_back(fCanonica(i));
	}
	for (int i = 8; i <= n; i += 2) {
		m = i / 2 + i / 2 % 2;

		for (j = 0; j < fatoracoes[m].size(); j++) {
			for (k = j; k < fatoracoes[m].size(); k++) {
				//  for (int l=0;l<1;l++) {
				for (unsigned l = 0; l < bip.bipartidos[i].size(); l++) {
					fatoracao f = combina(fatoracoes[m][j], fatoracoes[m][k],
							bip.bipartidos[i][l]);
					if (!f.valida_solucao()) {
						cout << "Solucao nao valida\n";
						exit(0);
					}
					fatoracoes[i].push_back(f);
				}
			}
		}
	}

#if 0

	cout<<"Bipartido:--------------- \n";

	for (int i=2;i<bip.bipartidos.size();i+=2) {
		cout<<"Tamanho: "<<i<<" ";
		cout<<"Quantidade:"<< bip.bipartidos[i].size()<<"\n";

		for (int j=0;j<bip.bipartidos[i].size();j++) {
			cout<<i<<"-> "<<j+1<<": "<<bip.bipartidos[i][j].nome<<"\n\n";
			//   cout<<fatoracoes[i][j]<<"\n";
		}
		cout<<"\n\n";
	}
	cout<<"-----------------------------\n";

	cout<<"Completo:--------------- \n";

	for (int i=2;i<fatoracoes.size();i+=2) {
		cout<<"Tamanho: "<<i<<" ";
		cout<<"Quantidade:"<< fatoracoes[i].size()<<"\n";

		for (int j=0;j<fatoracoes[i].size();j++) {
			cout<<i<<"-> "<<j+1<<": "<<fatoracoes[i][j].nome<<"\n\n";
			//   cout<<fatoracoes[i][j]<<"\n";
		}
		cout<<"\n\n";
	}
	cout<<"-----------------------------\n";

#endif

}

fatoracao operator+(fatoracao &a, fatoracao &b) {
	if (a.nequipes % 2 == 0) {
		fatoracao f(2 * a.nequipest);
		for (int e = 0; e < a.nequipest; e++) {
			for (int r = 0; r < a.nrodadas; r++) {
				f.setjogo(r, e, a.jogo(e, r));
				f.setjogo(r, e + a.nequipest, b.jogo(e, r) + a.nequipest);
			}
		}

		int ri = a.nrodadas;

		for (int r = ri; r < f.nrodadas; r++) {
			//cout<<r<<": ";
			for (int k = 0; k < a.nequipest; k++) {
				int p1 = k;
				int p2 = (r - ri + k) % a.nequipest + a.nequipest;
				f.setjogo(r, p1, p2);
				//cout<<p1<<" - "<<p2<<" ";
			}
			//cout<<"\n";
		}

		f.nome = string("(") + a.nome + b.nome + string(")");
		return f;
	} else {
		fatoracao f(2 * a.nequipes);
		for (int e = 0; e < a.nequipest; e++) {
			for (int r = 0; r < a.nrodadas; r++) {
				if (a.jogo(e, r) == a.nequipes || e == a.nequipes) {
				} else {
					f.setjogo(r, e, a.jogo(e, r));
				}

				if (b.jogo(e, r) == b.nequipes || e == b.nequipes) {
				} else {
					f.setjogo(r, e + a.nequipes, b.jogo(e, r) + a.nequipes);
				}
			}
		}

		for (int r = 0; r < a.nrodadas; r++) {
			f.setjogo(r, a.jogo(a.nequipes, r),
					b.jogo(b.nequipes, r) + a.nequipes);
		}
		int ri = a.nrodadas;
		for (int r = ri; r < f.nrodadas; r++) {
			//cout<<r<<": ";
			for (int k = 0; k < a.nequipes; k++) {
				int p1 = a.jogo(a.nequipes, k);
				int p2 = b.jogo(b.nequipes, (k + 1 + r - ri) % b.nequipes)
						+ a.nequipes;
				f.setjogo(r, p1, p2);
				//	cout<<p1<<" - "<<p2<<" ";
			}
			//	cout<<"\n";
		}
		f.nome = string("(") + a.nome + b.nome + string(")");
		return f;

	}
}

void fatoracao::binaria() {
	int np2 = nequipest / 2;
	fatoracao f1(np2), f2(np2);
	f1.canonica();
	f2.canonica();
	int impar = 0;
	if (nequipes % 2) {
		impar = 1;
	}
	*this = f1 + f2;
	if (impar) {
		nequipes--;
	}
	//cout<<f1<<"\n"<<f2<<"\n";
}

void fatoracao::recursiva() {
	if (nequipes <= 4) {
		canonica();

	} else {
		int np2 = nequipest / 2;
		fatoracao f1(np2), f2(np2);
		f1.recursiva();
		f2.recursiva();

		int impar = 0;
		if (nequipes % 2) {
			impar = 1;
		}
		*this = f1 + f2;
		if (impar) {
			nequipes--;
		}
	}
}

fCanonica::fCanonica(int n) :
		fatoracao(n) {
	canonica();
}
;

fBinaria::fBinaria(int n) :
		fatoracao(n) {
	binaria();
}
;

ostream & operator <<(ostream &co, bipartido & f) {
	for (int e = 0; e < f.nequipest; e++) {
		cout << setw(2) << e << ": ";
		for (int r = 0; r < f.nrodadas; r++) {
			if (f.jogo(e, r) == f.nequipes || e == f.nequipes) {
				cout << setw(3) << "*" << " ";
			} else {
				cout << setw(3) << f.jogo(e, r) << " ";
			}
		}
		cout << "\n";
	}
	return co;
}

bipartido::bipartido(int n) {
	nequipest = n + n % 2;
	nequipes = n;
	nrodadas = nequipest / 2;
	pequipeg2 = nrodadas;

	tabela.resize(nequipest, vector<int>(nrodadas));

//	tabela = new int*[nequipest];
//	for (int i = 0; i < nequipest; i++) {
//		tabela[i] = new int[nrodadas];
//	}
}

bipartido::bipartido() {
	nequipest = 0;
	nequipes = 0;
	//tabela = 0;
	nrodadas = 0;
	pequipeg2 = 0;
}
void bipartido::setjogo(int rodada, int equipe1, int equipe2) {
	tabela[equipe1][rodada] = equipe2;
	tabela[equipe2][rodada] = equipe1;
}

int bipartido::jogo(int equipe, int rodada) {
	return tabela[equipe][rodada];
}
int bipartido::jogobip(int equipe, int rodada) {
	int t = tabela[equipe][rodada];

	if (t >= pequipeg2) {
		return t - pequipeg2;
	} else {
		return t;
	}

}

void bipartido::canonica() {

	stringstream ss(stringstream::in | stringstream::out);
	ss << "b_" << nequipes;
	nome = ss.str();

	for (int r = 0; r < nrodadas; r++) {
		for (int e = 0; e < nrodadas; e++) {
			setjogo(r, e, ((r + e) % nrodadas) + pequipeg2);
		}
	}
}

bipartido bipartido::combina(bipartido p1, bipartido p2, bipartido p3,
		bipartido p4, int nequipesm1s) {
	int t1, t2, t3, t4;

	if (p1.nequipes == p2.nequipes && p1.nequipes == p3.nequipes
			&& p1.nequipes == p4.nequipes && p1.nequipes % 2 == 0) {

		bipartido b(2 * p1.nequipes);

		for (int r = 0; r < p1.nrodadas; r++) {
			//cout<<r<<": ";
			for (int e = 0; e < p1.nrodadas; e++) {
				t1 = e;
				t2 = p1.jogobip(e, r) + b.pequipeg2;
				//cout<<t1<<" - "<< t2<<" ";
				b.setjogo(r, t1, t2);
				t1 = e + b.pequipeg2 / 2;
				t2 = p2.jogobip(e, r) + b.pequipeg2 + b.pequipeg2 / 2;
				//cout<<t1<<" - "<< t2<<" ";
				b.setjogo(r, t1, t2);
			}
			//cout<<"\n";
		}

		for (int r = 0; r < p1.nrodadas; r++) {
			for (int e = 0; e < p1.nrodadas; e++) {
				t1 = e;
				t2 = p3.jogobip(e, r) + b.pequipeg2 + b.pequipeg2 / 2;
				//cout<<t1<<" - "<< t2<<" ";
				b.setjogo(r + b.nrodadas / 2, t1, t2);
				t1 = e + b.pequipeg2 / 2;
				t2 = p4.jogobip(e, r) + b.pequipeg2;
				//	cout<<t1<<" - "<< t2<<" ";
				b.setjogo(r + b.nrodadas / 2, t1, t2);

			}
			//cout<<"\n";
		}

		stringstream ss(stringstream::out);
		ss << "(" << p1.nome << " " << p2.nome << " " << p3.nome << " "
				<< p4.nome << ")_" << b.nequipes;
		b.nome = ss.str();
		return b;
	} else if (p1.nequipes == p2.nequipes && p1.nequipes == p3.nequipes
			&& p1.nequipes == p4.nequipes + 2 && p1.nequipes % 2 == 0) {

		bipartido b(2 * (p1.nequipes - 1));

		//cout<<p1<<"\n"<<
		//      p2<<"\n"<<
		//      p3<<"\n"<<
		//      p4<<"\n";

		int i1, i2;

		i1 = b.nequipes / 2 - b.nequipes / 4;
		i2 = b.pequipeg2 + b.nequipes / 4;
		//cout<<"i1: "<<i1<<"\n";
		//cout<<"nequipeg: "<<b.pequipeg2<<"\n";
		//cout<<"i2: "<<i2<<"\n";

		for (int r = 0; r < p1.nrodadas; r++) {
			//  cout<<r<<": ";
			for (int e = 0; e < p1.nrodadas; e++) {
				t1 = e;
				t2 = p1.jogobip(e, r) + b.pequipeg2;
				if (!(p1.jogo(e, r) == p1.nequipes - 1 || e == p1.nequipes - 1)) {
					//              cout<<t1<<" - "<< t2<<" ";
					b.setjogo(r, t1, t2);
				}

				t1 = e + i2;
				t2 = p2.jogobip(e, r) + i1;
				if (!(p2.jogo(e, r) == p2.nequipes - 1 || e == p2.nequipes - 1)) {
					//          cout<<t1<<" - "<< t2<<" ";
					b.setjogo(r, t1, t2);
				}

				//b.setjogo(r,t1,t2);
			}
			t1 = p1.jogo(p1.nequipes - 1, r);
			t2 = p2.jogo(p2.nequipes - 1, r) + i2;
			// cout<<t1<<" - "<< t2<<" ";
			b.setjogo(r, t1, t2);
			//cout<<"\n";
		}
		//cout<<"\n\n";

		int eq[p3.nequipes];

		for (int r = 0; r < p3.nrodadas; r++) {
			t1 = r;
			t2 = p3.jogobip(t1, 0) + p3.pequipeg2;

			t3 = p1.jogo(p1.nequipes - 1, r);
			t4 = p2.jogo(p2.nequipes - 1, r) + p3.pequipeg2;
			//cout<<t1<<" - "<<t2<<" ";
			//cout<<t3<<" - "<<t4<<"\n";
			eq[t1] = t3;
			eq[t2] = t4;

			//eq[e]= p1.jogo(p1.nequipes,0);
			//eq[p3.jogobip(e,0)+p3.pequipeg2]= p2.jogo(p2.nequipes,0);
		}
		//cout<<"aqui\n";

		for (int i = 0; i < p3.nequipes; i++) {
			//cout<<i<<":"<<eq[i]<<"\n";
		}
		for (int r = 1; r < p3.nrodadas; r++) {
			//cout<<r-1+p3.nrodadas<<": ";
			for (int e = 0; e < p3.nrodadas; e++) {
				t1 = eq[e];
				t2 = eq[p3.jogo(e, r)] - p3.pequipeg2 + i2;
				//cout<<t1<<" - "<<t2<<" ";
				b.setjogo(r - 1 + p3.nrodadas, t1, t2);
			}
			//cout<<"\n";
		}
		for (int r = 0; r < p4.nrodadas; r++) {
			//cout<<r+p3.nrodadas<<": ";
			for (int e = 0; e < p4.nrodadas; e++) {
				t1 = e + i1;
				t2 = p4.jogobip(e, r) + b.pequipeg2;
				// cout<<t1<<" - "<<t2<<" ";
				b.setjogo(r + p3.nrodadas, t1, t2);
			}
			//cout<<"\n";
		}

		stringstream ss(stringstream::out);
		ss << "(" << p1.nome << " " << p2.nome << " " << p3.nome << " "
				<< p4.nome << ")_" << b.nequipes;
		b.nome = ss.str();

		return b;
	} else {
		cout << "Erro: Combina bipartido\n";
		bipartido p(4);
		return p;
	}
}

void bipartido::geraFatoracoes(int n) {

	bipartidos.resize(n + 1);

	for (int i = 2; i <= n; i += 2) {
		bipartido f(i);
		f.canonica();
		bipartidos[i].push_back(f);
	}

	for (int i = 8; i <= n; i += 2) {
		if (i % 4 == 0) {
			for (unsigned j1 = 0; j1 < bipartidos[i / 2].size(); j1++) {
				for (unsigned j2 = j1; j2 < bipartidos[i / 2].size(); j2++) {
					for (unsigned j3 = 0; j3 < bipartidos[i / 2].size(); j3++) {
						for (unsigned j4 = j3; j4 < bipartidos[i / 2].size(); j4++) {
							if (j3 > j1 || (j3 == j1 && j4 >= j2)) {
								bipartidos[i].push_back(
										combina(bipartidos[i / 2][j1],
												bipartidos[i / 2][j2],
												bipartidos[i / 2][j3],
												bipartidos[i / 2][j4]));
							}
						}
					}
				}
			}

		} else {
			for (unsigned j1 = 0; j1 < bipartidos[i / 2 + 1].size(); j1++) {
				for (unsigned j2 = j1; j2 < bipartidos[i / 2 + 1].size(); j2++) {
					for (unsigned j3 = 0; j3 < bipartidos[i / 2 + 1].size(); j3++) {
						for (unsigned j4 = 0; j4 < bipartidos[i / 2 - 1].size();
								j4++) {

							bipartidos[i].push_back(
									combina(bipartidos[i / 2 + 1][j1],
											bipartidos[i / 2 + 1][j2],
											bipartidos[i / 2 + 1][j3],
											bipartidos[i / 2 - 1][j4]));
						}
					}
				}
			}

		}

	}

	//for(int i=2;i<=n;i+=2){
	//cout<<i<<": "<<bipartidos[i].size()<<"\n";
	//for(int j=0;j<bipartidos[i].size();j++){
	//cout<<"\t"<<bipartidos[i][j].nome<<"\n";

	// }
	// }

}

bipartido operator-(bipartido &a, bipartido &b) {
	bipartido d(a.nequipes);

	for (int e = 0; e < a.nequipest; e++) {
		for (int r = 0; r < a.nrodadas; r++) {
			d.tabela[e][r] = abs(a.tabela[e][r] - b.tabela[e][r]);

		}
	}
	return d;

}

int bipartido::valida_solucao() {

	int vezes_por_rodada[nequipest];

	for (int e = 0; e < nequipest; e++) {
		for (int i = 0; i < nequipest; i++) {
			vezes_por_rodada[i] = 0;
		}

		for (int r = 0; r < nrodadas; r++) {
			vezes_por_rodada[jogo(e, r)]++;
		}

		for (int i = 0; i < nequipest / 2; i++) {
			if ((e < nequipest / 2 && vezes_por_rodada[i] != 0)
					|| (e >= nequipest / 2 && vezes_por_rodada[i] != 1)) {
				cout << i << " " << nequipest / 2 << " " << vezes_por_rodada[i]
						<< "\n";
				return 0;
			}
		}
		for (int i = nequipest / 2; i < nequipest; i++) {

			if ((e < nequipest / 2 && vezes_por_rodada[i] != 1)
					|| (e >= nequipest / 2 && vezes_por_rodada[i] != 0)) {
				cout << i << " " << nequipest / 2 << " " << vezes_por_rodada[i]
						<< "\n";
				return 0;
			}

		}

	}

	return 1;

}

