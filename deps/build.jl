using CxxWrap, Libdl

run(`cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$(CxxWrap.prefix_path()) -DJulia_EXECUTABLE=$(joinpath(Sys.BINDIR,"julia")) .`)
run(`cmake --build . --config Release`)
