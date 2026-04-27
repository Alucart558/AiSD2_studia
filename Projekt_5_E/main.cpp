#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <string>
#include <algorithm>

class TrafficColoring {
private:
    int n;
    std::vector<std::vector<int>> adj; // Lista sąsiedztwa
    std::vector<int> colors;           // Wynik kolorowania

public:
    TrafficColoring(int size) : n(size), adj(size), colors(size, -1) {}

    void addCollision(int u, int v) {
        // Graf nieskierowany: kolizja u-v to to samo co v-u
        adj[u-1].push_back(v-1);
        adj[v-1].push_back(u-1);
    }

    void solveColoring() {
        // Przypisz pierwszy kolor do pierwszego wierzchołka
        colors[0] = 0;

        // Tablica pomocnicza do sprawdzania dostępności kolorów
        // available[cr] == true oznacza, że kolor cr jest zajęty przez sąsiada
        std::vector<bool> available(n, false);

        for (int u = 1; u < n; u++) {
            // Oznacz kolory używane przez sąsiadów jako niedostępne
            for (int neighbor : adj[u]) {
                if (colors[neighbor] != -1) {
                    available[colors[neighbor]] = true;
                }
            }

            // Znajdź pierwszy wolny kolor
            int cr;
            for (cr = 0; cr < n; cr++) {
                if (!available[cr]) break;
            }

            colors[u] = cr; // Przypisz kolor

            // Resetuj tablicę dla następnego wierzchołka
            for (int neighbor : adj[u]) {
                if (colors[neighbor] != -1) {
                    available[colors[neighbor]] = false;
                }
            }
        }
    }

    void printPhases() {
        std::map<int, std::vector<int>> phases;
        for (int i = 0; i < n; i++) {
            phases[colors[i]].push_back(i + 1);
        }

        std::cout << "Minimalna liczba faz (kolorow): " << phases.size() << std::endl;
        for (auto const& [phaseId, lanes] : phases) {
            std::cout << "Faza " << phaseId + 1 << ": Pasy ";
            for (int lane : lanes) std::cout << "L" << lane << " ";
            std::cout << std::endl;
        }
    }

    void generateColoredDot(const std::string& filename) {
        std::ofstream outFile(filename);
        std::string colorPalette[] = {"green", "lightblue", "orange", "pink", "yellow"};

        outFile << "graph G {\n";
        outFile << "    node [style=filled];\n";

        for (int i = 0; i < n; i++) {
            outFile << "    L" << (i + 1) << " [fillcolor=" << colorPalette[colors[i] % 5] << "];\n";
        }

        for (int u = 0; u < n; u++) {
            for (int v : adj[u]) {
                if (u < v) { // Rysuj krawędź tylko raz
                    outFile << "    L" << (u + 1) << " -- L" << (v + 1) << ";\n";
                }
            }
        }
        outFile << "}\n";
        outFile.close();
    }
};

int main() {
    TrafficColoring tc(6);

    // Relacje kolizji z zadania D:
    tc.addCollision(1, 2); tc.addCollision(1, 5); tc.addCollision(1, 6);
    tc.addCollision(2, 3); tc.addCollision(2, 4); tc.addCollision(2, 5);
    tc.addCollision(3, 4); tc.addCollision(3, 6);
    tc.addCollision(4, 5);
    tc.addCollision(5, 6);

    tc.solveColoring();
    tc.printPhases();
    tc.generateColoredDot("skrzyzowanie_pokolorowane.dot");

    return 0;
}