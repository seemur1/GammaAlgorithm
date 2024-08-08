#include <iostream>
#include<vector>
#include <unordered_map>
#include <set>
#include <algorithm>
using namespace std;

// Прелюдия - данное решение забыввает про то, что такое кодстайл в методе fiftees_solver.
// ВАЖНО - это было сделано специально, для того, чтобы эта задачка проходила в контесте по памяти.
// Тем не менее, код рабочий, все прочие методы реализованы нормально, и A* тут ест минимальнейшее кол-во памяти.

// Предпосчитанный массив степеней двойки. Задаваал от руки, потому что так быстрее.
const unsigned long long arr[16] = { 1, 16, 256, 4096, 65536, 1048576, 16777216, 268435456, 4294967296, 68719476736, 1099511627776,
17592186044416, 281474976710656, 4503599627370496, 72057594037927936, 1152921504606846976 };

// Предпосчитанное число, описывающее идеальное состояние.
const unsigned long long divider = 17293822569102704640;

// Предпосчитанные расстояния по x и y до правильно раасположенного нолика.
const int constants_x[16] = { 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0 };
const int constants_y[16] = { 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0 };

// Метод, находящий, какое число находится в позиции pos в состоянии marix.
int get_numeric(const unsigned long long int& matrix, const int& pos) {
	return ((matrix << ((16 - pos - 1) * 4)) & divider) >> 60;
}

// Метод, кла
unsigned long long int swap_numerics(const unsigned long long int matrix, const int& a, const int& b) {
	return matrix - get_numeric(matrix, b) * (arr[b] - arr[a]);
}

// Прелюдия: size - задел под игру с большими размерами полей.
// Метод, считающий для состояния matrix массив соседних состояний mates. (куда может перейти нолик)
void neighbours(const unsigned long long int& matrix, const int& size, vector<unsigned long long int>& mates) {
	bool ckecker = true;
	const unsigned short biggestValue = size * (size - 1);
    // Проходимся по всем позициям, пока не найдём нулевую позицию.
    // З.Ы: Тут возможно ускорение кода. (если хранить состояние - как пару типа состояние-позиция нуля)
    // Код писался под контест с ооочень низким ограничением по памяти, пришлось эту пару убрать.
	for (int i = 0; ((i < 16) && (ckecker)); ++i) {
	    // Как только находим позицию с нулём - заполняем маассив соседей.
		if (get_numeric(matrix, i) == 0) {
			ckecker = false;
			if (i >= size) { mates.push_back(swap_numerics(matrix, i, i - size)); }
			if (i < biggestValue) { mates.push_back(swap_numerics(matrix, i, i + size)); }
			if ((i & (size - 1)) < (size - 1)) { mates.push_back(swap_numerics(matrix, i, i + 1)); }
			if ((i & (size - 1)) > 0) { mates.push_back(swap_numerics(matrix, i, i - 1)); }
		}
	}
}

// Метод, считающий аналог манхеттенского расстояния для текущего состояния.
// Немного модифицировался для более оптимальной работы программы, в процессе написания искался подбором. Мб, можно и ещё лучше...)
int number_of_movements(const unsigned long long int& matrix) {
	int buffer1, buffer2, counter = 0;
	int sqr = 16;

	// Типа манхеттенское расстояние.
	for (int i = 0; i < sqr; ++i) {
		buffer1 = get_numeric(matrix, i);
		if (buffer1 != 0) {
			counter += abs(constants_x[i] - constants_x[sqr - buffer1]) + abs(constants_y[i] - constants_y[sqr - buffer1]);
		}
	}

	// Типа подсчёт инверсий на одном столбце.
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			buffer2 = get_numeric(matrix, i * 4 + j);
			for (int k = j + 1; k < 4; ++k) {
				buffer1 = get_numeric(matrix, i * 4 + k);
				if ((buffer1 > buffer2) && (buffer1 != 0) && (buffer2 != 0) && ((sqr - buffer1) / 4 == i) && ((sqr - buffer2) / 4 == i)) {
					counter += 2;
				}
			}
		}
	}

	// Типа подсчёт инверсий на одной строчке.
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			buffer2 = get_numeric(matrix, i * 4 + j);
			for (int k = (i + 1) * 4 + j; k < 16; k = k + 4) {
				buffer1 = get_numeric(matrix, k);
				if ((buffer1 > buffer2) && (buffer1 != 0) && (buffer2 != 0) && ((sqr - buffer1) % 4 == j) && ((sqr - buffer2) % 4 == j)) {
					counter += 2;
				}
			}
		}
	}

	return counter;
}

// Проверка на то, что кол-во инверсий - чётно. (иначе - не собрать(((()
bool is_buildable(const long long int& matrix) {
	int buffer, counter = 0, buffer1;
	for (int i = 15; i >= 0; --i) {
		buffer = get_numeric(matrix, i);
		if (buffer != 0) {
			for (int j = 15; j > i; --j) {
				buffer1 = get_numeric(matrix, j);
				if (buffer1 > buffer) { counter++; }
			}
		}
		else { counter += 1 + (15 - i) / 4; }
	}
	if (counter % 2 == 1) { return false; }
	else { return true; }
}

// Метод, ищущий в состоянии ноль.
// З.Ы: Опять же, есть проостранство для ускорения программы, если хранить все состояния парой. (немного потратив памяти)
int find_0(unsigned long long int matrix) {
	int pos = 0;

	while (matrix > 1) {
		if (matrix % 16 == 0) { return pos; }
		matrix = matrix / 16;
		pos++;
	}
	return 15;
}

// Метод, выводящий path в консоль.
// Полезен, так как find_path - ревёрсит путь => по нему перевёрнутому необходимо всё равно пройти ещё раз.
void print_path(vector<unsigned char>& path) {
	int size = path.size();

	for (int i = 0; i < size; ++i) {
		if (path[path.size() - 1] == 4) { std::cout << 'U'; }
		if (path[path.size() - 1] == 3) { std::cout << 'D'; }
		if (path[path.size() - 1] == 2) { std::cout << 'L'; }
		if (path[path.size() - 1] == 1) { std::cout << 'R'; }
		path.pop_back();
	}
}

// Метод, ищущий путь от last и его parent по d_used (его 3му значению) к началу. Идти от стартового положения не можем,
// Так как запоминали только отцов в fitees_solver.
void find_path(unordered_map<unsigned long long int, tuple<unsigned long long int, int, unsigned long long int>>& d_used,
	vector<unsigned char>& path, unsigned long long int&last, unsigned long long int& parent) {
	int pos1, pos2;
	// Идём, пока не натолкнёмся на старого знакомого. (безотцовщину)
	while (parent != -1) {
		pos1 = find_0(last);
		pos2 = find_0(parent);
		// В зависимости от того, где отноосительно нас лежал ноль - определяем тип поворота.
		if (pos2 + 1 == pos1) { path.push_back(1); }
		if (pos2 - 1 == pos1) { path.push_back(2); }
		if (pos2 + 4 == pos1) { path.push_back(3); }
		if (pos2 - 4 == pos1) { path.push_back(4); }

		last = parent;
		parent = get<2>(d_used[parent]);
	}
}

// Метод подсчёта path. (решение пятнашек)
void fiftees_solver(unordered_map<unsigned long long int, tuple<unsigned long long int, int, unsigned long long int>>& d_used,
	unsigned long long int& matrix, unsigned long long int& parent, unsigned long long int& last) {
	bool ckecker = true;

	// Чтобы не аллоцировать заново память - создаём все вспомогательные структуры заранее.
	set<tuple<unsigned long long int, unsigned long long int>> list_of_vertexes;
	vector<unsigned long long int> mates;
	tuple<unsigned long long int, unsigned long long int> vertex;

	// d_used - хеш-таблица всех состояний, которую мы регулярно пополняем. За счёт доступа к ней за O(1) - всё работает эффективно.
	// Структура кортежей из d_used:
	//0 - distance
	//1 - used
	//2 - parent

	d_used[matrix] = make_tuple(0, 1, -1);
	list_of_vertexes.insert({ number_of_movements(matrix), matrix });

	// Ищем состояние, пока его псевдоманхеттенское расстояние не станет равно нулю.
	// Снизу код, который из-за погони за минимальным расходованием памяти - не очень удобноо читать. Извините:).
	while (ckecker) {
		vertex = *list_of_vertexes.begin();
		get<1>(d_used[get<1>(vertex)]) = 1;
		list_of_vertexes.erase(list_of_vertexes.begin());
		mates.clear();
		neighbours(get<1>(vertex), 4, mates);
		for (int i = 0; i < mates.size(); ++i) {
			if ((d_used.find(mates[i]) == d_used.end()) || ((d_used.find(mates[i]) != d_used.end()) && (get<1>(d_used[mates[i]]) != 1))) {
				if (d_used.find(mates[i]) == d_used.end()) { get<0>(d_used[mates[i]]) = 100000000; }
				if (get<0>(d_used[mates[i]]) > get<0>(d_used[get<1>(vertex)]) + 1) {
					if (get<0>(d_used[mates[i]]) != 100000000) {
						list_of_vertexes.erase(list_of_vertexes.find({ number_of_movements(mates[i]) + get<0>(d_used[mates[i]]), mates[i] }));
					}
					get<0>(d_used[mates[i]]) = get<0>(d_used[get<1>(vertex)]) + 1;
					list_of_vertexes.insert({ number_of_movements(mates[i]) + get<0>(d_used[mates[i]]), mates[i] });
					get<2>(d_used[mates[i]]) = get<1>(vertex);
				}
			}
		}
		// Если псевдоманхеттенское расстояние равно нулю - мы дошли, сворачиваемся.
		if (number_of_movements(get<1>(vertex)) == 0) {
			ckecker = false;
			parent = get<2>(d_used[get<1>(vertex)]);
			last = get<1>(vertex);
		}
	}
}

//--------------------------------------------------------------------------------

int main() {
    // Было сделано для оптимизации. (чтобы заходило в контест) Решил не менять.
	ios_base::sync_with_stdio(0);
	cin.tie(nullptr);

	unordered_map<unsigned long long int, tuple<unsigned long long int, int, unsigned long long int>> d_used;
	vector<unsigned char> path;

	int position;
	unsigned long long int parent, last, amount, matrix = 0;

	for (int i = 0; i < 16; ++i) {
		cin >> position;
		matrix = matrix * 16 + position;
	}
	if (!is_buildable(matrix)) {
		std::cout << -1;
		return 0;
	}

	fiftees_solver(d_used, matrix, parent, last);
	find_path(d_used, path, last, parent);
	std::cout << path.size() << endl;
	print_path(path);
	return 0;
}