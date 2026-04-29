[README.md](https://github.com/user-attachments/files/27215146/README.md)
# Studiu experimental asupra eficienței algoritmilor de sortare

Acest proiect prezintă o analiză experimentală a unor algoritmi clasici de sortare:  
**Bubble Sort**, **Selection Sort**, **Insertion Sort**, **Merge Sort**, **QuickSort** și **HeapSort**.

Scopul proiectului este evaluarea performanței acestora în funcție de:
- timpul de execuție;
- consumul de memorie;
- tipul datelor de intrare.

---

## 📌 Conținut

- `Algorithm-Efficiency.cpp` — implementarea algoritmilor și sistemul de benchmark;
- `averages.csv` — rezultatele medii obținute în urma testelor;
- `results_*.csv` — rezultate brute pentru fiecare rulare;
- `generated_list_*.txt` — seturi de date generate;
- `Analiza_algoritmi_de_sortare.pdf` — lucrarea finală cu grafice și interpretări;

---

## 🧪 Descriere experiment

Testele au fost realizate pe mai multe tipuri de seturi de date:

- `random`
- `sorted`
- `reverse`
- `almost_sorted`
- `few_unique`

Pentru fiecare algoritm s-au măsurat:
- ⏱️ timpul de execuție (ms / ns)
- 🧠 memoria utilizată (KB)

Datele au fost:
- generate automat;
- rulate de mai multe ori;
- agregate în fișiere `.csv`.

---

## 💻 Configurație sistem

Testele au fost efectuate pe:

- CPU: AMD Ryzen 7 5800X (8C / 16T)
- RAM: 48GB DDR4
- GPU: NVIDIA RTX 4060
- OS: Windows 10 64-bit

---

### 🧪 Benchmark CPU (Cinebench R23)

- Multi-core: ~14323 puncte
- Single-core: ~1580 puncte

---

## 📈 Rezultate principale

- **QuickSort** este cel mai rapid în majoritatea cazurilor practice;
- **HeapSort** devine foarte eficient pe date invers sortate;
- **MergeSort** oferă performanță stabilă, dar consumă mai multă memorie;
- **Insertion Sort** și **Bubble Sort** sunt foarte rapide pe date deja sortate;
- Algoritmii **O(n²)** devin impractici pentru dimensiuni mari.

---

## 📊 Observații importante

- Graficele folosesc **scară logaritmică** pentru a evidenția diferențele mari de performanță;
- Performanța algoritmilor depinde semnificativ de distribuția datelor;
- Consumul de memorie diferă în funcție de implementare (recursivitate, alocări auxiliare etc.).

---

## 📚 Bibliografie

Algoritmii analizați sunt clasici și bine documentați în literatura de specialitate:

- J. von Neumann — Merge Sort (1945)
- C. A. R. Hoare — QuickSort (1962)
- J. W. J. Williams — HeapSort (1964)
- D. E. Knuth — *The Art of Computer Programming*

Algoritmii simpli (Bubble, Selection, Insertion) nu au un autor unic cunoscut, fiind metode fundamentale.

---

## 📎 Licență

Proiect realizat în scop educațional — 2026.

---

## 🔗 Note

Acest proiect nu propune algoritmi noi, ci analizează experimental comportamentul unor metode consacrate, evidențiind diferențele dintre teoria complexității și performanța practică.
