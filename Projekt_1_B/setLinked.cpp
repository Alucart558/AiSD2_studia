#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>
#include <memory>
#include <string>

// =====================
// setLinked (unique_ptr)
// =====================

class Node {
public:
    int value;
    std::unique_ptr<Node> next;

    explicit Node(int val) : value(val), next(nullptr) {}
};

class setLinked {
private:
    std::unique_ptr<Node> head;

public:
    setLinked() : head(nullptr) {}

    void insert(int value) {
        // insert at head
        if (!head || head->value > value) {
            if (!head || head->value != value) {
                auto newNode = std::make_unique<Node>(value);
                newNode->next = std::move(head);
                head = std::move(newNode);
            }
            return;
        }

        Node* current = head.get();
        while (current->next && current->next->value < value) {
            current = current->next.get();
        }

        if (!current->next || current->next->value != value) {
            auto newNode = std::make_unique<Node>(value);
            newNode->next = std::move(current->next);
            current->next = std::move(newNode);
        }
    }

    void remove(int value) {
        if (!head) return;

        if (head->value == value) {
            head = std::move(head->next);
            return;
        }

        Node* current = head.get();
        while (current->next && current->next->value != value) {
            current = current->next.get();
        }

        if (current->next) {
            current->next = std::move(current->next->next);
        }
    }

    bool contains(int value) const {
        const Node* current = head.get();
        while (current) {
            if (current->value == value) return true;
            current = current->next.get();
        }
        return false;
    }

    void print() const {
        std::cout << "{ ";
        const Node* current = head.get();
        while (current) {
            std::cout << current->value << " ";
            current = current->next.get();
        }
        std::cout << "}" << std::endl;
    }

    int size() const {
        int count = 0;
        const Node* current = head.get();
        while (current) {
            ++count;
            current = current->next.get();
        }
        return count;
    }

    void clear() {
        // iterative clear (also avoids deep recursion)
        while (head) {
            head = std::move(head->next);
        }
    }

    // =====================
    // Set operations
    // =====================

    setLinked setUnion(const setLinked& other) const {
        setLinked result;
        const Node* a = head.get();
        const Node* b = other.head.get();

        while (a || b) {
            if (!b || (a && a->value < b->value)) {
                result.insert(a->value);
                a = a->next.get();
            } else if (!a || (b && b->value < a->value)) {
                result.insert(b->value);
                b = b->next.get();
            } else { // equal
                result.insert(a->value);
                a = a->next.get();
                b = b->next.get();
            }
        }
        return result;
    }

    setLinked setIntersection(const setLinked& other) const {
        setLinked result;
        const Node* a = head.get();
        const Node* b = other.head.get();

        while (a && b) {
            if (a->value < b->value) {
                a = a->next.get();
            } else if (b->value < a->value) {
                b = b->next.get();
            } else {
                result.insert(a->value);
                a = a->next.get();
                b = b->next.get();
            }
        }
        return result;
    }

    setLinked setDifference(const setLinked& other) const {
        // elements in *this that are not in other
        setLinked result;
        const Node* a = head.get();
        const Node* b = other.head.get();

        while (a) {
            while (b && b->value < a->value) {
                b = b->next.get();
            }
            if (!b || a->value < b->value) {
                result.insert(a->value);
                a = a->next.get();
            } else { // equal -> skip
                a = a->next.get();
            }
        }
        return result;
    }

    bool equals(const setLinked& other) const {
        const Node* a = head.get();
        const Node* b = other.head.get();

        while (a && b) {
            if (a->value != b->value) return false;
            a = a->next.get();
            b = b->next.get();
        }
        return a == nullptr && b == nullptr;
    }
};

// =====================
// Correctness tests
// =====================

static void assertTrue(bool cond, const std::string& name) {
    std::cout << (cond ? "[OK]  " : "[FAIL] ") << name << std::endl;
}

void runCorrectnessTests() {
    std::cout << "=== TESTY POPRAWNOSCI setLinked ===" << std::endl;

    setLinked mySet;

    mySet.insert(5);
    mySet.insert(3);
    mySet.insert(8);
    mySet.insert(3);

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

void runSetOpsCorrectnessTests() {
    std::cout << "=== TESTY POPRAWNOSCI OPERACJI ZBIOROWYCH setLinked ===" << std::endl;

    setLinked A;
    for (int v : {1, 3, 5, 7, 9}) A.insert(v);

    setLinked B;
    for (int v : {2, 3, 5, 8}) B.insert(v);

    setLinked U = A.setUnion(B);          // {1,2,3,5,7,8,9}
    setLinked I = A.setIntersection(B);   // {3,5}
    setLinked D = A.setDifference(B);     // {1,7,9}

    assertTrue(U.size() == 7, "Union size");
    assertTrue(U.contains(1) && U.contains(2) && U.contains(3) && U.contains(5) && U.contains(7) && U.contains(8) && U.contains(9),
               "Union contents");

    assertTrue(I.size() == 2, "Intersection size");
    assertTrue(I.contains(3) && I.contains(5) && !I.contains(1) && !I.contains(2), "Intersection contents");

    assertTrue(D.size() == 3, "Difference size");
    assertTrue(D.contains(1) && D.contains(7) && D.contains(9) && !D.contains(3) && !D.contains(5), "Difference contents");

    setLinked A2;
    for (int v : {9, 7, 5, 3, 1}) A2.insert(v); // different insertion order, same final set
    assertTrue(A.equals(A2), "Equals (same elements)");
    assertTrue(!A.equals(B), "Equals (different sets)");

    setLinked E1, E2;
    assertTrue(E1.equals(E2), "Equals (both empty)");
    E1.insert(1);
    assertTrue(!E1.equals(E2), "Equals (one empty)");

    std::cout << "===============================================\n" << std::endl;
}

// =====================
// Benchmarks
// =====================

static setLinked makeRandomSet(int N, int range, std::mt19937& gen) {
    setLinked s;
    std::uniform_int_distribution<> dis(0, range);
    for (int i = 0; i < N; ++i) s.insert(dis(gen));
    return s;
}

void measureLinkedComplexity(int N) {
    using namespace std::chrono;

    // keep ops moderate so it finishes; adjust if needed
    const int ops = 20000;

    setLinked s;
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(0, 2 * N);

    for (int i = 0; i < N; ++i) {
        s.insert(dis(gen));
    }

    auto start = high_resolution_clock::now();
    for (int i = 0; i < ops; ++i) s.insert(dis(gen));
    auto end = high_resolution_clock::now();
    double insert_us = duration<double, std::micro>(end - start).count() / ops;

    start = high_resolution_clock::now();
    for (int i = 0; i < ops; ++i) s.contains(dis(gen));
    end = high_resolution_clock::now();
    double contains_us = duration<double, std::micro>(end - start).count() / ops;

    start = high_resolution_clock::now();
    for (int i = 0; i < ops; ++i) s.remove(dis(gen));
    end = high_resolution_clock::now();
    double remove_us = duration<double, std::micro>(end - start).count() / ops;

    // gnuplot-friendly row
    std::cout << N << " " << insert_us << " " << contains_us << " " << remove_us << "\n";
}

void runLinkedBenchmarks() {
    std::cout << "=== BADANIE ZLOZONOSCI setLinked (lista jednokierunkowa) ===" << std::endl;
    std::cout << "# N insert_us contains_us remove_us\n";

    // many points for gnuplot (no need to touch main)
    for (int N = 10; N <= 200; N += 5) {
        measureLinkedComplexity(N);
    }
    for (int N = 200; N <= 50000; N = static_cast<int>(N * 1.05) + 1) {
        measureLinkedComplexity(N);
    }
}

void runSetOpsBenchmarks() {
    using namespace std::chrono;

    std::cout << "=== BADANIE ZLOZONOSCI OPERACJI ZBIOROWYCH setLinked ===" << std::endl;
    std::cout << "# N union_us intersection_us difference_us equals_us\n";

    std::mt19937 gen(123);

    for (int N = 10; N <= 50000; N = static_cast<int>(N * 1.08) + 1) {
        // build two sets of size ~N (unique count depends on collisions)
        setLinked A = makeRandomSet(N, 2 * N, gen);
        setLinked B = makeRandomSet(N, 2 * N, gen);

        const int reps = 200; // keep runtime reasonable

        auto t0 = high_resolution_clock::now();
        for (int i = 0; i < reps; ++i) {
            volatile int sz = A.setUnion(B).size();
            (void)sz;
        }
        auto t1 = high_resolution_clock::now();

        for (int i = 0; i < reps; ++i) {
            volatile int sz = A.setIntersection(B).size();
            (void)sz;
        }
        auto t2 = high_resolution_clock::now();

        for (int i = 0; i < reps; ++i) {
            volatile int sz = A.setDifference(B).size();
            (void)sz;
        }
        auto t3 = high_resolution_clock::now();

        for (int i = 0; i < reps; ++i) {
            volatile bool eq = A.equals(B);
            (void)eq;
        }
        auto t4 = high_resolution_clock::now();

        double union_us = duration<double, std::micro>(t1 - t0).count() / reps;
        double inter_us = duration<double, std::micro>(t2 - t1).count() / reps;
        double diff_us  = duration<double, std::micro>(t3 - t2).count() / reps;
        double eq_us    = duration<double, std::micro>(t4 - t3).count() / reps;

        std::cout << N << " " << union_us << " " << inter_us << " " << diff_us << " " << eq_us << "\n";
    }

    std::cout << std::endl;
}

int main() {
    runCorrectnessTests();
    runSetOpsCorrectnessTests();

    runLinkedBenchmarks();
    runSetOpsBenchmarks();
    return 0;
}