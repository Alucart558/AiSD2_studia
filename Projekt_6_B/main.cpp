// Biblioteka do operacji wejścia/wyjścia (cout, cerr)
#include <iostream>
// Biblioteka do dynamicznych tablic (vector)
#include <vector>
// Biblioteka do operacji na tekstach/stringach (string)
#include <string>
// Biblioteka do operacji na plikach (ifstream, ofstream)
#include <fstream>
// Biblioteka do zbioru unikatowych elementów (unordered_set) - szybkie wyszukiwanie
#include <unordered_set>
// Biblioteka do kolejki (queue) - do BFS (Breadth-First Search)
#include <queue>
// Biblioteka do mapy haszowanej (unordered_map) - do przechowywania relacji "rodzic-dziecko"
#include <unordered_map>
// Biblioteka do algorytmów standardowych (transform, reverse)
#include <algorithm>

// Deklaracja že uzywamy standardowej przestrzeni nazw (std::)
using namespace std;

// Klasa implementująca algorytm "Drabina słów" (Word Ladder)
class WordLadder {
private:
    // Zbiór haszowany przechowujący słownik - umożliwia O(1) wyszukiwanie słów
    unordered_set<string> dictionary;

public:
    // Metoda wczytuje słownik z pliku i filtruje tylko wyrazów 4-literowe
    void loadDictionary(const string& filename) {
        // Otwieramy plik do czytania
        ifstream file(filename);
        // Zmienna tymczasowa do przechowywania każdego słowa z pliku
        string word;
        // Sprawdzamy czy plik został poprawnie otwarty
        if (!file.is_open()) {
            // Jeśli nie, wypisujemy błąd na standardowy strumień błędów
            cerr << "Nie mozna otworzyc pliku slownika!" << endl;
            // Kończymy funkcję, nie wczytując słownika
            return;
        }
        // Pętla - czytamy słowa z pliku dopóki są dostępne
        while (file >> word) {
            // Sprawdzamy czy wczytane słowo ma dokładnie 4 litery (warunek zadania)
            if (word.length() == 4) {
                // Algoritm transform - zamieniamy każdą DUŻĄ literę na małą, aby ujednolicić format
                // word.begin() do word.end() - zakres na którym operujemy
                // ::tolower - funkcja zamieniająca znaki na małe litery
                transform(word.begin(), word.end(), word.begin(), ::tolower);
                // Wstawiamy przetworzone słowo do zboru (unordered_set) - automatycznie eliminuje duplikaty
                dictionary.insert(word);
            }
        }
        // Zamykamy plik, zwalniając zasoby
        file.close();
    }

    // Metoda BFS (Breadth-First Search) szukająca najkrótszej ścieżki między dwoma słowami
    // Zwraca wektor słów reprezentujących tę ścieżkę
    vector<string> findPath(string start, string end) {
        // Sprawdzamy czy słowo docelowe (end) istnieje w słowniku
        // Jeśli nie, nie ma możliwości znalezienia ścieżki - zwracamy pusty wektor
        if (dictionary.find(end) == dictionary.end()) return {};

        // Tworzymy kolejkę (FIFO) do implementacji BFS - zaczynamy z startowego słowa
        queue<string> q;
        // Wstawiamy słowo startowe do kolejki - to będzie pierwszy odwiedzany wierzchołek
        q.push(start);

        // Mapa haszowana do przechowywania, skąd przyszliśmy do każdego słowa (relacja "rodzic")
        // Umożliwia nam rekonstrukcję ścieżki od końca do początku po znalezieniu celu
        unordered_map<string, string> parent;
        // Inicjalizujemy słowo startowe z pustym rodzicem (oznacza początek ścieżki)
        parent[start] = "";

        // Główna pętla BFS - dopóki są słowa do przetworzenia w kolejce
        while (!q.empty()) {
            // Pobieramy słowo z przodu kolejki (FIFO) - to obecne słowo, które przetwarzamy
            string current = q.front();
            // Usuwamy pobrane słowo z przodu kolejki
            q.pop();

            // Warunek zatrzymania: jeśli dotarliśmy do słowa docelowego, przerwij pętlę
            if (current == end) break;

            // Zewnętrzna pętla: iterujemy przez każdą pozycję w słowie (0-3 dla 4-literowych wyrazów)
            for (int i = 0; i < 4; ++i) {
                // Kopiujemy obecne słowo, aby nie zmienić oryginału
                string next = current;
                // Wewnętrzna pętla: próbujemy zamienić literę na pozycji i na wszystkie litery a-z
                for (char c = 'a'; c <= 'z'; ++c) {
                    // Zastępujemy literę na pozycji i nową literą c
                    next[i] = c;

                    // Sprawdzamy trzy warunki jednocześnie:
                    // 1. dictionary.count(next) - czy nowe słowo istnieje w słowniku
                    // 2. parent.find(next) == parent.end() - czy to słowo nie zostało jeszcze odwiedzone
                    // Jeśli oba warunki są spełnione, słowo jest sąsiadem obecnego słowa (różni się 1 literą)
                    if (dictionary.count(next) && parent.find(next) == parent.end()) {
                        // Zapisujemy obecne słowo jako rodzica nowego słowa - będzie nam potrzebne do rekonstrukcji ścieżki
                        parent[next] = current;
                        // Wstawiamy nowe słowo do kolejki, aby je przetwarzać w przyszłych iteracjach BFS
                        q.push(next);
                    }
                }
            }
        }

        // Rekonstrukcja ścieżki od słowa docelowego do słowa startowego, używając mapy "parent"
        vector<string> path;
        // Sprawdzamy czy słowo docelowe (end) znajduje się w mapie "parent"
        // Jeśli go tam nie ma, oznacza to że nigdy go nie odwiedziliśmy - nie ma ścieżki
        if (parent.find(end) == parent.end()) return {};

        // Pętla do rekonstrukcji ścieżki - zaczynamy od końca (end) i idziemy wstecz po rodziców
        // Warunek s != "" zapewnia że zatrzymamy się na słowie startowym (jego rodzic to pusty string)
        for (string s = end; s != ""; s = parent[s]) {
            // Dodajemy bieżące słowo do ścieżki (będzie w odwrotnej kolejności)
            path.push_back(s);
        }
        // Odwracamy wektor path, żeby słowa były w poprawnej kolejności (od start do end)
        reverse(path.begin(), path.end());
        // Zwracamy kompletną ścieżkę
        return path;
    }
};

// Główna funkcja programu - punkt wejścia
int main() {
    // Tworzymy obiekt klasy WordLadder - będzie przechowywał słownik i wykonywał wyszukiwanie
    WordLadder ladder;

    // Wczytujemy słownik z pliku "slowa.txt" zawierającego wszystkie 4-literowe wyrazy
    // upewnij się, że plik jest w folderze cmake-build-debug lub podaj pełną ścieżkę
    ladder.loadDictionary("slowa.txt");

    // Definiujemy słowo startowe - punkt wyjścia dla algorytmu BFS
    string start = "ster";
    // Definiujemy słowo docelowe - punkt docelowy dla algorytmu BFS
    string end = "atom";

    // Szukamy najkrótszej ścieżki od "ster" do "atom" - każda zmiana to zmiana 1 litery
    // Funkcja zwraca wektor słów reprezentujących tę ścieżkę, lub pusty wektor jeśli ścieżka nie istnieje
    vector<string> result = ladder.findPath(start, end);

    // Sprawdzamy czy funkcja findPath zwróciła pusty wektor (brak ścieżki)
    if (result.empty()) {
        // Jeśli nie znaleziono ścieżki, wyświetlamy komunikat o błędzie
        cout << "Nie znaleziono sciezki miedzy " << start << " a " << end << endl;
    } else {
        // Jeśli znaleziono ścieżkę, wyświetlamy liczbę kroków (liczba słów - 1)
        cout << "Najkrotsza sciezka (" << result.size() - 1 << " krokow):" << endl;
        // Pętla przechodzą przez wszystkie słowa w znalezionej ścieżce
        for (int i = 0; i < result.size(); ++i) {
            // Wyświetlamy każde słowo ze ścieżki
            cout << result[i]
                // Dodajemy separator " -> " między słowami, ale nie po ostatnim słowie
                << (i == result.size() - 1 ? "" : " -> ");
        }
        // Wyświetlamy koniec linii (endl) po kompletnej ścieżce
        cout << endl;
    }

    // Zwracamy 0 - informacją że program zakończył się prawidłowo
    return 0;
}