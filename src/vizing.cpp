//
//  vizing.cpp
//
//  Created by Tiago Januario on: Jan 22, 2013
//  Copyright 2013 Universita degli Studi di Udine. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <utility>
#include <algorithm>
using namespace std;

#include "vizing.h"
#include "graph.h"

Vizing::Vizing() {
  number_of_teams_ = 0;
  Initialize(0);
}

Vizing::Vizing(unsigned N) {
  Initialize(N);
}

void Vizing::CriaColoracaoVizing(Polygon& p) {

  // Color the local_graph_ using the Vizing's Algorithm
  BuildVizingPattern(local_graph_);

  BuildRandomHomeAwayAssignmentOnFirstStage();

  for (unsigned i = 0; i < pattern_output_.size() / 2; i++) {
    for (unsigned j = 0; j < pattern_output_[i].size(); j++) {
      p.table[pattern_output_[i][j].first][pattern_output_[i][j].second] = i
          + 1;
      p.table[pattern_output_[i][j].second][pattern_output_[i][j].first] = i
          + 1;
    }
  }

  p.BuildOpponentMatrix();
  //cout<<p<<endl;

}

void Vizing::Initialize(unsigned N) {

  // You can initializa with ZERO teams, but I think it is really a good idea
  if (N <= 0) {
    number_of_teams_ = 0;
    number_of_matches_ = 0;
    number_of_rounds_ = 0;
    list_of_rounds_size_ = 0;
  } else {  // Let us assume that the number of teams is different from zero...
    if (number_of_teams_ == N) {  // If the number of teams is equal to the previous value...
      local_graph_->clean();  // ... just clean the graph
    } else {
      // If it is not the first time you initialize...
      if (number_of_teams_ != 0) {
        delete local_graph_;
        delete[] local_path_;
        for (unsigned i = 0; i < number_of_teams_; ++i) {
          delete[] opponents_in_firts_stage_[i];
        }
        delete[] opponents_in_firts_stage_;

        // Delete Anagastopoulos matrix
        tournament_.clear();
      }

      number_of_teams_ = N;
      number_of_matches_ = N / 2;
      number_of_rounds_ = 2 * (N - 1);

      // Graph that will be colored with Vizing's Algorithm
      local_graph_ = new Graph(N);

      // Matrix of opponents
      opponents_in_firts_stage_ = new int*[number_of_teams_];
      for (unsigned i = 0; i < number_of_teams_; ++i) {
        opponents_in_firts_stage_[i] = new int[number_of_teams_];
      }

      // Anagnastopoulos matrix

      tournament_.resize(number_of_teams_,vector<int>(number_of_rounds_));

//      tournament_ = new int*[number_of_teams_];
//      for (unsigned i = 0; i < number_of_teams_; ++i) {
//        tournament_[i] = new int[number_of_rounds_];
//      }

      pattern_output_.clear();

      // Initialize the number of lines (rounds)
      pattern_output_.resize(number_of_rounds_);

      // Initialize the number of rows (matches)
      for (unsigned i = 0; i < number_of_rounds_; i++)
        pattern_output_[i].resize(number_of_matches_);

      list_of_rounds_size_ = number_of_rounds_ / 2;

      for (unsigned var = 0; var < list_of_rounds_size_; ++var) {
        round_random_index_.push_back(var);
      }

    }
  }
}

Vizing::~Vizing() {
  if (number_of_teams_ > 0) {
    delete local_graph_;
    delete[] local_path_;

    for (unsigned i = 0; i < number_of_teams_; ++i) {
      delete[] opponents_in_firts_stage_[i];
    }
    delete[] opponents_in_firts_stage_;
  }
}

vector<vector<pair<unsigned, unsigned> > > Vizing::BuildNaivePattern() {

  // Color the local_graph_ using the Vizing's Algorithm
  BuildVizingPattern(local_graph_);

  BuildRandomHomeAwayAssignmentOnFirstStage();

  MirrorTheFirstStageToSecondStage();

  return pattern_output_;

}

vector<vector<pair<unsigned, unsigned> > > Vizing::BuildDoubleVizingPattern() {
  /// FIRST STAGE ///
  // Color the local_graph_ using the Vizing's Algorithm
  BuildVizingPattern(local_graph_);
  // Build a random home away assignment for the first stage
  BuildRandomHomeAwayAssignmentOnFirstStage();

  /// SECOND STAGE ///
  local_graph_->clean();  // ... just clean the graph
  // Again, color the local_graph_ using the Vizing's Algorithm
  BuildVizingPattern(local_graph_);
  // Build the home away assignment using the information on opponents_in_firts_stage_
  BuildHomeAwayAssignmentUsingOpponentsMatrix();

  return pattern_output_;
}

void Vizing::InvertedAssignmentBasedOnOpponentMatrix(unsigned round,
                                                     unsigned match,
                                                     unsigned team,
                                                     unsigned opponent) {
  // Use the opponente matrix to reverse the match
  if (opponents_in_firts_stage_[team][opponent] == 1) {
    pattern_output_[round][match].first = opponent;
    pattern_output_[round][match].second = team;
  } else {
    pattern_output_[round][match].first = team;
    pattern_output_[round][match].second = opponent;
  }
}

void Vizing::BuildHomeAwayAssignmentUsingOpponentsMatrix() {

  unsigned round_index = 0;
  unsigned match_index = 0;

  // This is the first round on second stage
  round_index = number_of_rounds_ / 2;
  for (int round = 0; round < local_graph_->getVertices(); round++) {
    match_index = 0;
    for (int team = 0; team < local_graph_->getVertices(); team++) {
      if (local_graph_->color[team][round].status == true) {
        if (team < local_graph_->color[team][round].adj) {
          InvertedAssignmentBasedOnOpponentMatrix(
              round_index, match_index, team,
              local_graph_->color[team][round].adj);
          match_index++;
          //cout << *this << endl;
        }
      } else {
        InvertedAssignmentBasedOnOpponentMatrix(round_index, match_index, team,
                                                local_graph_->getVertices());
        match_index++;
        //cout << *this << endl;
      }
    }
    round_index++;
  }
}

void Vizing::MirrorTheFirstStageToSecondStage() {

  unsigned round_from_first_stage_;

  LoadRandomIndexes();

  for (unsigned r = number_of_rounds_ / 2; r < number_of_rounds_; r++) {
    round_from_first_stage_ = GetRandomRound();
    pattern_output_[r] = pattern_output_[round_from_first_stage_];
    for (unsigned m = 0; m < number_of_matches_; m++) {
      swap(pattern_output_[r][m].first, pattern_output_[r][m].second);
    }
  }
}

void Vizing::Print() {
  // Print the output using Anagnostopoulos's format
  for (unsigned r = 0; r < number_of_rounds_; ++r) {
    for (unsigned m = 0; m < number_of_matches_; ++m) {
      tournament_[pattern_output_[r][m].first][r] = pattern_output_[r][m].second
          + 1;
      tournament_[pattern_output_[r][m].second][r] = -(pattern_output_[r][m]
          .first + 1);
    }
  }

  for (unsigned t = 0; t < number_of_teams_; ++t) {
    for (unsigned r = 0; r < number_of_rounds_; ++r) {
      cout << setw(4) << tournament_[t][r];
    }
    cout << endl;
  }

}

ostream& operator<<(ostream& os, const Vizing& vizing) {
  for (unsigned r = 0; r < vizing.number_of_rounds_; ++r) {
    for (unsigned m = 0; m < vizing.number_of_matches_; ++m) {
      os << setw(5) << vizing.pattern_output_[r][m].first << '-'
         << vizing.pattern_output_[r][m].second;
    }
    os << endl;
  }

  return os;
}

void Vizing::BuildRandomHomeAwayAssignmentOnFirstStage() {

  unsigned round_index = 0;
  unsigned match_index = 0;

  round_index = 0;
  for (int round = 0; round < local_graph_->getVertices(); round++) {
    match_index = 0;
    for (int team = 0; team < local_graph_->getVertices(); team++) {
      if (local_graph_->color[team][round].status == true) {
        if (team < local_graph_->color[team][round].adj) {
          // Random home-away assignment
          if (rand() % 2) {
            pattern_output_[round_index][match_index].first = team;
            pattern_output_[round_index][match_index].second = local_graph_
                ->color[team][round].adj;
          } else {
            pattern_output_[round_index][match_index].first = local_graph_
                ->color[team][round].adj;
            pattern_output_[round_index][match_index].second = team;
          }
          opponents_in_firts_stage_[pattern_output_[round_index][match_index]
              .first][pattern_output_[round_index][match_index].second] = 1;
          opponents_in_firts_stage_[pattern_output_[round_index][match_index]
              .second][pattern_output_[round_index][match_index].first] = -1;
          match_index++;
        }
      } else {
        // Random home-away assignment
        if (rand() % 2) {
          pattern_output_[round_index][match_index].first = team;
          pattern_output_[round_index][match_index].second = local_graph_
              ->getVertices();
        } else {
          pattern_output_[round_index][match_index].first = local_graph_
              ->getVertices();
          pattern_output_[round_index][match_index].second = team;
        }
        opponents_in_firts_stage_[pattern_output_[round_index][match_index]
            .first][pattern_output_[round_index][match_index].second] = 1;
        opponents_in_firts_stage_[pattern_output_[round_index][match_index]
            .second][pattern_output_[round_index][match_index].first] = -1;
        match_index++;
      }

    }
    round_index++;
  }
}

// Mirror the tournament
// N is the only paremeter of the function
// Color a given graph using Vizings algorithm
void Vizing::BuildVizingPattern(Graph *grafo) {

  int empty = -1;

  local_path_ = new Passo[grafo->getVertices()];

// v e w1 sao os extremos da arestas e1 que ainda no foi colorida
// Vertice v
  int v = 0;

// Vertice w1
  int w1 = 0;

// 'a'  uma das cores no representada em v
  int a = 0;

// 'B1'!='a'  uma das cores no representadas em w1
  int B1 = 0;

// B2 e a segunda banana
  int B2 = 0;

// 'e' = aresta que liga os vrtices v e w1
  int e1 = 0;

// 'e2' = aresta que liga os vrtices v e w2
  int e2 = 0;

// Executa o mtodo de Vizing para todas as arestas
  bool caso2 = false;

  int terminou = false;

// Inicia Vizing pela primeira aresta do grafo
  e1 = grafo->ProcuraProximaArestaSemCor();

  while (!terminou) {
    if (caso2) {
      if (w1 == grafo->edgeVector[e1].start)
        v = grafo->edgeVector[e1].end;
      else
        v = grafo->edgeVector[e1].start;
    } else {
      v = grafo->edgeVector[e1].start;
      w1 = grafo->edgeVector[e1].end;
    }

    // Se alguma cor 'B1' nao esta representada nem no vertice v, nem no vrtice w1,
    // ento esta cor pode ser utilizada para colorir a aresta e1

    B2 = grafo->corFaltandoEmDoisVertices(v, w1, e1);

    //! Atualiza a cor da aresta com a cor que falta em v e w1
    if (B2 != empty) {
      grafo->InsereCorNaAresta(v, w1, e1, B2);
      caso2 = false;
    }

    // Caso contrario:
    // seja a cor a uma das cores no representada em v
    // seja B1!=a uma da cores no representadas em w1

    //!Passo 1
    else {
      //! Cor a!=empty no representada em v
      a = grafo->ProcuraCorFaltandoEmVertice(v);

      //! Cor B1!=a no representada em w1
      if (!caso2)
        B1 = grafo->ProcuraCorFaltandoEmVertice(w1);
      caso2 = false;

      // Chamemos H(a,B1) ?s componentes conexas induzidas pelas arestas coloridas com a ou B1.
      // H(a,b) so sempre ciclos pares ou caminhos simples.
      // -  1: H(a,B1)   caminho simple.
      // - Caso 2: H(a,B2)  ciclo

      //!Caso 1: H(a,B1)  caminho simples
      if (!FindAlternatingPath(grafo, v, w1, B1, a, e1)) {

        //! procura a aresta e2=(w1,w2)
        e2 = grafo->color[w1][a].e;
        ;

        // Remove the color a from the edge e2=(w1,w2)
        grafo->ApagaCorAresta(e2, a);

        // Colocamos a na aresta e1=(v,w1).
        grafo->InsereCorNaAresta(v, w1, e1, a);

        // Agora o problema  colorir e2=(w1,w2) que ficou sem cor e voltamos no começo...
        caso2 = true;

        // ...no permitindo recolorir e2=(w1,w2) com a.
        grafo->edgeVector[e2].taboo = a;
      }			 // endif
    }  //end
    if (!caso2) {
      e1 = grafo->ProcuraProximaArestaSemCor();
      if (e1 == empty)
        terminou = true;
    } else
      e1 = e2;
  }

}

void Vizing::LoadRandomIndexes() {
  list_of_rounds_size_ = round_random_index_.size();
}

unsigned Vizing::GetRandomRound() {
  if (!list_of_rounds_size_) {
    cerr << "Warning" << __LINE__ << __FILE__ << endl;
    LoadRandomIndexes();
  }

// put coments here
  unsigned index;
  unsigned value;

  index = rand() % Vizing::list_of_rounds_size_;

  value = Vizing::round_random_index_[index];

  swap(round_random_index_[index],
       round_random_index_[list_of_rounds_size_ - 1]);

  list_of_rounds_size_--;

  return value;
}

// Alterna cores do caminho
void Vizing::SwapColors(Graph *grafo, int i) {
  int j;

  for (j = 1; j < i; j++)
    grafo->ApagaCorAresta(local_path_[j].ei, local_path_[j].cold);

  for (j = 0; j < i; j++)
    grafo->InsereCorNaAresta(local_path_[j].vi, local_path_[j].wi,
                             local_path_[j].ei, local_path_[j].cnew);

}

// Busca em profundidade alternando as cores 'a' e 'B1'
// - a , cor faltando em v
// - B1 != a , cor faltando em w1
int Vizing::FindAlternatingPath(Graph *grafo, int v, int w1, int B1, int a,
                                int e) {

  int temp, i, e2, v2;

// Empty represents the absence of colors
  int empty = -1;

  local_path_[0].vi = v;
  local_path_[0].wi = w1;
  local_path_[0].cnew = B1;
  local_path_[0].ei = e;
  local_path_[0].cold = empty;

  v2 = grafo->color[v][B1].adj;
  for (i = 1; i < grafo->getVertices() && v2 != empty && v2 != w1; i++) {
    e2 = grafo->color[v][B1].e;
    local_path_[i].vi = v;
    local_path_[i].wi = v2;
    local_path_[i].cnew = a;
    local_path_[i].cold = B1;
    local_path_[i].ei = e2;

    v = v2;

    temp = a;
    a = B1;
    B1 = temp;

    v2 = grafo->color[v][B1].adj;
  }
  if (v2 == empty) {
    SwapColors(grafo, i);
    return true;
  }
  return false;
}
