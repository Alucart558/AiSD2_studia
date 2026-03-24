#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <random>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <algorithm>
#include <cmath>

class SetSimple {
private:
    size_t N;
    std::vector<bool> table;

public:
    SetSimple(size_t n) : N(n), table(n, false) {}

    void insert(size_t x) {
        if (x < N) table[x] = true;
    }

    void remove(size_t x) {
        if (x < N) table[x] = false;
    }

    bool contains(size_t x) const {
        if (x < N) return table[x];
        return false;
    }

    SetSimple setUnion(const SetSimple& other) const {
        SetSimple result(N);
        for (size_t i = 0; i < N; ++i) {
            if (this->table[i] || other.table[i]) result.table[i] = true;
        }
        return result;
    }

    SetSimple setIntersection(const SetSimple& other) const {
        SetSimple result(N);
        for (size_t i = 0; i < N; ++i) {
            if (this->table[i] && other.table[i]) result.table[i] = true;
        }
        return result;
    }

    SetSimple setDifference(const SetSimple& other) const {
        SetSimple result(N);
        for (size_t i = 0; i < N; ++i) {
            if (this->table[i] && !other.table[i]) result.table[i] = true;
        }
        return result;
    }

    bool isEqual(const SetSimple& other) const {
        if (N != other.N) return false;
        for (size_t i = 0; i < N; ++i) {
            if (this->table[i] != other.table[i]) return false;
        }
        return true;
    }

    void print() const {
        std::cout << "{ ";
        for (size_t i = 0; i < N; ++i) {
            if (table[i]) std::cout << i << " ";
        }
        std::cout << "}" << std::endl;
    }

    size_t getSize() const { return N; }

    //mapuje liczby całkowite z przedziału [n, m] na indeksy [0, m-n].
    size_t mapIntegerRange(int x, int n, int m) const {
        if (n >= m) throw std::invalid_argument("mapIntegerRange: n must be < m");
        if (x < n || x > m) throw std::out_of_range("mapIntegerRange: integer out of [n, m]");
        return static_cast<size_t>(x - n);
    }

    //mapuje ciąg n, n+2, n+4, ..., m na indeksy 0..k.
    size_t mapIntegerRangeStep2(int x, int n, int m) const {
        if (n >= m) throw std::invalid_argument("mapIntegerRangeStep2: n must be < m");
        if ((m - n) % 2 != 0) throw std::invalid_argument("mapIntegerRangeStep2: m and n must have the same parity");
        if (x < n || x > m || ((x - n) % 2 != 0)) {
            throw std::out_of_range("mapIntegerRangeStep2: x not in {n, n+2, ..., m}");
        }
        return static_cast<size_t>((x - n) / 2);
    }

    //mapuje literę 'a'..'z' na 0..25.
    size_t mapCharLetter(char c) const {
        if (c < 'a' || c > 'z') throw std::out_of_range("mapCharLetter: expected 'a'..'z'");
        return static_cast<size_t>(c - 'a');
    }

    //mapuje napisy dwuliterowe "aa".."zz" na 0..675 (bo 26×26).
    size_t mapTwoLetterString(const std::string& str) const {
        if (str.length() != 2) throw std::out_of_range("mapTwoLetterString: string length must be 2");
        char c1 = str[0], c2 = str[1];
        if (c1 < 'a' || c1 > 'z' || c2 < 'a' || c2 > 'z') {
            throw std::out_of_range("mapTwoLetterString: expected letters 'a'..'z'");
        }
        return (static_cast<size_t>(c1 - 'a') * 26) + static_cast<size_t>(c2 - 'a');
    }
};

static void runBasicSetTests() {
    std::cout << "=== TESTY PODSTAWOWYCH OPERACJI ZBIOROWYCH ===\n";
    size_t N = 10;
    SetSimple A(N), B(N);

    A.insert(1); A.insert(3); A.insert(5);
    B.insert(3); B.insert(5); B.insert(7);

    std::cout << "Zbior A: "; A.print();
    std::cout << "Zbior B: "; B.print();

    std::cout << "Czy 1 w A? " << (A.contains(1) ? "TAK" : "NIE") << "\n";
    std::cout << "Czy 7 w A? " << (A.contains(7) ? "TAK" : "NIE") << "\n";

    SetSimple Sum = A.setUnion(B);
    std::cout << "Suma A U B: "; Sum.print();

    SetSimple Inter = A.setIntersection(B);
    std::cout << "Przeciecie A n B: "; Inter.print();

    SetSimple Diff = A.setDifference(B);
    std::cout << "Roznica A \\\\ B: "; Diff.print();

    std::cout << "Czy A == B? " << (A.isEqual(B) ? "TAK" : "NIE") << "\n";
    SetSimple A_clone = A;
    std::cout << "Czy A == A_clone? " << (A.isEqual(A_clone) ? "TAK" : "NIE") << "\n";
    std::cout << "=============================================\n\n";
}

static void runMappingTests() {
    std::cout << "=== TESTY FUNKCJI MAPUJACYCH ===\n";

    int n1 = -3, m1 = 2;
    size_t N1 = static_cast<size_t>(m1 - n1 + 1);
    SetSimple S1(N1);

    std::cout << "\n[MAPOWANIE LICZB n, n+1, ..., m]  n=" << n1 << " m=" << m1 << "\n";
    for (int x = n1; x <= m1; ++x) {
        size_t idx = S1.mapIntegerRange(x, n1, m1);
        std::cout << "x = " << x << " -> index = " << idx << "\n";
        S1.insert(idx);
    }
    std::cout << "Reprezentacja zbioru (indeksy): "; S1.print();

    int n2 = 3, m2 = 11;
    size_t N2 = static_cast<size_t>((m2 - n2) / 2 + 1);
    SetSimple S2(N2);

    std::cout << "\n[MAPOWANIE LICZB n, n+2, ..., m]  n=" << n2 << " m=" << m2 << "\n";
    for (int x = n2; x <= m2; x += 2) {
        size_t idx = S2.mapIntegerRangeStep2(x, n2, m2);
        std::cout << "x = " << x << " -> index = " << idx << "\n";
        S2.insert(idx);
    }
    std::cout << "Reprezentacja zbioru (indeksy): "; S2.print();

    size_t N3 = 26;
    SetSimple S3(N3);

    std::cout << "\n[MAPOWANIE LITER 'a'..'z']\n";
    for (char c = 'a'; c <= 'z'; ++c) {
        size_t idx = S3.mapCharLetter(c);
        if (c < 'd' || c > 'w') std::cout << "c = '" << c << "' -> index = " << idx << "\n";
        S3.insert(idx);
    }
    std::cout << "Reprezentacja zbioru (indeksy): "; S3.print();

    size_t N4 = 26 * 26;
    SetSimple S4(N4);

    std::cout << "\n[MAPOWANIE DWULITEROWYCH NAPISOW 'aa'..'zz']\n";
    const char* testWords[] = { "aa", "ab", "az", "ba", "zz" };
    for (const char* w : testWords) {
        std::string str(w);
        size_t idx = S4.mapTwoLetterString(str);
        std::cout << "str = \"" << str << "\" -> index = " << idx << "\n";
        S4.insert(idx);
    }

    std::cout << "Przykladowa reprezentacja zbioru (tylko kilka indeksow ustawionych): { ";
    size_t maxPrint = 30;
    for (size_t i = 0; i < N4 && i < maxPrint; ++i) {
        if (S4.contains(i)) std::cout << i << " ";
    }
    std::cout << "... }\n";
    std::cout << "=================================\n\n";
}

static inline uint64_t nowNs() {
    using namespace std::chrono;
    return duration_cast<nanoseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

static double median(std::vector<double>& v) {
    std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
    return v[v.size() / 2];
}

static void runBenchmarksCsv() {
    using clock = std::chrono::steady_clock;

    std::cout
        << "N,fill,"
        << "insert_ns_avg,contains_ns_avg,"
        << "union_us_avg,intersection_us_avg,difference_us_avg,"
        << "union_ns_per_elem,intersection_ns_per_elem,difference_ns_per_elem\n";

    std::mt19937 gen(42);

    const size_t N_start = 1000;
    const size_t N_end = 10000000;
    const int trials = 9;

    volatile size_t sink = 0; // \[FIX\] function-scope sink

    for (size_t N = N_start; N <= N_end; ) {
        SetSimple s1(N), s2(N);
        std::uniform_int_distribution<size_t> dis(0, N - 1);

        const size_t fill = N / 2;
        for (size_t i = 0; i < fill; ++i) {
            s1.insert(dis(gen));
            s2.insert(dis(gen));
        }

        const int insertOps = 500000;
        std::vector<size_t> insertIdx(insertOps);
        for (int i = 0; i < insertOps; ++i) insertIdx[i] = dis(gen);

        const int containsOps = 800000;
        std::vector<size_t> containsIdx(containsOps);
        for (int i = 0; i < containsOps; ++i) containsIdx[i] = dis(gen);

        for (int i = 0; i < 20000 && i < insertOps; ++i) s1.insert(insertIdx[i]);
        for (int i = 0; i < 20000 && i < containsOps; ++i) sink += s1.contains(containsIdx[i]);

        std::vector<double> insert_ns_trials;
        insert_ns_trials.reserve(trials);
        for (int t = 0; t < trials; ++t) {
            auto t0 = clock::now();
            for (int i = 0; i < insertOps; ++i) s1.insert(insertIdx[i]);
            auto t1 = clock::now();
            double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
            insert_ns_trials.push_back(ns / insertOps);
        }
        double insert_ns_avg = median(insert_ns_trials);

        std::vector<double> contains_ns_trials;
        contains_ns_trials.reserve(trials);
        for (int t = 0; t < trials; ++t) {
            auto t0 = clock::now();
            for (int i = 0; i < containsOps; ++i) sink += s1.contains(containsIdx[i]);
            auto t1 = clock::now();
            double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
            contains_ns_trials.push_back(ns / containsOps);
        }
        double contains_ns_avg = median(contains_ns_trials);

        int repeats = 5;
        if (N >= 1'000'000) repeats = 3;
        if (N >= 5'000'000) repeats = 2;

        auto measureSetOpUsAvg = [&](auto&& op) -> double {
            std::vector<double> us_trials;
            us_trials.reserve(trials);

            for (int t = 0; t < trials; ++t) {
                auto t0 = clock::now();
                for (int r = 0; r < repeats; ++r) {
                    volatile auto tmp = op();
                    (void)tmp;
                }
                auto t1 = clock::now();
                double us = std::chrono::duration<double, std::micro>(t1 - t0).count();
                us_trials.push_back(us / repeats);
            }
            return median(us_trials);
        };

        double union_us_avg = measureSetOpUsAvg([&]() { return s1.setUnion(s2); });
        double intersection_us_avg = measureSetOpUsAvg([&]() { return s1.setIntersection(s2); });
        double difference_us_avg = measureSetOpUsAvg([&]() { return s1.setDifference(s2); });

        double union_ns_per_elem = (union_us_avg * 1000.0) / static_cast<double>(N);
        double intersection_ns_per_elem = (intersection_us_avg * 1000.0) / static_cast<double>(N);
        double difference_ns_per_elem = (difference_us_avg * 1000.0) / static_cast<double>(N);

        std::cout << N << ","
                  << fill << ","
                  << std::fixed << std::setprecision(3)
                  << insert_ns_avg << ","
                  << contains_ns_avg << ","
                  << union_us_avg << ","
                  << intersection_us_avg << ","
                  << difference_us_avg << ","
                  << union_ns_per_elem << ","
                  << intersection_ns_per_elem << ","
                  << difference_ns_per_elem
                  << "\n";

        size_t nextN = static_cast<size_t>(static_cast<double>(N) * 1.2);
        if (nextN <= N) nextN = N + 1;
        N = nextN;
    }

    (void)sink;
}

int main() {
    runBasicSetTests();
    runMappingTests();
    runBenchmarksCsv();

    std::cout << "\n=== PYTANIE O TYP DANYCH ===\n";
    std::cout << "Warunki dla typu danych elementow:\n";
    std::cout << "Aby mozna bylo zastosowac te implementacje (tablica boolowska), elementy musza byc:\n";
    std::cout << "1. Przeliczalne i mapowalne na liczby calkowite z zakresu [0, N-1].\n";
    std::cout << "2. Unikalne w sensie mapowania (funkcja mapujaca musi byc bijekcja w zakresie uzywanych wartosci).\n";
    std::cout << "Przyklady:\n";
    std::cout << " - liczby calkowite n,n+1,...,m mapowane przez mapIntegerRange\n";
    std::cout << " - liczby calkowite n,n+2,...,m mapowane przez mapIntegerRangeStep2\n";
    std::cout << " - litery 'a'..'z' mapowane przez mapCharLetter\n";
    std::cout << " - dwuliterowe napisy 'aa'..'zz' mapowane przez mapTwoLetterString\n";

    return 0;
}