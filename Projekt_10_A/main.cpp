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
    // validate square
    size_t n = A.size();
    for (size_t i = 0; i < n; ++i) {
        if (A[i].size() != n) {
            throw runtime_error("Matrix is not square or rows have unequal lengths");
        }
    }
    return A;
}

bool is_symmetric(const vector<vector<int>>& A) {
    size_t n = A.size();
    for (size_t i = 0; i < n; ++i)
        for (size_t j = i+1; j < n; ++j)
            if (A[i][j] != A[j][i]) return false;
    return true;
}

// Directed cycle detection using DFS colors
bool dfs_directed(int u, const vector<vector<int>>& adj, vector<int>& color) {
    color[u] = 1; // gray
    for (int v : adj[u]) {
        if (color[v] == 1) return true; // back edge -> cycle
        if (color[v] == 0 && dfs_directed(v, adj, color)) return true;
    }
    color[u] = 2; // black
    return false;
}

// Undirected cycle detection using DFS and parent check
bool dfs_undirected(int u, int parent, const vector<vector<int>>& adj, vector<int>& visited) {
    visited[u] = 1;
    for (int v : adj[u]) {
        if (!visited[v]) {
            if (dfs_undirected(v, u, adj, visited)) return true;
        } else if (v != parent) {
            return true;
        }
    }
    return false;
}

void check_and_print(const string &path) {
    try {
        auto A = read_matrix(path);
        size_t n = A.size();
        bool symmetric = is_symmetric(A);
        cout << "----------------------------------------\n";
        cout << "File: " << path << "\n";
        cout << "Detected graph type: " << (symmetric ? "Undirected (symmetric matrix)" : "Directed (asymmetric matrix)") << "\n";

        // Build adjacency lists
        vector<vector<int>> adj(n);
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                if (A[i][j]) adj[i].push_back((int)j);

        bool cyclic = false;
        if (!symmetric) {
            vector<int> color(n, 0);
            for (size_t i = 0; i < n; ++i) {
                if (color[i] == 0) {
                    if (dfs_directed((int)i, adj, color)) {
                        cyclic = true;
                        break;
                    }
                }
            }
        } else {
            vector<int> visited(n, 0);
            for (size_t i = 0; i < n; ++i) {
                if (!visited[i]) {
                    if (dfs_undirected((int)i, -1, adj, visited)) {
                        cyclic = true;
                        break;
                    }
                }
            }
        }

        cout << "Cycle check: " << (cyclic ? "Cyclic" : "Acyclic") << "\n";
    } catch (const exception &ex) {
        cerr << "Error processing " << path << ": " << ex.what() << "\n";
    }
}

int main(int argc, char** argv) {
    // Base path to Projekt_10_A (update here if your workspace path is different)
    const string base = "C:\\Users\\dawib\\CLionProjects\\AiSD2_studia\\Projekt_10_A\\";
    vector<string> filesToCheck;

    if (argc < 2) {
        // No argument -> check all three files
        filesToCheck = { base + "g1", base + "g2", base + "g3" };
        cout << "No matrix file provided — checking default set: g1, g2, g3\n";
    } else {
        // Support: "all", "g1"/"g2"/"g3", or explicit path(s)
        string arg = argv[1];
        if (arg == "all") {
            filesToCheck = { base + "g1", base + "g2", base + "g3" };
        } else if (arg == "g1" || arg == "g2" || arg == "g3") {
            filesToCheck = { base + arg };
        } else {
            // allow multiple arguments: treat each argv[i] as file path
            for (int i = 1; i < argc; ++i) filesToCheck.push_back(argv[i]);
        }
    }

    for (const auto &p : filesToCheck) check_and_print(p);

    cout << "----------------------------------------\n";
    return 0;
}