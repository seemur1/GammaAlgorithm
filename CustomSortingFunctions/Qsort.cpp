#include <iostream>
#include <vector>
#include <queue>

int useless;

void c_swap(int& fst, int& snd) {
    useless = snd;
    snd = fst;
    fst = useless;
}

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

int main() {
    std::cin.tie(nullptr), std::cout.tie(nullptr), std::ios_base::sync_with_stdio(false);
    int size, l_border, r_border, l_pos, r_pos;
    std::deque<int> l_borders, r_borders;

    // Инициализируем начальные данные.
    std::cin >> size;
    std::vector<int> sorted(size), buffer;
    l_borders.push_back(0), r_borders.push_back(size - 1);
    // Считываем данные.
    for (int i = 0; i < size && std::cin >> sorted[i]; ++i) {}
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
    for (int i = 0; i < size && std::cout << sorted[i] << ' '; ++i) {}
    return 0;
}