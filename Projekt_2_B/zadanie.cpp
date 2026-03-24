#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <string>
#include <algorithm>

class setHashed {
public:
    explicit setHashed(std::size_t bucketCount)
        : N(bucketCount), buckets(bucketCount, nullptr), elementCount(0) {
        if (N == 0) throw std::invalid_argument("bucketCount must be > 0");
    }

    ~setHashed() { clear(); }

    setHashed(const setHashed& other)
        : N(other.N), buckets(other.N, nullptr), elementCount(0) {
        for (std::size_t i = 0; i < N; ++i) {
            Node* p = other.buckets[i];
            Node* prevNew = nullptr;
            while (p) {
                Node* n = new Node(p->value);
                if (!prevNew) buckets[i] = n;
                else prevNew->next = n;
                prevNew = n;
                p = p->next;
                ++elementCount;
            }
        }
    }

    setHashed& operator=(const setHashed& other) {
        if (this == &other) return *this;
        clear();
        N = other.N;
        buckets.assign(N, nullptr);
        elementCount = 0;

        for (std::size_t i = 0; i < N; ++i) {
            Node* p = other.buckets[i];
            Node* prevNew = nullptr;
            while (p) {
                Node* n = new Node(p->value);
                if (!prevNew) buckets[i] = n;
                else prevNew->next = n;
                prevNew = n;
                p = p->next;
                ++elementCount;
            }
        }
        return *this;
    }

    // Optional: move for efficiency in benchmarks
    setHashed(setHashed&& other) noexcept
        : N(other.N), buckets(std::move(other.buckets)), elementCount(other.elementCount) {
        other.N = 0;
        other.elementCount = 0;
        // buckets moved; leave other in safe destructible state
    }

    setHashed& operator=(setHashed&& other) noexcept {
        if (this == &other) return *this;
        clear();
        N = other.N;
        buckets = std::move(other.buckets);
        elementCount = other.elementCount;
        other.N = 0;
        other.elementCount = 0;
        return *this;
    }

    void insert(int x) {
        std::size_t idx = hash(x);
        Node*& head = buckets[idx];

        if (!head) {
            head = new Node(x);
            ++elementCount;
            return;
        }

        if (x < head->value) {
            head = new Node(x, head);
            ++elementCount;
            return;
        }
        if (x == head->value) return;

        Node* prev = head;
        Node* curr = head->next;
        while (curr && curr->value < x) {
            prev = curr;
            curr = curr->next;
        }
        if (curr && curr->value == x) return;

        prev->next = new Node(x, curr);
        ++elementCount;
    }

    void erase(int x) {
        std::size_t idx = hash(x);
        Node*& head = buckets[idx];
        if (!head) return;

        if (head->value == x) {
            Node* tmp = head;
            head = head->next;
            delete tmp;
            --elementCount;
            return;
        }

        Node* prev = head;
        Node* curr = head->next;
        while (curr && curr->value < x) {
            prev = curr;
            curr = curr->next;
        }
        if (curr && curr->value == x) {
            prev->next = curr->next;
            delete curr;
            --elementCount;
        }
    }

    bool contains(int x) const {
        std::size_t idx = hash(x);
        Node* curr = buckets[idx];
        while (curr && curr->value < x) curr = curr->next;
        return curr && curr->value == x;
    }

    std::size_t size() const { return elementCount; }

    void clear() {
        // If N==0 \(\) after move, nothing to do
        for (std::size_t i = 0; i < buckets.size(); ++i) {
            Node* curr = buckets[i];
            while (curr) {
                Node* tmp = curr;
                curr = curr->next;
                delete tmp;
            }
            buckets[i] = nullptr;
        }
        elementCount = 0;
    }

    void print(std::ostream& os = std::cout) const {
        os << "{ ";
        bool first = true;
        for (std::size_t i = 0; i < buckets.size(); ++i) {
            Node* curr = buckets[i];
            while (curr) {
                if (!first) os << ", ";
                os << curr->value;
                first = false;
                curr = curr->next;
            }
        }
        os << " }";
    }

    friend setHashed set_union(const setHashed& a, const setHashed& b);
    friend setHashed set_intersection(const setHashed& a, const setHashed& b);
    friend setHashed set_difference(const setHashed& a, const setHashed& b);
    friend bool set_equal(const setHashed& a, const setHashed& b);

private:
    struct Node {
        int value;
        Node* next;
        Node(int v, Node* n = nullptr) : value(v), next(n) {}
    };

    std::size_t N;
    std::vector<Node*> buckets;
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
        setHashed::Node* p = a.buckets[i];
        setHashed::Node* q = b.buckets[i];
        setHashed::Node** tail = &result.buckets[i];

        while (p || q) {
            int val;
            if (!q || (p && p->value < q->value)) {
                val = p->value;
                p = p->next;
            } else if (!p || (q && q->value < p->value)) {
                val = q->value;
                q = q->next;
            } else {
                val = p->value;
                p = p->next;
                q = q->next;
            }
            *tail = new setHashed::Node(val);
            tail = &((*tail)->next);
            ++result.elementCount;
        }
    }
    return result;
}

setHashed set_intersection(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) throw std::runtime_error("Bucket counts differ in set_intersection");
    setHashed result(a.N);

    for (std::size_t i = 0; i < a.N; ++i) {
        setHashed::Node* p = a.buckets[i];
        setHashed::Node* q = b.buckets[i];
        setHashed::Node** tail = &result.buckets[i];

        while (p && q) {
            if (p->value == q->value) {
                *tail = new setHashed::Node(p->value);
                tail = &((*tail)->next);
                ++result.elementCount;
                p = p->next;
                q = q->next;
            } else if (p->value < q->value) {
                p = p->next;
            } else {
                q = q->next;
            }
        }
    }
    return result;
}

setHashed set_difference(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) throw std::runtime_error("Bucket counts differ in set_difference");
    setHashed result(a.N);

    for (std::size_t i = 0; i < a.N; ++i) {
        setHashed::Node* p = a.buckets[i];
        setHashed::Node* q = b.buckets[i];
        setHashed::Node** tail = &result.buckets[i];

        while (p) {
            while (q && q->value < p->value) q = q->next;
            if (!q || q->value != p->value) {
                *tail = new setHashed::Node(p->value);
                tail = &((*tail)->next);
                ++result.elementCount;
            }
            p = p->next;
        }
    }
    return result;
}

bool set_equal(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) return false;
    if (a.elementCount != b.elementCount) return false;

    for (std::size_t i = 0; i < a.N; ++i) {
        setHashed::Node* p = a.buckets[i];
        setHashed::Node* q = b.buckets[i];
        while (p && q) {
            if (p->value != q->value) return false;
            p = p->next;
            q = q->next;
        }
        if (p || q) return false;
    }
    return true;
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

    std::cout << "A \\(U\\) B = "; U.print(); std::cout << "\n";
    std::cout << "A \\(I\\) B = "; I.print(); std::cout << "\n";
    std::cout << "A \\\\ B = "; D.print(); std::cout << "\n";

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
            << "set xlabel 'n \\(elements\\)'\n"
            << "set ylabel 'time \\(ns\\)'\n"
            << "set grid\n"
            << "plot 'insert.dat' using 1:2 with lines lw 2 title 'insert'\n";
    }
    {
        std::ofstream gp(outDir / "contains.gp");
        gp
            << "set terminal pngcairo size 1200,800\n"
            << "set output 'contains.png'\n"
            << "set title 'Average time: contains'\n"
            << "set xlabel 'n \\(elements\\)'\n"
            << "set ylabel 'time \\(ns\\)'\n"
            << "set grid\n"
            << "plot 'contains.dat' using 1:2 with lines lw 2 title 'contains'\n";
    }
}

static void benchmark_example() {
    namespace fs = std::filesystem;

    const std::size_t BUCKETS = 1009; // prime, reduces clustering a bit
    const int nStart = 1000;
    const int nEnd = 20000;
    const int nStep = 1000;

    const int trials = 2000;
    const int warmup = 200;

    std::mt19937 rng(123456);
    std::uniform_int_distribution<int> dist(0, 1000000000);

    fs::path outDir = fs::current_path() / "data";
    fs::create_directories(outDir);

    // INSERT benchmark -> `data/insert.dat`
    {
        std::ofstream out(outDir / "insert.dat");
        out << "# n avg_time_ns\n";

        for (int n = nStart; n <= nEnd; n += nStep) {
            setHashed s(BUCKETS);
            for (int i = 0; i < n; ++i) s.insert(dist(rng));

            // warmup
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

    // CONTAINS benchmark -> `data/contains.dat`
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

            // warmup
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