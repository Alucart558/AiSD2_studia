#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ============================================================
//  ADT grafu na macierzy sąsiedztwa
//  Przyjęcie: graf nieskierowany
//
//  Wierzchołki są identyfikowane przez ID nadawane przy addVertex()
//  ID są stabilne: po removeVertex() wierzchołek jest tylko oznaczany
//  jako usunięty, bez przebudowy całej struktury.
// ============================================================
class GraphMatrix {
private:
    std::vector<std::vector<bool>> adj_;   // macierz połączeń , czy jest krawedz
    std::vector<std::vector<int>> edgeValue_; // wartości krawędzi
    std::vector<bool> active_;             // czy wierzchołek istnieje (ID)
    std::vector<int> vertexValue_;         // wartości wierzchołków

    //czy ID jest w zakresie tablic
    void validateId(int x) const {
        if (x < 0 || x >= static_cast<int>(active_.size())) {
            throw std::out_of_range("Nieprawidlowy identyfikator wierzcholka");
        }
    }

    //czy wierzcholek istnieje
    void validateActive(int x) const {
        validateId(x);
        if (!active_[x]) {
            throw std::logic_error("Wierzcholek zostal usuniety");
        }
    }

public:
    // addVertex(G, x) - dodaje wierzchołek z wartością x
    // Złożoność: O(n) - trzeba rozszerzyć wiersze macierzy
    int addVertex(int value = 0) {
        //n - liczba wierzcholkow przed dodaniem nowego
        const int n = static_cast<int>(active_.size());

        //kazdy wiersz dodaje nowa kolumne z false (macierz polaczen)
        for (auto& row : adj_) {
            row.push_back(false);
        }
        //kazdy wiersz dodaje nowa kolumne z 0 (macierz wartosci krawedzi)
        for (auto& row : edgeValue_) {
            row.push_back(0);
        }

        //dodanie nowego wiersza
        adj_.emplace_back(n + 1, false);
        edgeValue_.emplace_back(n + 1, 0);

        //informacje o nowym wierzcholku
        active_.push_back(true);
        vertexValue_.push_back(value);

        return n; // nowy ID
    }

    // removeVertex(G, x) - usuwa wierzchołek x
    // Złożoność: O(n)
    void removeVertex(int x) {
        validateActive(x);

        //n - liczba wierzcholkow przed usuniecem
        const int n = static_cast<int>(active_.size());
        for (int i = 0; i < n; ++i) {
            // czyszczenie wszystkich krawedzi zwiazanych z x
            adj_[x][i] = false;
            adj_[i][x] = false;
            edgeValue_[x][i] = 0;
            edgeValue_[i][x] = 0;
        }

        //oznaczamy wierzcholek x jako usuniety
        active_[x] = false;
        //czyscimy jego wartosc
        vertexValue_[x] = 0;
    }

    // adjacent(G, x, y) - sprawdza, czy istnieje krawędź miedzy wierzcholkami
    // Złożoność: O(1)
    bool adjacent(int x, int y) const {
        //sprawdza poprawnosc ID
        validateActive(x);
        validateActive(y);
        //odczytuje komorke macierzy sasiedztwa
        return adj_[x][y];
    }

    // neighbours(G, x) - zwraca sąsiadów x
    // Złożoność: O(n)
    std::vector<int> neighbours(int x) const {
        validateActive(x);

        //lista ID sasiadow
        std::vector<int> result;
        result.reserve(active_.size());

        const int n = static_cast<int>(active_.size());
        for (int y = 0; y < n; ++y) {
            //jesli jest aktywny i istnieje krawedz
            if (active_[y] && adj_[x][y]) {
                result.push_back(y);
            }
        }
        return result;
    }

    // addEdge(G, x, y) - dodaje krawędź
    // Złożoność: O(1)
    void addEdge(int x, int y, int value = 0) {
        validateActive(x);
        validateActive(y);

        adj_[x][y] = true;
        adj_[y][x] = true;
        edgeValue_[x][y] = value;
        edgeValue_[y][x] = value;
    }

    // removeEdge(G, x, y) - usuwa krawędź
    // Złożoność: O(1)
    void removeEdge(int x, int y) {
        validateActive(x);
        validateActive(y);

        adj_[x][y] = false;
        adj_[y][x] = false;
        edgeValue_[x][y] = 0;
        edgeValue_[y][x] = 0;
    }

    // getVertexValue(G, x)
    int getVertexValue(int x) const {
        validateActive(x);
        return vertexValue_[x];
    }

    // setVertexValue(G, x, v)
    void setVertexValue(int x, int v) {
        validateActive(x);
        vertexValue_[x] = v;
    }

    // getEdgeValue(G, x, y)
    int getEdgeValue(int x, int y) const {
        validateActive(x);
        validateActive(y);

        if (!adj_[x][y]) {
            throw std::logic_error("Brak krawedzi");
        }
        return edgeValue_[x][y];
    }

    // setEdgeValue(G, x, y, v)
    void setEdgeValue(int x, int y, int v) {
        validateActive(x);
        validateActive(y);

        if (!adj_[x][y]) {
            throw std::logic_error("Brak krawedzi");
        }

        edgeValue_[x][y] = v;
        edgeValue_[y][x] = v;
    }
};

// ============================================================
//  Demo poprawności
// ============================================================
void demo() {
    std::cout << "=== Demo poprawnosci (graf na macierzy sasiedztwa) ===\n";

    GraphMatrix g;

    int v0 = g.addVertex(10);
    int v1 = g.addVertex(20);
    int v2 = g.addVertex(30);
    int v3 = g.addVertex(40);

    g.addEdge(v0, v1, 5);
    g.addEdge(v0, v2, 7);
    g.addEdge(v1, v3, 9);

    std::cout << "Wierzcholki i ich wartosci:\n";
    std::cout << "  v0 = " << g.getVertexValue(v0) << "\n";
    std::cout << "  v1 = " << g.getVertexValue(v1) << "\n";
    std::cout << "  v2 = " << g.getVertexValue(v2) << "\n";
    std::cout << "  v3 = " << g.getVertexValue(v3) << "\n\n";

    std::cout << "Sasiedzi v0: ";
    for (int x : g.neighbours(v0)) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    std::cout << "adjacent(v0, v1) = " << g.adjacent(v0, v1) << "\n";
    std::cout << "adjacent(v2, v3) = " << g.adjacent(v2, v3) << "\n";
    std::cout << "getEdgeValue(v0, v1) = " << g.getEdgeValue(v0, v1) << "\n";

    g.setVertexValue(v2, 99);
    g.setEdgeValue(v0, v1, 55);

    std::cout << "\nPo zmianach:\n";
    std::cout << "  getVertexValue(v2) = " << g.getVertexValue(v2) << "\n";
    std::cout << "  getEdgeValue(v0, v1) = " << g.getEdgeValue(v0, v1) << "\n";

    g.removeEdge(v0, v2);
    std::cout << "Po removeEdge(v0, v2): adjacent(v0, v2) = " << g.adjacent(v0, v2) << "\n";

    g.removeVertex(v3);
    std::cout << "Po removeVertex(v3), sasiedzi v1: ";
    for (int x : g.neighbours(v1)) {
        std::cout << x << " ";
    }
    std::cout << "\n\n";
}

// ============================================================
//  Benchmark: mierzymy neighbours(G, x)
//  Dla macierzy sąsiedztwa złożoność to O(n),
//  więc wykres powinien rosnąć w przybliżeniu liniowo.
// ============================================================
void benchmarkNeighbours(const std::string& filename) {
    std::ofstream out(filename);
    out << "# n  czas_ns\n";

    std::cout << "=== Benchmark neighbours(G, x) -> " << filename << " ===\n";

    const int REPS = 300;
    //np.4096 dla wiekszej ilosci punktow
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
        // Budujemy raz graf gwiazdę:
        // 0 jest połączony ze wszystkimi innymi.
        GraphMatrix g;
        for (int i = 0; i < n; ++i) {
            g.addVertex(i);
        }
        for (int i = 1; i < n; ++i) {
            g.addEdge(0, i, i);
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

        out << n << "  " << med << "\n";
        std::cout << "  n=" << n << "  mediana=" << med << " ns\n";
    }

    if (sink == 123456789) {
        std::cout << "";
    }

    std::cout << "Dane zapisane do " << filename << "\n\n";
}

// ============================================================
//  writeGnuplot - generowanie skryptu gnuplot
// ============================================================
void writeGnuplot(const std::string& dataFile, const std::string& scriptFile) {
    std::ofstream gp(scriptFile);

    gp << R"(set title 'Zlozonosc neighbours(G, x) - graf na macierzy sasiedztwa'
set xlabel 'Liczba wierzcholkow (n)'
set ylabel 'Czas [ns]'
set grid
set key top left

# Dopasowanie liniowe: f(x) = a*x + b
f(x) = a*x + b
fit f(x) ')"
       << dataFile <<
R"(' using 1:2 via a,b

plot ')"
       << dataFile <<
R"(' using 1:2 with points pt 7 ps 0.8 lc rgb '#0077BB' title 'pomiary', \
     f(x) with lines lw 2 lc rgb '#CC3311' title sprintf('fit: %.3f*n + %.1f', a, b)
pause -1 'Nacisnij Enter'
)";

    std::cout << "Skrypt gnuplot zapisany do " << scriptFile << "\n";
    std::cout << "Uruchom: gnuplot " << scriptFile << "\n";
}

// ============================================================
int main() {
    demo();
    benchmarkNeighbours("benchmark_graph.txt");
    writeGnuplot("benchmark_graph.txt", "plot_graph.gp");
    return 0;
}
