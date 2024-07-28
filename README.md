# DejaVu

This is a minimalist integration of the graph automorphism solver [DejaVu](https://automorphisms.org).

Sample run:
```julia
julia> using DejaVu, Graphs

julia> c = cycle_graph(10) # from Graphs
{10, 10} undirected simple Int64 graph

julia> g = StaticGraph(c)
DejaVu.StaticGraphAllocated(Ptr{Nothing} @0x0000600000b13840)

julia> d = Solver()
DejaVu.SolverAllocated(Ptr{Nothing} @0x0000600002737f30)

julia> automorphisms!(d,g)
preprocessing
______________________________________________________________
T (ms)     delta(ms)  proc        p1              p2
______________________________________________________________
0.08       0.08       regular     10              20

solving_component 1/1 (n=10)
______________________________________________________________
T (ms)     delta(ms)  proc        p1              p2
______________________________________________________________
0.14       0.03       sel         2               39
0.18       0.04       dfs         2-0             ~2*10^1
0.19       0.01       done        1               2

julia> group_size(d) |> String
"2*10^1"
```

[![Build Status](https://github.com/laurentbartholdi/DejaVu.jl/actions/workflows/CI.yml/badge.svg?branch=main)](https://github.com/laurentbartholdi/DejaVu.jl/actions/workflows/CI.yml?query=branch%3Amain)
