//
// Created by markus on 20.09.19.
//

#include <stack>
#include <iostream>
#include <assert.h>
#include "ir_tools.h"
#include "refinement.h"
#include "selector.h"
#include "invariant.h"
#include "group.h"

// compute a canonical labelling of the sgraph using individualization-refinement
void ir_tools::label_graph(sgraph *g, bijection *canon_p) {
    coloring c;
    g->initialize_coloring(&c);

    group G(g->v.size()); // automorphism group
    refinement R;
    selector S;
    trail T(g->v.size()); // a trail for backtracking and undoing refinements
    // ToDo: also keep array of individualized vertices for base fixing and backjumping -- so do this for current path and best path
    invariant I; // invariant, hopefully becomes complete in leafs such that automorphisms can be found
    invariant best_I; // so far best explored invariant
    bijection best_leaf;
    bool other_best_leaf = false;

    I.push_level();
    std::set<std::pair<int, int>> changes;
    R.refine_coloring(g, &c, &changes, &I);
    T.push_op_r(&changes);
    bool backtrack = false;

    while(T.last_op() != OP_END) {
        int s;
        if(!backtrack) {
            s = S.select_color(g, &c);
            if (s == -1) {
                std::cout << "Discrete coloring found." << std::endl;
                I.print();
                backtrack = true;
                // either this is the new best leaf or we can derive an automorphism
                if(other_best_leaf) {
                    std::cout << "Certifying automorphism" << std::endl;
                    bijection leaf;
                    leaf.read_from_coloring(&c);
                    bijection automorphism = leaf;
                    automorphism.inverse();
                    automorphism.compose(best_leaf);
                    assert(g->certify_automorphism(automorphism));
                    bool added = G.add_permutation(&automorphism);
                    std::cout << "Automorphism added: " << added << std::endl;
                } else {
                    other_best_leaf = true;
                    best_leaf.read_from_coloring(&c);
                    best_I = I;
                }
            }
        }

        if(T.last_op() == OP_I && !backtrack) { // add new operations to trail...
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //                                                REFINEMENT
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            changes.clear();
            I.push_level();
            R.refine_coloring(g, &c, &changes, &I);
            T.push_op_r(&changes);

            if(other_best_leaf) {
                // compare invariant
                switch (I.top_is_geq(best_I.get_level(I.current_level()))) {
                    case -1:
                        std::cout << "Better branch" << std::endl;
                        // this branch is better, let's not continue comparing
                        other_best_leaf = false;
                        break;
                    case 0:
                        std::cout << "Equal" << std::endl;
                        // equally good
                        break;
                    case 1:
                        std::cout << "Pruned through invariant" << std::endl;
                        // other branch was better, go back
                        backtrack = true;
                        continue;
                        break;
                    default:
                        assert(false);
                }
            }

        } else if(T.last_op() == OP_R  && !backtrack) {
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //                                             INDIVIDUALIZATION
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // collect all elements of color s
            std::stack<int> color_s;
            int i = s;
            while((i == s) || (i == 0) || c.ptn[i - 1] != 0) {
                color_s.push(c.lab[i]);
                i += 1;
            }
            int v = color_s.top();
            color_s.pop();
            assert(color_s.size() > 0);
            // individualize first vertex of class, save the rest of the class in trail
            R.individualize_vertex(g, &c, v);
            T.push_op_i(&color_s, v);
        } else if(T.last_op() == OP_I && backtrack) { // backtrack trail, undo operations...
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //                                       BACKTRACK INDIVIDUALIZATION
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // backtrack until we can do a new operation
            int v = T.top_op_i_v();
            R.undo_individualize_vertex(g, &c, v);
            T.pop_op_i_v();

            // undo individualization
            if(T.top_op_i_class().empty()) {
                // we tested the entire color class, need to backtrack further
                T.pop_op_i_class();
                continue;
            } else {
                // there is another vertex we have to try, so we are done backtracking
                int v = T.top_op_i_class().top();
                T.top_op_i_class().pop();
                T.push_op_i_v(v);
                R.individualize_vertex(g, &c, v);
                backtrack = false;
            }
        }  else if(T.last_op() == OP_R && backtrack) {
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //                                             UNDO REFINEMENT
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // we are backtracking, so we have to undo refinements
            I.pop_level();
            R.undo_refine_color_class(g, &c, &T.top_op_r());
            T.pop_op_r();
        }
    }
    std::cout << "Group size: ";
    G.print_group_size();
    T.free_path();
}

void trail::push_op_r(std::set<std::pair<int, int>>* color_class_changes) {
    trail_operation.push(OP_R);
    trail_color_class_changes.push(std::set<std::pair<int, int>>());
    trail_color_class_changes.top().swap(*color_class_changes);
}

void trail::push_op_i(std::stack<int>* individualizaiton_todo, int v) {
    trail_operation.push(OP_I);
    trail_op_i_class.push(std::stack<int>());
    trail_op_i_class.top().swap(*individualizaiton_todo);
    push_op_i_v(v);
}

trail::trail(int domain_size) {
    trail_operation.push(OP_END);
    this->domain_size = domain_size;
    ipath = new int[domain_size];
    ipos = 0;
}

ir_operation trail::last_op() {
    return trail_operation.top();
}

std::set<std::pair<int, int>>& trail::top_op_r() {
    return trail_color_class_changes.top();
}

void trail::pop_op_r() {
    trail_operation.pop();
    trail_color_class_changes.pop();
}

std::stack<int>& trail::top_op_i_class() {
    return trail_op_i_class.top();
}

void trail::pop_op_i_class() {
    trail_operation.pop();
    trail_op_i_class.pop();
}

int trail::top_op_i_v() {
    return trail_op_i_v.top();
}

void trail::pop_op_i_v() {
    ipos -= 1;
    trail_op_i_v.pop();
}

void trail::push_op_i_v(int v) {
    assert(ipos >= 0 && ipos < domain_size);
    ipath[ipos] = v;
    ipos += 1;
    trail_op_i_v.push(v);
}

void trail::free_path() {
    delete[] ipath;
}
