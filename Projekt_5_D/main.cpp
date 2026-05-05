#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

class TrafficGraph {
private:
    int n;
    std::vector<std::vector<int>> adjMatrix;
    std::vector<int> colors; // kolor/faza dla każdego wierzchołka

public:
    TrafficGraph(int size)
        : n(size), adjMatrix(size, std::vector<int>(size, 0)), colors(size, -1) {}

    // Dodajemy krawędź nieskierowaną (kolizję)
    void addCollision(int u, int v) {
        if (u >= 1 && u <= n && v >= 1 && v <= n) {
            adjMatrix[u - 1][v - 1] = 1;
            adjMatrix[v - 1][u - 1] = 1;
        }
    }

    // Zachłanne kolorowanie grafu
    void colorGraph() {
        colors.assign(n, -1); //ustawia wszystkie kolory na -1
        colors[0] = 0; // pierwszy wierzchołek dostaje pierwszy kolor

        //Przygotowanie tablicy dostępnych kolorów
        std::vector<bool> available(n, true);

        //Iteruje po każdym wierzchołku od 1 do n-1
        for (int u = 1; u < n; ++u) {
            //Resetowanie dostępnych kolorów
            std::fill(available.begin(), available.end(), true);

            //Przechodzi po wszystkich wierzchołkach v
            for (int v = 0; v < n; ++v) {
                //czy sa sasiadami i istnieja
                if (adjMatrix[u][v] == 1 && colors[v] != -1) {
                    //ten kolor nie jest dostepny
                    available[colors[v]] = false;
                }
            }
            //szukamy 1 dostepnego koloru
            int cr;
            for (cr = 0; cr < n; ++cr) {
                if (available[cr]) {
                    break;
                }
            }

            colors[u] = cr;
        }
    }

    int getNumberOfPhases() const {
        int maxColor = -1;
        //Pętla szuka maksymalnego koloru w tablicy colors
        for (int c : colors) {
            if (c > maxColor) {
                maxColor = c;
            }
        }
        //liczba faz = l. kolorow +1
        return maxColor + 1;
    }

    void printPhaseAssignments() const {
        std::cout << "Przypisanie trajektorii do faz:\n";
        for (int c = 0; c < getNumberOfPhases(); ++c) {
            std::cout << "Faza " << (c + 1) << ": ";
            for (int i = 0; i < n; ++i) {
                if (colors[i] == c) {
                    std::cout << "L" << (i + 1) << " ";
                }
            }
            std::cout << "\n";
        }
    }

    std::string colorName(int color) const {
        static const std::vector<std::string> names = {
            "lightblue", "lightgreen", "lightpink", "lightgoldenrod", "orange",
            "violet", "cyan", "gray", "yellow", "salmon"
        };
        if (color >= 0 && color < (int)names.size()) {
            return names[color];
        }
        return "white";
    }

    void generateDotFile(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) return;

        outFile << "graph TrafficLight {\n";
        outFile << "    node [shape=circle, style=filled];\n";
        outFile << "    label=\"Graf kolizji na skrzyzowaniu\";\n";

        // Wierzchołki z kolorami
        for (int i = 0; i < n; ++i) {
            outFile << "    L" << (i + 1)
                    << " [fillcolor=\"" << colorName(colors[i]) << "\"];\n";
        }

        // Krawędzie kolizji
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (adjMatrix[i][j] == 1) {
                    outFile << "    L" << (i + 1) << " -- L" << (j + 1) << ";\n";
                }
            }
        }

        outFile << "}\n";
        outFile.close();
        std::cout << "Wygenerowano plik: " << filename << std::endl;
    }
};

int main() {
    TrafficGraph g(6); // 6 pasów ruchu

    // Kolizje odczytane z rysunku skrzyżowania
    g.addCollision(1, 2);
    g.addCollision(1, 5);
    g.addCollision(1, 6);
    g.addCollision(2, 3);
    g.addCollision(2, 4);
    g.addCollision(2, 5);
    g.addCollision(3, 4);
    g.addCollision(3, 6);
    g.addCollision(4, 5);
    g.addCollision(5, 6);

    // Kolorowanie grafu = przypisanie faz
    g.colorGraph();

    // Wypisz wynik
    std::cout << "Liczba faz (zachlannie wyznaczona): "
              << g.getNumberOfPhases() << "\n\n";

    g.printPhaseAssignments();

    // Zapisz graf do pliku DOT
    g.generateDotFile("skrzyzowanie.dot");

    return 0;
}