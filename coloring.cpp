//
// Created by markus on 19.09.19.
//

#include "coloring.h"
#include <algorithm>
#include <iterator>
#include <cstring>

void coloring::rewrite_ptn(coloring *c) {
    for(int i = 0; i < c->ptn_sz; i += 1) {
        ptn[i] = c->ptn[i];
    }
}

coloring::~coloring() {
    if(init) {
        delete[] ptn;
        delete[] lab;
    }
}

void coloring::copy(coloring *c) {
    if(init) {
        if(lab_sz != c->lab_sz || ptn_sz != c->ptn_sz) {
            delete[] lab;
            delete[] ptn;
            init = false;
        } else {
             memcpy(ptn, c->ptn, c->ptn_sz*sizeof(int));
             vertex_to_col = c->vertex_to_col;
             return;
        }
    }

    if(!init) {
        lab = new int[c->lab_sz];
        ptn = new int[c->ptn_sz];
    }

    memcpy(lab, c->lab, c->lab_sz*sizeof(int));
    memcpy(ptn, c->ptn, c->ptn_sz*sizeof(int));

    lab_sz = c->lab_sz;
    ptn_sz = c->ptn_sz;

    vertex_to_col = c->vertex_to_col;
    vertex_to_lab = c->vertex_to_lab;

    init = true;
}

void coloring::copy_force(coloring *c) {
    if(init) {
        if(lab_sz != c->lab_sz || ptn_sz != c->ptn_sz) {
            delete[] lab;
            delete[] ptn;
            init = false;
        }
    }

    if(!init) {
        lab = new int[c->lab_sz];
        ptn = new int[c->ptn_sz];
    }

    memcpy(lab, c->lab, c->lab_sz*sizeof(int));
    memcpy(ptn, c->ptn, c->ptn_sz*sizeof(int));

    lab_sz = c->lab_sz;
    ptn_sz = c->ptn_sz;

    vertex_to_col = c->vertex_to_col;
    vertex_to_lab = c->vertex_to_lab;

    init = true;
}

void coloring::initialize(int domain_size) {
    lab = new int[domain_size];
    ptn = new int[domain_size];

    vertex_to_col.resize(domain_size);
    vertex_to_lab.resize(domain_size);

    init = true;

    lab_sz = domain_size;
    ptn_sz = domain_size;
}

bool coloring::check() {
    bool comp = true;

    for(int i = 0; i < lab_sz;++i) {
        comp = comp && (lab[i] >= 0 && lab[i] < lab_sz);
        comp = comp && (lab[vertex_to_lab[i]] == i);
    }

    // assert proper ptn
    for(int i = 0; i < lab_sz;) {
        if(i != 0)
            comp = comp && (ptn[i - 1] == 0);
        i += ptn[i] + 1;
    }

    return comp;
}