set title 'Zlozonosc neighbours(G, x) - graf na macierzy sasiedztwa'
set xlabel 'Liczba wierzcholkow (n)'
set ylabel 'Czas [ns]'
set grid
set key top left

# Dopasowanie liniowe: f(x) = a*x + b
f(x) = a*x + b
fit f(x) 'benchmark_graph.txt' using 1:2 via a,b

plot 'benchmark_graph.txt' using 1:2 with points pt 7 ps 0.8 lc rgb '#0077BB' title 'pomiary', \
     f(x) with lines lw 2 lc rgb '#CC3311' title sprintf('fit: %.3f*n + %.1f', a, b)
pause -1 'Nacisnij Enter'
