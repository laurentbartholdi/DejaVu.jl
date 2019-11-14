//
// Created by markus on 14.10.19.
//

#include <iostream>
#include "utility.h"
#include "configuration.h"

int intRand(const int & min, const int & max, int seed) {
    static thread_local std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(min,max);
    return distribution(generator);
}

double doubleRand(const double & min, const double & max, int seed) {
    static thread_local std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(min,max);
    return floor(distribution(generator));
}


shared_switches::shared_switches() {
    done_shared_group.store(false);
    done_created_group.store(false);
    _ack_done.store(0);
    win_id.store(-2);
    checked.store(0);
}

bool shared_switches::check_strategy_tournament(int id, strategy_metrics* m) {
    thread_local bool ichecked = false;

    if(!ichecked) {
        tournament_mutex.lock();
        if((m->restarts < win_metrics.restarts) || (m->restarts == win_metrics.restarts && m->expected_bfs_size < win_metrics.expected_bfs_size) || win_id == -2) {
            std::cout << m->restarts << ", " << m->expected_bfs_size << std::endl;
            win_metrics = *m;
            win_id  = id;
        }
        checked++;
        tournament_mutex.unlock();
    }

    ichecked = true;
    return (checked == config.CONFIG_THREADS_REFINEMENT_WORKERS + 1);
}

bool shared_switches::ack_done() {
    thread_local bool ichecked = false;

    if(!ichecked) {
        _ack_done++;
    }

    ichecked = true;
    return (checked == config.CONFIG_THREADS_REFINEMENT_WORKERS + 1);
}

void shared_switches::reset_leaf_tournament() {
    done_shared_group.store(false);
    done_created_group.store(false);
    win_id.store(-2);
    checked.store(0);
}

void shared_switches::iterate_tolerance() {
    tolerance *= 2;
}