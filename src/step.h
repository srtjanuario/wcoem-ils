/*
 * step.h
 *
 *  Created on: Feb 8, 2013
 *      Author: januario
 */

#ifndef STEP_H_
#define STEP_H_

class Passo {
public:
   // Current edge
   int ei;

   // New color
   int cnew;

   // Old color
   int cold;

   // Current head vertex - v_i
   int vi;

   // Current tail vertex - w_i
   int wi;
};


#endif /* STEP_H_ */
