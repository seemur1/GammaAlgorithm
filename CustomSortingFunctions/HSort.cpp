#include <iostream>
#include <vector>

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

int main() {
    std::ios_base::sync_with_stdio(false), std::cin.tie(nullptr), std::cout.tie(nullptr);
    int size;
    std::cin >> size;
    std::vector<int> sorted(size);
    for (int i = 0; (i < size) && std::cin >> sorted[i]; ++i) {}
    for (int i = ((int)sorted.size() - 1) / 2; i >= 0; sDwn(sorted, i--, sorted.size())) {}
    for (int i = sorted.size() - 1; i > 0; sDwn(sorted, 0, i--)) {
        c_swap(sorted[0], sorted[i]);
    }
    for (int i = 0; (i < size) && std::cout << sorted[i] << ' '; ++i) {}
    return 0;
}