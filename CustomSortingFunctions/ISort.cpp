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

template<typename T>
void insertion_sort(std::vector<T>& sorted) {
    for (int i = 1; i < sorted.size(); ++i) {
        for (int j = i; (j > 0) && (sorted[j] < sorted[j - 1]); --j) {
            c_swap(sorted[j], sorted[j - 1]);
        }
    }
}

int main() {
    int size;

    std::cin >> size;
    std::vector<int> sorted(size);
    // Считываем массив
    for (int i = 0; (i < size) && (std::cin >> sorted[i]); ++i) {}
    insertion_sort<int>(sorted);
    for (int i = 0; (i < size) && (std::cout << sorted[i] << ' '); ++i) {}
    return 0;
}