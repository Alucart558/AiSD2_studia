#include <iostream>
#include <memory>
#include <chrono>
#include <random>
#include <iomanip>

class Node {
public:
    int value;
    std::shared_ptr<Node> next;

    Node(int val) : value(val), next(nullptr) {}
};

class setLinked {
private:
    std::shared_ptr<Node> head;

public:
    setLinked() : head(nullptr) {}

    // Wstawia element do zbioru w porzadku rosnacym, bez duplikatow
    void insert(int value) {
        auto newNode = std::make_shared<Node>(value);

        // Wstawienie na poczatek
        if (!head || head->value > value) {
            if (!head || head->value != value) {
                newNode->next = head;
                head = newNode;
            }
            return;
        }

        auto current = head;
        while (current->next && current->next->value < value) {
            current = current->next;
        }

        // Jesli nie ma duplikatu, wstawiamy
        if (!current->next || current->next->value != value) {
            newNode->next = current->next;
            current->next = newNode;
        }
    }

    // Usuwa element ze zbioru
    void remove(int value) {
        if (!head) return;

        if (head->value == value) {
            head = head->next;
            return;
        }

        auto current = head;
        while (current->next && current->next->value != value) {
            current = current->next;
        }

        if (current->next) {
            current->next = current->next->next;
        }
    }

    // Sprawdza, czy element jest w zbiorze
    bool contains(int value) const {
        auto current = head;
        while (current) {
            if (current->value == value) return true;
            current = current->next;
        }
        return false;
    }

    // Wypisuje elementy zbioru
    void print() const {
        std::cout << "{ ";
        auto current = head;
        while (current) {
            std::cout << current->value << " ";
            current = current->next;
        }
        std::cout << "}" << std::endl;
    }

    // Zwraca rozmiar (liczbe elementow) zbioru
    int size() const {
        int count = 0;
        auto current = head;
        while (current) {
            ++count;
            current = current->next;
        }
        return count;
    }

    // Czyści zbiór
    void clear() {
        head.reset();
    }
};

// --- Testy poprawności ---

void runCorrectnessTests() {
    std::cout << "=== TESTY POPRAWNOSCI setLinked ===" << std::endl;

    setLinked mySet;

    mySet.insert(5);
    mySet.insert(3);
    mySet.insert(8);
    mySet.insert(3); // duplikat, nie powinien zostac dodany

    std::cout << "Zbior po wstawieniach: ";
    mySet.print();

    mySet.remove(3);
    std::cout << "Zbior po usunieciu 3: ";
    mySet.print();

    std::cout << "Czy 5 jest w zbiorze? " << (mySet.contains(5) ? "TAK" : "NIE") << std::endl;
    std::cout << "Czy 10 jest w zbiorze? " << (mySet.contains(10) ? "TAK" : "NIE") << std::endl;

    std::cout << "Rozmiar zbioru: " << mySet.size() << std::endl;
    std::cout << "===============================\n" << std::endl;
}

// --- Badanie złożoności ---

void measureLinkedComplexity(int N) {
    using namespace std::chrono;

    setLinked s;

    // Generujemy liczby w zakresie [0, 2N), czesc sie powtorzy
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, 2 * N);

    // Wstepnie wstawiamy N elementow
    for (int i = 0; i < N; ++i) {
        s.insert(dis(gen));
    }

    int ops = 10000; // liczba prob dla sredniej

    // 1. Insert
    auto start = high_resolution_clock::now();
    for (int i = 0; i < ops; ++i) {
        s.insert(dis(gen));
    }
    auto end = high_resolution_clock::now();
    double timeInsert = duration<double, std::micro>(end - start).count() / ops;

    // 2. Contains
    start = high_resolution_clock::now();
    for (int i = 0; i < ops; ++i) {
        s.contains(dis(gen));
    }
    end = high_resolution_clock::now();
    double timeContains = duration<double, std::micro>(end - start).count() / ops;

    // 3. Remove
    start = high_resolution_clock::now();
    for (int i = 0; i < ops; ++i) {
        s.remove(dis(gen));
    }
    end = high_resolution_clock::now();
    double timeRemove = duration<double, std::micro>(end - start).count() / ops;

    // Wyniki w formacie przyjaznym gnuplotowi
    std::cout << std::setw(10) << N
              << std::setw(20) << timeInsert
              << std::setw(20) << timeContains
              << std::setw(20) << timeRemove << std::endl;
}

void runLinkedBenchmarks() {
    std::cout << "=== BADANIE ZLOZONOSCI setLinked (lista jednokierunkowa) ===" << std::endl;
    std::cout << "Podejscie: dla roznych N (liczba elementow w zbiorze) mierzymy sredni czas\n";
    std::cout << "operacji insert/contains/remove. Kazda operacja jest liniowa (O(n)),\n";
    std::cout << "bo trzeba przejsc liste od poczatku do miejsca wstawienia/znalezienia.\n\n";

    std::cout << std::setw(10) << "N"
              << std::setw(20) << "Insert[us]"
              << std::setw(20) << "Contains[us]"
              << std::setw(20) << "Remove[us]" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    measureLinkedComplexity(1000);
    measureLinkedComplexity(2000);
    measureLinkedComplexity(5000);
    measureLinkedComplexity(10000);
    measureLinkedComplexity(20000);

    std::cout << "\nWNIOSKI:" << std::endl;
    std::cout << "Czasy insert/contains/remove rosna mniej wiecej liniowo z N," << std::endl;
    std::cout << "co jest zgodne z teoretyczna zlozonoscia O(n) dla listy jednokierunkowej." << std::endl;
}

int main() {
    // Testy poprawnosci dzialania operacji
    runCorrectnessTests();

    // Analiza zlozonosci obliczeniowej
    runLinkedBenchmarks();

    return 0;
}