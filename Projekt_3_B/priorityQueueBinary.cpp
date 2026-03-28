#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <fstream>
#include <random>
#include <stdexcept>
#include <cmath>

// ============================================================
//  Kopiec binarny — min-heap
//  Własności:
//    heap[0]           = minimum (korzeń)
//    rodzic(i)         = (i-1)/2
//    lewe dziecko(i)   = 2*i+1
//    prawe dziecko(i)  = 2*i+2
//    heap[parent(i)] <= heap[i]   dla każdego i > 0
// ============================================================
class PriorityQueueBinary {
    std::vector<int> heap;

    // Przywracanie własności kopca w górę (po insert)
    // Złożoność: O(log n)
    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (heap[parent] > heap[i]) {
                std::swap(heap[parent], heap[i]);
                i = parent;
            } else break;
        }
    }

    // Przywracanie własności kopca w dół (po extractMin)
    // Złożoność: O(log n)
    void siftDown(int i) {
        int n = (int)heap.size();
        while (true) {
            int left  = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;

            if (left  < n && heap[left]  < heap[smallest]) smallest = left;
            if (right < n && heap[right] < heap[smallest]) smallest = right;

            if (smallest != i) {
                std::swap(heap[i], heap[smallest]);
                i = smallest;
            } else break;
        }
    }

public:
    // Wstawienie elementu x — O(log n)
    void push(int x) {
        heap.push_back(x);
        siftUp((int)heap.size() - 1);
    }

    // Zwraca i usuwa element o najmniejszym priorytecie — O(log n)
    int extractMin() {
        if (heap.empty()) throw std::underflow_error("Kolejka jest pusta");

        int minVal = heap[0];          // korzeń = minimum
        heap[0] = heap.back();         // przenieś ostatni na korzeń
        heap.pop_back();               // usuń ostatni
        if (!heap.empty()) siftDown(0);// napraw kopiec — O(log n)
        return minVal;
    }

    bool empty() const { return heap.empty(); }
    int  size()  const { return (int)heap.size(); }
    int  peek()  const { return heap.empty() ? -1 : heap[0]; }
};

// ============================================================
//  Demo poprawności
// ============================================================
void demo() {
    std::cout << "=== Demo poprawnosci (kopiec binarny) ===\n";
    PriorityQueueBinary pq;

    std::vector<int> elems = {7, 3, 15, 1, 9, 0, 12, 5, 8};
    std::cout << "Wstawianie: ";
    for (int e : elems) { pq.push(e); std::cout << e << " "; }
    std::cout << "\n";

    std::cout << "extractMin (posortowane rosnaco):\n";
    while (!pq.empty())
        std::cout << "  -> " << pq.extractMin() << "\n";
    std::cout << "\n";
}

// ============================================================
//  Benchmark kopca binarnego
//  Mierzymy czas extractMin w funkcji n (rozmiar kopca).
//  Pesymistyczny przypadek: minimum zawsze w korzeniu,
//  ale siftDown musi zejść na sam dół — wypełniamy kopiec
//  tak by po usunięciu minimum siftDown szedł jak najgłębiej.
//  Najprościej: wstawiamy n losowych elementów (gwarantuje
//  log n ścieżkę), mierzymy czas pojedynczego extractMin.
// ============================================================
void benchmarkBinary(const std::string& filename) {
    std::ofstream out(filename);
    out << "# n  czas_ns   (n = rozmiar kopca przed extractMin)\n";
    std::cout << "=== Benchmark kopca binarnego -> " << filename << " ===\n";

    std::mt19937 rng(42);
    const int REPS   = 800;
    const int N_MAX  = 1 << 20;  // 1 048 576 = 2^20

    // Wykładniczo rozmieszczone punkty pomiarowe: potęgi 2 i ich połówki
    std::vector<int> sizes;
    for (int p = 4; (1 << p) <= N_MAX; ++p) {
        sizes.push_back(1 << p);
        if ((1 << p) * 3 / 2 <= N_MAX)
            sizes.push_back((1 << p) * 3 / 2);
    }

    for (int n : sizes) {
        std::vector<long long> times(REPS);

        // Losowe wartości dla tego n
        std::uniform_int_distribution<int> dist(0, n * 10);

        for (int r = 0; r < REPS; ++r) {
            // Zbuduj kopiec o rozmiarze n
            PriorityQueueBinary pq;
            // Wstaw n elementów (heap build)
            for (int i = 0; i < n; ++i) pq.push(dist(rng));

            // Mierz wyłącznie extractMin
            auto t0 = std::chrono::high_resolution_clock::now();
            pq.extractMin();
            auto t1 = std::chrono::high_resolution_clock::now();
            times[r] = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
        }

        std::sort(times.begin(), times.end());
        long long med = times[REPS / 2];
        out << n << "  " << med << "\n";
        std::cout << "  n=" << n << "  mediana=" << med << " ns\n";
    }
    std::cout << "Zapisano do " << filename << "\n\n";
}

// ============================================================
//  Benchmark tablicowy (zad. A) — do porównania
//  Pesymistyczny: jeden element na pozycji k = n-1.
// ============================================================
void benchmarkArray(const std::string& filename) {
    std::ofstream out(filename);
    out << "# n  czas_ns   (n = rozmiar kolejki; min na pozycji n-1)\n";
    std::cout << "=== Benchmark tablicowy (zad. A, worst-case) -> " << filename << " ===\n";

    const int REPS   = 800;
    const int N_MAX  = 1 << 20;

    std::vector<int> sizes;
    for (int p = 4; (1 << p) <= N_MAX; ++p) {
        sizes.push_back(1 << p);
        if ((1 << p) * 3 / 2 <= N_MAX)
            sizes.push_back((1 << p) * 3 / 2);
    }

    static bool arr[1 << 20];

    for (int n : sizes) {
        std::vector<long long> times(REPS);

        for (int r = 0; r < REPS; ++r) {
            // Tylko jeden element na pozycji n-1 (pesymistyczny przypadek)
            std::fill(arr, arr + n, false);
            arr[n - 1] = true;

            auto t0 = std::chrono::high_resolution_clock::now();
            int minElem = -1;
            for (int i = 0; i < n; ++i) {
                if (arr[i]) { minElem = i; break; }
            }
            if (minElem != -1) arr[minElem] = false;
            auto t1 = std::chrono::high_resolution_clock::now();
            times[r] = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
        }

        std::sort(times.begin(), times.end());
        long long med = times[REPS / 2];
        out << n << "  " << med << "\n";
        std::cout << "  n=" << n << "  mediana=" << med << " ns\n";
    }
    std::cout << "Zapisano do " << filename << "\n\n";
}

// ============================================================
//  Skrypt gnuplot — porównanie obu implementacji
// ============================================================
void writeGnuplot(const std::string& fileHeap,
                  const std::string& fileArr,
                  const std::string& script) {
    std::ofstream gp(script);
    gp <<
R"(set title 'extractMin: kopiec binarny O(log n) vs tablica bool O(N)'
set xlabel 'Rozmiar kolejki (n)'
set ylabel 'Czas [ns]'
set logscale x 2
set logscale y
set grid
set key top left

# Dopasowanie log dla kopca:  f(x) = a * log2(x) + b
f(x) = a * log(x)/log(2) + b
fit f(x) ')" << fileHeap << R"(' using 1:2 via a,b

# Dopasowanie liniowe dla tablicy: g(x) = c * x + d
g(x) = c * x + d
fit g(x) ')" << fileArr << R"(' using 1:2 via c,d

plot ')" << fileHeap << R"(' using 1:2 with points pt 7 ps 0.8 lc rgb '#0077BB' title 'kopiec binarny (pomiary)', \
     f(x) with lines lw 2 lc rgb '#0077BB' title sprintf('fit: %.2f*log2(n)+%.1f', a, b), \
     ')" << fileArr  << R"(' using 1:2 with points pt 5 ps 0.8 lc rgb '#CC3311' title 'tablica bool — worst-case (pomiary)', \
     g(x) with lines lw 2 lc rgb '#CC3311' title sprintf('fit: %.4f*n+%.1f', c, d)
pause -1 'Nacisnij Enter'
)";
    std::cout << "Skrypt gnuplot -> " << script << "\n";
    std::cout << "Uzycie: gnuplot " << script << "\n";
}

int main() {
    demo();
    benchmarkBinary("bench_heap.txt");
    benchmarkArray("bench_array_cmp.txt");
    writeGnuplot("bench_heap.txt", "bench_array_cmp.txt", "plot_comparison.gp");
    return 0;
}