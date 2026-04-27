#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>

// Struktury pomocnicze do przechowywania wag/wartości
struct Edge {
    int target;
    int value; // Wartość skojarzona z krawędzią (np. waga)
};

struct Vertex {
    int value; // Wartość skojarzona z wierzchołkiem
    std::list<Edge> edges;
    bool exists = false;
};

class Graph {
private:
    std::vector<Vertex> vertices;
    bool isDirected;

public:
    Graph(bool directed = true) : isDirected(directed) {}

    // 1. adjacent(G, x, y)
    bool adjacent(int x, int y) {
        if (x >= vertices.size() || !vertices[x].exists) return false;
        for (auto const& edge : vertices[x].edges) {
            if (edge.target == y) return true;
        }
        return false;
    }

    // 2. neighbors(G, x)
    void printNeighbors(int x) {
        if (x < vertices.size() && vertices[x].exists) {
            std::cout << "Sąsiedzi wierzchołka " << x << ": ";
            for (auto const& edge : vertices[x].edges) {
                std::cout << edge.target << " ";
            }
            std::cout << std::endl;
        }
    }

    // 3. addVertex(G, x)
    void addVertex(int x) {
        if (x >= vertices.size()) vertices.resize(x + 1);
        vertices[x].exists = true;
    }

    // 4. removeVertex(G, x)
    void removeVertex(int x) {
        if (x < vertices.size()) {
            vertices[x].exists = false;
            vertices[x].edges.clear();
            for (auto& v : vertices) {
                v.edges.remove_if([x](const Edge& e) { return e.target == x; });
            }
        }
    }

    // 5. addEdge(G, x, y)
    void addEdge(int x, int y) {
        if (adjacent(x, y)) return;
        vertices[x].edges.push_back({y, 0});
        if (!isDirected) {
            vertices[y].edges.push_back({x, 0});
        }
    }

    // 6. removeEdge(G, x, y)
    void removeEdge(int x, int y) {
        if (x < vertices.size()) {
            vertices[x].edges.remove_if([y](const Edge& e) { return e.target == y; });
            if (!isDirected && y < vertices.size()) {
                vertices[y].edges.remove_if([x](const Edge& e) { return e.target == x; });
            }
        }
    }

    // 7. getVertexValue / 8. setVertexValue
    int getVertexValue(int x) { return vertices[x].value; }
    void setVertexValue(int x, int v) { vertices[x].value = v; }

    // 9. getEdgeValue / 10. setEdgeValue
    int getEdgeValue(int x, int y) {
        for (auto& edge : vertices[x].edges) {
            if (edge.target == y) return edge.value;
        }
        return -1;
    }
    void setEdgeValue(int x, int y, int v) {
        for (auto& edge : vertices[x].edges) {
            if (edge.target == y) edge.value = v;
        }
    }
};

int main() {
    // Inicjalizacja grafu skierowanego
    Graph g(true);

    // Test addVertex i addEdge
    g.addVertex(0);
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);

    // Test setVertexValue i setEdgeValue
    g.setVertexValue(0, 100); // Wierzchołek 0 ma wartość 100
    g.setEdgeValue(0, 1, 50);  // Krawędź 0->1 ma wagę 50

    // Test adjacent
    std::cout << "Czy 0 jest sąsiadem 1? " << (g.adjacent(0, 1) ? "Tak" : "Nie") << std::endl;
    std::cout << "Czy 2 jest sąsiadem 0? " << (g.adjacent(2, 0) ? "Tak" : "Nie") << std::endl;

    // Test neighbors
    g.printNeighbors(0);

    // Test wartości
    std::cout << "Wartość wierzchołka 0: " << g.getVertexValue(0) << std::endl;
    std::cout << "Wartość krawędzi 0->1: " << g.getEdgeValue(0, 1) << std::endl;

    // Test removeEdge i removeVertex
    std::cout << "\nUsuwanie krawędzi 0->2..." << std::endl;
    g.removeEdge(0, 2);
    g.printNeighbors(0);

    std::cout << "Usuwanie wierzchołka 1..." << std::endl;
    g.removeVertex(1);
    g.printNeighbors(0);

    return 0;
}