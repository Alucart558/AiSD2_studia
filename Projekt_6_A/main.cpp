// Biblioteka do wyświetlania na konsoli
#include <iostream>
// Biblioteka do użycia wektora dynamicznego
#include <vector>
// Biblioteka do formatowania wyjścia (np. setw, setprecision)
#include <iomanip>
// Biblioteka do operacji na plikach (odczyt, zapis)
#include <fstream>
// Biblioteka do algorytmów jak sort, find, itp.
#include <algorithm>

// Zdefiniowanie stałej N jako rozmiar szachownicy 8x8
const int N = 8;

// Tablica przesunięć osi X dla 8 możliwych ruchów skoczka w szachach
// Skoczek porusza się w kształcie litery L (2 pola w jednym kierunku, 1 pole w innym)
int dx[] = {2, 1, -1, -2, -2, -1, 1, 2};
// Tablica przesunięć osi Y dla 8 możliwych ruchów skoczka
// Każdy element dx[i] odpowiada elementowi dy[i] - razem tworzą jeden ruch
int dy[] = {1, 2, 2, 1, -1, -2, -2, -1};

// Funkcja sprawdzająca czy dane pole (x, y) jest bezpieczne do ruchu
// Zwraca true jeśli: pole jest w granicach szachownicy (0 do N-1) i nie zostało odwiedzone (wartość -1)
bool isSafe(int x, int y, int sol[N][N]) {
    // Sprawdzenie czy x i y są w zakresie [0, N), oraz czy pole nie było wcześniej odwiedzone (sol[x][y] == -1)
    return (x >= 0 && x < N && y >= 0 && y < N && sol[x][y] == -1);
}

// Funkcja liczenia dostępnych ruchów z danego pola - implementacja Reguły Warndorffa
// Reguła Warndorffa: zawsze wybieraj pole, z którego będzie najmniej dostępnych następnych ruchów
// (zmniejsza to ryzyko wejścia w ślepą uliczkę)
int getDegree(int x, int y, int sol[N][N]) {
    // Inicjalizacja licznika dostępnych ruchów na 0
    int count = 0;
    // Pętla przechodzący przez wszystkie 8 możliwych ruchów skoczka
    for (int i = 0; i < 8; i++)
        // Jeśli pole docelowe (x + dx[i], y + dy[i]) jest bezpieczne (w granicach i nieodwiedzone),
        // to zwiększ licznik o 1
        if (isSafe(x + dx[i], y + dy[i], sol))
            count++;
    // Zwróć liczbę dostępnych ruchów z pola (x, y)
    return count;
}

// Funkcja generująca plik w formacie Graphviz (DOT) wizualizujący trasę skoczka
// Tworzy graf skierowany z numeracją pól i strzałkami pokazującymi kolejność ruchów
void generateGraphviz(int sol[N][N]) {
    // Otwórz plik "trasa_skoczka.dot" do zapisu (jeśli istnieje, to go nadpisz)
    std::ofstream out("trasa_skoczka.dot");
    // Napisz nagłówek pliku DOT - deklaracja grafu skierowanego o nazwie G
    out << "digraph G {\n";
    // Ustaw atrybuty domyślne dla wszystkich węzłów: kwadratowe, wypełnione, białe tło, czcionka Arial
    out << "    node [shape=square, style=filled, fillcolor=white, fontname=\"Arial\"];\n";
    // Ustaw layout neato - pozwala to na ręczne ustawienie pozycji węzłów (za pomocą atrybutu pos)
    out << "    layout=neato;\n";

    // Pętla przez wszystkie wiersze szachownicy
    for (int i = 0; i < N; i++) {
        // Pętla przez wszystkie kolumny szachownicy
        for (int j = 0; j < N; j++) {
            // Napisz definicję węzła dla pola (i, j):
            // - nazwa węzła: p + i + j (np. p00 dla pola (0,0))
            // - etykieta: sol[i][j] + 1 (numer ruchu, +1 bo numerujemy od 1, a tablica od 0)
            // - pozycja: i, j (ręczne rozmieszczenie węzłów w kształcie szachownicy)
            out << "    p" << i << j << " [label=\"" << sol[i][j] + 1 << "\", pos=\"" << i << "," << j << "!\"];\n";
        }
    }

    // Utwórz wektor par (liczba ruchu, (x, y)) do przechowywania kolejności ruchów
    // Rozmiar wektora to N*N = 64 (liczba pól na szachownicy)
    std::vector<std::pair<int, int>> path(N * N);
    // Pętla przez wszystkie wiersze szachownicy
    for (int i = 0; i < N; i++)
        // Pętla przez wszystkie kolumny szachownicy
        for (int j = 0; j < N; j++)
            // Przypisz do path[numer ruchu] współrzędne pola (i, j)
            // W ten sposób path to tablica gdzie path[k] = (x, y) dla k-tego ruchu
            path[sol[i][j]] = {i, j};

    // Pętla przez wszystkie ruchy od 0 do N*N-2 (bo rysujemy krawędzie między kolejnymi ruchami)
    for (int i = 0; i < N * N - 1; i++) {
        // Napisz krawędź skierowaną od pola ruchu i do pola ruchu i+1:
        // - path[i] to współrzędne pola dla ruchu i
        // - path[i+1] to współrzędne pola dla ruchu i+1
        // - color=red, penwidth=2 : krawędź będzie czerwona i gruba (szerokość 2)
        out << "    p" << path[i].first << path[i].second << " -> p"
            << path[i+1].first << path[i+1].second << " [color=red, penwidth=2];\n";
    }

    // Napisz zamykający nawias grafu DOT
    out << "}\n";
    // Zamknij plik
    out.close();
}

// Funkcja rozwiązująca problem skoczka szachowego używając backtrackingu i Reguły Warndorffa
// Parametry: x, y - aktualna pozycja skoczka; movei - numer obecnego ruchu; sol - tablica przechowująca numerację ruchów
bool solveKT(int x, int y, int movei, int sol[N][N]) {
    // Warunek bazowy: jeśli odwiedliśmy wszystkie pola (movei == N*N = 64),
    // to znaleźliśmy rozwiązanie i zwracamy true
    if (movei == N * N) return true;

    // Wektor do przechowywania par (liczba dostępnych ruchów, (x, y)) dla każdego możliwego następnego ruchu
    // Używamy go do posortowania ruchów wg Reguły Warndorffa
    std::vector<std::pair<int, std::pair<int, int>>> nextMoves;

    // Pętla przechodzący przez wszystkie 8 możliwych ruchów skoczka
    for (int i = 0; i < 8; i++) {
        // Oblicz następną pozycję: aktualna pozycja + przesunięcie dla ruchu i (oś X)
        int next_x = x + dx[i];
        // Oblicz następną pozycję: aktualna pozycja + przesunięcie dla ruchu i (oś Y)
        int next_y = y + dy[i];
        // Jeśli następne pole jest bezpieczne (w granicach i nieodwiedzone),
        // to dodaj je do listy możliwych ruchów razem z jego stopniem (liczbą dostępnych następnych ruchów)
        if (isSafe(next_x, next_y, sol)) {
            // Dodaj parę: (stopień następnego pola, (współrzędne następnego pola))
            nextMoves.push_back({getDegree(next_x, next_y, sol), {next_x, next_y}});
        }
    }

    // Posortuj wszystkie możliwe następne ruchy w porządku rosnącym wg ich stopnia (Reguła Warndorffa)
    // Pola z mniejszą liczbą dostępnych ruchów będą pierwsze - zmniejsza to ryzyko wejścia w ślepą uliczkę
    std::sort(nextMoves.begin(), nextMoves.end());

    // Pętla przechodzący przez wszystkie możliwe ruchy w porządku posortowanym wg Reguły Warndorffa
    for (auto move : nextMoves) {
        // Rozpakuj współrzędną X następnego pola z pary (stopień, (x, y))
        int nx = move.second.first;
        // Rozpakuj współrzędną Y następnego pola
        int ny = move.second.second;
        // Oznacz odwiedzenie pola - zapisz numer ruchu (movei) w sol[nx][ny]
        sol[nx][ny] = movei;
        // Rekurencyjnie spróbuj znaleźć rozwiązanie od następnego pola
        // Jeśli rekurencja zwróci true, oznacza to znalezienie pełnego rozwiązania
        if (solveKT(nx, ny, movei + 1, sol)) return true;
        // Backtracking: jeśli ścieżka od (nx, ny) nie prowadzi do rozwiązania,
        // przestaw pole na nieodwiedzone (-1) i spróbuj innego ruchu
        sol[nx][ny] = -1;
    }

    // Jeśli żaden z możliwych ruchów nie prowadzi do rozwiązania, zwróć false
    return false;
}

// Główna funkcja programu
int main() {
    // Deklaracja tablicy sol[8][8] do przechowywania numeracji ruchów skoczka
    int sol[N][N];
    // Pętla przez wszystkie wiersze szachownicy
    for (int x = 0; x < N; x++)
        // Pętla przez wszystkie kolumny szachownicy
        for (int y = 0; y < N; y++)
            // Inicjalizuj każde pole wartością -1 (pole nieodwiedzone)
            sol[x][y] = -1;

    // Zdefiniuj współrzędne pola startowego
    // Startujemy z pola (0,0) - lewy górny róg szachownicy
    int startX = 0, startY = 0;
    // Oznacz pole startowe jako pierwszy ruch (numer 0)
    sol[startX][startY] = 0;

    // Spróbuj znaleźć trasę skoczka począwszy od pola startowego, ruch numer 1 (następny po 0)
    if (solveKT(startX, startY, 1, sol)) {
        // Jeśli znaleziono trasę, wyświetl komunikat na konsoli
        std::cout << "Znaleziono trase! Generowanie pliku DOT..." << std::endl;
        // Wygeneruj plik DOT z wizualizacją trasy
        generateGraphviz(sol);
    } else {
        // Jeśli nie znaleziono trasy, wyświetl komunikat o błędzie
        std::cout << "Nie znaleziono trasy." << std::endl;
    }

    // Zwróć 0 aby wskazać pomyślne zakończenie programu
    return 0;
}

