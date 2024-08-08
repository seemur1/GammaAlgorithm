#include <iostream>
#include <vector>

int useless;
std::string useless_string;

void c_swap(int& fst, int& snd) {
    useless = snd;
    snd = fst;
    fst = useless;
}

void c_swap(std::string& fst, std::string& snd) {
    useless_string = std::move(snd);
    snd = std::move(fst);
    fst = std::move(useless_string);
}

// Бин. поиск. Обрабатывает так же случай с "краем" снаружи массива.
template<typename T>
int search(const std::vector<T>& sorted, T& value, int l_b, int r_b) {
    for (int mid; r_b - l_b > 1; l_b = (sorted[mid] > value) ? l_b - (r_b = mid) + mid : mid) {
        mid = (l_b + r_b) / 2;
    }
    return (sorted[l_b] > value) ? l_b : ((sorted[r_b] > value) ? r_b : r_b + 1);
}

template<typename T>
void bin_insertion_sort(std::vector<T>& sorted) {
    int pos;
    for (int i = 1; i < sorted.size(); ++i) {
        pos = search(sorted, sorted[i], 0, i - 1);
        for (int j = i; (j > pos); --j) {
            c_swap(sorted[j], sorted[j - 1]);
        }
    }
}

int main() {
    int size, pos;

    std::cin >> size;
    std::vector<int> sorted(size);
    // Считываем массив
    for (int i = 0; (i < size) && (std::cin >> sorted[i]); ++i) {}
    bin_insertion_sort<int>(sorted);
    for (int i = 0; (i < size) && (std::cout << sorted[i] << ' '); ++i) {}
    return 0;
}