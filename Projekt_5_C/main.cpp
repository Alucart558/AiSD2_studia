#include <iostream>       // Biblioteka do operacji wejścia/wyjścia (cout, endl)
#include <vector>         // Biblioteka do dynamicznej tablicy wierzchołków
#include <list>           // Biblioteka do listy krawędzi każdego wierzchołka
#include <string>         // Biblioteka do obsługi ciągów znaków (nieużywana w tym kodzie)
#include <algorithm>      // Biblioteka do algorytmów STL, m.in. remove_if (usuwanie warunkowe)

// Struktura reprezentująca krawędź grafu
struct Edge {
    int target;        // Identyfikator wierzchołka docelowego (dokąd prowadzi krawędź)
    int value;         // Waga/wartość krawędzi (np. koszt, odległość w sieci)
};

// Struktura reprezentująca wierzchołek grafu
struct Vertex {
    int value;               // Wartość/etykieta przechowywana w wierzchołku
    std::list<Edge> edges;   // Lista krawędzi wychodzących z tego wierzchołka
    bool exists;             // Flaga określająca czy wierzchołek istnieje w grafie (domyślnie: false)
};

// Klasa reprezentująca graf (skierowany lub nieskierowany)
class Graph {
private:
    std::vector<Vertex> vertices;  // Wektor zawierający wierzchołki grafu (dostęp przez indeks)
    bool isDirected;               // Flaga określająca typ grafu (true = skierowany, false = nieskierowany)

public:
    // Konstruktor inicjalizujący graf z określonym typem (domyślnie: skierowany)
    Graph(bool directed = true) : isDirected(directed) {}

    // 1. adjacent(G, x, y) - Sprawdza czy między wierzchołkami x i y istnieje bezpośrednia krawędź
    bool adjacent(int x, int y) {
        // Sprawdzenie czy wierzchołek x istnieje w wektorze i czy jest zaznaczony jako istniejący
        if (x >= vertices.size() || !vertices[x].exists) return false;
        // Przeszukanie listy krawędzi wychodzących z wierzchołka x
        for (auto const& edge : vertices[x].edges) {
            // Jeśli znaleziono krawędź do wierzchołka y, zwraca true
            if (edge.target == y) return true;
        }
        // Jeśli pętla się skończyła bez znalezienia krawędzi, zwraca false
        return false;
    }

    // 2. neighbors(G, x) - Wypisuje wszystkich sąsiadów (wierzchołki bezpośrednio połączone z x)
    void printNeighbors(int x) {
        // Sprawdzenie czy wierzchołek x istnieje
        if (x < vertices.size() && vertices[x].exists) {
            // Wypisanie przygotowania do listy sąsiadów
            std::cout << "Sąsiedzi wierzchołka " << x << ": ";
            // Iteracja po wszystkich krawędziach wychodzących z wierzchołka x
            for (auto const& edge : vertices[x].edges) {
                // Wypisanie identyfikatora wierzchołka docelowego
                std::cout << edge.target << " ";
            }
            // Przejście do nowej linii
            std::cout << std::endl;
        }
    }

    // 3. addVertex(G, x) - Dodaje nowy wierzchołek o indeksie x do grafu
    void addVertex(int x) {
        // Rozszerzenie wektora jeśli x znajduje się poza aktualnym rozmiarem
        if (x >= vertices.size()) vertices.resize(x + 1);
        // Zaznaczenie wierzchołka jako istniejącego
        vertices[x].exists = true;
    }

    // 4. removeVertex(G, x) - Usuwa wierzchołek x i wszystkie jego krawędzie
    void removeVertex(int x) {
        // Sprawdzenie czy wierzchołek x istnieje w wektorze
        if (x < vertices.size()) {
            // Zaznaczenie wierzchołka jako nieistniejącego (logiczne usunięcie)
            vertices[x].exists = false;
            // Wyczyszczenie listy krawędzi wychodzących z wierzchołka x
            vertices[x].edges.clear();
            // Iteracja po WSZYSTKICH wierzchołkach w grafie
            for (auto& v : vertices) {
                // Usunięcie wszystkich krawędzi wchodzących do wierzchołka x z dowolnego wierzchołka
                // remove_if: usuwa elementy spełniające warunek, bez zmiany rozmiaru listy
                // erase: usuwa efektywnie elementy zmienionego zakresu
                v.edges.remove_if([x](const Edge& e) { return e.target == x; });
            }
        }
    }

    // 5. addEdge(G, x, y) - Dodaje krawędź od wierzchołka x do wierzchołka y
    void addEdge(int x, int y) {
        // Sprawdzenie czy krawędź już istnieje (unikanie duplikatów)
        if (adjacent(x, y)) return;
        // Dodanie krawędzi do listy krawędzi wychodzących z wierzchołka x (waga domyślnie 0)
        vertices[x].edges.push_back({y, 0});
        // Jeśli graf jest nieskierowany, dodaj również krawędź powrotną (x <- y)
        if (!isDirected) {
            vertices[y].edges.push_back({x, 0});
        }
    }

    // 6. removeEdge(G, x, y) - Usuwa krawędź od wierzchołka x do wierzchołka y
    void removeEdge(int x, int y) {
        // Sprawdzenie czy wierzchołek x istnieje w wektorze
        if (x < vertices.size()) {
            // Usunięcie wszystkich krawędzi z wierzchołka x do y
            vertices[x].edges.remove_if([y](const Edge& e) { return e.target == y; });
            // Jeśli graf jest nieskierowany, usuń również krawędź powrotną
            if (!isDirected && y < vertices.size()) {
                vertices[y].edges.remove_if([x](const Edge& e) { return e.target == x; });
            }
        }
    }

    // 7. getVertexValue - Zwraca wartość przechowywana w wierzchołku x
    int getVertexValue(int x) { return vertices[x].value; }

    // 8. setVertexValue - Ustawia wartość przechowywana w wierzchołku x
    void setVertexValue(int x, int v) { vertices[x].value = v; }

    // 9. getEdgeValue - Zwraca wartość (wagę) krawędzi od x do y
    int getEdgeValue(int x, int y) {
        // Iteracja po listach krawędzi wychodzących z wierzchołka x
        for (auto& edge : vertices[x].edges) {
            // Jeśli znaleziono krawędź do y, zwraca jej wagę
            if (edge.target == y) return edge.value;
        }
        // Jeśli krawędź nie istnieje, zwraca -1 (kod błędu)
        return -1;
    }

    // 10. setEdgeValue - Ustawia wartość (wagę) krawędzi od x do y
    void setEdgeValue(int x, int y, int v) {
        // Iteracja po listach krawędzi wychodzących z wierzchołka x
        for (auto& edge : vertices[x].edges) {
            // Jeśli znaleziono krawędź do y, przypisz jej nową wagę
            if (edge.target == y) edge.value = v;
        }
    }
};

// Główna funkcja programu
int main() {
    // Inicjalizacja grafu skierowanego (true oznacza że krawędzie mają kierunek)
    Graph g(true);

    // === ETAP 1: Dodanie wierzchołków do grafu ===
    g.addVertex(0);  // Dodanie wierzchołka o indeksie 0
    g.addVertex(1);  // Dodanie wierzchołka o indeksie 1
    g.addVertex(2);  // Dodanie wierzchołka o indeksie 2

    // === ETAP 2: Dodanie krawędzi między wierzchołkami ===
    g.addEdge(0, 1); // Dodanie krawędzi 0 -> 1 (z 0 do 1, bo graf jest skierowany)
    g.addEdge(0, 2); // Dodanie krawędzi 0 -> 2
    g.addEdge(1, 2); // Dodanie krawędzi 1 -> 2

    // === ETAP 3: Przypisanie wartości wierzchołkom i wagom krawędziom ===
    g.setVertexValue(0, 100);  // Przypisanie wartości 100 do wierzchołka 0
    g.setEdgeValue(0, 1, 50);  // Przypisanie wagi 50 do krawędzi 0->1

    // === ETAP 4: Testowanie funkcji adjacent() - sprawdzenie czy istnieje krawędź ===
    // Sprawdzenie czy 0->1 istnieje (powinno być true, bo dodaliśmy tę krawędź w ETAP 2)
    std::cout << "Czy 0 jest sąsiadem 1? " << (g.adjacent(0, 1) ? "Tak" : "Nie") << std::endl;
    // Sprawdzenie czy 2->0 istnieje (powinno być false, bo takiej krawędzi nie dodaliśmy)
    std::cout << "Czy 2 jest sąsiadem 0? " << (g.adjacent(2, 0) ? "Tak" : "Nie") << std::endl;

    // === ETAP 5: Wypisanie sąsiadów wierzchołka (funkcja printNeighbors) ===
    // Wypisze wierzchołki do których prowadzą krawędzie z wierzchołka 0 (czyli 1 i 2)
    g.printNeighbors(0);

    // === ETAP 6: Wypisanie wartości wierzchołków i wag krawędzi ===
    // Wypisanie wartości przypisanej do wierzchołka 0 (powinna być 100 z ETAP 3)
    std::cout << "Wartość wierzchołka 0: " << g.getVertexValue(0) << std::endl;
    // Wypisanie wagi przypisanej do krawędzi 0->1 (powinna być 50 z ETAP 3)
    std::cout << "Wartość krawędzi 0->1: " << g.getEdgeValue(0, 1) << std::endl;

    // === ETAP 7: Testowanie usuwania krawędzi ===
    std::cout << "\nUsuwanie krawędzi 0->2..." << std::endl;
    // Usunięcie krawędzi 0->2, wierzchołek 2 będzie już stracony z sąsiadów 0
    g.removeEdge(0, 2);
    // Wypisanie sąsiadów po usunięciu - powinna pozostać tylko krawędź 0->1
    g.printNeighbors(0);

    // === ETAP 8: Testowanie usuwania wierzchołka ===
    std::cout << "Usuwanie wierzchołka 1..." << std::endl;
    // Usunięcie wierzchołka 1 oraz wszystkich krawędzi do/z niego
    // (includes removal of edge 0->1 that was added in ETAP 2)
    g.removeVertex(1);
    // Wypisanie sąsiadów nach usunięcia wierzchołka - lista powinna być pusta
    g.printNeighbors(0);

    // Zwrócenie kodu wyjścia (0 oznacza sukces)
    return 0;
}