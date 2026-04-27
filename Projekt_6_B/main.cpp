#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_set>
#include <queue>
#include <unordered_map>
#include <algorithm>

using namespace std;

class WordLadder {
private:
    unordered_set<string> dictionary;

public:
    // Wczytywanie słownika i filtrowanie 4-literowych wyrazów
    void loadDictionary(const string& filename) {
        ifstream file(filename);
        string word;
        if (!file.is_open()) {
            cerr << "Nie mozna otworzyc pliku slownika!" << endl;
            return;
        }
        while (file >> word) {
            if (word.length() == 4) {
                // Zamiana na male litery dla spojnosci
                transform(word.begin(), word.end(), word.begin(), ::tolower);
                dictionary.insert(word);
            }
        }
        file.close();
    }

    // Algorytm BFS szukający najkrótszej ścieżki
    vector<string> findPath(string start, string end) {
        if (dictionary.find(end) == dictionary.end()) return {};

        queue<string> q;
        q.push(start);

        // Mapa do przechowywania "rodzica" danego słowa (skąd przyszliśmy)
        unordered_map<string, string> parent;
        parent[start] = "";

        while (!q.empty()) {
            string current = q.front();
            q.pop();

            if (current == end) break;

            // Generujemy wszystkie mozliwe slowa rozniace sie o jedna litere
            for (int i = 0; i < 4; ++i) {
                string next = current;
                for (char c = 'a'; c <= 'z'; ++c) {
                    next[i] = c;

                    // Jesli slowo jest w slowniku i jeszcze go nie odwiedzilismy
                    if (dictionary.count(next) && parent.find(next) == parent.end()) {
                        parent[next] = current;
                        q.push(next);
                    }
                }
            }
        }

        // Rekonstrukcja ścieżki
        vector<string> path;
        if (parent.find(end) == parent.end()) return {};

        for (string s = end; s != ""; s = parent[s]) {
            path.push_back(s);
        }
        reverse(path.begin(), path.end());
        return path;
    }
};

int main() {
    WordLadder ladder;

    // Wczytaj slownik (upewnij sie, ze plik jest w folderze cmake-build-debug lub podaj pelna sciezke)
    ladder.loadDictionary("slowa.txt");

    string start = "ster";
    string end = "atom";

    vector<string> result = ladder.findPath(start, end);

    if (result.empty()) {
        cout << "Nie znaleziono sciezki miedzy " << start << " a " << end << endl;
    } else {
        cout << "Najkrotsza sciezka (" << result.size() - 1 << " krokow):" << endl;
        for (int i = 0; i < result.size(); ++i) {
            cout << result[i] << (i == result.size() - 1 ? "" : " -> ");
        }
        cout << endl;
    }

    return 0;
}