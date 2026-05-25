MST – Prim vs Kruskal (zadanie: sieć 1 + 127 klientów)
=====================================================

Opis zadania
------------
Dysponujemy plikiem z listą połączeń pomiędzy serwerem (węzeł 1) i klientami (węzły 2..128).
Każda linia pliku ma format:
  u v w
gdzie u, v to numery węzłów (liczone od 1), a w to średnie opóźnienie (waga krawędzi).

Zadanie:
- zbudować minimalne drzewo rozpinające (MST) grafu używając dwóch metod:
  1) algorytm Prima,
  2) algorytm Kruskala,
- porównać efektywność obu algorytmów.

Plik źródłowy
-------------
`mst.cpp` — program w C++:
- wczytuje plik z krawędziami (`mst.exe <plik> [liczba_węzłów]`),
- uruchamia Prim i Kruskal,
- wypisuje krawędzie MST i sumaryczną wagę,
- mierzy czasy wykonania w mikrosekundach.

Przykład uruchomienia
---------------------
PowerShell:
```powershell
g++ -std=c++17 -O2 -pipe mst.cpp -o mst.exe
.\mst.exe czasy 128