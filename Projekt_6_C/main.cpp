#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>

class TaskScheduler {
private:
    int numTasks;
    std::map<int, std::vector<int>> adj;
    std::map<int, int> inDegree;
    std::map<int, std::string> taskNames;

public:
    TaskScheduler() : numTasks(0) {}

    void addTask(int id, std::string name) {
        taskNames[id] = name;
        if (inDegree.find(id) == inDegree.end()) {
            inDegree[id] = 0;
        }
        numTasks++;
    }

    void addDependency(int start, int end) {
        adj[start].push_back(end);
        inDegree[end]++;
    }

    void printSchedule() {
        std::queue<int> q;

        // Znajdź zadania, które nie mają żadnych wymagań (stopień wejściowy 0)
        for (auto const& [id, degree] : inDegree) {
            if (degree == 0) {
                q.push(id);
            }
        }

        std::cout << "Kolejność wykonywania czynności:" << std::endl;
        int count = 0;
        while (!q.empty()) {
            int curr = q.front();
            q.pop();

            count++;
            std::cout << count << ". " << taskNames[curr] << std::endl;

            // Zmniejsz stopień wejściowy sąsiadów
            for (int neighbor : adj[curr]) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    q.push(neighbor);
                }
            }
        }

        if (count != taskNames.size()) {
            std::cout << "Błąd: Wykryto cykl w grafie! Nie można ustalić kolejności." << std::endl;
        }
    }
};

int main() {
    TaskScheduler kitchen;

    // Definiujemy czynności
    kitchen.addTask(1, "Nagrzej patelnię");
    kitchen.addTask(2, "Zmieszaj jajko, olej, mleko i proszek");
    kitchen.addTask(3, "Wylej ciasto na patelnię");
    kitchen.addTask(4, "Przewróć i podpiecz z drugiej strony");
    kitchen.addTask(5, "Podgrzej syrop klonowy");
    kitchen.addTask(6, "Zjedz naleśnika z syropem");

    // Definiujemy zależności (krawędzie skierowane)
    kitchen.addDependency(1, 3); // Patelnia musi być ciepła przed wylaniem ciasta
    kitchen.addDependency(2, 3); // Ciasto musi być zmieszane przed wylaniem
    kitchen.addDependency(3, 4); // Musi być rumiany z jednej strony przed przewróceniem
    kitchen.addDependency(4, 6); // Musi być gotowy przed jedzeniem
    kitchen.addDependency(5, 6); // Syrop musi być ciepły przed polaniem

    kitchen.printSchedule();

    return 0;
}