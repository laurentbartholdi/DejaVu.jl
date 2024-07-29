module DejaVu

using CxxWrap, Libdl, Graphs, DataStructures

export StaticGraph,
  set_print, automorphisms!, group_size,
  orbits

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
        add_edge!(sg,e.src,e.dst)
    end
    sg
end

function StaticGraph(nv::Integer,ne::Integer)
    sg = StaticGraph()
    initialize_graph!(sg,nv,ne)
    sg
end
    
function group_size(d::Solver)
    n = __group_size(d)
    (m,e) = (__mantissa(n),__exponent(n))
    BigInt(round(m*BigFloat(10)^e))
end

Graphs.add_vertex!(g,color,degree) = __add_vertex!(g,color,degree)+1
Graphs.add_vertex!(g,degree) = __add_vertex!(g,0,degree)+1
Graphs.add_edge!(g,src,dst) = __add_edge!(g,minmax(src-1,dst-1)...)
Graphs.ne(g) = __ne(g)
Graphs.nv(g) = __nv(g)

function Base.show(io::IO, g::StaticGraph)
    try
        print(io,"{$(nv(g)),$(ne(g))//2} undirected simple DejaVu graph")
    catch
        print(io,"incomplete DejaVu graph")
    end
end

function orbits(g::StaticGraph)
    n = nv(g)
    o = Orbit(n)
    d = Solver()
    set_print(d,false)
    automorphisms!(d,g,o)
    r = [find_orbit(o,i-1)+1 for i=1:n]
    IntDisjointSets{Int}(r,zeros(Int,n),n - sum(orbit_size(o,i-1)-1 for i=1:n if r[i]==i))
end

const __data = Any[nothing,nothing]

function __wrapper(n,v,n_supp,v_supp)
    img = unsafe_wrap(Vector{Cint},v,n)
    supp = unsafe_wrap(Vector{Cint},v_supp,n_supp)
    __data[2]==nothing ? __data[1](img,supp) : __data[1](img,supp,__data[2])
    nothing
end

function automorphisms!(d::Solver,g::StaticGraph,f::Function,data = nothing)
    __data[1] = f # ugly, not multithreaded
    __data[2] = data
    automorphisms!(d,g,@cfunction(__wrapper,Cvoid,(Cint,Ptr{Cint},Cint,Ptr{Cint})))
end

# compute automorphisms as permutations

end
