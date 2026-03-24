#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

class setHashed {
public:
    explicit setHashed(std::size_t bucketCount)
        : N(bucketCount), buckets(bucketCount), elementCount(0) {
        if (N == 0) throw std::invalid_argument("bucketCount must be > 0");
    }

    // Wstawia element x, jeśli go nie ma.
    // Kubełek jest utrzymywany w porządku rosnącym.
    void insert(int x) {
        //pokazuje konktetny bucket
        auto& b = buckets[hash(x)];
        //szuka miejsca w kubełku
        auto it = std::lower_bound(b.begin(), b.end(), x);
        if (it != b.end() && *it == x) return;   // już jest
        b.insert(it, x);
        ++elementCount;
    }

    // Usuwa element x, jeśli istnieje.
    void erase(int x) {
        auto& b = buckets[hash(x)];
        auto it = std::lower_bound(b.begin(), b.end(), x);
        if (it == b.end() || *it != x) return;   // brak
        b.erase(it);
        --elementCount;
    }

    // Sprawdza, czy element x jest w zbiorze.
    bool contains(int x) const {
        const auto& b = buckets[hash(x)];
        auto it = std::lower_bound(b.begin(), b.end(), x);
        return it != b.end() && *it == x;
    }

    // Zwraca liczbę elementów w zbiorze.
    std::size_t size() const { return elementCount; }

    // Czyści cały zbiór.
    void clear() {
        for (auto& b : buckets) b.clear();
        elementCount = 0;
    }

    // Wypisuje zbiór w kolejności kubełków (nie globalnie posortowany).
    void print(std::ostream& os = std::cout) const {
        os << "{ ";
        bool first = true;
        for (const auto& b : buckets) {
            for (int v : b) {
                if (!first) os << ", ";
                os << v;
                first = false;
            }
        }
        os << " }";
    }

    friend setHashed set_union(const setHashed& a, const setHashed& b);
    friend setHashed set_intersection(const setHashed& a, const setHashed& b);
    friend setHashed set_difference(const setHashed& a, const setHashed& b);
    friend bool set_equal(const setHashed& a, const setHashed& b);

private:
    std::size_t N;
    std::vector<std::vector<int>> buckets;
    std::size_t elementCount;

    // Hash: bezpieczny dla liczb ujemnych (zawsze wynik 0..N-1).
    std::size_t hash(int x) const {
        long long v = x;
        long long mod = static_cast<long long>(N);
        long long r = v % mod;
        if (r < 0) r += mod;
        return static_cast<std::size_t>(r);
    }
};

// Suma zbiorów: elementy z a lub b.
// Robimy osobno na każdym kubełku, bo oba zbiory mają ten sam N i hash.
setHashed set_union(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) throw std::runtime_error("Bucket counts differ in set_union");
    //tworzymy pusty bucket
    setHashed result(a.N);

    for (std::size_t i = 0; i < a.N; ++i) {
        const auto& A = a.buckets[i];
        const auto& B = b.buckets[i];
        auto& R = result.buckets[i];

        R.reserve(A.size() + B.size());
        //merguje 2 zbiory
        std::set_union(A.begin(), A.end(), B.begin(), B.end(), std::back_inserter(R));
        result.elementCount += R.size();
    }
    return result;
}

// Część wspólna: elementy, które są w a i w b.
setHashed set_intersection(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) throw std::runtime_error("Bucket counts differ in set_intersection");
    setHashed result(a.N);

    for (std::size_t i = 0; i < a.N; ++i) {
        const auto& A = a.buckets[i];
        const auto& B = b.buckets[i];
        auto& R = result.buckets[i];

        R.reserve(std::min(A.size(), B.size()));
        std::set_intersection(A.begin(), A.end(), B.begin(), B.end(), std::back_inserter(R));
        result.elementCount += R.size();
    }
    return result;
}

// Różnica: elementy, które są w a, ale nie ma ich w b.
setHashed set_difference(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) throw std::runtime_error("Bucket counts differ in set_difference");
    setHashed result(a.N);

    for (std::size_t i = 0; i < a.N; ++i) {
        const auto& A = a.buckets[i];
        const auto& B = b.buckets[i];
        auto& R = result.buckets[i];

        R.reserve(A.size());
        std::set_difference(A.begin(), A.end(), B.begin(), B.end(), std::back_inserter(R));
        result.elementCount += R.size();
    }
    return result;
}

// Równość zbiorów: porównujemy rozmiar i zawartość kubełków.
bool set_equal(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) return false;
    if (a.elementCount != b.elementCount) return false;
    return a.buckets == b.buckets;
}

static void simple_demo() {
    const std::size_t BUCKETS = 7;
    setHashed A(BUCKETS), B(BUCKETS);

    A.insert(1); A.insert(2); A.insert(5); A.insert(10); A.insert(20);
    B.insert(2); B.insert(3); B.insert(5); B.insert(7);  B.insert(20);

    std::cout << "A = "; A.print(); std::cout << "\n";
    std::cout << "B = "; B.print(); std::cout << "\n";

    setHashed U = set_union(A, B);
    setHashed I = set_intersection(A, B);
    setHashed D = set_difference(A, B);

    std::cout << "A (U) B = "; U.print(); std::cout << "\n";
    std::cout << "A (I) B = "; I.print(); std::cout << "\n";
    std::cout << "A \\ B = "; D.print(); std::cout << "\n";

    std::cout << "contains 5 in A: " << (A.contains(5) ? "YES" : "NO") << "\n";
    std::cout << "contains 4 in A: " << (A.contains(4) ? "YES" : "NO") << "\n";

    setHashed C = A;
    std::cout << "C = A, set_equal(A, C) = " << (set_equal(A, C) ? "YES" : "NO") << "\n";
    C.erase(1);
    std::cout << "After erase 1 from C: set_equal(A, C) = " << (set_equal(A, C) ? "YES" : "NO") << "\n";
}

static void write_gnuplot_scripts(const std::filesystem::path& outDir) {
    {
        std::ofstream gp(outDir / "insert.gp");
        gp
            << "set terminal pngcairo size 1200,800\n"
            << "set output 'insert.png'\n"
            << "set title 'Average time: insert'\n"
            << "set xlabel 'n (elements)'\n"
            << "set ylabel 'time (ns)'\n"
            << "set grid\n"
            << "plot 'insert.dat' using 1:2 with lines lw 2 title 'insert'\n";
    }
    {
        std::ofstream gp(outDir / "contains.gp");
        gp
            << "set terminal pngcairo size 1200,800\n"
            << "set output 'contains.png'\n"
            << "set title 'Average time: contains'\n"
            << "set xlabel 'n (elements)'\n"
            << "set ylabel 'time (ns)'\n"
            << "set grid\n"
            << "plot 'contains.dat' using 1:2 with lines lw 2 title 'contains'\n";
    }
}

static void benchmark_example() {
    namespace fs = std::filesystem;

    const std::size_t BUCKETS = 1009;
    const int nStart = 1000;
    const int nEnd = 20000;
    const int nStep = 1000;

    const int trials = 2000;
    const int warmup = 200;

    std::mt19937 rng(123456);
    std::uniform_int_distribution<int> dist(0, 1000000000);

    fs::path outDir = fs::current_path() / "data";
    fs::create_directories(outDir);

    // INSERT benchmark -> data/insert.dat
    {
        std::ofstream out(outDir / "insert.dat");
        out << "# n avg_time_ns\n";

        for (int n = nStart; n <= nEnd; n += nStep) {
            setHashed s(BUCKETS);
            for (int i = 0; i < n; ++i) s.insert(dist(rng));

            for (int t = 0; t < warmup; ++t) {
                int x = dist(rng);
                s.insert(x);
                s.erase(x);
            }

            long long totalNs = 0;
            for (int t = 0; t < trials; ++t) {
                int x = dist(rng);
                auto start = std::chrono::steady_clock::now();
                s.insert(x);
                auto end = std::chrono::steady_clock::now();
                s.erase(x);
                totalNs += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            }

            double avgNs = static_cast<double>(totalNs) / trials;
            out << n << " " << avgNs << "\n";
            std::cout << "[insert] n=" << n << " avg=" << avgNs << " ns\n";
        }
    }

    // CONTAINS benchmark -> data/contains.dat
    {
        std::ofstream out(outDir / "contains.dat");
        out << "# n avg_time_ns\n";

        for (int n = nStart; n <= nEnd; n += nStep) {
            setHashed s(BUCKETS);
            std::vector<int> inserted;
            inserted.reserve(static_cast<std::size_t>(n));

            for (int i = 0; i < n; ++i) {
                int x = dist(rng);
                s.insert(x);
                inserted.push_back(x);
            }

            for (int t = 0; t < warmup; ++t) {
                int x = (t % 2 == 0) ? inserted[static_cast<std::size_t>(t) % inserted.size()] : dist(rng);
                volatile bool res = s.contains(x);
                (void)res;
            }

            long long totalNs = 0;
            for (int t = 0; t < trials; ++t) {
                int x = (t % 2 == 0)
                            ? inserted[static_cast<std::size_t>(t) % inserted.size()]
                            : dist(rng);

                auto start = std::chrono::steady_clock::now();
                volatile bool res = s.contains(x);
                (void)res;
                auto end = std::chrono::steady_clock::now();

                totalNs += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            }

            double avgNs = static_cast<double>(totalNs) / trials;
            out << n << " " << avgNs << "\n";
            std::cout << "[contains] n=" << n << " avg=" << avgNs << " ns\n";
        }
    }

    write_gnuplot_scripts(outDir);

    std::cout << "\nSaved:\n";
    std::cout << "  " << (outDir / "insert.dat").string() << "\n";
    std::cout << "  " << (outDir / "contains.dat").string() << "\n";
    std::cout << "  " << (outDir / "insert.gp").string() << "\n";
    std::cout << "  " << (outDir / "contains.gp").string() << "\n";
}

int main() {
    std::cout << "Demo of hashed set:\n";
    simple_demo();

    std::cout << "\nRunning benchmark and generating gnuplot data...\n";
    benchmark_example();

    std::cout << "\nTo generate PNG charts with gnuplot, run in `data/`:\n";
    std::cout << "  gnuplot insert.gp\n";
    std::cout << "  gnuplot contains.gp\n";
    return 0;
}