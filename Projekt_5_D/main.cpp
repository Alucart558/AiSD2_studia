#include <iostream>
#include <vector>
#include <fstream>
#include <string>

class TrafficGraph {
private:
    int n;
    std::vector<std::vector<int>> adjMatrix;

public:
    TrafficGraph(int size) : n(size), adjMatrix(size, std::vector<int>(size, 0)) {}

    // Dodajemy krawędź nieskierowaną (kolizję)
    void addCollision(int u, int v) {
        if (u >= 1 && u <= n && v >= 1 && v <= n) {
            adjMatrix[u-1][v-1] = 1;
            adjMatrix[v-1][u-1] = 1;
        }
    }

    void generateDotFile(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) return;

        // Używamy 'graph' zamiast 'digraph' dla grafu nieskierowanego
        outFile << "graph TrafficLight {\n";
        outFile << "    node [shape=circle, style=filled, fillcolor=lightyellow];\n";
        outFile << "    label=\"Graf kolizji na skrzyzowaniu\";\n";

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

    // Definiujemy kolizje zgodnie z analizą rysunku:
    g.addCollision(1, 2); g.addCollision(1, 5); g.addCollision(1, 6);
    g.addCollision(2, 3); g.addCollision(2, 4); g.addCollision(2, 5);
    g.addCollision(3, 4); g.addCollision(3, 6);
    g.addCollision(4, 5);
    g.addCollision(5, 6);

    g.generateDotFile("skrzyzowanie.dot");

    return 0;
}