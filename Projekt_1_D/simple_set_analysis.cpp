#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>

class SimpleSet {
private:
    std::vector<std::string> elements; // Jednowymiarowa tablica przechowująca ciągi znaków

public:
    // Wstawianie do zbioru (bez duplikatow)
    void insert(const std::string& str) {
        if (std::find(elements.begin(), elements.end(), str) == elements.end()) {
            elements.push_back(str);
        }
    }

    // Usuwanie ze zbioru
    void remove(const std::string& str) {
        auto it = std::find(elements.begin(), elements.end(), str);
        if (it != elements.end()) {
            elements.erase(it);
        }
    }

    // Sprawdzanie czy element należy do zbioru
    bool contains(const std::string& str) const {
        return std::find(elements.begin(), elements.end(), str) != elements.end();
    }

    // Wypisywanie elementów zbioru (pomocnicza metoda)
    void print() const {
        std::cout << "{ ";
        for (const auto& el : elements) {
            std::cout << el << " ";
        }
        std::cout << "}" << std::endl;
    }

    // Rozmiar zbioru
    std::size_t size() const {
        return elements.size();
    }

    // Czyszczenie zbioru
    void clear() {
        elements.clear();
    }
};

// Analiza złożoności dla pojedynczego N
void analyzeComplexityForN(int numTests) {
    using namespace std::chrono;

    SimpleSet set;

    // Wstawianie elementów
    auto start = high_resolution_clock::now();
    for (int i = 0; i < numTests; ++i) {
        set.insert("element" + std::to_string(i));
    }
    auto end = high_resolution_clock::now();
    double insertMs = duration<double, std::milli>(end - start).count();

    // Sprawdzanie elementów
    start = high_resolution_clock::now();
    for (int i = 0; i < numTests; ++i) {
        set.contains("element" + std::to_string(i));
    }
    end = high_resolution_clock::now();
    double containsMs = duration<double, std::milli>(end - start).count();

    // Usuwanie elementów
    start = high_resolution_clock::now();
    for (int i = 0; i < numTests; ++i) {
        set.remove("element" + std::to_string(i));
    }
    end = high_resolution_clock::now();
    double removeMs = duration<double, std::milli>(end - start).count();

    // Dane w formacie pod gnuplot: N insert[ms] contains[ms] remove[ms]
    std::cout << std::setw(10) << numTests
              << std::setw(15) << insertMs
              << std::setw(15) << containsMs
              << std::setw(15) << removeMs << std::endl;
}

// Analiza złożoności dla kilku N
void analyzeComplexity() {
    std::cout << "=== BADANIE ZLOZONOSCI SimpleSet (vector<string>) ===" << std::endl;
    std::cout << "Struktura danych: wektor lancuchow, operacje korzystaja z std::find.\n";
    std::cout << "Kazda pojedyncza operacja insert/contains/remove jest liniowa O(n).\n";
    std::cout << "Przy wykonywaniu numTests operacji, calkowity czas jest z grubsza O(n^2).\n\n";

    std::cout << std::setw(10) << "N"
              << std::setw(15) << "insert[ms]"
              << std::setw(15) << "contains[ms]"
              << std::setw(15) << "remove[ms]" << std::endl;
    std::cout << std::string(55, '-') << std::endl;

    analyzeComplexityForN(1000);
    analyzeComplexityForN(2000);
    analyzeComplexityForN(5000);
    analyzeComplexityForN(10000);
    analyzeComplexityForN(20000);

    std::cout << "\nWNIOSKI:" << std::endl;
    std::cout << "1. std::find przeszukuje wektor liniowo, wiec pojedyncze operacje sa O(n)." << std::endl;
    std::cout << "2. Przy wykonywaniu wielu operacji dla rosnacego N, calkowity czas rosnie szybciej niz liniowo (ok. kwadratowo)." << std::endl;
}

int main() {
    SimpleSet set;

    // Testy podstawowych operacji
    std::cout << "=== TESTY POPRAWNOSCI SimpleSet ===" << std::endl;
    set.insert("apple");
    set.insert("banana");
    set.insert("cherry");

    std::cout << "Zbior po wstawieniu: ";
    set.print();

    set.remove("banana");
    std::cout << "Zbior po usunieciu 'banana': ";
    set.print();

    std::cout << "Czy 'apple' jest w zbiorze? " << (set.contains("apple") ? "TAK" : "NIE") << std::endl;
    std::cout << "Czy 'banana' jest w zbiorze? " << (set.contains("banana") ? "TAK" : "NIE") << std::endl;
    std::cout << "===================================\n" << std::endl;

    // Analiza złożoności obliczeniowej
    analyzeComplexity();

    return 0;
}
