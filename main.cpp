#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <random>
#include <iomanip>

class SetSimple {
private:
    // Rozmiar uniwersum (maksymalna liczba elementów)
    size_t N;
    // Tablica reprezentująca zbiór. true = element należy, false = nie należy.
    // std::vector<bool> jest w C++ specjalizowany i zajmuje 1 bit na element,
    // co jest bardzo efektywne pamięciowo.
    std::vector<bool> table;

public:
    // Konstruktor
    SetSimple(size_t n) : N(n), table(n, false) {}

    // --- Operacje Element-Zbiór ---

    // Wstawianie do zbioru: O(1)
    void insert(size_t x) {
        if (x < N) {
            table[x] = true;
        }
    }

    // Usuwanie ze zbioru: O(1)
    void remove(size_t x) {
        if (x < N) {
            table[x] = false;
        }
    }

    // Sprawdzanie czy element należy: O(1)
    bool contains(size_t x) const {
        if (x < N) {
            return table[x];
        }
        return false;
    }

    // --- Operacje Zbiór-Zbiór ---

    // Suma zbiorów (OR): O(N)
    SetSimple setUnion(const SetSimple& other) const {
        // Zakładamy, że zbiory mają to samo uniwersum N
        SetSimple result(N);
        for (size_t i = 0; i < N; ++i) {
            // Element jest w sumie, jeśli jest w A LUB w B
            if (this->table[i] || other.table[i]) {
                result.table[i] = true;
            }
        }
        return result;
    }

    // Część wspólna (AND): O(N)
    SetSimple setIntersection(const SetSimple& other) const {
        SetSimple result(N);
        for (size_t i = 0; i < N; ++i) {
            // Element jest w przecięciu, jeśli jest w A I w B
            if (this->table[i] && other.table[i]) {
                result.table[i] = true;
            }
        }
        return result;
    }

    // Różnica zbiorów (A \ B): O(N)
    SetSimple setDifference(const SetSimple& other) const {
        SetSimple result(N);
        for (size_t i = 0; i < N; ++i) {
            // Element jest w różnicy, jeśli jest w A I NIE ma go w B
            if (this->table[i] && !other.table[i]) {
                result.table[i] = true;
            }
        }
        return result;
    }

    // Sprawdzanie identyczności: O(N)
    bool isEqual(const SetSimple& other) const {
        if (N != other.N) return false;
        for (size_t i = 0; i < N; ++i) {
            if (this->table[i] != other.table[i]) {
                return false;
            }
        }
        return true;
    }

    // Metoda pomocnicza do wypisywania (dla małych N)
    void print() const {
        std::cout << "{ ";
        for (size_t i = 0; i < N; ++i) {
            if (table[i]) {
                std::cout << i << " ";
            }
        }
        std::cout << "}" << std::endl;
    }

    size_t getSize() const { return N; }
};

// --- Testy Poprawności ---

void runTests() {
    std::cout << "=== TESTY POPRAWNOSCI ===" << std::endl;
    size_t N = 10;
    SetSimple A(N);
    SetSimple B(N);

    // Test wstawiania
    A.insert(1);
    A.insert(3);
    A.insert(5);

    B.insert(3);
    B.insert(5);
    B.insert(7);

    std::cout << "Zbior A: "; A.print();
    std::cout << "Zbior B: "; B.print();

    // Test contains
    std::cout << "Czy 1 w A? " << (A.contains(1) ? "TAK" : "NIE") << std::endl;
    std::cout << "Czy 7 w A? " << (A.contains(7) ? "TAK" : "NIE") << std::endl;

    // Test Sumy (1, 3, 5, 7)
    SetSimple Sum = A.setUnion(B);
    std::cout << "Suma A U B: "; Sum.print();

    // Test Przecięcia (3, 5)
    SetSimple Inter = A.setIntersection(B);
    std::cout << "Przeciecie A n B: "; Inter.print();

    // Test Różnicy A \ B (1)
    SetSimple Diff = A.setDifference(B);
    std::cout << "Roznica A \\ B: "; Diff.print();

    // Test Identyczności
    std::cout << "Czy A == B? " << (A.isEqual(B) ? "TAK" : "NIE") << std::endl;
    SetSimple A_clone = A;
    std::cout << "Czy A == A_clone? " << (A.isEqual(A_clone) ? "TAK" : "NIE") << std::endl;

    std::cout << "=========================\n" << std::endl;
}

// --- Badanie Złożoności ---

void measureComplexity(size_t N) {
    SetSimple s1(N);
    SetSimple s2(N);

    // Wypełnianie losowe (aby operacje miały co robić)
    // Używamy deterministycznego ziarna dla powtarzalności
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, N - 1);

    // Wypełniamy ok. połowę zbioru
    for(size_t i=0; i<N/2; ++i) {
        s1.insert(dis(gen));
        s2.insert(dis(gen));
    }

    using namespace std::chrono;

    // 1. Badanie INSERT (Element-Zbiór)
    // Wykonujemy wiele operacji, aby zmierzyć średni czas, bo pojedyncza jest za szybka
    auto start = high_resolution_clock::now();
    int ops = 100000;
    for(int i=0; i<ops; ++i) {
        s1.insert(dis(gen));
    }
    auto end = high_resolution_clock::now();
    double timeInsert = duration<double, std::nano>(end - start).count() / ops;

    // 2. Badanie UNION (Zbiór-Zbiór)
    start = high_resolution_clock::now();
    volatile auto resUnion = s1.setUnion(s2); // volatile by kompilator nie usunął
    end = high_resolution_clock::now();
    double timeUnion = duration<double, std::micro>(end - start).count();

    // 3. Badanie INTERSECTION (Zbiór-Zbiór)
    start = high_resolution_clock::now();
    volatile auto resInter = s1.setIntersection(s2);
    end = high_resolution_clock::now();
    double timeInter = duration<double, std::micro>(end - start).count();

    std::cout << std::setw(10) << N
              << std::setw(20) << timeInsert
              << std::setw(20) << timeUnion
              << std::setw(20) << timeInter << std::endl;
}

void runBenchmarks() {
    std::cout << "=== BADANIE ZLOZONOSCI ===" << std::endl;
    std::cout << "Podejscie: Mierzymy czas wykonania operacji dla roznych wielkosci uniwersum N.\n";
    std::cout << "Dla operacji Insert (O(1)) mierzymy srednia z 100k powtorzen (w nanosekundach).\n";
    std::cout << "Dla operacji na zbiorach (O(N)) mierzymy pojedyncze wykonanie (w mikrosekundach).\n\n";

    std::cout << std::setw(10) << "N (Size)"
              << std::setw(20) << "Insert [ns]"
              << std::setw(20) << "Union [us]"
              << std::setw(20) << "Intersect [us]" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    // Testujemy dla rosnących potęg 10
    measureComplexity(1000);       // 10^3
    measureComplexity(10000);      // 10^4
    measureComplexity(100000);     // 10^5
    measureComplexity(1000000);    // 10^6
    measureComplexity(10000000);   // 10^7

    std::cout << "\nWNIOSKI:" << std::endl;
    std::cout << "1. Insert: Czas jest staly (nie zalezy od N). Zgodne z teoria O(1)." << std::endl;
    std::cout << "2. Union/Intersect: Czas rosnie liniowo wraz z N. Zgodne z teoria O(N)." << std::endl;
    std::cout << "   Dzieje sie tak, poniewaz musimy przeiterowac cala tablice o rozmiarze N," << std::endl;
    std::cout << "   niezaleznie od tego ile elementow faktycznie znajduje sie w zbiorze." << std::endl;
}

int main() {
    runTests();
    runBenchmarks();

    std::cout << "\n=== PYTANIE O TYP DANYCH ===" << std::endl;
    std::cout << "Warunki dla typu danych elementow:" << std::endl;
    std::cout << "Aby mozna bylo zastosowac te implementacje (tablica boolowska), elementy musza byc:" << std::endl;
    std::cout << "1. Przeliczalne i mapowalne na liczby calkowite z zakresu [0, N-1]." << std::endl;
    std::cout << "2. Unikalne w sensie mapowania (funkcja mapujaca musi byc bijekcja w zakresie uzywanych wartosci)." << std::endl;
    std::cout << "Przyklad: Jesli elementami sa znaki 'a'-'z', mozna je mapowac na 0-25." << std::endl;

    return 0;
}