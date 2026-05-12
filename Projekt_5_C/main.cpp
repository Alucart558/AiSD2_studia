#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <fstream>
#include <chrono>

// Struktura reprezentująca krawędź grafu
struct Edge {
    int target;   // identyfikator wierzchołka docelowego
    int value;    // waga krawędzi
};

// Struktura reprezentująca wierzchołek grafu
struct Vertex {
    int value;              // wartość przechowywana w wierzchołku
    std::list<Edge> edges;  // lista krawędzi wychodzących
    bool exists = false;    // czy wierzchołek istnieje
};

// Klasa reprezentująca graf skierowany / nieskierowany
class Graph {
private:
    std::vector<Vertex> vertices;
    bool isDirected;

public:
    Graph(bool directed = true) : isDirected(directed) {}

    // adjacent(G, x, y) - sprawdza czy x ma krawędź do y
    // Złożoność: O(deg(x))
    bool adjacent(int x, int y) {
        if (x < 0 || x >= static_cast<int>(vertices.size()) || !vertices[x].exists) {
            return false;
        }

        for (const auto& edge : vertices[x].edges) {
            if (edge.target == y) {
                return true;
            }
        }
        return false;
    }

    // neighbours(G, x) - zwraca listę sąsiadów wierzchołka x
    // Złożoność: O(deg(x))
    std::vector<int> neighbours(int x) {
        std::vector<int> result;

        if (x < 0 || x >= static_cast<int>(vertices.size()) || !vertices[x].exists) {
            return result;
        }

        for (const auto& edge : vertices[x].edges) {
            result.push_back(edge.target);
        }
        return result;
    }

    // printNeighbors(G, x) - wypisuje sąsiadów wierzchołka x
    void printNeighbors(int x) {
        if (x < 0 || x >= static_cast<int>(vertices.size()) || !vertices[x].exists) {
            return;
        }

        std::cout << "Sąsiedzi wierzchołka " << x << ": ";
        for (const auto& edge : vertices[x].edges) {
            std::cout << edge.target << " ";
        }
        std::cout << std::endl;
    }

    // addVertex(G, x) - dodaje wierzchołek o indeksie x
    void addVertex(int x) {
        if (x < 0) return;

        if (x >= static_cast<int>(vertices.size())) {
            vertices.resize(x + 1);
        }
        vertices[x].exists = true;
    }

    // removeVertex(G, x) - usuwa wierzchołek x i wszystkie krawędzie związane z nim
    // Złożoność: O(V + E)
    void removeVertex(int x) {
        if (x < 0 || x >= static_cast<int>(vertices.size()) || !vertices[x].exists) {
            return;
        }

        vertices[x].exists = false;
        vertices[x].edges.clear();

        for (auto& v : vertices) {
            v.edges.remove_if([x](const Edge& e) {
                return e.target == x;
            });
        }
    }

    // addEdge(G, x, y) - dodaje krawędź x -> y
    // Złożoność: O(deg(x))
    void addEdge(int x, int y) {
        if (x < 0 || y < 0) return;
        if (x >= static_cast<int>(vertices.size()) || y >= static_cast<int>(vertices.size())) return;
        if (!vertices[x].exists || !vertices[y].exists) return;

        if (adjacent(x, y)) {
            return;
        }

        vertices[x].edges.push_back({y, 0});

        if (!isDirected) {
            vertices[y].edges.push_back({x, 0});
        }
    }

    // removeEdge(G, x, y) - usuwa krawędź x -> y
    // Złożoność: O(deg(x))
    void removeEdge(int x, int y) {
        if (x < 0 || y < 0) return;
        if (x >= static_cast<int>(vertices.size()) || y >= static_cast<int>(vertices.size())) return;
        if (!vertices[x].exists || !vertices[y].exists) return;

        vertices[x].edges.remove_if([y](const Edge& e) {
            return e.target == y;
        });

        if (!isDirected) {
            vertices[y].edges.remove_if([x](const Edge& e) {
                return e.target == x;
            });
        }
    }

    // getVertexValue(G, x)
    int getVertexValue(int x) {
        return vertices[x].value;
    }

    // setVertexValue(G, x, v)
    void setVertexValue(int x, int v) {
        vertices[x].value = v;
    }

    // getEdgeValue(G, x, y)
    int getEdgeValue(int x, int y) {
        if (x < 0 || y < 0 || x >= static_cast<int>(vertices.size()) || y >= static_cast<int>(vertices.size())) {
            return -1;
        }

        for (const auto& edge : vertices[x].edges) {
            if (edge.target == y) {
                return edge.value;
            }
        }
        return -1;
    }

    // setEdgeValue(G, x, y, v)
    void setEdgeValue(int x, int y, int v) {
        if (x < 0 || y < 0 || x >= static_cast<int>(vertices.size()) || y >= static_cast<int>(vertices.size())) {
            return;
        }

        for (auto& edge : vertices[x].edges) {
            if (edge.target == y) {
                edge.value = v;
            }
        }
    }
};

// Benchmark: mierzymy złożoność neighbours(G, x)
void benchmarkNeighbours(const std::string& filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku: " << filename << std::endl;
        return;
    }

    out << "# n czas_ns\n";
    std::cout << "=== Benchmark neighbours(G, x) -> " << filename << " ===\n";

    const int REPS = 300;
    const int N_MAX = 2048;

    std::vector<int> sizes;
    for (int p = 6; (1 << p) <= N_MAX; ++p) {
        sizes.push_back(1 << p);
        if ((1 << p) * 3 / 2 <= N_MAX) {
            sizes.push_back((1 << p) * 3 / 2);
        }
    }

    volatile std::size_t sink = 0;

    for (int n : sizes) {
        Graph g(true);

        for (int i = 0; i < n; ++i) {
            g.addVertex(i);
        }

        // Graf gwiazda: 0 połączony ze wszystkimi
        for (int i = 1; i < n; ++i) {
            g.addEdge(0, i);
        }

        std::vector<long long> times(REPS);

        for (int r = 0; r < REPS; ++r) {
            auto t0 = std::chrono::high_resolution_clock::now();

            auto neigh = g.neighbours(0);
            sink += neigh.size();

            auto t1 = std::chrono::high_resolution_clock::now();
            times[r] = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
        }

        std::sort(times.begin(), times.end());
        long long med = times[REPS / 2];

        out << n << " " << med << "\n";
        std::cout << "  n=" << n << "  mediana=" << med << " ns\n";
    }

    if (sink == 123456789) {
        std::cout << "";
    }

    std::cout << "Dane zapisane do " << filename << "\n";
}

// Generuje skrypt gnuplot
void writeGnuplot(const std::string& dataFile, const std::string& scriptFile) {
    std::ofstream gp(scriptFile);
    if (!gp.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku: " << scriptFile << std::endl;
        return;
    }

    gp << R"(set title 'Zlozonosc neighbours(G, x) - graf na listach sasiedztwa'
set xlabel 'Liczba wierzcholkow (n)'
set ylabel 'Czas [ns]'
set grid
set key top left

f(x) = a*x + b
fit f(x) ')" << dataFile << R"(' using 1:2 via a,b

plot ')" << dataFile << R"(' using 1:2 with points pt 7 ps 0.8 lc rgb '#0077BB' title 'pomiary', \
     f(x) with lines lw 2 lc rgb '#CC3311' title sprintf('fit: %.3f*n + %.1f', a, b)
pause -1 'Nacisnij Enter'
)";

    std::cout << "Skrypt gnuplot zapisany do " << scriptFile << "\n";
}

// Demo działania grafu
void demo() {
    std::cout << "=== Demo poprawnosci ===\n";

    Graph g(true);

    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);

    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);

    g.setVertexValue(0, 100);
    g.setEdgeValue(0, 1, 50);

    std::cout << "Czy 0 jest sąsiadem 1? " << (g.adjacent(0, 1) ? "Tak" : "Nie") << std::endl;
    std::cout << "Czy 2 jest sąsiadem 0? " << (g.adjacent(2, 0) ? "Tak" : "Nie") << std::endl;

    g.printNeighbors(0);

    std::cout << "Wartość wierzchołka 0: " << g.getVertexValue(0) << std::endl;
    std::cout << "Wartość krawędzi 0->1: " << g.getEdgeValue(0, 1) << std::endl;

    std::cout << "\nUsuwanie krawędzi 0->2...\n";
    g.removeEdge(0, 2);
    g.printNeighbors(0);

    std::cout << "Usuwanie wierzchołka 1...\n";
    g.removeVertex(1);
    g.printNeighbors(0);
}

int main() {
    demo();
    benchmarkNeighbours("benchmark_graph.txt");
    writeGnuplot("benchmark_graph.txt", "plot_graph.gp");
    return 0;
}