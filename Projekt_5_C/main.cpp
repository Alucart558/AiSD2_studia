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
    // Działanie:
    // 1. Weryfikuje czy wierzchołek x istnieje i jest w prawidłowym zakresie
    // 2. Iteruje przez wszystkie krawędzie wychodzące z wierzchołka x
    // 3. Jeśli znajdzie krawędź, której wierzchołek docelowy to y, zwraca true
    // 4. Jeśli przejdzie przez wszystkie krawędzie bez znalezienia, zwraca false
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
    // Działanie:
    // 1. Tworzy pusty wektor wynikowy
    // 2. Weryfikuje czy wierzchołek x istnieje i jest w prawidłowym zakresie
    // 3. Jeśli warunki nie są spełnione, zwraca pusty wektor
    // 4. Iteruje przez wszystkie krawędzie wychodzące z x
    // 5. Dodaje identyfikator każdego wierzchołka docelowego do wektora wynikowego
    // 6. Zwraca wektor zawierający wszystkie ID sąsiadów
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

    // printNeighbors(G, x) - wypisuje sąsiadów wierzchołka x na wyjście standardowe
    // Działanie:
    // 1. Weryfikuje czy wierzchołek x istnieje i jest w prawidłowym zakresie
    // 2. Jeśli nie, funkcja się kończy bez wypisania nic
    // 3. Wypisuje nagłówek "Sąsiedzi wierzchołka [x]: "
    // 4. Iteruje przez wszystkie krawędzie wychodzące z x
    // 5. Wypisuje ID każdego wierzchołka docelowego oddzielonego spacją
    // 6. Wypisuje znak nowej linii
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

    // addVertex(G, x) - dodaje wierzchołek o indeksie x do grafu
    // Działanie:
    // 1. Weryfikuje czy indeks x jest nieujemny (> 0)
    // 2. Jeśli x jest ujemny, funkcja się kończy
    // 3. Jeśli x jest poza zakresem obecnego wektora vertices, zmienia rozmiar wektora
    //    na x+1 (inicjalizuje nowe wierzchołki jako nieistniejące)
    // 4. Ustawia flagę exists na true dla wierzchołka x, co oznacza że wierzchołek istnieje
    void addVertex(int x) {
        if (x < 0) return;

        if (x >= static_cast<int>(vertices.size())) {
            vertices.resize(x + 1);
        }
        vertices[x].exists = true;
    }

    // removeVertex(G, x) - usuwa wierzchołek x i wszystkie krawędzie z nim związane
    // Złożoność: O(V + E)
    // Działanie:
    // 1. Weryfikuje czy wierzchołek x istnieje i jest w prawidłowym zakresie
    // 2. Jeśli warunki nie są spełnione, funkcja się kończy
    // 3. Ustawia flagę exists na false - oznacza usunięcie logiczne wierzchołka
    // 4. Czyści listę krawędzi wychodzących z wierzchołka x
    // 5. Iteruje przez wszystkie pozostałe wierzchołki w grafie
    // 6. Z każdego wierzchołka usuwa wszystkie krawędzie, które prowadzą do x
    //    (używając remove_if do usunięcia krawędzi z wnętrze listy)
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

    // addEdge(G, x, y) - dodaje krawędź x -> y do grafu
    // Złożoność: O(deg(x))
    // Działanie:
    // 1. Weryfikuje czy indeksy x i y są nieujemne
    // 2. Weryfikuje czy oba indeksy są w zakresie wektora vertices
    // 3. Weryfikuje czy oba wierzchołki istnieją (są zaznaczone jako exists=true)
    // 4. Jeśli krawędź x->y już istnieje (sprawdzana metodą adjacent),
    //    funkcja się kończy bez dodania duplikatu
    // 5. Dodaje nową krawędź do listy edges wierzchołka x z wagą 0
    // 6. Jeśli graf jest nieskierowany, dodaje również krawędź zwrotną y->x
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

    // removeEdge(G, x, y) - usuwa krawędź x -> y z grafu
    // Złożoność: O(deg(x))
    // Działanie:
    // 1. Weryfikuje czy indeksy x i y są nieujemne
    // 2. Weryfikuje czy oba indeksy są w zakresie wektora vertices
    // 3. Weryfikuje czy oba wierzchołki istnieją
    // 4. Jeśli warunki nie są spełnione, funkcja się kończy
    // 5. Z listy edges wierzchołka x usuwa wszystkie krawędzie prowadzące do y
    //    (przy użyciu remove_if)
    // 6. Jeśli graf jest nieskierowany, również usuwa krawędź zwrotną y->x
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

    // getVertexValue(G, x) - zwraca wartość przechowywana w wierzchołku x
    // Działanie:
    // 1. Bezpośrednio zwraca pole value wierzchołka o indeksie x
    // 2. Nie wykonuje żadnej weryfikacji zakresu lub istnienia wierzchołka
    //    (zakładamy poprawne parametry)
    int getVertexValue(int x) {
        return vertices[x].value;
    }

    // setVertexValue(G, x, v) - ustawia wartość v dla wierzchołka x
    // Działanie:
    // 1. Bezpośrednio przypisuje wartość v do pola value wierzchołka o indeksie x
    // 2. Nie wykonuje żadnej weryfikacji zakresu lub istnienia wierzchołka
    //    (zakładamy poprawne parametry)
    void setVertexValue(int x, int v) {
        vertices[x].value = v;
    }

    // getEdgeValue(G, x, y) - zwraca wagę krawędzi x -> y
    // Działanie:
    // 1. Weryfikuje czy indeksy x i y są w prawidłowym zakresie
    // 2. Jeśli nie, zwraca -1 (oznacza błąd/brak krawędzi)
    // 3. Iteruje przez listę edges wierzchołka x szukając krawędzi do y
    // 4. Jeśli znajdzie krawędź z targetem równym y, zwraca jej wage (pole value)
    // 5. Jeśli przejdzie przez wszystkie krawędzie bez znalezienia, zwraca -1
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

    // setEdgeValue(G, x, y, v) - ustawia wagę v dla krawędzi x -> y
    // Działanie:
    // 1. Weryfikuje czy indeksy x i y są w prawidłowym zakresie
    // 2. Jeśli nie, funkcja się kończy bez zmian
    // 3. Iteruje przez listę edges wierzchołka x szukając krawędzi do y
    // 4. Jeśli znajdzie krawędź z targetem równym y, zmienia jej wage na v
    // 5. Jeśli nie znajdzie krawędzi, nic się nie zmienia
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

// benchmarkNeighbours(filename) - mierzy i analizuje złożoność czasową metody neighbours()
// Działanie:
// 1. Otwiera plik do zapisu wyników
// 2. Weryfikuje czy plik został otwarty pomyślnie
// 3. Pisze nagłówek z kolumnami: n (rozmiar) i czas_ns (czas w nanosekundach)
// 4. Generuje serie rozmiarów grafów (potęgi 2 i ich 1.5 wielokrotności, do N_MAX=2048)
// 5. Dla każdego rozmiaru n:
//    a. Tworzy nowy graf z n wierzchołkami
//    b. Łączy wierzchołek 0 ze wszystkimi pozostałymi (graf gwiazda)
//    c. 300 razy wykonuje neighbours(0) i mierzy czas w nanosekundach
//    d. Sortuje zmierzone czasy i bierze medianę
//    e. Wypisuje wynik do pliku i na konsolę
// 6. Zmienna sink zapobiega optymalizacjom kompilatora
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

// writeGnuplot(dataFile, scriptFile) - generuje skrypt gnuplot do wizualizacji wyników
// Działanie:
// 1. Otwiera plik scriptFile do zapisu
// 2. Weryfikuje czy plik został otwarty pomyślnie
// 3. Wypisuje do pliku skrypt gnuplot zawierający:
//    a. Tytuł i etykiety osi
//    b. Funkcję liniową f(x) = a*x + b do dopasowania do danych
//    c. Komendę fit do znalezienia parametrów a i b na podstawie danych z dataFile
//    d. Polecenie plot do narysowania zarówno pomiarów (punkty) jak i dopasowanej funkcji
//    e. Instrukcję pause czekającą na Enter
// 4. Wypisuje informację na konsolę że plik został utworzony
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

// demo() - funkcja demonstracyjna pokazująca prawidłowość działania grafu
// Działanie:
// 1. Wypisuje nagłówek "=== Demo poprawnosci ==="
// 2. Tworzy nowy graf skierowany
// 3. Dodaje trzy wierzchołki: 0, 1, 2
// 4. Dodaje trzy krawędzie: 0->1, 0->2, 1->2
// 5. Testuje getVertexValue i setVertexValue - ustawia wartość 100 dla wierzchołka 0
// 6. Testuje getEdgeValue i setEdgeValue - ustawia wagę 50 dla krawędzi 0->1
// 7. Testuje adjacent() - sprawdza czy 0 jest sąsiadem 1 (tak) i czy 2 jest sąsiadem 0 (nie)
// 8. Testuje printNeighbors() - wypisuje sąsiadów wierzchołka 0
// 9. Wypisuje wartości wierzchołka i krawędzi
// 10. Testuje removeEdge() - usuwa krawędź 0->2 i wypisuje sąsiadów (powinno zostać tylko 1)
// 11. Testuje removeVertex() - usuwa wierzchołek 1 i wypisuje sąsiadów (powinno być puste)
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

// main() - główna funkcja programu
// Działanie:
// 1. Wykonuje funkcję demo() do sprawdzenia poprawności działania grafu
// 2. Wykonuje benchmarkNeighbours() do pomiaru złożoności metody neighbours()
//    i zapisania wyników do pliku benchmark_graph.txt
// 3. Wykonuje writeGnuplot() do wygenerowania skryptu gnuplot do wizualizacji wyników
// 4. Zakańcza program zwracając 0
int main() {
    demo();
    benchmarkNeighbours("benchmark_graph.txt");
    writeGnuplot("benchmark_graph.txt", "plot_graph.gp");
    return 0;
}