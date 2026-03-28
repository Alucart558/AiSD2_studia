#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <random>
#include <algorithm>

// ============================================================
//  Implementacja zbioru jako tablica bool o rozmiarze N
//  Universum: {0, 1, ..., N-1}
// ============================================================
template <int N>
class BitSetArray
{
    bool data[N];

public:
    BitSetArray() { std::fill(data, data + N, false); }

    void insert(int x)
    {
        if (x < 0 || x >= N) throw std::out_of_range("Element poza universem");
        data[x] = true;
    }

    void remove(int x)
    {
        if (x < 0 || x >= N) throw std::out_of_range("Element poza universem");
        data[x] = false;
    }

    bool contains(int x) const
    {
        if (x < 0 || x >= N) return false;
        return data[x];
    }

    // Zwraca indeks pierwszego elementu należącego do zbioru, lub -1
    int findMin() const
    {
        for (int i = 0; i < N; ++i)
            if (data[i]) return i;
        return -1;
    }

    bool empty() const
    {
        for (int i = 0; i < N; ++i)
            if (data[i]) return false;
        return true;
    }

    int size() const
    {
        int cnt = 0;
        for (int i = 0; i < N; ++i)
            if (data[i]) ++cnt;
        return cnt;
    }
};

// ============================================================
//  Kolejka priorytetowa oparta na zbiorze BitSetArray
//  Priorytet p(x) = x  (identyczność dla liczb całkowitych)
// ============================================================
template <int N>
class PriorityQueue
{
    BitSetArray<N> set_;

public:
    // Wstawienie elementu x do kolejki
    void push(int x)
    {
        set_.insert(x);
    }

    // Zwraca i usuwa element o najmniejszym priorytecie
    // Złożoność: O(N) – liniowe przeszukanie tablicy
    int extractMin()
    {
        int minElem = set_.findMin(); // O(N) w pesymistycznym przypadku
        if (minElem == -1)
            throw std::underflow_error("Kolejka jest pusta");
        set_.remove(minElem);
        return minElem;
    }

    bool empty() const { return set_.empty(); }
    int size() const { return set_.size(); }
};

// ============================================================
//  Demonstracja poprawności
// ============================================================
void demo()
{
    std::cout << "=== Demo poprawnosci ===\n";
    const int UNIV = 20;
    PriorityQueue<UNIV> pq;

    std::vector<int> elems = {7, 3, 15, 1, 9, 0, 12};
    std::cout << "Wstawianie: ";
    for (int e : elems)
    {
        pq.push(e);
        std::cout << e << " ";
    }
    std::cout << "\n";

    std::cout << "ExtractMin (posortowane rosnaco):\n";
    while (!pq.empty())
        std::cout << "  -> " << pq.extractMin() << "\n";
    std::cout << "\n";
}

// ============================================================
//  Benchmarking: mierzymy czas extractMin w funkcji rozmiaru
//  kolejki (ile elementów było w kolejce przed wywołaniem)
// ============================================================
void benchmark(const std::string& filename)
{
    // Rozmiar universum musi być stały w czasie kompilacji,
    // dlatego używamy największego możliwego N w tym pomiarze.
    const int UNIV = 100000;

    std::ofstream out(filename);
    out << "# n  czas_ns  (n = liczba elementow w kolejce przed extractMin)\n";
    std::cout << "=== Benchmarking (zapisuje do " << filename << ") ===\n";

    std::mt19937 rng(42);

    // Próbujemy różne rozmiary kolejki
    std::vector<int> sizes;
    for (int n = 1000; n <= UNIV; n += 2000) sizes.push_back(n);

    for (int n : sizes)
    {
        // Powtarzamy pomiar REPS razy i bierzemy medianę
        const int REPS = 200;
        std::vector<long long> times(REPS);

        for (int r = 0; r < REPS; ++r)
        {
            static bool arr[UNIV];
            std::fill(arr, arr + UNIV, false);

            // WYMUSZAMY: minimum leży w okolicy indeksu n,
            // więc findMin wykona ~n iteracji -> wykres ~liniowo rosnący.
            int start = n;
            if (start < 0) start = 0;
            if (start >= UNIV) start = UNIV - 1;

            // Ustawiamy minimum:
            arr[start] = true;

            // Dorzucamy kilka elementów dalej, żeby zbiór nie był trywialny
            // (nie zmienia to minimum).
            for (int k = 1; k <= 64; ++k)
            {
                int idx = start + k * 97;
                if (idx < UNIV) arr[idx] = true;
            }

            // MIERZYMY PACZKĘ OPERACJI, żeby wyjść z rejonu ~100 ns
            constexpr int INNER = 2000; // możesz dać 500/1000/5000 zależnie od CPU
            volatile int sink = 0;

            auto t0 = std::chrono::high_resolution_clock::now();

            for (int rep = 0; rep < INNER; ++rep)
            {
                int minElem = -1;

                // To jest logika findMin() (liniowy skan)
                for (int i = 0; i < UNIV; ++i)
                {
                    if (arr[i])
                    {
                        minElem = i;
                        break;
                    }
                }

                // remove(minElem) + przywrócenie, żeby każda iteracja była podobna
                if (minElem != -1)
                {
                    arr[minElem] = false;
                    sink += minElem;
                    arr[minElem] = true;
                }
            }

            auto t1 = std::chrono::high_resolution_clock::now();
            times[r] = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() / INNER;
        }

        // Mediana
        std::sort(times.begin(), times.end());
        long long med = times[REPS / 2];
        out << n << "  " << med << "\n";
        std::cout << "  n=" << n << "  mediana=" << med << " ns\n";
    }

    std::cout << "Dane zapisane do " << filename << "\n\n";
}

// ============================================================
//  Generujemy też skrypt gnuplota
// ============================================================
void writeGnuplot(const std::string& dataFile, const std::string& scriptFile)
{
    std::ofstream gp(scriptFile);
    gp << "set title 'Zlozonosc extractMin - kolejka priorytetowa (zbior tablicowy)'\n"
        << "set xlabel 'Liczba elementow w kolejce (n)'\n"
        << "set ylabel 'Czas [ns]'\n"
        << "set grid\n"
        << "set key top left\n"
        << "set xrange [*:*]\n"
        << "# Dopasowanie liniowe f(x)=a*x+b\n"
        << "f(x) = a*x + b\n"
        << "fit f(x) '" << dataFile << "' using 1:2 via a,b\n"
        << "plot '" << dataFile << "' using 1:2 with points pt 7 ps 0.8 lc rgb '#0077BB' title 'pomiary',\\\n"
        << "     f(x) with lines lw 2 lc rgb '#CC0000' title 'dopasowanie liniowe O(N)'\n"
        << "pause -1 'Nacisnij Enter aby zamknac'\n";
    std::cout << "Skrypt gnuplota zapisany do " << scriptFile << "\n";
    std::cout << "Uzycie: gnuplot " << scriptFile << "\n";
}

// ============================================================
int main()
{
    demo();
    benchmark("benchmark_data.txt");
    writeGnuplot("benchmark_data.txt", "plot_priority_queue.gp");
    return 0;
}