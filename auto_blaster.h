//
// Created by markus on 23.09.19.
//

#ifndef DEJAVU_AUTO_BLASTER_H
#define DEJAVU_AUTO_BLASTER_H


#include <random>
#include "sgraph.h"
#include "invariant.h"
#include "concurrentqueue.h"
#include "invariant_acc.h"
#include "pipeline_group.h"
#include "refinement_bucket.h"
#include "selector.h"


typedef std::vector<moodycamel::ConcurrentQueue<std::tuple<int, int>>> com_pad;

struct alignas(64) auto_workspace {
    refinement R;
    selector S;
    coloring c;
    invariant I;
    work_set first_level_fail;
    work_set first_level_succ;
    int first_level_sz = 0;
    int first_level = 1;
    int base_size = 0;
    int first_level_succ_point = -1;
    int skiplevels = 1;
    std::tuple<int, int>* dequeue_space;
    int dequeue_space_sz = 0;

    std::tuple<int, int>* enqueue_space;
    int enqueue_space_sz = 0;

    moodycamel::ConsumerToken* ctok;
    std::vector<moodycamel::ProducerToken*> ptoks;

    pipeline_group* G;

    coloring* start_c;
    invariant start_I;

    com_pad* communicator_pad;
    int communicator_id;

    int measure1 = 0;
    int measure2 = 0;
};

class auto_blaster {
    //moodycamel::ConcurrentQueue<bijection> Q;
    //invariant start_I;
    //coloring start_c;
    //coloring_bucket start_cb;
public:
    void sample(sgraph* g, bool master, bool* done);

    void
    find_automorphism_prob(sgraph *g, bool compare, invariant *canon_I, bijection *canon_leaf, bijection *automorphism,
                           std::default_random_engine *re, int *restarts, bool* done, int selector_seed, auto_workspace* w);

    void
    find_automorphism_prob_bucket(sgraph* g, bool compare, invariant* canon_I, bijection* canon_leaf,
                                                     bijection* automorphism, std::default_random_engine* re, int *restarts, bool *done, int selector_seed,  refinement_bucket* R);

    void sample_pipelined(sgraph *g, bool master, bool *done, bool* done_fast, pipeline_group* G, coloring* start_c, bijection* canon_leaf, invariant* canon_I,
                          com_pad* communicator_pad, int communicator_id);

    void
    find_automorphism_bt(sgraph *g, bool compare, invariant *canon_I, bijection *canon_leaf, bijection *automorphism,
                         std::default_random_engine *re, int *restarts, bool *done, int selector_seed);

    void sample_pipelined_bucket(sgraph *g, bool master, bool *done, pipeline_group *G);

    void fast_automorphism_non_uniform(sgraph *g, bool compare, invariant *canon_I, bijection *canon_leaf,
                                       bijection *automorphism, int *restarts,
                                       bool *done,
                                       int selector_seed, auto_workspace *w);
};


#endif //DEJAVU_AUTO_BLASTER_H
