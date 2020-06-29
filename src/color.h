//
//  cor.h
//  PATAT
//
//  Created by Tiago Januario on 12/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef COLOR_H
#define COLOR_H

//Classe que representa uma cor em um vertice
class Cor{
public:
	int status; /* true, if the color is associated to an edge, false otherwise */

	int adj; /* Adjacent vertex */

	int disponivel_index; /* Index of this color in */

	int e; /* Edge which is colored with this color */
};

#endif
