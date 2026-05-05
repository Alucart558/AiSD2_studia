#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <string>

class Graph {
private:
    // Używamy mapy, aby obsłużyć wierzchołki o dowolnych nazwach (string)
    std::map<std::string, std::set<std::string>> adjList;
    /*
    To lista sąsiedztwa grafu:
    klucz std::string = nazwa wierzchołka,
    wartość std::set<std::string> = zbiór jego sąsiadów.
    Czyli np.:
    "A" -> {"B", "C"}
    "B" -> {"A", "C"}
    */

public:
    // Metoda dodająca krawędź skierowaną
    void addEdge(std::string u, std::string v) {
        //Dodaje v do zbioru sąsiadów wierzchołka u.
        adjList[u].insert(v);
        // Jeśli graf miałby być nieskierowany, dodalibyśmy: adjList[v].insert(u);
    }

    // Metoda generująca plik .dot
    void generateDotFile(const std::string& filename) {
        //Metoda zapisuje graf do pliku o nazwie filename.
        std::ofstream outFile(filename);

        if (!outFile.is_open()) {
            std::cerr << "Błąd: Nie można otworzyć pliku do zapisu!" << std::endl;
            return;
        }

        outFile << "digraph G {\n";
        outFile << "    node [shape=circle];\n";

        // Iteracja po wszystkich wierzchołkach w mapie
        for (auto const& [node, neighbors] : adjList) {
            // Jeśli wierzchołek nie ma sąsiadów, warto go i tak zdefiniować
            if (neighbors.empty()) {
                outFile << "    \"" << node << "\";\n";
            }

            //Przechodzi po wszystkich sąsiadach aktualnego wierzchołka.
            for (const std::string& neighbor : neighbors) {
                outFile << "    \"" << node << "\" -> \"" << neighbor << "\";\n";
            }
        }

        outFile << "}\n";
        outFile.close();

        std::cout << "Wygenerowano plik: " << filename << std::endl;
        std::cout << "Aby stworzyć JPG, uruchom: dot -Tjpg " << filename << " -o graph.jpg" << std::endl;
    }
};

int main() {
    Graph g;

    // Tworzymy graf z zadania: 3 wierzchołki, połączenia w obie strony
    g.addEdge("A", "A");
    g.addEdge("B", "A");
    g.addEdge("A", "B");
    g.addEdge("C", "A");
    g.addEdge("B", "C");
    g.addEdge("C", "C");

    // Generujemy plik wejściowy dla Graphviz
    g.generateDotFile("graf_cpp.dot");

    return 0;
}