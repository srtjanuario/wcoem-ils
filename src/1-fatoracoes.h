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
#ifndef _FATORACOES_H_
#define _FATORACOES_H_

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdlib.h>
using namespace std;

class bipartido {
public:

    int nequipes;
    int nequipest;
    int nrodadas;
    vector< vector<int> > tabela;
    int pequipeg2;
    string nome;
    vector< vector< bipartido > > bipartidos;

    void geraFatoracoes(int n);

    bipartido(int n);
    bipartido();
    void setjogo(int rodada, int equipe1, int equipe2);
    int jogo(int equipe, int rodada);
    int jogobip(int equipe, int rodada);
    void canonica();
    bipartido combina(bipartido p1,bipartido p2,bipartido p3,bipartido p4,int nequipesm1s=0);


    int valida_solucao();

    friend bipartido operator+ (bipartido &a, bipartido &b);
    friend bipartido operator- (bipartido &a, bipartido &b);

    friend ostream & operator << (ostream &co, bipartido & f);

};

class fatoracao {
public:
    int nequipes;
    int nequipest;
    int nrodadas;
    vector< vector <int> > tabela;
    string nome;

    vector< vector< fatoracao > > fatoracoes;

    void geraFatoracoes(int n);


    fatoracao();
    fatoracao(int _n);
    fatoracao(fatoracao &a, fatoracao &b, bipartido &bip);


    int valida_solucao();
    void setjogo(int rodada, int equipe1, int equipe2);
    int jogo(int equipe, int rodada);
    friend ostream & operator << (ostream &co, fatoracao & f);
    void canonica();
    friend fatoracao operator+ (fatoracao &a, fatoracao &b);


    fatoracao combina(fatoracao &a, fatoracao &b, bipartido &bip,int decequipes=0);

    void binaria();
    void recursiva();
};



class fCanonica: public fatoracao {
public:
    fCanonica(int n);
};

class fBinaria: public fatoracao {
public:
    fBinaria(int n);
};

#endif /*1FATORACOES_H_*/

