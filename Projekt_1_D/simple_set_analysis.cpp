#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

class SimpleSet {
private:
    std::vector<std::string> elements; // Jednowymiarowa tablica przechowująca ciągi znaków

public:
    // Wstawianie do zbioru
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
};

void analyzeComplexity() {
    SimpleSet set;
    const int numTests = 10000;

    // Wstawianie elementów
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numTests; ++i) {
        set.insert("element" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Czas wstawiania: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;

    // Sprawdzanie elementów
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numTests; ++i) {
        set.contains("element" + std::to_string(i));
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Czas sprawdzania: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;

    // Usuwanie elementów
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numTests; ++i) {
        set.remove("element" + std::to_string(i));
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Czas usuwania: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;
}

int main() {
    SimpleSet set;

    // Testy podstawowych operacji
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

    // Analiza złożoności obliczeniowej
    analyzeComplexity();

    return 0;
}
