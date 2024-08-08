#include <iostream>
#include <vector>
#include <utility>

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

int main() {
    int size;

    std::cin >> size;
    std::vector<int> sorted(size);
    for (int i = 0; i < size && std::cin >> sorted[i]; ++i) {}
    det_quick_sort(sorted);
    for (int i = 0; i < size && std::cout << sorted[i] << ' '; ++i) {}
    return 0;
}
