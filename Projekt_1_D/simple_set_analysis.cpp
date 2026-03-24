#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>

class SimpleSet {
private:
    std::vector<std::string> elements;

public:
    void insert(const std::string& str) {
        if (std::find(elements.begin(), elements.end(), str) == elements.end()) {
            elements.push_back(str);
        }
    }

    void remove(const std::string& str) {
        auto it = std::find(elements.begin(), elements.end(), str);
        if (it != elements.end()) {
            elements.erase(it);
        }
    }

    bool contains(const std::string& str) const {
        return std::find(elements.begin(), elements.end(), str) != elements.end();
    }

    void print() const {
        std::cout << "{ ";
        for (const auto& el : elements) std::cout << el << " ";
        std::cout << "}\n";
    }

    std::size_t size() const { return elements.size(); }
    void clear() { elements.clear(); }
};

static void analyzeComplexityForN(int numTests) {
    using namespace std::chrono;

    SimpleSet set;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < numTests; ++i) {
        set.insert("element" + std::to_string(i));
    }
    auto end = high_resolution_clock::now();
    double insertMs = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    for (int i = 0; i < numTests; ++i) {
        set.contains("element" + std::to_string(i));
    }
    end = high_resolution_clock::now();
    double containsMs = duration<double, std::milli>(end - start).count();

    start = high_resolution_clock::now();
    for (int i = 0; i < numTests; ++i) {
        set.remove("element" + std::to_string(i));
    }
    end = high_resolution_clock::now();
    double removeMs = duration<double, std::milli>(end - start).count();

    // Plain gnuplot-friendly output: N insert_ms contains_ms remove_ms
    std::cout << numTests << " " << std::fixed << std::setprecision(6)
              << insertMs << " " << containsMs << " " << removeMs << "\n";
}

static void analyzeComplexityManyPoints() {
    // Many points for gnuplot: adjust as needed.
    // Tip: avoid too huge end/step, because this implementation is ~O(N^2).
    const int startN = 200;
    const int endN   = 50000;
    const int stepN  = 200;

    std::cout << "# SimpleSet(vector<string>) benchmark\n";
    std::cout << "# columns: N insert_ms contains_ms remove_ms\n";
    std::cout << "# range: " << startN << " .. " << endN << ", step " << stepN << "\n";

    for (int n = startN; n <= endN; n += stepN) {
        analyzeComplexityForN(n);
    }
}

int main() {
    SimpleSet set;

    std::cout << "=== CORRECTNESS TESTS SimpleSet ===\n";
    set.insert("apple");
    set.insert("banana");
    set.insert("cherry");
    std::cout << "After insert: ";
    set.print();

    set.remove("banana");
    std::cout << "After remove banana: ";
    set.print();

    std::cout << "Contains apple? " << (set.contains("apple") ? "YES" : "NO") << "\n";
    std::cout << "Contains banana? " << (set.contains("banana") ? "YES" : "NO") << "\n";
    std::cout << "==================================\n\n";

    // Many benchmark points to copy/paste into a file for gnuplot.
    analyzeComplexityManyPoints();
    return 0;
}