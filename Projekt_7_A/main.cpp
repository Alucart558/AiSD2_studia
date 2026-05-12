#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <string>
#include <iomanip>

using namespace std;

struct Edge {
    int to;
    int weight;
};

const int INF = numeric_limits<int>::max() / 4;

vector<int> dijkstra(const vector<vector<Edge>>& graph, int start) {
    int n = graph.size();
    vector<int> dist(n, INF);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, v] = pq.top();
        pq.pop();

        if (d != dist[v]) continue;

        for (const auto& e : graph[v]) {
            if (dist[e.to] > dist[v] + e.weight) {
                dist[e.to] = dist[v] + e.weight;
                pq.push({dist[e.to], e.to});
            }
        }
    }

    return dist;
}

int main() {
    // Wierzchołki:
    // 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, 7=H, 8=I, 9=J
    vector<string> names = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
    vector<vector<Edge>> graph(10);

    auto addUndirected = [&](int u, int v, int w) {
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    };

    // Wszystkie krawędzie dwukierunkowe
    addUndirected(0, 1, 3);  // A <-> B
    addUndirected(1, 2, 2);  // B <-> C
    addUndirected(1, 3, 1);  // B <-> D

    addUndirected(2, 7, 2);  // C <-> H
    addUndirected(2, 8, 2);  // C <-> I

    addUndirected(3, 4, 1);  // D <-> E
    addUndirected(4, 5, 1);  // E <-> F

    addUndirected(5, 7, 2);  // F <-> H
    addUndirected(5, 6, 1);  // F <-> G

    addUndirected(6, 7, 1);  // G <-> H

    addUndirected(7, 9, 1);  // H <-> J
    addUndirected(7, 8, 2);  // H <-> I

    int startA = 0;
    int startJ = 9;

    vector<int> distToA = dijkstra(graph, startA);
    vector<int> distToJ = dijkstra(graph, startJ);

    cout << "Najkrotszy czas do A:\n";
    for (int i = 0; i < (int)names.size(); ++i) {
        cout << setw(2) << names[i] << " -> A = ";
        if (distToA[i] >= INF) cout << "brak drogi";
        else cout << distToA[i];
        cout << '\n';
    }

    cout << "\nNajkrotszy czas do J:\n";
    for (int i = 0; i < (int)names.size(); ++i) {
        cout << setw(2) << names[i] << " -> J = ";
        if (distToJ[i] >= INF) cout << "brak drogi";
        else cout << distToJ[i];
        cout << '\n';
    }

    return 0;
}