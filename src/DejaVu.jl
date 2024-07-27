module DejaVu

using CxxWrap, Libdl, Graphs

export StaticGraph, add_vertex!, add_edge!,
  Solver, set_print, automorphisms!, group_size,
  Orbit, orbit_size, find_orbit, represents_orbit, combine_orbits, are_in_same_orbit, reset

get_path() = joinpath(@__DIR__,"../deps/lib","libdejavu.$(Libdl.dlext)")

@wrapmodule get_path

function __init__()
    @initcxx
end

function StaticGraph(g::AbstractGraph,color=i->0)
    sg = StaticGraph()
    initialize_graph!(sg,nv(g),ne(g))
    for i=1:nv(g)
        add_vertex!(sg,isa(color,Function) ? color(i) : color[i],degree(g,i))
    end
    for e=edges(g)
        add_edge!(sg,minmax(e.src-1,e.dst-1)...)
    end
    sg
end

end
