//
//  edge.h
//  PATAT
//
//  Created by Tiago Januario on 12/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef PATAT_edge_h
#define PATAT_edge_h

//Classe que representa uma aresta no grafo
class Edge{
public:

    //! The beggining of edge.
    int start;
    
    //! The end of edge.
    int end;
    
    //! The color of edge.
    int color;
    
    //!If a color is taboo, it is not allowed to color the edge with this color.
    int taboo;
};

#endif
