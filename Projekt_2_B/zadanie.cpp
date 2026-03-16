#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <stdexcept>

/*
 * setHashed – implementacja zbioru liczb całkowitych
 * --------------------------------------------------
 * - Zbiór jest reprezentowany jako tablica kubełków (bucketów).
 * - Każdy kubełek to POSORTOWANA lista jednokierunkowa, bez duplikatów.
 * - Funkcja haszująca: bucket(x) = x % N (zabezpieczone przed liczbami ujemnymi).
 *
 * Zaimplementowane operacje:
 *  - insert(x)        – wstawianie elementu do zbioru
 *  - erase(x)         – usuwanie elementu ze zbioru
 *  - contains(x)      – sprawdzenie, czy element należy do zbioru
 *  - clear()          – czyszczenie zbioru
 *  - size()           – liczba elementów w zbiorze
 *
 * Operacje na dwóch zbiorach (jako funkcje zaprzyjaźnione):
 *  - set_union(a,b)         – suma zbiorów
 *  - set_intersection(a,b)  – część wspólna
 *  - set_difference(a,b)    – różnica (a \ b)
 *  - set_equal(a,b)         – sprawdzenie identyczności zbiorów
 *
 * W funkcji main:
 *  - proste testy poprawności,
 *  - przykładowy pomiar czasu insert/contains i zapis plików .dat dla gnuplota.
 */

class setHashed {
public:
    // Konstruktor – tworzy zbiór z daną liczbą kubełków
    explicit setHashed(std::size_t bucketCount)
        : N(bucketCount), buckets(bucketCount, nullptr), elementCount(0) {}

    // Destruktor – zwalnia pamięć
    ~setHashed() {
        clear();
    }

    // Konstruktor kopiujący – głębokie kopiowanie list w kubełkach
    setHashed(const setHashed& other)
        : N(other.N), buckets(other.N, nullptr), elementCount(0) {
        for (std::size_t i = 0; i < N; ++i) {
            Node* p = other.buckets[i];
            Node* prevNew = nullptr;
            while (p) {
                Node* n = new Node(p->value);
                if (!prevNew) {
                    buckets[i] = n;
                } else {
                    prevNew->next = n;
                }
                prevNew = n;
                p = p->next;
                ++elementCount;
            }
        }
    }

    // Operator przypisania – głębokie kopiowanie
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
                if (!prevNew) {
                    buckets[i] = n;
                } else {
                    prevNew->next = n;
                }
                prevNew = n;
                p = p->next;
                ++elementCount;
            }
        }
        return *this;
    }

    // Wstawianie elementu x do zbioru
    void insert(int x) {
        std::size_t idx = hash(x);
        Node*& head = buckets[idx]; // referencja do wskaźnika na głowę listy

        // kubełek pusty
        if (!head) {
            head = new Node(x);
            ++elementCount;
            return;
        }

        // wstawianie przed pierwszy element
        if (x < head->value) {
            Node* n = new Node(x, head);
            head = n;
            ++elementCount;
            return;
        }
        // jeśli element już jest na początku – nic nie robimy
        if (x == head->value) {
            return;
        }

        // szukamy miejsca w środku / na końcu listy
        Node* prev = head;
        Node* curr = head->next;
        while (curr && curr->value < x) {
            prev = curr;
            curr = curr->next;
        }
        // jeżeli element już istnieje – nie duplikujemy
        if (curr && curr->value == x) {
            return;
        }
        // wstawiamy nowy węzeł pomiędzy prev a curr
        Node* n = new Node(x, curr);
        prev->next = n;
        ++elementCount;
    }

    // Usuwanie elementu x ze zbioru (jeśli istnieje)
    void erase(int x) {
        std::size_t idx = hash(x);
        Node*& head = buckets[idx];

        if (!head) return;

        // usunięcie elementu z początku listy
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

    // Sprawdzenie, czy x należy do zbioru
    bool contains(int x) const {
        std::size_t idx = hash(x);
        Node* curr = buckets[idx];

        while (curr && curr->value < x) {
            curr = curr->next;
        }
        return curr && curr->value == x;
    }

    // Liczba elementów w zbiorze
    std::size_t size() const {
        return elementCount;
    }

    // Wyczyszczenie zbioru (zwolnienie pamięci i ustawienie pustych kubełków)
    void clear() {
        for (std::size_t i = 0; i < N; ++i) {
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

    // Proste wypisanie zawartości (do debugowania)
    void print(std::ostream& os = std::cout) const {
        os << "{ ";
        bool first = true;
        for (std::size_t i = 0; i < N; ++i) {
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

    // Funkcje zaprzyjaźnione – operacje na dwóch zbiorach
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

    std::size_t N;               // liczba kubełków
    std::vector<Node*> buckets;  // tablica kubełków
    std::size_t elementCount;    // liczba elementów w zbiorze

    // Funkcja haszująca – wynik zawsze w [0, N-1]
    std::size_t hash(int x) const {
        long long v = x;
        long long mod = static_cast<long long>(N);
        long long r = v % mod;
        if (r < 0) r += mod;
        return static_cast<std::size_t>(r);
    }
};

// SUMA zbiorów: A ∪ B
setHashed set_union(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) {
        throw std::runtime_error("Bucket counts differ in set_union");
    }
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
            } else { // p->value == q->value
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

// CZĘŚĆ WSPÓLNA: A ∩ B
setHashed set_intersection(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) {
        throw std::runtime_error("Bucket counts differ in set_intersection");
    }
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

// RÓŻNICA: A \ B
setHashed set_difference(const setHashed& a, const setHashed& b) {
    if (a.N != b.N) {
        throw std::runtime_error("Bucket counts differ in set_difference");
    }
    setHashed result(a.N);

    for (std::size_t i = 0; i < a.N; ++i) {
        setHashed::Node* p = a.buckets[i];
        setHashed::Node* q = b.buckets[i];
        setHashed::Node** tail = &result.buckets[i];

        while (p) {
            while (q && q->value < p->value) {
                q = q->next;
            }
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

// SPRAWDZANIE IDENTYCZNOŚCI ZBIORÓW
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
        if (p || q) return false; // różna długość listy w kubełku
    }
    return true;
}

/*
 * Funkcja pomocnicza: prosty test poprawności operacji na zbiorach.
 */
void simple_demo() {
    const std::size_t BUCKETS = 7;
    setHashed A(BUCKETS);
    setHashed B(BUCKETS);

    // A = {1, 2, 5, 10, 20}
    A.insert(1);
    A.insert(2);
    A.insert(5);
    A.insert(10);
    A.insert(20);

    // B = {2, 3, 5, 7, 20}
    B.insert(2);
    B.insert(3);
    B.insert(5);
    B.insert(7);
    B.insert(20);

    std::cout << "A = "; A.print(); std::cout << "\n";
    std::cout << "B = "; B.print(); std::cout << "\n";

    setHashed U = set_union(A, B);
    setHashed I = set_intersection(A, B);
    setHashed D = set_difference(A, B);

    std::cout << "A ∪ B = "; U.print(); std::cout << "\n";
    std::cout << "A ∩ B = "; I.print(); std::cout << "\n";
    std::cout << "A \\ B = "; D.print(); std::cout << "\n";

    std::cout << "contains 5 in A: " << (A.contains(5) ? "TAK" : "NIE") << "\n";
    std::cout << "contains 4 in A: " << (A.contains(4) ? "TAK" : "NIE") << "\n";

    setHashed C = A;
    std::cout << "C = A, set_equal(A, C) = " << (set_equal(A, C) ? "TAK" : "NIE") << "\n";
    C.erase(1);
    std::cout << "Po usunieciu 1 z C: set_equal(A, C) = " << (set_equal(A, C) ? "TAK" : "NIE") << "\n";
}

/*
 * Funkcja pomocnicza: zapisuje dane (n, średni czas) dla insert i contains
 * do plików insert.dat oraz contains.dat – można potem użyć w gnuplocie.
 */
void benchmark_example() {
    const std::size_t BUCKETS = 101; // np. liczba pierwsza

    std::mt19937 rng(123456);
    std::uniform_int_distribution<int> dist(0, 1000000000);

    // Pomiar insert
    {
        std::ofstream out("insert.dat");
        out << "# n avg_time_ns\n";

        for (int n = 1000; n <= 20000; n += 1000) {
            setHashed s(BUCKETS);

            // wypełniamy zbiór n elementami
            for (int i = 0; i < n; ++i) {
                s.insert(dist(rng));
            }

            const int trials = 1000;
            long long totalNs = 0;

            for (int t = 0; t < trials; ++t) {
                int x = dist(rng);
                auto start = std::chrono::high_resolution_clock::now();
                s.insert(x);   // mierzona operacja
                auto end = std::chrono::high_resolution_clock::now();
                s.erase(x);    // usuwamy, by nie zmieniać zbytnio rozmiaru
                totalNs += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            }

            double avgNs = static_cast<double>(totalNs) / trials;
            out << n << " " << avgNs << "\n";
            std::cout << "[insert] n=" << n << " avg=" << avgNs << " ns\n";
        }
    }

    // Pomiar contains
    {
        std::ofstream out("contains.dat");
        out << "# n avg_time_ns\n";

        for (int n = 1000; n <= 20000; n += 1000) {
            setHashed s(BUCKETS);
            std::vector<int> inserted;
            inserted.reserve(n);

            for (int i = 0; i < n; ++i) {
                int x = dist(rng);
                s.insert(x);
                inserted.push_back(x);
            }

            const int trials = 1000;
            long long totalNs = 0;

            for (int t = 0; t < trials; ++t) {
                int x;
                // co druga próba – element istniejący, co druga – losowy
                if (t % 2 == 0) {
                    x = inserted[t % inserted.size()];
                } else {
                    x = dist(rng);
                }

                auto start = std::chrono::high_resolution_clock::now();
                volatile bool res = s.contains(x); // volatile, żeby kompilator nie wyciął
                (void)res;
                auto end = std::chrono::high_resolution_clock::now();
                totalNs += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            }

            double avgNs = static_cast<double>(totalNs) / trials;
            out << n << " " << avgNs << "\n";
            std::cout << "[contains] n=" << n << " avg=" << avgNs << " ns\n";
        }
    }
}

int main() {
    std::cout << "DEMO dzialania zbioru haszowanego:\n";
    simple_demo();

    std::cout << "\nUruchamiam przykladowy benchmark (insert, contains)...\n";
    benchmark_example();

    std::cout << "\nWygenerowano pliki: insert.dat oraz contains.dat (w katalogu z programem).\n";
    std::cout << "Mozesz je otworzyc w gnuplocie, np.:\n";
    std::cout << "  plot 'insert.dat' using 1:2 with lines title 'insert'\n";
    std::cout << "  plot 'contains.dat' using 1:2 with lines title 'contains'\n";

    return 0;
}