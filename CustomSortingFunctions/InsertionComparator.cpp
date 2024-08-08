// В данной части лабораторной работы, сравнивается скорость работы сортировок вставками
// (бинарными вставками / обычная сортировка) при сортировке строк разной длины. Строки генерируются рандомно.

#include <iostream>
#include <vector>
#include <ctime>
#include <random>

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
//--------------------------------------------------------------------------------------------------
template<typename T>
void insertion_sort(std::vector<T>& sorted) {
    for (int i = 1; i < sorted.size(); ++i) {
        for (int j = i; (j > 0) && (sorted[j] < sorted[j - 1]); --j) {
            c_swap(sorted[j], sorted[j - 1]);
        }
    }
}
//--------------------------------------------------------------------------------------------------
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

//--------------------------------------------------------------------------------------------------
template<typename T>
unsigned long long int check_time(void (*func)(std::vector<T>&), const std::vector<T>& sorted, int turns) {
    unsigned long long int res = 0, start, finish;
    for (int i = 0; i < turns; ++i) {
        std::vector<T> buffer = sorted;
        start = clock(); // начальное время
        func(buffer);
        finish = clock(); // конечное время
        res += finish - start;
    }
    return res / turns;
}

std::vector<int> sizes_n = {20, 100, 500, 2000, 10000}, sizes_m = {1, 10, 100};
std::vector<void (*)(std::vector<std::string>&)> funcs = {insertion_sort<std::string>, bin_insertion_sort<std::string>};
std::vector<std::string> names = {"insertion_sort", "bin_insertion_sort"};

int main() {
    char buff;
    std::vector<std::vector<std::vector<u_int>>> results(sizes_n.size(),
                                                         std::vector<std::vector<u_int>>(sizes_m.size(),
                                                                 std::vector<u_int>(funcs.size(), 0)));
    std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count()); // создание генератора случайных чисел
    std::uniform_int_distribution<int> dist1(33, 126);
    std::string s1;
    for (int i = 0; i < sizes_m.size(); ++i) {
        for (int j = 0; j < sizes_n.size(); ++j) {
            std::vector<std::string> sorted(sizes_n[j]);
            // Считываем массивы
            for (auto & l : sorted) {
                l = "";
                l.resize(sizes_m[i]);
                for (char & k : l) {
                    buff = (char)(dist1(rnd)); // выбор случайной буквы для добавления в строку
                    k = (buff != '\n') ? buff : 'n';
                }
            }
            // Прогоняем замеры.
            for (int k = 0; k < funcs.size(); ++k) {
                results[j][i][k] = check_time<std::string>(funcs[k], sorted, 3);
            }
        }
    }
    for (int i = 0; i < names.size(); ++i) {
        std::cout << names[i] << '\n';
        for (auto & result : results) {
            for (auto & k : result) {
                std::cout << k[i] << ' ';
            }
            std::cout << '\n';
        }
    }
    return 0;
}
