using Test, DejaVu, Graphs

@testset "Polygon" begin
    c = cycle_graph(10)
    @test nv(c)==ne(c)==10

    d = Solver()
    set_print(d,false)
    
    g = StaticGraph(c)
    automorphisms!(d,g)
    @test group_size(d) |> String == "2*10^1"

    g = StaticGraph(c,==(1))
    automorphisms!(d,g)
    @test group_size(d) |> String == "2*10^0"
    o = Orbit(10)
    automorphisms!(d,g,o)
    @test [orbit_size(o,i) for i=0:9] == [1,2,2,2,2,1,2,2,2,2]
end
