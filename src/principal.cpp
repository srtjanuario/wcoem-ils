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
#include <ctime>
#include <getopt.h>
using namespace std;

int main(int argc, char *argv[]) {

	char optstring[] = "i:a:t:vr:dcq";
	int option_index = 0;
	int opcao = 0;

	// Input instance
	string instancia;

	// Algorithm. Only ILS is implemented
	string algoritmo = "ils";

	// Running time
	int tempo = 5;

	// random seed
	unsigned seed = (unsigned) time(0) % 10000;

	// maximum number of iddle iterations
	int iddleiterations = 100;

	// maximum number of maxrandommoves
	int maxrandommoves = 0;

	int maxbadmoves = 1000;

	double beta = 0.01;

	ptvef p;

	struct option long_options[] = {
	//begin options
	// "instance": an even number bigger than or equals to 4
			{ "instancia", required_argument, 0, 'n' },
			// "algorithm": only ils
			{ "algoritmo", required_argument, 0, 'a' },
			// "time": integer in seconds
			{ "tempo", required_argument, 0, 't' },
			// "kls" (kind of local search): n3, n4, n5, n3Un4, n4Un5
			{ "tbl", required_argument, 0, '5' },
			// "kis": kind of initial solution canonical, vizing
			{ "tsi", required_argument, 0, '4' },
			// "seed": an integer number
			{ "seed", required_argument, 0, 's' },
			// Print some information
			{ "beta", required_argument, 0, 'b' },
			// Print some information
			{ "maxbadmoves", required_argument, 0, 'd' },
			// Print some information
			{ "version", no_argument, 0, 'v' },
			// Maximum number of random moves
			{ "maxrandommoves", required_argument, 0, 'r' },
			// Maximum number of iddle iterations
			{ "iddleiterations", required_argument, 0, 'i' },
			// Leave it as it is
			{ 0, 0, 0, 0 }
	//end options
			};

	opcao = 0;
	while (opcao != -1) {
		opcao = getopt_long(argc, argv, optstring, long_options, &option_index);
		switch (opcao) {
		case 't':
			tempo = atoi(optarg);
			break;
		case '5':
			p.tbl = string(optarg);
			break;
		case '4':
			p.tsi = atoi(optarg);
			break;
		case 'n':
			instancia = string(optarg);
			break;
		case 's':
			seed = atoi(optarg);
			break;
		case 'a':
			algoritmo = string(optarg);
			break;
		case 'i':
			maxrandommoves = atoi(optarg);
			break;
		case 'r':
			iddleiterations = atoi(optarg);
			break;
		case 'v':
			cout << "Carry-over effect calculator 2014" << endl;
			exit(0);
		}
	}

	srand(seed);
	p.parser(instancia);
	p.iddleiterations = iddleiterations;
	p.maxrandommoves = maxrandommoves;
	p.b = beta;
	p.maxBadMoves = maxbadmoves;

	if (algoritmo != "") {
		tabela media;
		if (algoritmo == "ils") {
			media = p.ils(tempo);
		} else if (algoritmo == "guedes") {
			media = p.ilsGuedes();
		} else if (algoritmo == "state") {
			media = p.state();
		} else {
			cout << "can't recognize this algorithm" << endl;
		}
		media.insere("seed", seed);
		media.print();
	} else {
		cout << "algorithm is undefined" << endl;
	}

	return 0;
}

