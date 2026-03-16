#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <random>
#include <iomanip>
#include <string>
#include <stdexcept>

class SetSimple {
private:
    // Rozmiar uniwersum (maksymalna liczba elementów)
    size_t N;
    // Tablica reprezentująca zbiór. true = element należy, false = nie należy.
    // std::vector<bool> jest w C++ specjalizowany i zajmuje 1 bit na element,
    // co jest bardzo efektywne pamięciowo.
    std::vector<bool> table;

public:
    // Konstruktor, table(n, false) - tablica n elementów, wszystkie to false
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
    // Wywołanie: A.setUnion(B)
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

    // =========================
    // MAPOWANIA ELEMENTÓW
    // =========================

    // 1. Zbiór liczb całkowitych: n, n+1, ..., m  (n < m)
    // N musi być równe (m - n + 1).
    size_t mapIntegerRange(int x, int n, int m) const {
        if (n >= m) {
            throw std::invalid_argument("mapIntegerRange: n must be < m");
        }
        if (x < n || x > m) {
            throw std::out_of_range("mapIntegerRange: integer out of [n, m]");
        }
        // Przesuwamy tak, aby n -> 0, n+1 -> 1, ..., m -> m-n
        return static_cast<size_t>(x - n);
    }

    // 2. Zbiór liczb całkowitych: n, n+2, n+4, ..., m (n < m, m i n tej samej parzystości)
    // N musi być równe: ((m - n) / 2) + 1.
    size_t mapIntegerRangeStep2(int x, int n, int m) const {
        if (n >= m) {
            throw std::invalid_argument("mapIntegerRangeStep2: n must be < m");
        }
        // Sprawdzamy, czy ciąg kończy się dokładnie na m
        if ((m - n) % 2 != 0) {
            throw std::invalid_argument("mapIntegerRangeStep2: m and n must have the same parity");
        }
        if (x < n || x > m || ((x - n) % 2 != 0)) {
            throw std::out_of_range("mapIntegerRangeStep2: x not in {n, n+2, ..., m}");
        }
        // n -> 0, n+2 -> 1, n+4 -> 2, ...
        return static_cast<size_t>((x - n) / 2);
    }

    // 3. Litery bez polskich znaków: 'a'..'z'
    // N musi być równe 26.
    size_t mapCharLetter(char c) const {
        if (c < 'a' || c > 'z') {
            throw std::out_of_range("mapCharLetter: expected 'a'..'z'");
        }
        return static_cast<size_t>(c - 'a'); // 0..25
    }

    // 4. Dwuliterowe napisy: każda litera z zakresu 'a'..'z'
    // N musi być równe 26 * 26 = 676.
    // "aa" -> 0, "ab" -> 1, ..., "az" -> 25, "ba" -> 26, ..., "zz" -> 675
    size_t mapTwoLetterString(const std::string& str) const {
        if (str.length() != 2) {
            throw std::out_of_range("mapTwoLetterString: string length must be 2");
        }
        char c1 = str[0];
        char c2 = str[1];

        if (c1 < 'a' || c1 > 'z' || c2 < 'a' || c2 > 'z') {
            throw std::out_of_range("mapTwoLetterString: expected letters 'a'..'z'");
        }

        return (static_cast<size_t>(c1 - 'a') * 26) + static_cast<size_t>(c2 - 'a');
    }
};

// --- Testy Poprawności ---

void runBasicSetTests() {
    std::cout << "=== TESTY PODSTAWOWYCH OPERACJI ZBIOROWYCH ===" << std::endl;
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

    std::cout << "=============================================\n" << std::endl;
}

// --- Testy mapowań wymaganych w zadaniu ---

void runMappingTests() {
    std::cout << "=== TESTY FUNKCJI MAPUJACYCH ===" << std::endl;

    // 1) Liczby calkowite n, n+1, ..., m
    int n1 = -3;
    int m1 = 2;
    size_t N1 = static_cast<size_t>(m1 - n1 + 1); // 6 elementów
    SetSimple S1(N1);

    std::cout << "\n[MAPOWANIE LICZB n, n+1, ..., m]  n=" << n1 << " m=" << m1 << std::endl;
    for (int x = n1; x <= m1; ++x) {
        size_t idx = S1.mapIntegerRange(x, n1, m1);
        std::cout << "x = " << x << " -> index = " << idx << std::endl;
        S1.insert(idx);
    }
    std::cout << "Reprezentacja zbioru (indeksy): "; S1.print();

    // 2) Liczby calkowite n, n+2, ..., m
    int n2 = 3;
    int m2 = 11; // 3,5,7,9,11
    size_t N2 = static_cast<size_t>((m2 - n2) / 2 + 1); // 5 elementów
    SetSimple S2(N2);

    std::cout << "\n[MAPOWANIE LICZB n, n+2, ..., m]  n=" << n2 << " m=" << m2 << std::endl;
    for (int x = n2; x <= m2; x += 2) {
        size_t idx = S2.mapIntegerRangeStep2(x, n2, m2);
        std::cout << "x = " << x << " -> index = " << idx << std::endl;
        S2.insert(idx);
    }
    std::cout << "Reprezentacja zbioru (indeksy): "; S2.print();

    // 3) Litery 'a'..'z'
    size_t N3 = 26;
    SetSimple S3(N3);

    std::cout << "\n[MAPOWANIE LITER 'a'..'z']" << std::endl;
    for (char c = 'a'; c <= 'z'; ++c) {
        size_t idx = S3.mapCharLetter(c);
        if (c < 'd' || c > 'w') { // nie wypisuj wszystkiego, tylko kilka brzegowych
            std::cout << "c = '" << c << "' -> index = " << idx << std::endl;
        }
        S3.insert(idx);
    }
    std::cout << "Reprezentacja zbioru (indeksy): "; S3.print();

    // 4) Dwuliterowe napisy "aa".."zz"
    size_t N4 = 26 * 26;
    SetSimple S4(N4);

    std::cout << "\n[MAPOWANIE DWULITEROWYCH NAPISOW 'aa'..'zz']" << std::endl;
    std::string s;
    s.resize(2);

    // pokaż kilka przykładów zamiast wszystkich 676
    const char* testWords[] = { "aa", "ab", "az", "ba", "zz" };
    for (const char* w : testWords) {
        std::string str(w);
        size_t idx = S4.mapTwoLetterString(str);
        std::cout << "str = \"" << str << "\" -> index = " << idx << std::endl;
        S4.insert(idx);
    }

    std::cout << "Przykladowa reprezentacja zbioru (tylko kilka indeksow ustawionych): ";
    // Wypisujemy tylko pierwsze ~30 indeksów, żeby nie spamować
    std::cout << "{ ";
    size_t maxPrint = 30;
    for (size_t i = 0; i < N4 && i < maxPrint; ++i) {
        if (S4.contains(i)) {
            std::cout << i << " ";
        }
    }
    std::cout << "... }" << std::endl;

    std::cout << "=================================\n" << std::endl;
}

// --- Badanie Złożoności ---

void measureComplexity(size_t N) {
    SetSimple s1(N);
    SetSimple s2(N);

    // Wypełnianie losowe (aby operacje miały co robić)
    // Używamy deterministycznego ziarna dla powtarzalności
    // losowe liczby z zakresu 0 -> N-1
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, static_cast<int>(N - 1));

    // Wypełniamy ok. połowę zbioru
    for (size_t i = 0; i < N / 2; ++i) {
        s1.insert(dis(gen));
        s2.insert(dis(gen));
    }

    using namespace std::chrono;

    // 1. Badanie INSERT (Element-Zbiór)
    // Wykonujemy wiele operacji, aby zmierzyć średni czas, bo pojedyncza jest za szybka
    auto start = high_resolution_clock::now();
    int ops = 100000;
    for (int i = 0; i < ops; ++i) {
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
    runBasicSetTests();
    runMappingTests();
    runBenchmarks();

    std::cout << "\n=== PYTANIE O TYP DANYCH ===" << std::endl;
    std::cout << "Warunki dla typu danych elementow:" << std::endl;
    std::cout << "Aby mozna bylo zastosowac te implementacje (tablica boolowska), elementy musza byc:" << std::endl;
    std::cout << "1. Przeliczalne i mapowalne na liczby calkowite z zakresu [0, N-1]." << std::endl;
    std::cout << "2. Unikalne w sensie mapowania (funkcja mapujaca musi byc bijekcja w zakresie uzywanych wartosci)." << std::endl;
    std::cout << "Przyklady:" << std::endl;
    std::cout << " - liczby calkowite n,n+1,...,m mapowane przez mapIntegerRange" << std::endl;
    std::cout << " - liczby calkowite n,n+2,...,m mapowane przez mapIntegerRangeStep2" << std::endl;
    std::cout << " - litery 'a'..'z' mapowane przez mapCharLetter" << std::endl;
    std::cout << " - dwuliterowe napisy 'aa'..'zz' mapowane przez mapTwoLetterString" << std::endl;

    return 0;
}
