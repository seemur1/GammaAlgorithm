// В данной части лабораторной работы, сравнивается скорость работы некоторых нелинейных сортировок:
// вставками / бинарными всставками / кучей / быстрая / слиянием / быстрая с выбором пивота.
// ВСЕ сортировки (кроме быстрой с выбором пивота) - реализованы БЕЗ рекурсии! (итеративно)
// Интересное наблюдение: Реализованная сортировка слиянием работает сопоставимо быстро с
// быстрой сортировкой! (следствие эффективной работы с памятью)

#include <iostream>
#include <ctime>
#include <vector>
#include <utility>
#include <random>
#include <limits>
#include <map>
#include <deque>

//--------------------------------------------------------------------------------------------------
void insertion_sort(std::vector<int>& sorted) {
    for (int i = 1; i < sorted.size(); ++i) {
        for (int j = i; (j > 0) && (sorted[j] < sorted[j - 1]); --j) {
            sorted[j] = sorted[j - 1] - (sorted[j - 1] = sorted[j]) + sorted[j];
        }
    }
}
//--------------------------------------------------------------------------------------------------
// Бин. поиск. Обрабатывает так же случай с "краем" снаружи массива.
int search(const std::vector<int>& sorted, int value, int l_b, int r_b) {
    for (int mid; r_b - l_b > 1; l_b = (sorted[mid] > value) ? l_b - (r_b = mid) + mid : mid) {
        mid = (l_b + r_b) / 2;
    }
    return (sorted[l_b] > value) ? l_b : ((sorted[r_b] > value) ? r_b : r_b + 1);
}

void bin_insertion_sort(std::vector<int>& sorted) {
    int pos;
    for (int i = 1; i < sorted.size(); ++i) {
        pos = search(sorted, sorted[i], 0, i - 1);
        for (int j = i; (j > pos); --j) {
            sorted[j] = sorted[j - 1] - (sorted[j - 1] = sorted[j]) + sorted[j];
        }
    }
}
//--------------------------------------------------------------------------------------------------

int useless;

void c_swap(int& fst, int& snd) {
    useless = snd;
    snd = fst;
    fst = useless;
}

int sDwn(std::vector<int> &arr, int pos, int sze) {
    int max_p = pos, lft = pos * 2 + 1, rgt = pos * 2 + 2, bff = arr[pos];
    max_p = ((lft < sze) && arr[lft] > arr[max_p]) ? lft : max_p;
    max_p = ((rgt < sze) && arr[rgt] > arr[max_p]) ? rgt : max_p;
    (max_p != pos) ? arr[pos] = arr[max_p] + (arr[max_p] = bff) - bff : 0;
    return (max_p != pos) ? sDwn(arr, max_p, sze) : 0;
}

void heap_sort(std::vector<int>& sorted) {
    for (int i = ((int)sorted.size() - 1) / 2; i >= 0; sDwn(sorted, i--, sorted.size())) {}
    for (int i = sorted.size() - 1; i > 0; sDwn(sorted, 0, i--)) {
        c_swap(sorted[0], sorted[i]);
    }
}
//--------------------------------------------------------------------------------------------------
// Слияние отрезка массива.
void merge(std::vector<int> &srt, std::vector<int> &bff, int pos) {
    int f_p = pos, s_p = pos + bff.size(), r_b = std::min(s_p + bff.size(), srt.size()), l_b = s_p;
    for (int i = 0; i < bff.size(); bff[i++] = srt[pos + i]) {}
    for (int i = f_p; i < r_b; ++i) {
        srt[i] = s_p >= r_b || (f_p < l_b && bff[f_p - pos] < srt[s_p]) ? bff[f_p++ - pos] : srt[s_p++];
    }
}

void merge_sort(std::vector<int>& sorted) {
    int deepness = static_cast<int>(std::log2(sorted.size())) + 1, curr_length = 1;
    std::vector<int> buffer;
    buffer.reserve(sorted.size());
    for (int i = 0; i < deepness; ++i) {
        for (int j = 0; j < sorted.size() - curr_length; j += curr_length * 2) {
            buffer.resize(curr_length);
            merge(sorted, buffer, j);
        }
        curr_length *= 2;
    }
}

//--------------------------------------------------------------------------------------------------
// "Фильтруем" отрезок между l_b и r_b с разбиением Хоара. Возвращам новый индекс "пивота".
void filter(std::vector<int> &arr, int *l_p, int *r_p, int pivot) {
    while (*l_p <= *r_p) {
        for (; arr[*l_p] < pivot; ++(*l_p)) {}
        for (; arr[*r_p] > pivot; --(*r_p)) {}
        if (*l_p <= *r_p) {
            c_swap(arr[(*l_p)++], arr[(*r_p)--]);
        }
    }
}

void quick_sort(std::vector<int>& sorted) {
    int l_border, r_border, l_pos, r_pos;
    std::deque<int> l_borders, r_borders;
    l_borders.push_back(0), r_borders.push_back((int)sorted.size() - 1);
    // Сортируем быстрой сортировкой.
    while (!l_borders.empty()) {
        // Узнаём диапазон для обработки.
        l_pos = l_border = l_borders.front(), r_pos = r_border = r_borders.front();
        l_borders.pop_front(), r_borders.pop_front();
        // Сортируем по pivot + указываем новые отрезки для обработки.

        filter(sorted, &l_pos, &r_pos, sorted[(l_pos + r_pos) / 2]);
        if (l_border < r_pos) {
            l_borders.push_back(l_border), r_borders.push_back(r_pos);
        }
        if (l_pos < r_border) {
            l_borders.push_back(l_pos), r_borders.push_back(r_border);
        }
    }
}
//--------------------------------------------------------------------------------------------------

int findMedian(std::vector<int>& sorted, int pos, int n) {
    sort(sorted.begin() + pos, sorted.begin() + pos + n);
    return sorted[pos + n / 2];
}

int partition(std::vector<int>& sorted, int l, int r, int value) {
    int pos;
    for (pos = l; pos < r && (sorted[pos] != value); pos++) {}
    std::swap(sorted[pos], sorted[r]);

    for (int j = pos = l; j <= r - 1; j++) {
        if (sorted[j] <= value) {
            std::swap(sorted[pos++], sorted[j]);
        }
    }
    std::swap(sorted[pos], sorted[r]);
    return pos;
}

int find_k(std::vector<int>& sorted, int l, int r, int k) {

    if (k > 0 && k <= r - l + 1) {
        int size = r - l + 1, pos, medOfMed, res;
        std::vector<int> median((size + 4) / 5);

        for (pos = 0; pos < size / 5; median[pos++] = findMedian(sorted, l + pos * 5, 5)) {}
        if (pos * 5 < size) {
            median[pos++] = findMedian(sorted, l + pos * 5, size % 5);
        }
        medOfMed = (pos == 1) ? median[pos - 1] : find_k(median, 0, pos - 1, pos / 2);
        res = partition(sorted, l, r, medOfMed);

        if (res - l == k - 1) {
            return sorted[res];
        } else if (res - l > k - 1) {
            return find_k(sorted, l, res - 1, k);
        }
        return find_k(sorted, res + 1, r,k - res + l - 1);
    }

    return INT_MAX;
}

void quickSort(std::vector<int>& sorted, int l, int r) {
    if (l < r) {
        int p = partition(sorted, l, r, find_k(sorted, l, r, (r - l + 1) / 2));
        quickSort(sorted, l, p - 1);
        quickSort(sorted, p + 1, r);
    }
}

void det_quick_sort(std::vector<int>& sorted) {
    quickSort(sorted, 0, sorted.size() - 1);
}

//--------------------------------------------------------------------------------------------------

// Засекаем время.
template<typename T>
unsigned long long int check_time(void (*func)(std::vector<T>&), const std::vector<T>& sorted,
                                  std::vector<std::vector<std::pair<u_int, u_int>>>& times, int pos, int sort_pos, int turns) {
    unsigned long long int res = 0;
    for (int i = 0; i < turns; ++i) {
        std::vector<T> buffer = sorted;
        times[pos][sort_pos].first = clock(); // начальное время
        func(buffer);
        times[pos][sort_pos].second = clock(); // конечное время
        res += times[pos][sort_pos].second - times[pos][sort_pos].first;
    }
    return res / turns;
}

std::vector<int> sizes = {20, 100, 500, 2000, 10000, 100000, 1000000, 10000000};
std::vector<void (*)(std::vector<int>&)> funcs = {insertion_sort, bin_insertion_sort, heap_sort, merge_sort, quick_sort, det_quick_sort};

int main() {
    std::vector<std::vector<std::pair<u_int, u_int>>> times(sizes.size(), std::vector<std::pair<u_int, u_int>>(funcs.size(), {-1, -1}));
    std::vector<std::vector<u_int>> results(sizes.size(), std::vector<u_int>(funcs.size(), -1));
    std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count()); // создание генератора случайных чисел
    std::uniform_int_distribution<int> rng(-1000000000, 1000000000);
    for (int i = 0; i < sizes.size(); ++i) {
        std::vector<int> sorted(sizes[i]);
        // Считываем массив
        for (int j = 0; j < sorted.size(); ++j) {
            sorted[j] = rng(rnd);
        }
        // Прогоняем замеры.
        for (int j = 0; j < funcs.size(); ++j) {
            if (j >= 2 || sorted.size() < 100000) {
                results[i][j] = check_time<int>(funcs[j], sorted, times, i, j, 3);
            }
        }
    }
    for (int i = 0; i < results[0].size(); ++i) {
        for (auto & result : results) {
            std::cout << result[i] << ' ';
        }
        std::cout << '\n';
    }
    return 0;
}
