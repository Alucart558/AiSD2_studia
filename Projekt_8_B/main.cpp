#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <filesystem>

using namespace std;

static const long long INF = numeric_limits<long long>::max() / 4;

struct Edge {
    string a, b;
    long long w;
};

struct CityPos {
    string name;
    double lat = 0.0;
    double lon = 0.0;
};

static string sanitizeFilename(string s) {
    for (char &c : s) {
        if (c == ' ' || c == '/' || c == '\\' || c == ':' || c == '*' ||
            c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            c = '_';
        }
    }
    return s;
}

static string dotEscape(const string& s) {
    string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else out += c;
    }
    return out;
}

static vector<int> restorePath(int u, int v, const vector<vector<int>>& nextHop) {
    if (u < 0 || v < 0) return {};
    if (nextHop[u][v] == -1) return {};
    vector<int> path = {u};
    while (u != v) {
        u = nextHop[u][v];
        if (u == -1) return {};
        path.push_back(u);
        if (path.size() > nextHop.size() + 5) return {};
    }
    return path;
}

static void printTable(const vector<string>& cities,
                       const vector<vector<long long>>& dist) {
    vector<string> sortedCities = cities;
    sort(sortedCities.begin(), sortedCities.end());

    unordered_map<string, int> id;
    for (int i = 0; i < (int)cities.size(); ++i) id[cities[i]] = i;

    size_t nameWidth = 0;
    for (const auto& c : sortedCities) nameWidth = max(nameWidth, c.size());
    nameWidth = max<size_t>(nameWidth, 10);

    cout << left << setw(nameWidth) << "";
    for (const auto& c : sortedCities) {
        cout << " | " << setw(8) << c;
    }
    cout << "\n";

    cout << string((int)nameWidth + 3 + (int)sortedCities.size() * 11, '-') << "\n";

    for (const auto& rowCity : sortedCities) {
        int i = id[rowCity];
        cout << left << setw(nameWidth) << rowCity;
        for (const auto& colCity : sortedCities) {
            int j = id[colCity];
            cout << " | ";
            if (dist[i][j] == INF) cout << setw(8) << "INF";
            else cout << setw(8) << dist[i][j];
        }
        cout << "\n";
    }
}

static void generateDotForPair(const string& outName,
                               const vector<string>& cities,
                               const unordered_map<string, CityPos>& pos,
                               const vector<vector<long long>>& dist,
                               const vector<vector<int>>& nextHop,
                               int src, int dst,
                               const vector<Edge>& edges,
                               const unordered_map<string, int>& id) {
    ofstream out(outName);
    if (!out) {
        cerr << "Nie mozna zapisac pliku: " << outName << "\n";
        return;
    }

    vector<int> path = restorePath(src, dst, nextHop);

    map<pair<int,int>, bool> pathEdges;
    for (size_t i = 1; i < path.size(); ++i) {
        pathEdges[{path[i-1], path[i]}] = true;
        pathEdges[{path[i], path[i-1]}] = true; // dla nieskierowanego podświetlenia
    }

    out << "graph G {\n";
    out << "    layout=neato;\n";
    out << "    overlap=false;\n";
    out << "    splines=true;\n";
    out << "    node [shape=circle, style=filled, fillcolor=lightgray, fontsize=10];\n";

    // Wierzchołki z pozycjami
    for (const auto& city : cities) {
        auto it = pos.find(city);
        if (it != pos.end()) {
            double x = it->second.lon;
            double y = it->second.lat;
            out << "    \"" << dotEscape(city) << "\" [pos=\""
                << x << "," << y << "!\"];\n";
        } else {
            out << "    \"" << dotEscape(city) << "\";\n";
        }
    }

    // Krawędzie
    for (const auto& e : edges) {
        int u = id.at(e.a);
        int v = id.at(e.b);
        bool onPath = pathEdges.count({u, v}) > 0;

        out << "    \"" << dotEscape(e.a) << "\" -- \"" << dotEscape(e.b) << "\""
            << " [label=\"" << e.w << "\"";

        if (onPath) {
            out << ", color=red, penwidth=3";
        } else {
            out << ", color=gray70, penwidth=1";
        }
        out << "];\n";
    }

    out << "}\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const string timesFile = "czasy";
    const string positionsFile = "positions";

    ifstream inTimes(timesFile);
    if (!inTimes) {
        cerr << "Nie mozna otworzyc pliku: " << timesFile << "\n";
        return 1;
    }

    ifstream inPos(positionsFile);
    if (!inPos) {
        cerr << "Nie mozna otworzyc pliku: " << positionsFile << "\n";
        return 1;
    }

    vector<Edge> edges;
    unordered_map<string, int> id;
    vector<string> cities;

    // Wczytanie krawedzi
    string line;
    while (getline(inTimes, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string a, b;
        long long w;
        if (!(iss >> a >> b >> w)) continue;

        edges.push_back({a, b, w});

        if (!id.count(a)) {
            id[a] = (int)cities.size();
            cities.push_back(a);
        }
        if (!id.count(b)) {
            id[b] = (int)cities.size();
            cities.push_back(b);
        }
    }

    // Wczytanie pozycji
    unordered_map<string, CityPos> positions;
    while (getline(inPos, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string name;
        double lat, lon;
        if (!(iss >> name >> lat >> lon)) continue;
        positions[name] = {name, lat, lon};

        if (!id.count(name)) {
            id[name] = (int)cities.size();
            cities.push_back(name);
        }
    }

    int n = (int)cities.size();
    vector<vector<long long>> dist(n, vector<long long>(n, INF));
    vector<vector<int>> nextHop(n, vector<int>(n, -1));

    for (int i = 0; i < n; ++i) {
        dist[i][i] = 0;
        nextHop[i][i] = i;
    }

    // Graf nieskierowany
    for (const auto& e : edges) {
        int u = id[e.a];
        int v = id[e.b];
        if (e.w < dist[u][v]) {
            dist[u][v] = e.w;
            dist[v][u] = e.w;
            nextHop[u][v] = v;
            nextHop[v][u] = u;
        }
    }

    // Floyd–Warshall z odtwarzaniem ścieżki
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            if (dist[i][k] == INF) continue;
            for (int j = 0; j < n; ++j) {
                if (dist[k][j] == INF) continue;
                long long through = dist[i][k] + dist[k][j];
                if (through < dist[i][j]) {
                    dist[i][j] = through;
                    nextHop[i][j] = nextHop[i][k];
                }
            }
        }
    }

    // Tabela wynikowa
    cout << "Najkrotsze czasy przejazdu (Floyd–Warshall):\n\n";
    printTable(cities, dist);
    cout << "\n";

    // Folder na pliki DOT
    filesystem::create_directory("dot_output");

    // Generowanie DOT dla każdej pary miast
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (dist[i][j] == INF) continue;

            string fileName = "dot_output/" +
                sanitizeFilename(cities[i]) + "__" +
                sanitizeFilename(cities[j]) + ".dot";

            generateDotForPair(fileName, cities, positions, dist, nextHop, i, j, edges, id);
        }
    }

    cout << "Wygenerowano pliki DOT w folderze dot_output/\n";
    cout << "Kazdy plik odpowiada jednej parze miast i zawiera zaznaczona najkrotsza trase.\n";

    return 0;
}