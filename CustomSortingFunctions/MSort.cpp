#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

// Слияние отрезка массива.
void merge(std::vector<int> &srt, std::vector<int> &bff, int pos) {
    int f_p = pos, s_p = pos + bff.size(), r_b = std::min(s_p + bff.size(), srt.size()), l_b = s_p;
    for (int i = 0; i < bff.size(); bff[i++] = srt[pos + i]) {}
    for (int i = f_p; i < r_b; ++i) {
        srt[i] = s_p >= r_b || (f_p < l_b && bff[f_p - pos] < srt[s_p]) ? bff[f_p++ - pos] : srt[s_p++];
    }
}

int main() {
    std::cin.tie(nullptr), std::cout.tie(nullptr), std::ios_base::sync_with_stdio(false);
    int size, deepness, curr_length = 1;

    std::cin >> size;

    std::vector<int> sorted(size), buffer;
    deepness = static_cast<int>(std::log2(size)) + 1;

    for (int i = 0; i < size && std::cin >> sorted[i]; ++i) {}
    buffer.reserve(sorted.size());
    for (int i = 0; i < deepness; ++i) {
        for (int j = 0; j < sorted.size() - curr_length; j += curr_length * 2) {
            buffer.resize(curr_length);
            merge(sorted, buffer, j);
        }
        curr_length *= 2;
    }
    for (int i = 0; i < size && std::cout << sorted[i] << ' '; ++i) {}
    return 0;
}