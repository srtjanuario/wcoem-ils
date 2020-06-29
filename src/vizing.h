//
//  vizing.h
//
//  Created by Tiago Januario on: Jan 22, 2013
//  Copyright 2013 Universita degli Studi di Udine. All rights reserved.
//
//  From Google C++ Style Code
//  1 - Functions should start with a capital letter and have a
//      capital letter for each new word. No underscores.
//  2 - If your function crashes upon an error, you should
//      append OrDie to the function name.
//  3 - Accessors and mutators (get and set functions) should match
//      the name of the variable they are getting and setting.
//  4 - Data members are lowercase and always end with a trailing underscore.

#ifndef _EXPORT_HOME_JANUARIO_DROPBOX_ECLIPSE_WORKSPACEC47_PATTERN_SRC_VIZING_H_
#define _EXPORT_HOME_JANUARIO_DROPBOX_ECLIPSE_WORKSPACEC47_PATTERN_SRC_VIZING_H_

#include <vector>
#include <utility>

#include "graph.h"
#include "step.h"
#include "polygon.h"
class Vizing {
 public:

  // Just a simple constructor, nothing really interesting...
  Vizing();

  // This a different constructor, (N must be even).
  Vizing(unsigned N);

  // Destructor
  ~Vizing();

  void Print();

  // Create a new pattern using the current number of teams in the tournament
  vector<vector<pair<unsigned, unsigned> > > BuildNaivePattern();

  // Create a new pattern using
  vector<vector<pair<unsigned, unsigned> > > BuildDoubleVizingPattern();

  // Print the patter in Di Gaspero/Schaerf format
  friend ostream& operator<<(ostream& os, const Vizing& vizing);

  // If you just need to change the number of teams in the tournament
  // use this function.
  // This funcition is used by:
  // - Vizing(unsigned N);
  void Initialize(unsigned N);

  void CriaColoracaoVizing(Polygon& p);

 private:

  vector< vector<int> > tournament_;

  // Number of teams in the tournament
  unsigned number_of_teams_;

  // Number of matches in the tournament
  unsigned number_of_matches_;

  // Number of rounds in the tournament
  unsigned number_of_rounds_;

  // Output of the function printPattern
  vector<vector<pair<unsigned, unsigned> > > pattern_output_;

  // This matrix is used to help setting the home-away assignment
  // in the second stage of the tournament when the Vizing's algorithm
  // is used
  int ** opponents_in_firts_stage_;

  void BuildRandomHomeAwayAssignmentOnFirstStage();

  void BuildHomeAwayAssignmentUsingOpponentsMatrix();

  void MirrorTheFirstStageToSecondStage();

  void InvertedAssignmentBasedOnOpponentMatrix(unsigned round_index,
                                               unsigned match_index,
                                               unsigned team,
                                               unsigned opponent);

  // Local graph, to be colored with Vizing's Algorithm
  Graph * local_graph_;

  // Alternating path inside the graph
  Passo *local_path_;

  // Find an a/b1-alternating-path inside the Graph
  int FindAlternatingPath(Graph *g, int v, int w1, int a, int b1, int e);

  // Swap colors inside de graph
  void SwapColors(Graph *g, int i);

  // A function that colors a graph with delta+1 colors
  void BuildVizingPattern(Graph *g);

  // Some SwissTools, better put in a different file

  unsigned GetRandomRound();

  void LoadRandomIndexes();

  unsigned list_of_rounds_size_;

  vector<unsigned> round_random_index_;

};

#endif  // _EXPORT_HOME_JANUARIO_DROPBOX_ECLIPSE_WORKSPACEC47_PATTERN_SRC_VIZING_H_
