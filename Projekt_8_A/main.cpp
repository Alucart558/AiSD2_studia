#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

static const long long INF = numeric_limits<long long>::max() / 4;

struct Edge {
    string a, b;
    long long w;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string filename = "czasy";
    ifstream in(filename);
    if (!in) {
        cerr << "Nie mozna otworzyc pliku: " << filename << "\n";
        return 1;
    }

    vector<Edge> edges;
    unordered_map<string, int> id;
    vector<string> cities;

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;

        istringstream iss(line);
        string city1, city2;
        long long time;

        if (!(iss >> city1 >> city2 >> time)) {
            continue; // pomijamy niepoprawne linie
        }

        edges.push_back({city1, city2, time});

        if (!id.count(city1)) {
            id[city1] = (int)cities.size();
            cities.push_back(city1);
        }
        if (!id.count(city2)) {
            id[city2] = (int)cities.size();
            cities.push_back(city2);
        }
    }

    int n = (int)cities.size();
    vector<vector<long long>> dist(n, vector<long long>(n, INF));

    for (int i = 0; i < n; ++i) dist[i][i] = 0;

    // graf nieskierowany
    for (const auto& e : edges) {
        int u = id[e.a];
        int v = id[e.b];
        dist[u][v] = min(dist[u][v], e.w);
        dist[v][u] = min(dist[v][u], e.w);
    }

    // Floyd-Warshall
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            if (dist[i][k] == INF) continue;
            for (int j = 0; j < n; ++j) {
                if (dist[k][j] == INF) continue;
                long long through = dist[i][k] + dist[k][j];
                if (through < dist[i][j]) {
                    dist[i][j] = through;
                }
            }
        }
    }

    // sortowanie nazw miast dla czytelnej tabeli
    vector<string> sortedCities = cities;
    sort(sortedCities.begin(), sortedCities.end());

    vector<int> perm;
    perm.reserve(n);
    for (const auto& c : sortedCities) perm.push_back(id[c]);

    // szerokość kolumny
    size_t nameWidth = 0;
    for (const auto& c : sortedCities) nameWidth = max(nameWidth, c.size());
    nameWidth = max<size_t>(nameWidth, 10);

    cout << left << setw(nameWidth) << "";
    for (const auto& c : sortedCities) {
        cout << " | " << setw(8) << c;
    }
    cout << "\n";

    cout << string((int)nameWidth + 3 + n * 11, '-') << "\n";

    for (size_t ii = 0; ii < sortedCities.size(); ++ii) {
        int i = perm[ii];
        cout << left << setw(nameWidth) << sortedCities[ii];

        for (size_t jj = 0; jj < sortedCities.size(); ++jj) {
            int j = perm[jj];
            cout << " | ";
            if (dist[i][j] == INF) {
                cout << setw(8) << "INF";
            } else {
                cout << setw(8) << dist[i][j];
            }
        }
        cout << "\n";
    }

    return 0;
}