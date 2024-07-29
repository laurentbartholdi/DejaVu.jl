#include "jlcxx/jlcxx.hpp"
#include "jlcxx/tuple.hpp"

#include "dejavu.h"

/*
void get_orbits(dejavu::groups::orbit &orbits, dejavu::static_graph &g)
{
  //  int n = g->get_sgraph()->v_size;
  
  //  dejavu::groups::orbit orbits(n);

  dejavu::hooks::orbit_hook hook(orbits);
  dejavu::solver d;
  d.set_print(false);
  std::cout << "HOOK " << hook.get_hook() << "\n";
  d.automorphisms(&g, hook.get_hook());
}
*/

JLCXX_MODULE define_julia_module(jlcxx::Module& mod)
{
  mod.add_type<dejavu::static_graph>("StaticGraph")
    .method("initialize_graph!", &dejavu::static_graph::initialize_graph)
    .method("__add_vertex!", &dejavu::static_graph::add_vertex)
    .method("__add_edge!", &dejavu::static_graph::add_edge)
    .method("__nv", [](dejavu::static_graph &g) { return g.get_sgraph()->v_size; })
    .method("__ne", [](dejavu::static_graph &g) { return g.get_sgraph()->e_size; });

  mod.add_type<dejavu::groups::orbit>("Orbit")
    .constructor<int>()
    .method("find_orbit", &dejavu::groups::orbit::find_orbit)
    .method("orbit_size", &dejavu::groups::orbit::orbit_size)
    .method("represents_orbit", &dejavu::groups::orbit::represents_orbit)
    .method("combine_orbits", &dejavu::groups::orbit::combine_orbits)
    .method("are_in_same_orbit", &dejavu::groups::orbit::are_in_same_orbit)
    .method("reset", &dejavu::groups::orbit::reset);
  
  mod.add_type<dejavu_hook>("DejaVuHook");

  mod.add_type<dejavu::hooks::hook_interface>("HookInterface");

  mod.add_type<dejavu::hooks::orbit_hook>("OrbitHook")
    .constructor<dejavu::groups::orbit>()
    .method("get_hook", [](dejavu::hooks::orbit_hook &h) { return h.get_hook(); });

  mod.add_type<dejavu::big_number>("__BigNumber")
    .method("__mantissa", [](dejavu::big_number &n) { return (double) n.mantissa; }) // Julia does not have "long double"
    .method("__exponent", [](dejavu::big_number &n) { return n.exponent; });
    //mod.set_override_module(jl_base_module);
    //mod.method("String", [](dejavu::big_number &b) { std::stringstream s; s << b; return s.str(); });
    //mod.unset_override_module();

  mod.add_type<dejavu::solver>("Solver")
    .method("automorphisms!", [](dejavu::solver &s, dejavu::static_graph &g) { s.automorphisms(&g); })
    .method("automorphisms!", [](dejavu::solver &s, dejavu::static_graph &g, dejavu_hook *h) { s.automorphisms(&g,h); })
    .method("automorphisms!", [](dejavu::solver &s, dejavu::static_graph &g, void *h) { s.automorphisms(&g,dejavu_hook((void (*)(int,const int*,int,const int*)) h)); })
    .method("automorphisms!", [](dejavu::solver &s, dejavu::static_graph &g, dejavu::groups::orbit &o) {
      dejavu::hooks::orbit_hook h(o);
      s.automorphisms(&g,h.get_hook());
    })
    .method("automorphisms!", [](dejavu::solver &s, dejavu::static_graph &g, dejavu::hooks::hook_interface &h) { s.automorphisms(&g,h); })
    .method("__group_size", &dejavu::solver::get_automorphism_group_size)
    .method("deterministic_termination", &dejavu::solver::get_deterministic_termination)
    .method("set_print", &dejavu::solver::set_print);    
}
