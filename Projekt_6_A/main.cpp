#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <algorithm>

const int N = 8; // Rozmiar szachownicy

// Ruchy skoczka
int dx[] = {2, 1, -1, -2, -2, -1, 1, 2};
int dy[] = {1, 2, 2, 1, -1, -2, -2, -1};

bool isSafe(int x, int y, int sol[N][N]) {
    return (x >= 0 && x < N && y >= 0 && y < N && sol[x][y] == -1);
}

// Liczy dostępne ruchy z danego pola (Reguła Warndorffa)
int getDegree(int x, int y, int sol[N][N]) {
    int count = 0;
    for (int i = 0; i < 8; i++)
        if (isSafe(x + dx[i], y + dy[i], sol))
            count++;
    return count;
}

bool solveKT(int x, int y, int movei, int sol[N][N]);

void generateGraphviz(int sol[N][N]) {
    std::ofstream out("trasa_skoczka.dot");
    out << "digraph G {\n";
    out << "    node [shape=square, style=filled, fillcolor=white, fontname=\"Arial\"];\n";
    out << "    layout=neato;\n"; // layout neato pozwala na ręczne ustawienie pozycji

    // Rozmieszczenie pól w układzie szachownicy
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            out << "    p" << i << j << " [label=\"" << sol[i][j] + 1 << "\", pos=\"" << i << "," << j << "!\"];\n";
        }
    }

    // Rysowanie ścieżki zgodnie z numeracją ruchów
    std::vector<std::pair<int, int>> path(N * N);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            path[sol[i][j]] = {i, j};

    for (int i = 0; i < N * N - 1; i++) {
        out << "    p" << path[i].first << path[i].second << " -> p"
            << path[i+1].first << path[i+1].second << " [color=red, penwidth=2];\n";
    }

    out << "}\n";
    out.close();
}

int main() {
    int sol[N][N];
    for (int x = 0; x < N; x++)
        for (int y = 0; y < N; y++)
            sol[x][y] = -1;

    // Startujemy z pola (0,0)
    int startX = 0, startY = 0;
    sol[startX][startY] = 0;

    if (solveKT(startX, startY, 1, sol)) {
        std::cout << "Znaleziono trase! Generowanie pliku DOT..." << std::endl;
        generateGraphviz(sol);
    } else {
        std::cout << "Nie znaleziono trasy." << std::endl;
    }

    return 0;
}

bool solveKT(int x, int y, int movei, int sol[N][N]) {
    if (movei == N * N) return true;

    // Znajdź następne ruchy i posortuj je według reguły Warndorffa
    std::vector<std::pair<int, std::pair<int, int>>> nextMoves;
    for (int i = 0; i < 8; i++) {
        int next_x = x + dx[i];
        int next_y = y + dy[i];
        if (isSafe(next_x, next_y, sol)) {
            nextMoves.push_back({getDegree(next_x, next_y, sol), {next_x, next_y}});
        }
    }

    std::sort(nextMoves.begin(), nextMoves.end());

    for (auto move : nextMoves) {
        int nx = move.second.first;
        int ny = move.second.second;
        sol[nx][ny] = movei;
        if (solveKT(nx, ny, movei + 1, sol)) return true;
        sol[nx][ny] = -1; // Backtracking
    }

    return false;
}