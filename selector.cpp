//
// Created by markus on 19.09.19.
//

#include "selector.h"

// "first largest", -1 if coloring is discrete
int selector::select_color(sgraph *g, coloring *c) {
    int largest_cell  = -1;
    int largest_cell_sz = -1;
    for(int i = 0; i < c->ptn.size();){
        if(c->ptn[i] > largest_cell_sz && c->ptn[i] > 0) {
            largest_cell = i;
            largest_cell_sz = c->ptn[i];
        }
        i += c->ptn[i] + 1;
    }
    return largest_cell;
}
