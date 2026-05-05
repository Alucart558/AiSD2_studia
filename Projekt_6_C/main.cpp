// Biblioteka do obsługi wejścia/wyjścia (cout, endl, itp.)
#include <iostream>
// Biblioteka do obsługi dynamicznych tablic (std::vector) - zarezerwowana, ale nieużywana w tym kodzie
#include <vector>
// Biblioteka do obsługi napisów (std::string)
#include <string>
// Biblioteka do obsługi kolejki FIFO (std::queue) - potrzebna do algorytmu Kahn'a
#include <queue>
// Biblioteka do obsługi map (słowników) asocjacyjnych (std::map)
#include <map>

// Klasa do zarządzania planowaniem zadań z uwzględnieniem zależności między nimi
class TaskScheduler {
private:
    // Licznik ilości dodanych zadań (zarezerwowany, ale nie wykorzystywany w pełni)
    int numTasks;
    // Mapa listy sąsiedztwa: dla każdego zadania (klucz) przechowuje wektor zadań zależnych od niego (wartość)
    std::map<int, std::vector<int>> adj;
    // Mapa przechowująca stopień wejściowy (in-degree) każdego zadania - ile zadań musi być wykonane przed nim
    std::map<int, int> inDegree;
    // Mapa przechowująca nazwy zadań: ID zadania (klucz) -> nazwa zadania (wartość)
    std::map<int, std::string> taskNames;

public:
    // Konstruktor inicjujący licznik zadań na 0
    TaskScheduler() : numTasks(0) {}

    // Metoda dodająca nowe zadanie o podanym ID i nazwie
    void addTask(int id, std::string name) {
        // Przypisz nazwę do ID zadania w mapie taskNames
        taskNames[id] = name;
        // Jeśli zadanie o tym ID nie istnieje jeszcze w mapie inDegree
        if (inDegree.find(id) == inDegree.end()) {
            // Inicjalizuj stopień wejściowy na 0 (brak zależności na początku)
            inDegree[id] = 0;
        }
        // Zwiększ licznik zadań o 1
        numTasks++;
    }

    // Metoda dodająca zależność: zadanie 'start' musi być wykonane przed 'end'
    void addDependency(int start, int end) {
        // Dodaj 'end' do listy sąsiadów (zadań zależnych) zadania 'start'
        adj[start].push_back(end);
        // Zwiększ stopień wejściowy zadania 'end' (ponieważ ma jedno więcej wymagania)
        inDegree[end]++;
    }

    // Metoda wypisująca kolejność wykonywania zadań używając algorytmu topologicznego sortowania (Kahn)
    void printSchedule() {
        // Utwórz pustą kolejkę FIFO do przechowywania ID zadań
        std::queue<int> q;

        // Tej linii po prostu czyszczam - znajduję wszystkie zadania, które mogą być wykonane od razu (bez wymagań)
        for (auto const& [id, degree] : inDegree) {
            // Jeśli stopień wejściowy zadania wynosi 0 (nie ma żadnych wymagań)
            if (degree == 0) {
                // Dodaj to zadanie do kolejki jako gotowe do wykonania
                q.push(id);
            }
        }

        // Wypisz nagłówek listy zadań
        std::cout << "Kolejność wykonywania czynności:" << std::endl;
        // Inicjalizuj licznik do numerowania pozycji na liście (1, 2, 3, itd.)
        int count = 0;
        // Pętla wykonuje się dopóki w kolejce są jakieś zadania
        while (!q.empty()) {
            // Usuń zadanie z przodu kolejki i przypisz jego ID do zmiennej curr (current)
            int curr = q.front();
            q.pop();

            // Zwiększ licznik o 1 (to będzie numer bieżącego zadania na liście)
            count++;
            // Wypisz nr i nazwę bieżącego zadania w formacie: "1. Nagrzej patelnię"
            std::cout << count << ". " << taskNames[curr] << std::endl;

            // Przejdź przez wszystkie zadania, które zależą od bieżącego zadania 'curr'
            for (int neighbor : adj[curr]) {
                // Zmniejsz stopień wejściowy zadania zależnego (ponieważ jedna z jego zależności już będzie spełniona)
                inDegree[neighbor]--;
                // Jeśli stopień wejściowy spadł na 0 (wszystkie zależności są spełnione)
                if (inDegree[neighbor] == 0) {
                    // Dodaj to zadanie do kolejki jako gotowe do wykonania
                    q.push(neighbor);
                }
            }
        }

        // Sprawdź czy wszystkie zadania zostały wykonane (licznik powinien być równy liczbie zadań)
        if (count != taskNames.size()) {
            // Jeśli licznik jest mniejszy, oznacza to że istnieje cykl w grafie zależności (nieskończona pętla)
            std::cout << "Błąd: Wykryto cykl w grafie! Nie można ustalić kolejności." << std::endl;
        }
    }
};

// Główna funkcja programu - punkt wejścia aplikacji
int main() {
    // Utwórz obiekt planisty zadań dla przykładu z gotowaniem naleśników
    TaskScheduler kitchen;

    // Dodaj 6 zadań (czynności przy gotowaniu naleśników)
    // Każde zadanie ma unikalny ID (1-6) i nazwę opisującą czynność
    kitchen.addTask(1, "Nagrzej patelnię");
    kitchen.addTask(2, "Zmieszaj jajko, olej, mleko i proszek");
    kitchen.addTask(3, "Wylej ciasto na patelnię");
    kitchen.addTask(4, "Przewróć i podpiecz z drugiej strony");
    kitchen.addTask(5, "Podgrzej syrop klonowy");
    kitchen.addTask(6, "Zjedz naleśnika z syropem");

    // Dodaj zależności między zadaniami (krawędzie w grafie skierowanym)
    // Format: zadanie A musi być wykonane PRZED zadaniem B
    // Zadanie 1: nagrzanie patelni musi być wykonane przed wylewaniem ciasta (zadanie 3)
    kitchen.addDependency(1, 3);
    // Zadanie 2: mieszanie składników musi być wykonane przed wylewaniem ciasta (zadanie 3)
    kitchen.addDependency(2, 3);
    // Zadanie 3: ciasto na patelni musi być już obecne przed przewróceniem (zadanie 4)
    kitchen.addDependency(3, 4);
    // Zadanie 4: naleśnik musi być usmażony z dwóch stron zanim będzie można go jeść (zadanie 6)
    kitchen.addDependency(4, 6);
    // Zadanie 5: syrop musi być podgrzany przed polaniem naleśnika (zadanie 6)
    kitchen.addDependency(5, 6);

    // Wykonaj algorytm topologicznego sortowania i wypisz kolejność wykonywania zadań
    kitchen.printSchedule();

    // Zwróć kod 0 (sukces) do systemu operacyjnego
    return 0;
}