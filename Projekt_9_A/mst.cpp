// mst.cpp
// Uniwersalna wersja: wykrywa czy wejście ma formę numericzną (u v w)
// czy nazwową (stringA stringB w). Wypisuje MST Primem i Kruskalem.
// Jeśli uruchamiasz bez argumentów, spróbuje znaleźć plik 'czasy' w kilku lokalizacjach.
//
// Kompilacja:
//   g++ -std=c++17 -O2 -pipe mst.cpp -o mst.exe
//
// Uruchomienie (CLion: Run bez parametrów): program sam znajdzie plik 'czasy'.
// Możesz też podać:
//   mst.exe <plik> [num_nodes] [start_node_or_name] [--dot=plik.dot]
//
// Przykłady:
//   mst.exe czasy
//   mst.exe czasy 128
//   mst.exe czasy 128 Warszawa
//   mst.exe czasy --dot=mst.dot

#include <bits/stdc++.h>
using namespace std;
using ll = long long;

struct Edge { int u, v; ll w; };
struct RawEdge { string a, b; ll w; };

struct DSU {
    vector<int> p, r;
    DSU(int n=0): p(n), r(n,0) { for(int i=0;i<n;++i) p[i]=i; }
    int find(int a){ return p[a]==a ? a : p[a]=find(p[a]); }
    bool unite(int a,int b){
        a=find(a); b=find(b);
        if(a==b) return false;
        if(r[a]<r[b]) swap(a,b);
        p[b]=a;
        if(r[a]==r[b]) r[a]++;
        return true;
    }
};

static inline long long now_us() {
    using namespace std::chrono;
    return chrono::duration_cast<std::chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count();
}

bool isIntegerToken(const string& s) {
    if(s.empty()) return false;
    size_t i = 0;
    if (s[0] == '+' || s[0] == '-') i = 1;
    for(; i < s.size(); ++i) if(!isdigit((unsigned char)s[i])) return false;
    return true;
}

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string filename;
    int hintN = 0;
    string startArg;
    string dotOut;

    // parse simple args:
    // argv[1] = filename (optional)
    // argv[2] = num_nodes (optional)
    // argv[3] = start (optional)
    // any arg like --dot=NAME sets dotOut
    for (int i = 1; i < argc; ++i) {
        string s = argv[i];
        if (s.rfind("--dot=", 0) == 0) {
            dotOut = s.substr(strlen("--dot="));
        } else if (s == "--dot" && i+1 < argc) {
            dotOut = argv[++i];
        } else if (filename.empty()) {
            filename = s;
        } else if (hintN == 0 && isIntegerToken(s)) {
            // could be num_nodes or start numeric; treat as num_nodes if second arg position
            try { hintN = stoi(s); } catch(...) { hintN = 0; }
        } else if (startArg.empty()) {
            startArg = s;
        }
    }

    if (argc >= 3 && filename.empty() && isIntegerToken(argv[2])) {
        // fallback (rare)
        try { hintN = stoi(argv[2]); } catch(...) { hintN = 0; }
    }

    if (filename.empty()) {
        vector<string> candidates = {
            "czasy",
            "Projekt_8_A/czasy",
            "Projekt_8_B/czasy",
            "../Projekt_8_A/czasy",
            "../Projekt_8_B/czasy",
            "C:\\Users\\dawib\\CLionProjects\\AiSD2_studia\\Projekt_8_A\\czasy",
            "C:\\Users\\dawib\\CLionProjects\\AiSD2_studia\\Projekt_8_B\\czasy"
        };
        for (const auto &c : candidates) {
            ifstream test(c);
            if (test) { filename = c; break; }
        }
        if (filename.empty()) {
            cerr << "Brak argumentu i nie znaleziono pliku domyślnego 'czasy'.\n";
            cerr << "Podaj plik w Run Configuration lub skopiuj 'czasy' obok exe.\n";
            return 1;
        } else {
            cout << "Używam domyślnego pliku: " << filename << "\n";
        }
    }

    ifstream in(filename);
    if (!in) {
        cerr << "Nie można otworzyć pliku: " << filename << "\n";
        return 1;
    }

    // Czytamy wszystkie linie; spróbujemy rozpoznać format na podstawie pierwszej poprawnej linii.
    vector<RawEdge> raw;
    string line;
    bool formatDetermined = false;
    bool numericMode = false; // true = tokens are integers (node ids), false = tokens are names
    while(getline(in, line)) {
        if(line.empty()) continue;
        istringstream iss(line);
        string t1, t2, t3;
        if (!(iss >> t1 >> t2 >> t3)) continue; // skip bad lines
        // try parse weight
        bool weightOk = false;
        ll w = 0;
        try {
            size_t idx = 0;
            w = stoll(t3, &idx);
            if (idx == t3.size()) weightOk = true;
        } catch(...) { weightOk = false; }
        if (!weightOk) continue;

        if (!formatDetermined) {
            if (isIntegerToken(t1) && isIntegerToken(t2)) numericMode = true;
            else numericMode = false;
            formatDetermined = true;
        }
        raw.push_back({t1, t2, w});
    }
    in.close();

    if (!formatDetermined || raw.empty()) {
        cerr << "No nodes found in input (brak poprawnych linii) albo format nie rozpoznany.\n";
        cerr << "Upewnij się, że każda linia ma format: <u> <v> <w> gdzie u/v są numerami lub nazwami bez spacji.\n";
        return 1;
    }

    // Mapowanie i budowa edge list
    vector<Edge> edges;
    vector<string> nameById;
    unordered_map<string,int> id;
    int maxNodeIndex = 0;

    if (numericMode) {
        for (auto &re : raw) {
            long long uu=0, vv=0;
            try {
                uu = stoll(re.a);
                vv = stoll(re.b);
            } catch(...) { continue; }
            int u = (int)uu;
            int v = (int)vv;
            maxNodeIndex = max({maxNodeIndex, u, v});
            edges.push_back({u-1, v-1, re.w}); // 0-based
        }
        int n = maxNodeIndex;
        if (hintN > 0) n = max(n, hintN);
        if (n <= 0) { cerr << "No nodes found in numeric input.\n"; return 1; }

        // Determine start if provided
        int start = 0;
        if (!startArg.empty()) {
            if (isIntegerToken(startArg)) {
                try {
                    int s = stoi(startArg);
                    if (s >= 1 && s <= n) start = s-1;
                    else cerr << "Start index spoza zakresu, używam domyślnego 0\n";
                } catch(...) {}
            } else {
                cerr << "Start argument nie jest numerem; ignoruję dla trybu numeric\n";
            }
        }

        // adjacency
        vector<vector<pair<int,ll>>> adj(n);
        for (const auto &e : edges) {
            if (e.u < 0 || e.u >= n || e.v < 0 || e.v >= n) continue;
            adj[e.u].push_back({e.v, e.w});
            adj[e.v].push_back({e.u, e.w});
        }

        // PRIM
        long long t0 = now_us();
        vector<char> used(n, 0);
        vector<ll> minE(n, LLONG_MAX);
        vector<int> selE(n, -1);
        minE[start] = 0;
        using T = tuple<ll,int,int>;
        priority_queue<T, vector<T>, greater<T>> pq;
        pq.push({0, start, -1});
        vector<Edge> primEdges;
        ll primTotal = 0;
        while(!pq.empty()){
            auto [w, v, parent] = pq.top(); pq.pop();
            if (used[v]) continue;
            used[v] = 1;
            if (parent != -1){
                primEdges.push_back({parent, v, w});
                primTotal += w;
            }
            for(auto &pr : adj[v]){
                int to = pr.first; ll wt = pr.second;
                if(!used[to] && wt < minE[to]){
                    minE[to] = wt;
                    selE[to] = v;
                    pq.push({wt, to, v});
                }
            }
        }
        bool primConnected = (int)primEdges.size() == n-1;
        long long t1 = now_us();

        // KRUSKAL
        long long t2 = now_us();
        auto sortedEdges = edges;
        sort(sortedEdges.begin(), sortedEdges.end(), [](const Edge &a, const Edge &b){
            if (a.w != b.w) return a.w < b.w;
            if (a.u != b.u) return a.u < b.u;
            return a.v < b.v;
        });
        DSU dsu(n);
        vector<Edge> kruskalEdges;
        ll kruskalTotal = 0;
        for (auto &e : sortedEdges) {
            if (e.u < 0 || e.u >= n || e.v < 0 || e.v >= n) continue;
            if (dsu.unite(e.u, e.v)) {
                kruskalEdges.push_back(e);
                kruskalTotal += e.w;
                if ((int)kruskalEdges.size() == n-1) break;
            }
        }
        bool kruskalConnected = (int)kruskalEdges.size() == n-1;
        long long t3 = now_us();

        // PRINT (numeric)
        cout << "Format: numeric (węzły podane jako liczby 1..N)\n";
        cout << "Nodes (n) = " << n << ", Edges (m) = " << edges.size() << "\n\n";

        cout << "Prim's algorithm (start=" << (start+1) << "):\n";
        if (!primConnected) cout << "  Graf może być niespójny (Prim znalazł " << primEdges.size() << " krawędzi).\n";
        else {
            cout << "  MST edges (" << primEdges.size() << "):\n";
            for (auto &e : primEdges) cout << "    " << (e.u+1) << " - " << (e.v+1) << " : " << e.w << "\n";
            cout << "  Total weight: " << primTotal << "\n";
        }
        cout << "  Time: " << (t1 - t0) << " us\n\n";

        cout << "Kruskal's algorithm:\n";
        if (!kruskalConnected) cout << "  Graf może być niespójny (Kruskal znalazł " << kruskalEdges.size() << " krawędzi).\n";
        else {
            cout << "  MST edges (" << kruskalEdges.size() << "):\n";
            for (auto &e : kruskalEdges) cout << "    " << (e.u+1) << " - " << (e.v+1) << " : " << e.w << "\n";
            cout << "  Total weight: " << kruskalTotal << "\n";
        }
        cout << "  Time: " << (t3 - t2) << " us\n\n";

        if (primConnected && kruskalConnected) {
            if (primTotal == kruskalTotal) cout << "Oba algorytmy dają równą sumę wag: " << primTotal << "\n";
            else cout << "Uwaga: różne sumy wag (Prim=" << primTotal << ", Kruskal=" << kruskalTotal << ")\n";
        } else cout << "MST nie znalezione w pełnym zakresie (graf niespójny?)\n";

        // DOT export (optional)
        if (!dotOut.empty() && kruskalConnected) {
            ofstream dot(dotOut);
            if (dot) {
                dot << "graph MST {\n";
                for (auto &e : kruskalEdges) {
                    dot << "  \"" << (e.u+1) << "\" -- \"" << (e.v+1) << "\" [label=\"" << e.w << "\"];\n";
                }
                dot << "}\n";
                cout << "Zapisano MST (Kruskal) do: " << dotOut << "\n";
            } else cout << "Nie można zapisać pliku DOT: " << dotOut << "\n";
        }

        return 0;
    } // koniec numericMode

    // ---------------- string (names without spaces) mode ----------------
    for (auto &re : raw) {
        if (!id.count(re.a)) { id[re.a] = (int)nameById.size(); nameById.push_back(re.a); }
        if (!id.count(re.b)) { id[re.b] = (int)nameById.size(); nameById.push_back(re.b); }
        int u = id[re.a];
        int v = id[re.b];
        edges.push_back({u, v, re.w});
    }

    int n = (int)nameById.size();
    if (hintN > 0) n = max(n, hintN);
    if (n <= 0) { cerr << "No nodes found in name input.\n"; return 1; }

    // Determine start if provided (name or numeric)
    int start = 0;
    if (!startArg.empty()) {
        if (isIntegerToken(startArg)) {
            try {
                int s = stoi(startArg);
                if (s >= 1 && s <= n) start = s-1;
                else cerr << "Start index spoza zakresu, używam domyślnego 0\n";
            } catch(...) {}
        } else {
            if (id.count(startArg)) start = id[startArg];
            else {
                // maybe underscores vs spaces: try replace '_' with ' ' and search
                string alt = startArg;
                for (auto &c : alt) if (c == '_') c = ' ';
                if (id.count(alt)) start = id[alt];
                else cerr << "Start name nie znaleziony, używam domyślnego 0\n";
            }
        }
    }

    vector<vector<pair<int,ll>>> adj(n);
    for (auto &e : edges) {
        if (e.u < 0 || e.u >= n || e.v < 0 || e.v >= n) continue;
        adj[e.u].push_back({e.v, e.w});
        adj[e.v].push_back({e.u, e.w});
    }

    // PRIM
    long long t0 = now_us();
    vector<char> used(n, 0);
    vector<ll> minE(n, LLONG_MAX);
    vector<int> parent(n, -1);
    using T = tuple<ll,int,int>;
    priority_queue<T, vector<T>, greater<T>> pq;
    minE[start] = 0;
    pq.push({0, start, -1});
    vector<Edge> primEdges;
    ll primTotal = 0;
    while (!pq.empty()) {
        auto [cw, v, p] = pq.top(); pq.pop();
        if (used[v]) continue;
        used[v] = 1;
        if (p != -1) {
            primEdges.push_back({p, v, cw});
            primTotal += cw;
        }
        for (auto &pr : adj[v]) {
            int to = pr.first; ll wt = pr.second;
            if (!used[to] && wt < minE[to]) {
                minE[to] = wt;
                pq.push({wt, to, v});
                parent[to] = v;
            }
        }
    }
    bool primConnected = (int)primEdges.size() == n-1;
    long long t1 = now_us();

    // KRUSKAL
    long long t2 = now_us();
    auto sortedEdges = edges;
    sort(sortedEdges.begin(), sortedEdges.end(), [](const Edge &x, const Edge &y){
        if (x.w != y.w) return x.w < y.w;
        if (x.u != y.u) return x.u < y.u;
        return x.v < y.v;
    });
    DSU dsu(n);
    vector<Edge> kruskalEdges;
    ll kruskalTotal = 0;
    for (auto &e : sortedEdges) {
        if (dsu.unite(e.u, e.v)) {
            kruskalEdges.push_back(e);
            kruskalTotal += e.w;
            if ((int)kruskalEdges.size() == n-1) break;
        }
    }
    bool kruskalConnected = (int)kruskalEdges.size() == n-1;
    long long t3 = now_us();

    // PRINT (names)
    cout << "Format: string (węzły podane jako nazwy, bez spacji)\n";
    cout << "Wczytano " << n << " węzłów i " << edges.size() << " krawędzi.\n\n";

    cout << "Prim's algorithm (start=\"" << nameById[start] << "\"):\n";
    if (!primConnected) cout << "  Graf może być niespójny; Prim znalazł " << primEdges.size() << " krawędzi.\n";
    else {
        cout << "  Krawędzie MST (" << primEdges.size() << "):\n";
        for (auto &e : primEdges) cout << "    " << nameById[e.u] << " - " << nameById[e.v] << " : " << e.w << "\n";
        cout << "  Suma wag: " << primTotal << "\n";
    }
    cout << "  Czas: " << (t1 - t0) << " us\n\n";

    cout << "Kruskal's algorithm:\n";
    if (!kruskalConnected) cout << "  Graf może być niespójny; Kruskal znalazł " << kruskalEdges.size() << " krawędzi.\n";
    else {
        cout << "  Krawędzie MST (" << kruskalEdges.size() << "):\n";
        for (auto &e : kruskalEdges) cout << "    " << nameById[e.u] << " - " << nameById[e.v] << " : " << e.w << "\n";
        cout << "  Suma wag: " << kruskalTotal << "\n";
    }
    cout << "  Czas: " << (t3 - t2) << " us\n\n";

    if (primConnected && kruskalConnected) {
        if (primTotal == kruskalTotal) cout << "Oba algorytmy dają równą sumę wag: " << primTotal << "\n";
        else cout << "Uwaga: różne sumy wag (Prim=" << primTotal << ", Kruskal=" << kruskalTotal << ")\n";
    } else cout << "MST nie znalezione w pełnym zakresie (graf niespójny?)\n";

    // DOT export (optional)
    if (!dotOut.empty() && kruskalConnected) {
        ofstream dot(dotOut);
        if (dot) {
            dot << "graph MST {\n";
            for (auto &e : kruskalEdges) {
                string a = nameById[e.u];
                string b = nameById[e.v];
                dot << "  \"" << a << "\" -- \"" << b << "\" [label=\"" << e.w << "\"];\n";
            }
            dot << "}\n";
            cout << "Zapisano MST (Kruskal) do: " << dotOut << "\n";
        } else cout << "Nie można zapisać pliku DOT: " << dotOut << "\n";
    }

    cout << "\nUwaga: parser zakłada, że nazwy węzłów nie zawierają spacji.\n";
    cout << "Jeśli nazwy mają spacje, wklej pierwsze 3 linie pliku 'czasy' a dopasuję parser.\n";

    return 0;
}