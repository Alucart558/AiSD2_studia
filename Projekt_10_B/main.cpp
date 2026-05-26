#include <bits/stdc++.h>
using namespace std;

vector<vector<int>> read_matrix(const string &path) {
    ifstream in(path);
    if (!in) {
        throw runtime_error("Cannot open file: " + path);
    }

    vector<vector<int>> A;
    string line;
    while (getline(in, line)) {
        if (line.find_first_not_of(" \t\r\n") == string::npos) continue;
        stringstream ss(line);
        vector<int> row;
        int x;
        while (ss >> x) row.push_back(x);
        if (!row.empty()) A.push_back(row);
    }

    size_t n = A.size();
    if (n == 0) {
        throw runtime_error("Empty matrix");
    }
    for (size_t i = 0; i < n; ++i) {
        if (A[i].size() != n) {
            throw runtime_error("Matrix is not square");
        }
    }
    return A;
}

bool is_symmetric(const vector<vector<int>>& A) {
    size_t n = A.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (A[i][j] != A[j][i]) return false;
        }
    }
    return true;
}

vector<vector<int>> build_adj(const vector<vector<int>>& A) {
    size_t n = A.size();
    vector<vector<int>> adj(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            if (A[i][j] != 0) adj[i].push_back((int)j);
        }
    }
    return adj;
}

vector<vector<int>> transpose_graph(const vector<vector<int>>& adj) {
    size_t n = adj.size();
    vector<vector<int>> tr(n);
    for (size_t u = 0; u < n; ++u) {
        for (int v : adj[u]) {
            tr[v].push_back((int)u);
        }
    }
    return tr;
}

void dfs_simple(int u, const vector<vector<int>>& adj, vector<int>& vis) {
    vis[u] = 1;
    for (int v : adj[u]) {
        if (!vis[v]) dfs_simple(v, adj, vis);
    }
}

void dfs_order(int u, const vector<vector<int>>& adj, vector<int>& vis, vector<int>& order) {
    vis[u] = 1;
    for (int v : adj[u]) {
        if (!vis[v]) dfs_order(v, adj, vis, order);
    }
    order.push_back(u);
}

void dfs_component(int u, const vector<vector<int>>& tr, vector<int>& vis) {
    vis[u] = 1;
    for (int v : tr[u]) {
        if (!vis[v]) dfs_component(v, tr, vis);
    }
}

bool is_connected_undirected(const vector<vector<int>>& adj) {
    int n = (int)adj.size();
    vector<int> vis(n, 0);
    dfs_simple(0, adj, vis);
    for (int i = 0; i < n; ++i) {
        if (!vis[i]) return false;
    }
    return true;
}

bool is_strongly_connected_directed(const vector<vector<int>>& adj) {
    int n = (int)adj.size();

    // 1) Order vertices by finish time
    vector<int> vis(n, 0), order;
    for (int i = 0; i < n; ++i) {
        if (!vis[i]) dfs_order(i, adj, vis, order);
    }

    // 2) DFS on transposed graph in reverse order
    vector<vector<int>> tr = transpose_graph(adj);
    fill(vis.begin(), vis.end(), 0);

    int scc_count = 0;
    for (int i = n - 1; i >= 0; --i) {
        int v = order[i];
        if (!vis[v]) {
            ++scc_count;
            dfs_component(v, tr, vis);
            if (scc_count > 1) return false;
        }
    }
    return true;
}

void check_connectivity(const string& path) {
    try {
        auto A = read_matrix(path);
        bool undirected = is_symmetric(A);
        auto adj = build_adj(A);

        cout << "----------------------------------------\n";
        cout << "File: " << path << "\n";
        cout << "Detected graph type: "
             << (undirected ? "Undirected (symmetric matrix)" : "Directed (asymmetric matrix)")
             << "\n";

        bool connected = undirected
                         ? is_connected_undirected(adj)
                         : is_strongly_connected_directed(adj);

        if (undirected) {
            cout << "Connectivity check (connected): "
                 << (connected ? "Connected" : "Disconnected")
                 << "\n";
        } else {
            cout << "Connectivity check (strongly connected): "
                 << (connected ? "Strongly connected" : "Not strongly connected")
                 << "\n";
        }
    } catch (const exception& ex) {
        cout << "----------------------------------------\n";
        cout << "File: " << path << "\n";
        cout << "Error: " << ex.what() << "\n";
    }
}

int main(int argc, char** argv) {
    const string base = "C:\\Users\\dawib\\CLionProjects\\AiSD2_studia\\Projekt_10_B\\";
    vector<string> files;

    if (argc < 2) {
        // No args -> test all sample files
        files = { base + "h1", base + "h2", base + "h3" };
        cout << "No file argument provided. Checking default set: h1, h2, h3\n";
    } else {
        // Support shortcuts and explicit paths
        string arg = argv[1];
        if (arg == "all") {
            files = { base + "h1", base + "h2", base + "h3" };
        } else if (arg == "h1" || arg == "h2" || arg == "h3") {
            files = { base + arg };
        } else {
            for (int i = 1; i < argc; ++i) files.push_back(argv[i]);
        }
    }

    for (const string& p : files) {
        check_connectivity(p);
    }

    cout << "----------------------------------------\n";
    return 0;
}