using Test, DejaVu, Graphs

@testset "Polygon" begin
    d = DejaVu.Solver()

    g = StaticGraph(3,2)
    (v1,v2,v3) = (add_vertex!(g,1),add_vertex!(g,2),add_vertex!(g,1))
    add_edge!(g,v1,v2)
    add_edge!(g,v2,v3)
    automorphisms!(d,g)
    @test group_size(d) == 2
    
    c = cycle_graph(10)
    @test nv(c)==ne(c)==10

    set_print(d,false)
    
    g = StaticGraph(c)
    automorphisms!(d,g)
    @test group_size(d) == 20

    g = StaticGraph(c,==(1))
    automorphisms!(d,g)
    @test group_size(d) == 2
    o = DejaVu.Orbit(10)
    automorphisms!(d,g,o)
    @test [DejaVu.orbit_size(o,i) for i=0:9] == [1,2,2,2,2,1,2,2,2,2]
end
