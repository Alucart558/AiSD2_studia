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

    void insert(int x) {
        auto& b = buckets[hash(x)];
        auto it = std::lower_bound(b.begin(), b.end(), x);
        if (it != b.end() && *it == x) return;
        b.insert(it, x);
        ++elementCount;
    }

    void erase(int x) {
        auto& b = buckets[hash(x)];
        auto it = std::lower_bound(b.begin(), b.end(), x);
        if (it == b.end() || *it != x) return;
        b.erase(it);
        --elementCount;
    }

    bool contains(int x) const {
        const auto& b = buckets[hash(x)];
        auto it = std::lower_bound(b.begin(), b.end(), x);
        return it != b.end() && *it == x;
    }

    std::size_t size() const { return elementCount; }

    void clear() {
        for (auto& b : buckets) b.clear();
        elementCount = 0;
    }

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

    std::size_t hash(int x) const {
        long long v = x;
        long long mod = static_cast<long long>(N);
        long long r = v % mod;
        if (r < 0) r += mod;
        return static_cast<std::size_t>(r);
    }
};

setHashed set_union(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) throw std::runtime_error("Bucket counts differ in set_union");
    setHashed result(a.N);

    for (std::size_t i = 0; i < a.N; ++i) {
        const auto& A = a.buckets[i];
        const auto& B = b.buckets[i];
        auto& R = result.buckets[i];

        R.reserve(A.size() + B.size());
        std::set_union(A.begin(), A.end(), B.begin(), B.end(), std::back_inserter(R));
        result.elementCount += R.size();
    }
    return result;
}

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

bool set_equal(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) return false;
    if (a.elementCount != b.elementCount) return false;
    return a.buckets == b.buckets;
}

static void simple_demo() {
    const std::size_t BUCKETS = 7;
    setHashed A(BUCKETS), B(BUCKETS);

    A.insert(1);
    A.insert(2);
    A.insert(5);
    A.insert(10);
    A.insert(20);
    B.insert(2);
    B.insert(3);
    B.insert(5);
    B.insert(7);
    B.insert(20);

    std::cout << "A = ";
    A.print();
    std::cout << "\n";
    std::cout << "B = ";
    B.print();
    std::cout << "\n";

    setHashed U = set_union(A, B);
    setHashed I = set_intersection(A, B);
    setHashed D = set_difference(A, B);

    std::cout << "A (U) B = ";
    U.print();
    std::cout << "\n";
    std::cout << "A (I) B = ";
    I.print();
    std::cout << "\n";
    std::cout << "A \\ B = ";
    D.print();
    std::cout << "\n";

    std::cout << "contains 5 in A: " << (A.contains(5) ? "YES" : "NO") << "\n";
    std::cout << "contains 4 in A: " << (A.contains(4) ? "YES" : "NO") << "\n";

    setHashed C = A;
    std::cout << "C = A, set_equal(A, C) = " << (set_equal(A, C) ? "YES" : "NO") << "\n";
    C.erase(1);
    std::cout << "After erase 1 from C: set_equal(A, C) = " << (set_equal(A, C) ? "YES" : "NO") << "\n";
}

static void write_one_gnuplot_script(
    const std::filesystem::path& outDir,
    const std::string& baseName,
    const std::string& title
) {
    std::ofstream gp(outDir / (baseName + ".gp"));
    gp << "set terminal pngcairo size 1200,800\n";
    gp << "set output '" << baseName << ".png'\n";
    gp << "set title '" << title << "'\n";
    gp << "set xlabel 'n (elements)'\n";
    gp << "set ylabel 'time (ns)'\n";
    gp << "set grid\n";
    gp << "plot '" << baseName << ".dat' using 1:2 with lines lw 2 title '" << baseName << "'\n";
}

static void write_gnuplot_scripts(const std::filesystem::path& outDir) {
    write_one_gnuplot_script(outDir, "insert", "Average time: insert");
    write_one_gnuplot_script(outDir, "contains", "Average time: contains");
    write_one_gnuplot_script(outDir, "union", "Average time: union");
    write_one_gnuplot_script(outDir, "intersection", "Average time: intersection");
    write_one_gnuplot_script(outDir, "difference", "Average time: difference");
    write_one_gnuplot_script(outDir, "equal", "Average time: equal");
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

    auto build_sets = [&](int n, setHashed& a, setHashed& b, std::vector<int>& insertedA) {
        a.clear();
        b.clear();
        insertedA.clear();
        insertedA.reserve(static_cast<std::size_t>(n));

        for (int i = 0; i < n; ++i) {
            int x = dist(rng);
            a.insert(x);
            insertedA.push_back(x);
        }
        for (int i = 0; i < n; ++i) {
            int x = dist(rng);
            b.insert(x);
        }
    };

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

    // UNION benchmark -> data/union.dat
    {
        std::ofstream out(outDir / "union.dat");
        out << "# n avg_time_ns\n";

        for (int n = nStart; n <= nEnd; n += nStep) {
            setHashed A(BUCKETS), B(BUCKETS);
            std::vector<int> insertedA;
            build_sets(n, A, B, insertedA);

            for (int t = 0; t < warmup; ++t) {
                volatile std::size_t sz = set_union(A, B).size();
                (void)sz;
            }

            long long totalNs = 0;
            for (int t = 0; t < trials; ++t) {
                auto start = std::chrono::steady_clock::now();
                setHashed U = set_union(A, B);
                auto end = std::chrono::steady_clock::now();
                volatile std::size_t sz = U.size();
                (void)sz;

                totalNs += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            }

            double avgNs = static_cast<double>(totalNs) / trials;
            out << n << " " << avgNs << "\n";
            std::cout << "[union] n=" << n << " avg=" << avgNs << " ns\n";
        }
    }

    // INTERSECTION benchmark -> data/intersection.dat
    {
        std::ofstream out(outDir / "intersection.dat");
        out << "# n avg_time_ns\n";

        for (int n = nStart; n <= nEnd; n += nStep) {
            setHashed A(BUCKETS), B(BUCKETS);
            std::vector<int> insertedA;
            build_sets(n, A, B, insertedA);

            for (int t = 0; t < warmup; ++t) {
                volatile std::size_t sz = set_intersection(A, B).size();
                (void)sz;
            }

            long long totalNs = 0;
            for (int t = 0; t < trials; ++t) {
                auto start = std::chrono::steady_clock::now();
                setHashed I = set_intersection(A, B);
                auto end = std::chrono::steady_clock::now();
                volatile std::size_t sz = I.size();
                (void)sz;

                totalNs += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            }

            double avgNs = static_cast<double>(totalNs) / trials;
            out << n << " " << avgNs << "\n";
            std::cout << "[intersection] n=" << n << " avg=" << avgNs << " ns\n";
        }
    }

    // DIFFERENCE benchmark -> data/difference.dat
    {
        std::ofstream out(outDir / "difference.dat");
        out << "# n avg_time_ns\n";

        for (int n = nStart; n <= nEnd; n += nStep) {
            setHashed A(BUCKETS), B(BUCKETS);
            std::vector<int> insertedA;
            build_sets(n, A, B, insertedA);

            for (int t = 0; t < warmup; ++t) {
                volatile std::size_t sz = set_difference(A, B).size();
                (void)sz;
            }

            long long totalNs = 0;
            for (int t = 0; t < trials; ++t) {
                auto start = std::chrono::steady_clock::now();
                setHashed D = set_difference(A, B);
                auto end = std::chrono::steady_clock::now();
                volatile std::size_t sz = D.size();
                (void)sz;

                totalNs += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            }

            double avgNs = static_cast<double>(totalNs) / trials;
            out << n << " " << avgNs << "\n";
            std::cout << "[difference] n=" << n << " avg=" << avgNs << " ns\n";
        }
    }

    // EQUAL benchmark -> data/equal.dat
    {
        std::ofstream out(outDir / "equal.dat");
        out << "# n avg_time_ns\n";

        for (int n = nStart; n <= nEnd; n += nStep) {
            setHashed A(BUCKETS), B(BUCKETS);
            std::vector<int> insertedA;
            build_sets(n, A, B, insertedA);

            // Make half the time equal\(\), half not equal\(\) to avoid branch bias.
            setHashed Bsame = A;

            for (int t = 0; t < warmup; ++t) {
                const setHashed& rhs = (t % 2 == 0) ? Bsame : B;
                volatile bool eq = set_equal(A, rhs);
                (void)eq;
            }

            long long totalNs = 0;
            for (int t = 0; t < trials; ++t) {
                const setHashed& rhs = (t % 2 == 0) ? Bsame : B;

                auto start = std::chrono::steady_clock::now();
                volatile bool eq = set_equal(A, rhs);
                (void)eq;
                auto end = std::chrono::steady_clock::now();

                totalNs += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            }

            double avgNs = static_cast<double>(totalNs) / trials;
            out << n << " " << avgNs << "\n";
            std::cout << "[equal] n=" << n << " avg=" << avgNs << " ns\n";
        }
    }

    write_gnuplot_scripts(outDir);

    std::cout << "\nSaved:\n";
    std::cout << "  " << (outDir / "insert.dat").string() << "\n";
    std::cout << "  " << (outDir / "contains.dat").string() << "\n";
    std::cout << "  " << (outDir / "union.dat").string() << "\n";
    std::cout << "  " << (outDir / "intersection.dat").string() << "\n";
    std::cout << "  " << (outDir / "difference.dat").string() << "\n";
    std::cout << "  " << (outDir / "equal.dat").string() << "\n";

    std::cout << "  " << (outDir / "insert.gp").string() << "\n";
    std::cout << "  " << (outDir / "contains.gp").string() << "\n";
    std::cout << "  " << (outDir / "union.gp").string() << "\n";
    std::cout << "  " << (outDir / "intersection.gp").string() << "\n";
    std::cout << "  " << (outDir / "difference.gp").string() << "\n";
    std::cout << "  " << (outDir / "equal.gp").string() << "\n";
}

int main() {
    std::cout << "Demo of hashed set:\n";
    simple_demo();

    std::cout << "\nRunning benchmark and generating gnuplot data...\n";
    benchmark_example();

    std::cout << "\nTo generate PNG charts with gnuplot, run in `data/`:\n";
    std::cout << "  gnuplot insert.gp\n";
    std::cout << "  gnuplot contains.gp\n";
    std::cout << "  gnuplot union.gp\n";
    std::cout << "  gnuplot intersection.gp\n";
    std::cout << "  gnuplot difference.gp\n";
    std::cout << "  gnuplot equal.gp\n";
    return 0;
}