#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <algorithm>

#include <queue>
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>
using namespace std;

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Важные структуры, для того, чтобы не запутаться с Гамма-алгоритмом. (Хотя мог и всё через tuple делать)))))

struct Segment {
    unordered_map<int, list<int>*>* edgeList;
    unordered_set<int>* specialVerticles;
    // Пояснение, почему Хеш-таблица - в описании метода resetConnections.
    unordered_set<int>* containingEdges;

    Segment(int, int);
    Segment(unordered_map<int, list<int>*>*);
    Segment(vector<list<int>*>*);
    Segment(Segment*);
};

Segment::Segment(unordered_map<int, list<int>*>* edgeList) {
    this->edgeList = new unordered_map<int, list<int> *>();
    // Так мы не копируем содержимое списков, что очень хорошо по памяти.
    for (pair<const int, list<int> *> & i : *edgeList) { (*this->edgeList)[i.first] = i.second; }
    specialVerticles = new unordered_set<int>();
    containingEdges = new unordered_set<int>();
}

Segment::Segment(vector<list<int>*>* linkTable) {
    edgeList = new unordered_map<int, list<int> *>();
    // Создаём Unordered_map на этом векторе.
    // Так мы не копируем содержимое списков, что очень хорошо по памяти.
    for (int i = 0; i < linkTable->size(); ++i) { (*edgeList)[i] = (*linkTable)[i]; }
    specialVerticles = new unordered_set<int>();
    containingEdges = new unordered_set<int>();
}

// Конструктор для создания сегмента-грани.
Segment::Segment(int first, int second) {
    edgeList = new unordered_map<int, list<int> *>();
    (*edgeList)[first] = new list<int>;
    (*edgeList)[first]->push_back(second);
    (*edgeList)[second] = new list<int>;
    (*edgeList)[second]->push_back(first);
    specialVerticles = new unordered_set<int>();
    specialVerticles->insert(first);
    specialVerticles->insert(second);
    containingEdges = new unordered_set<int>();
}
Segment::Segment(Segment* input) {
    // Так мы не копируем содержимое списков, что очень хорошо по памяти.
    edgeList = new unordered_map<int, list<int>*>();
    for (pair<const int, list<int> *> & i : *input->edgeList) { (*this->edgeList)[i.first] = i.second; }
    specialVerticles = new unordered_set<int>(*(input->specialVerticles));
    containingEdges = new unordered_set<int>(*(input->containingEdges));
}

struct Edge {
    list<int>* verticlesInEdge;
    unordered_set<int>* verticlesInEdgeCharacteristic;
    unordered_set<int>* segmentsInEdge;

    Edge(list<int>*);
    Edge(Edge*);
};

Edge::Edge(list<int>* edgeVerticles) {
    verticlesInEdge = edgeVerticles;
    // Заполняем хеш-таблицу вершин.
    verticlesInEdgeCharacteristic = new unordered_set<int>();
    for (int & i: *edgeVerticles) { verticlesInEdgeCharacteristic->insert(i); }
    segmentsInEdge = new unordered_set<int>();
}
Edge::Edge(Edge* input) {
    verticlesInEdge = new list<int>(*input->verticlesInEdge);
    verticlesInEdgeCharacteristic = new unordered_set<int>(*input->verticlesInEdgeCharacteristic);
    segmentsInEdge = new unordered_set<int>(*input->segmentsInEdge);
}

bool operator < (Segment& a, Segment &b) {
    if (a.containingEdges->size() < b.containingEdges->size()) { return true; }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Полезные методы.

// Метод, считающий количество рёбер в связном графе.
int countEdges(vector<list<int>*>& linkTable) {
    int counter = 0;
    for (list<int>* & i : linkTable) { counter += i->size(); }
    return counter / 2;
}

// Метод, который находит значение ret для данной вершины.
int findRet(vector<list<int>*>& linkTable, vector<list<int>>& kids, vector<int>& parents, vector<int>& in, vector<int>& ret,  int currVerticle) {
    int minimal = linkTable.size();
    // Находим минимаальное среди всх ret потомков.
    for (int & i : kids[currVerticle]) {
        if (minimal > ret[i]) { minimal = ret[i]; }
    }
    // Находим минимальное среди всех in соседей, если таковое - меньше, запоминаем его.
    for (int & i : *linkTable[currVerticle]) {
        if ((i != parents[currVerticle]) && (minimal > in[i])) { minimal= in[i]; }
    }
    // Записываем итоговое значение ret.
    return min(minimal, in[currVerticle]);
}

// Метод, выписывющий правила пользования приложением.
void writeRules() {
    cout << "Добро пожаловать в утилиту, позволяющую определить планарность графа!" << endl;
    cout << "ВАЖНО!!!! Не беспокойтесь, если ваш граф будет несвязен / будет содержать точки сочленения / будет деревом," << endl;
    cout << "То наше приложение разделит ваш граф на подходящие по ТЗ компоненты, и проверит их на планарность." << endl;
    cout << "Таким образом, для абсолютно любого графа приложение сможет определить, планарен ли он. " << endl;
    cout << "Ниже будет написана краткая инструкция того, как пользоваться данным приложением." << endl;
    cout << endl;
    cout << "Создайте в папке cmake-build-debug, (соседка main.cpp) текстовый файл с описанием связей в анализируемом графе." << endl;
    cout << endl;
    cout << "Правила заполнения файла: " << endl;
    cout << "В N строках вы указываете для каждой точки через пробел номера точек, с которыми она связана." << endl;
    cout << "(так как граф - неориентированный, кажжое ребро должно быть отмечено сразу в 2 вершинах!!!)" << endl;
    cout << "Если в графе существует N точек, то номер каждой лежит в диапазоне от 0 до N не включительно." << endl;
    cout << "" << endl;
    cout << "После того, как файл составлен, введите его название. Программ запустится, и выведет вам результат." << endl;
    cout << "Начало работы..." << endl;
    cout << endl;
}

// Метод, конвертирующий строку в граф.
vector<list<int>*>* stringToGraph(vector<string>& lines) {
    // Инициализируем структуры, необходимые для перегонки.
    vector<list<string>> dictionary(lines.size());
    vector<list<int>*>* newGraph = new vector<list<int>*>(lines.size());
    string buffer;

    // Сначала делим строку по пробелаам
    for (int i = 0; i < lines.size(); ++i) {
        istringstream ss(lines[i]);
        while (ss >> buffer) { dictionary[i].push_back(buffer); }
    }
    // А потом каждое число перегоняем в int.
    for (int i = 0; i < dictionary.size(); ++i) {
        (*newGraph)[i] = new list<int>();
        for (string & j : dictionary[i]) { (*newGraph)[i]->push_back(stoi(j)); }
    }
    return newGraph;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Методы "реструктуризации" подграфов.

// Важная напоминалка: Используем переименование только на первых порах! При выполнении непосредственно Гамма-Алгортима,
// переичменовывать при отрезании СТРОГО ЗАПРЕЩЕНО!!!! (потеряется связь вершины с укладкой)
//
// Метод, проходящийся по исходной таблице связанностей linkTable с вершины startingPoint, и ищущей всю её компоненту связанности.
// Возвращает переименованную таблицу связанностей этой компоненты связанностей.
vector<list<int>*>* rename(vector<list<int>*>& linkTable, int startingPoint, vector<bool>& wasNotAllowedToBePutHere,
                           vector<int>& newNames, vector<int>& oldNames) {
    // Создаём новую таблицу связанностей. Нужно для "ужатия" исходной таблицы.
    vector<list<int>*>* newLinkTable = new vector<list<int>*>;
    // Создаём запоминалку того, кто из вершин был посещён на старте работы программы. Храним хеш-таблицей для лучшей ассимптотики.
    unordered_set<int> amountOfVisited;
    // Для корректной работы BFS создаём массив "посещений", и очередь.
    queue<int> queueOfVisited;
    list<int>* buffer;

    int currentVerticle, verticleCounter = 0, counterstep = 0;

    queueOfVisited.push(startingPoint);
    amountOfVisited.insert(startingPoint);

    // Проходимся BFS-ом.
    while(!queueOfVisited.empty()) {
        // Узнаём номер обрабатываемой вершины, доставая первую из очереди.
        currentVerticle = queueOfVisited.front();
        queueOfVisited.pop();
        // Даём этой вершине "корректный" номер.
        newNames[currentVerticle] = verticleCounter;
        oldNames[verticleCounter] = currentVerticle;
        // Если вершина не была блокирующей - проходимся по всем её соседям.
        if (!wasNotAllowedToBePutHere[currentVerticle]) {
            for (int & i : *linkTable[currentVerticle]) {
                // Если мы ещё не посещали эту вершину - закидываем её в очередь, и отмечаем её, как посещённую.
                if (!amountOfVisited.count(i)) {
                    queueOfVisited.push(i);
                    amountOfVisited.insert(i);
                }
            }
        }
        // Добавляем в новую таблицу связанностей соответствующую строчку (чтобы сохранить порядок)
        newLinkTable->push_back(linkTable[currentVerticle]);
        verticleCounter++;
    }

    // Теперь в новой таблице связанностей переименовываем все связи.
    for (list<int>*& i : *newLinkTable) {
        // Если вершина не была граничной, то просто переименовываем значения (как раньше).
        if (!wasNotAllowedToBePutHere[oldNames[counterstep]]) {
            // Проходимся по всем соседям вершины, и переимновывам их.
            for (list<int>::iterator iter = i->begin(); (iter != i->end()); ++iter) { *iter = newNames[*iter]; }
        }
            // А если была - то смотрим, кто был посещён, и создаём новую копию.
        else {
            buffer = new list<int>;
            for (list<int>::iterator iter = i->begin(); (iter != i->end()); ++iter) {
                // Если посещали эту вершину - добавляем её в newNames.
                if (amountOfVisited.count(*iter)) { buffer->push_back(newNames[*iter]); }
            }
            i = buffer;
        }
        counterstep++;
    }
    //"Докидываем" все посещённые вершины к блокирующим.
    for (const int & i: amountOfVisited) { wasNotAllowedToBePutHere[i] = true; }
    // Возвращаем полученную переименованную компоненту связанности.
    return newLinkTable;
}

// Метод, принимающий на вход произвольную таблицу связанности, и разделяющий её на компоненты связанности.
list<vector<list<int>*>*> separateLinkTable(vector<list<int>*>& linkTable, vector<bool>& isProcessed) {

    // Чтобы постоянно не аллоцировать большие куски памяти - аллоцируем "снаружи" массивы переименований.
    // Все вершины обзаведутся новыми именами => лишняя память выделена не будет => большой нужды в использовании
    // unordered_map тут нет. (Memory oferflow тут не будет, т.к. оно бы возникло ещё раньше при создании linkTable)
    // Создаём словарь переименований (чтобы за O(1) потом переименовывать)
    vector<int> newNames(linkTable.size(), -1), oldNames(linkTable.size(), -1);
    // Создаём описание графа.
    list<vector<list<int>*>*> listOfTables;
    // Обрабатываем исходную таблицу связанности, пока не пройдём все вершины / пока все вершины, кроме хотя бы одной
    // (если осталась только одна -  то она изолированная, а значит -  её можно уложить) не были обработаны.
    for (int i = 0; i < linkTable.size(); ++i) {
        if (!isProcessed[i]) { listOfTables.push_back(rename(linkTable, i, isProcessed, newNames, oldNames)); }
    }
    return listOfTables;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// Методы поиска точек сочленения.

//Метод, находящий все точки сочленения в графе linkTable.
//Оффтоп - для обхода Dfs было применено релтзованное ранее решение обхода через стек.
//Оффтоп2 - как оказалось, можно обходить БЕЗ СТЕКА!0!0!0!
//Оффтоп3 - за счёт Rename не съедается лишняя память (все векторы "скучкованы") => можно хранить информацию
// о родилелях и т.д. не в unordered_set, а в простых векторах, что как увеличивает скорость работы, так и упрощает написание кода.
vector<bool>* findBridges(vector<list<int>*>& linkTable) {

    bool checker;
    int currVerticle, counter = 0, verticleToVisit;

    // Векторы, необходимые для работы поиска "мостовых" вершин.
    vector<int> in(linkTable.size(), -1), ret(linkTable.size());
    // Векторы, необходимые для корректной работы DFS с постобработкой:
    vector<list<int>::iterator> positionInNeighbours(linkTable.size());
    vector<list<int>> kids(linkTable.size());
    vector<int> parents(linkTable.size());
    vector<bool> isVisited(linkTable.size(), false);
    vector<bool>* segmentPoints = new vector<bool>(linkTable.size(), false);
    // Для корректной работы алгоритма заполняем массив ссылками на начало списка.
    for (int i = 0; i < positionInNeighbours.size(); ++i) { positionInNeighbours[i] = linkTable[i]->begin(); }

    // Инициализируем данные для первого шага DFS.
    verticleToVisit = 0;
    parents[0] = -1;
    isVisited[0] = true;

    //Собственно, сам DFS.
    while(verticleToVisit != -1) {
        checker = true;
        // Достаём из стека номер вершины, которую нужно обработать.
        currVerticle = verticleToVisit;
        // Заполняем при необходимости время входа.
        if (in[currVerticle] == -1) { in[currVerticle] = counter++; }
        // Идём по всем соседям, начиная с того, на котором в прошлый раз остановились.
        for (list<int>::iterator i = positionInNeighbours[currVerticle]; checker && (i != linkTable[currVerticle]->end()); ++i) {
            if (!isVisited[*i]) {
                checker = false;
                isVisited[*i] = true;
                verticleToVisit = *i;
                parents[*i] = currVerticle;
                kids[currVerticle].push_back(*i);
                // ЭТА СТРОЧКА ДОЛЖНА БЫТЬ ПОСЛЕДНЕЙ!!!!
                positionInNeighbours[currVerticle] = i++;
            }
        }
        // Если не в кого идти, прокидываю в стек отца + считаю ret.
        if (checker) {
            ret[currVerticle] = findRet(linkTable, kids, parents, in, ret, currVerticle);
            // Если отец - корень, то verticleToVisit == -1 => обход прекращается)))))
            verticleToVisit = parents[currVerticle];
        }
    }
    // Если шли больше, чем в 1 вершину из исходной точки - она тоже - точка сочленения.
    if (kids[0].size() > 1) { (*segmentPoints)[0] = true; }
    // Сравниваем все in и out, чтобы найти все точки сочленения, и написать их в segmentPoints.
    for (int i = 1; i < in.size(); ++i) {
        // Проходимся по всем вершинам, в которых побывали из данной.
        for (int & j : kids[i]) {
            // Если выполнено условие из алгоритма - помечаем эту вершину точкой сочленения.
            if (ret[j] >= in[i]) { (*segmentPoints)[i] = true; }
        }
    }
    // Возвращаем список всех "мостов".
    return segmentPoints;
}

//----------------------------------------------------------------Гамма-алгоритм-----------------------------------------------------------------------------
// Гамма-алгоритм.

// Преамбула: Во всех предыдущих методах был использован метод переименования. Здесь все структуры поиндексно связываются
// друг с другом, а посему было принято решение не использовать метод rename в этом блоке кода. В связи с этим, в "отрезанных"
// кусках вершины хранятся довольно "разреженно" => вместо векторов почти повсеместно использовались hash-table.
// Впереди вас ждёт обилие комментариев. Знйте, что они были оставлены скорее не для проверяющего - а для разработчика, чтобы он чего не забыл....)

// ---------------------------------------------------------------Описние структур:------------------------------------------------------------------------

// Храним 2 структуры - Segments (вырезанные "куски") и Edges (укладка)
// Устройство структур:

// Segment consists of:
// edgeList - Cвои рёбра. (хранятся в виде unordered_map, т.к вершины хранятся разреженно.).
// specialVerticles - Набор "граничных" вершин. (хранятся хеш-таблицей, т.к. вершины хранятся разреженно.).
// containingEdges - Набор индексов граней, в которой содержится днный сегмент. (хеш-тблица int-ов == индексу соотв. грани. Требуется проверка + удление за O(1)).

// Edges consists of:
// verticlesInEdge - Список вершин, из которых состоит ребро (хранятся в виде list, так как описывают проход по циклу)
// verticlesInEdgeCharacteristic - verticlesInEdge, не учитывающий очерёдность. Нужен для быстрого поиска вершин. (хранится в виде hash_table)
// segmentsInEdge - Список номеров сегментов, которые лежат в этой грани.

// -----------------------------------------------------------Методы для гамма-алгоритма-------------------------------------------------------------------

// Оффтоп - хотел хранить секторы отсортированной по кол-ву содержащих его граней кучей, но при таком хранении
// После каждого добавления "вырезавшихся" секторов приходилось бы менять массивы ссылок всех граней на все сегменты, что является довольно
// ресурсозатратным процессом => было принято решение находить "вырезаемую" грань за О(n) (храня массив сегментов "стеком"
// перезназначать ссылки особо не надо)))). Кроме того, обычно в самом конце лежат сегменты, которые вписать можно только 1
// способом (интересное наблюдение) => при проходе с конца должны быстро находить нужные сгменты.


// Метод, ищущий номер сегмента на вписание.
// Принимает на вход список сегментов, и кол-во граней.
// ВАЖНО - если нашли сегмент, который нельзя вписать - возвращаем -1.
int segmentToPut(vector<Segment*>& listOfSegments, int amountOfEdges) {
    int min = amountOfEdges + 1, res, i;
    // Проходимся по всем сегментам из списка сегментов, ищем сегмент с наименьшим кол-вом сегментов, в который его можно вписать.
    for (i = listOfSegments.size() - 1; (i >= 0) && (min >= 1); --i) {
        if (listOfSegments[i]->containingEdges->size() < min) {
            min = listOfSegments[i]->containingEdges->size();
            res = i;
        }
    }
    // Если в какой-то момент нашёлся min == 0, то i не уменьшилось до -1 => возвращаем -1.
    if (min == 0) { return -1; }
    // А иначе возвращаем номер соответствующего сегмента.
    return res;
}

// Метод, считающий путь от вершины from до вершины to в графе, и возвращающий его.
// ГАРАНТИРУЕТСЯ, что вершина с номером from и to СОДЕРЖАТСЯ в segmentToLook!!!!
list<int>* findPath(int from, int to, Segment& segmentToLook) {

    bool checker, pathFound = false;
    int currVerticle, verticleToVisit;

    // Векторы, необходимые для корректной работы DFS с постобработкой:
    unordered_map<int, list<int>::iterator> positionInNeighbours;
    unordered_map<int, int> parents;
    unordered_set<int> isVisited;
    list<int>* result = new list<int>();
    // Для корректной работы алгоритма заполняем массив ссылками на начало списка.
    for (pair<const int, list<int> *> & i: *segmentToLook.edgeList) { positionInNeighbours[i.first] = i.second->begin(); }

    // Инициализируем данные для первого шага DFS.
    verticleToVisit = from;
    parents[from] = -1;
    isVisited.insert(from);
    result->push_back(from);

    //Собственно, сам DFS. По условию в нём точно найдётся путь от from до to => пока
    while(!pathFound && (verticleToVisit != -1)) {
        checker = true;
        // Достаём из стека номер вершины, которую нужно обработать.
        currVerticle = verticleToVisit;
        // Закидываем в стек свой номер, если пришли в себя впервые.
        if (result->back() != currVerticle) { result->push_back(currVerticle); }

        // Идём по всем соседям, начиная с того, на котором в прошлый раз остановились.
        for (list<int>::iterator i = positionInNeighbours[currVerticle]; checker && (i != (*segmentToLook.edgeList)[currVerticle]->end()); ++i) {
            // Если нашли вершину, которую искали, и она не наш отец - записываем последнюю вершину в result, и прекращаем поиск.
            if ((*i == to) && ((!parents.count(currVerticle)) || (parents[currVerticle] != to))) {
                result->push_back(to);
                pathFound = true;
                checker = false;
            }
            // Если вершина не искомая, то:
            else {
                // Если вершина не была посещена - идём в неё, если эта вершина не "граничная"
                if (!isVisited.count(*i) && !segmentToLook.specialVerticles->count(*i)) {
                    checker = false;
                    isVisited.insert(*i);
                    verticleToVisit = *i;
                    parents[*i] = currVerticle;
                    // ЭТА СТРОЧКА ДОЛЖНА БЫТЬ ПОСЛЕДНЕЙ!!!!
                    positionInNeighbours[currVerticle] = i++;
                }
            }
        }
        // Если не в кого идти, прокидываю в стек отца. Удаляю последнюю вершину из пути (себя)
        if (checker) {
            verticleToVisit = parents[currVerticle];
            result->pop_back();
        }
    }
    // Если не нашли путь - ругаемся.
    if (!pathFound) { return nullptr; }
    return result;
}

// Метод проверяющий, является ли значение соседом итератора iter.
bool isNeighbour(list<int>::iterator iterator, list<int>& currentPath, int value) {
    list<int>::iterator buffer = iterator;
    // Если итератор указывает на начало списка, то смотрим на "хвост" списка, и на правого соседа.
    if (iterator == currentPath.begin()) {
        if (value == *currentPath.rbegin()) { return true; }
        if (value == *++buffer) {return true; }
        return false;
    }
    // Если итератор указывает на конец списка, то смотрим на начало списка, и на левого соседа.
    else if (*iterator == *currentPath.rbegin()) {
        if (value == *currentPath.begin()) { return true; }
        if (value == *--buffer) {return true; }
        return false;
    }
    // Иначе смотрим на левого и на правого соседа.
    else {
        if (value == *--buffer) { return true; }
        // Так как до этого сдвинули buffer "налево", нужно его 2 раза направо сдвинуть.
        ++buffer;
        if (value == *++buffer) {return true; }
        return false;
    }
}

// Метод, возвращаюший список всех вырезавшихся граней - сегментов.
vector<Segment*>* findEdgeSegment(Segment& segmentToCut, list<int>& cycle) {
    // Создаём массив новых рёбер.
    vector<Segment*>* newEdgeSegmentList = new vector<Segment*>();
    // Из-за того, что рёбра "работают" в 2 стороны, рёбра могут начать вкидываться по 2 копии. Так как нам
    // это не нужно - приходится использовать was_neighboured, запомниающий для вершины список всех бьющих в неё соседей из cycle.
    unordered_map<int, unordered_set<int>> was_neighboured;
    // Чтобы быстро проверять, лежит ли вершин в цикле - создаём хеш-таблицу.
    unordered_set<int> isInCycle;
    // Заполняем её.
    for (int i : cycle) { isInCycle.insert(i); }

    // Проходимся по всем вершинам из вырезааемого пути.
    for (list<int>::iterator i = cycle.begin(); i != cycle.end(); ++i) {
        // Проходимся по всем их соседям в данном сегменте.
        for (int & j: *segmentToCut.edgeList->operator[](*i)) {
            // Если соседняя вершина - "крайняя", и не вырезается то добавляем ребро.
            if (segmentToCut.specialVerticles->count(j) && (!isNeighbour(i, cycle, j))) { newEdgeSegmentList->push_back(new Segment(*i, j)); }
            // Если соседняя вершина - так же лежит в цикле, мы это ребро ещё не добавляли, и оно - не сосед в самом цикле - также добавляем ребро.
            else if ((isInCycle.count(j)) && (!isNeighbour(i, cycle, j)) && (!was_neighboured[*i].count(j))) {
                newEdgeSegmentList->push_back(new Segment(*i, j));
                was_neighboured[j].insert(*i);
            }
        }
    }
    return newEdgeSegmentList;
}


// Оффтоп: по механизму работы очень похож на separateLinkTable. (почти 1-в-1, только тут юзаются хеш-таблицы) Разработчик радуется.

// Метод, находящий кусок сегмента, ограниченный wasNotAllowedToBePutHere. Возвращает вырезавшийся сегмент.
Segment* findBigSegment(Segment& segmentToCut, int startingPoint, unordered_set<int>& wasNotAllowedToBePutHere, list<int>& cycle) {
    // Создаём новую таблицу связанностей. Нужно для "ужатия" исходной таблицы.
    unordered_map<int, list<int>*>* newLinkTable = new unordered_map<int, list<int>*>;
    // Создаём запоминалку того, кто из вершин был посещён на старте работы программы. Храним хеш-таблицей для лучшей ассимптотики.
    unordered_set<int> amountOfVisited;
    // Для корректной работы BFS создаём массив "посещений", и очередь.
    queue<int> queueOfVisited;
    list<int>* buffer;
    Segment* newSegment;

    int currentVerticle;
    // Заполняем информацию для первого шага BFS.
    queueOfVisited.push(startingPoint);
    amountOfVisited.insert(startingPoint);

    // Проходимся BFS-ом.
    while(!queueOfVisited.empty()) {
        // Узнаём номер обрабатываемой вершины, доставая первую из очереди.
        currentVerticle = queueOfVisited.front();
        queueOfVisited.pop();
        // Если вершина не была блокирующей - проходимся по всем её соседям.
        if (!wasNotAllowedToBePutHere.count(currentVerticle)) {
            for (int & i : *(segmentToCut.edgeList)->operator[](currentVerticle)) {
                // Если мы ещё не посещали эту вершину - закидываем её в очередь, и отмечаем её, как посещённую.
                if (!amountOfVisited.count(i)) {
                    queueOfVisited.push(i);
                    amountOfVisited.insert(i);
                }
            }
        }
        // Добавляем в новую таблицу связанностей соответствующую строчку (чтобы сохранить порядок)
        (*newLinkTable)[currentVerticle] = segmentToCut.edgeList->operator[](currentVerticle);
    }

    // Теперь меняем соседей у "граничных" клеток.
    for (pair<const int, list<int> *>& i : *newLinkTable) {
        // Если вершина не была вырезаемой / "крайней", то оставляем всё, как есть.
        // А вот если была, то:
        if (wasNotAllowedToBePutHere.count(i.first)) {
            // Создаём новый список соседей.
            buffer = new list<int>;
            // Проходимся по всем соседям i.
            for (list<int>::iterator iter = i.second->begin(); iter != i.second->end(); ++iter) {
                // Если посещали эту вершину, и она не была вырезаемой, то:
                if (amountOfVisited.count(*iter) && (!wasNotAllowedToBePutHere.count(*iter)))  { buffer->push_back(*iter); }
            }
            // Запоминаем изменённую вершину в buffer.
            i.second = buffer;
        }
    }
    newSegment = new Segment(newLinkTable);

    // Заполняем все specialVerticles в последнем сегменте.
    for (int & i: cycle) {
        // Если посещали эту "граничную" вершину, то кладём её в соотв. список.
        if (amountOfVisited.count(i)) { newSegment->specialVerticles->insert(i); }
    }
    // Так же запоминаем все старые "граничные" вершины.
    // Проходимся по всем граничным вершинам разрезаемого сегманта.
    for (const int & i : *segmentToCut.specialVerticles) {
        // Если вершина с таким же номером содержится в новом - помечаем её как specialVerticles.
        if (newSegment->edgeList->count(i)) { newSegment->specialVerticles->insert(i); }
    }
    // Запоминаем, в кого заходили, если ещё про это не помним.
    for (const int & i: amountOfVisited) {
        if (!wasNotAllowedToBePutHere.count(i)) { wasNotAllowedToBePutHere.insert(i); }
    }
    return newSegment;
}

// Оффтоп: по механизму работы очень похож на separateLinkTable. (почти 1-в-1, только тут юзаются хеш-таблицы) Разработчик вновь радуется.

// Метод, вырезающий набор вершин verticlesToCut из сегментаsegmentToCut, и возвращающий набор "отрезанных" сегментов.
vector<Segment*>* separateSegment(list<int>& verticlesToCut, Segment& segmentToCut) {

    // Инициализируем hash_table вершин, которые будут вырезаться, и которые были посещены. Тут же заполняем его.
    unordered_set<int> visited_verticles;
    for (int & i: verticlesToCut) { visited_verticles.insert(i); }
    // Также мы должны будем игнорировать "крайние" вершины при проходе => и их записываем в "блокирующие"
    for (const int & i : *segmentToCut.specialVerticles) { visited_verticles.insert(i); }

    // Иницилизируем массив "вырезавшихся" сегментов.
    vector<Segment*>* newSegments = new vector<Segment*>(), *buffer;
    // Проходимся по всем вершинам сегмента.
    for (pair<const int, list<int> *> & i : *segmentToCut.edgeList) {
        // Если ещё не посетили данную вершину:
        if (!visited_verticles.count(i.first)) {
            // Докидываем большой вырезавшийся сегмент из данной компоненты связанности.
            newSegments->push_back(findBigSegment(segmentToCut, i.first, visited_verticles, verticlesToCut));
        }
    }
    // Находим все "вырезавшиеся" рёбра.
    buffer = findEdgeSegment(segmentToCut, verticlesToCut);
    // Дозаписываем их в конец ко всем полученным компонентам.
    for (Segment* & i : *buffer) { newSegments->push_back(i); }
    return newSegments;
}

// Метод, определяющий, как заполнять конец грани edgeToFulfil.
void fulfilEdge(list<int>& pathToSplit, list<int>& edgeToFulfil, int value) {
    // Либо записываем путь в грань из начала в конец,
    if (value == *pathToSplit.begin()) {
        list<int>::iterator end = pathToSplit.end();
        end--;
        // Так как последний элемент этого пути уже был добавлен - его также повторно не добавляем.
        for (list<int>::iterator i = pathToSplit.begin(); i != end; ++i) { edgeToFulfil.push_back(*i); }
    }
        // Либо из конца в начало.
    else {
        list<int>::reverse_iterator end = pathToSplit.rend();
        end--;
        // Так как последний элемент этого цикла уже был добавлен - его также повторно не добавляем.
        for (list<int>::reverse_iterator i = pathToSplit.rbegin(); i != end; ++i) { edgeToFulfil.push_back(*i); }
    }
}

//  Метод, заполняющий грань, идя по verticleList, начиная с iter, и правильно записывая вершины из verticleList и pathToSplit в новую грань.
list<int>* fillNewEdge(list<int>& verticleList, list<int>& pathToSplit, list<int>::iterator& iter) {
    int mode = 0;
    list<int>* newEdge = new list<int>();
    // Начинаем резать грань pathToSplit'ом
    for (; mode < 2; iter++) {
        // Если упёрлись в конец грани - переходим в её начало.
        if (iter == verticleList.end()) { iter = verticleList.begin(); }
        // Если встретили конец, или начало вставляемого пути - увеличиваем mode. Как только пройдём и конец, и начало
        // (mode == 2) - прекращаем заполнение первой половины firstEdge.
        if ((*iter == *(pathToSplit.begin())) || (*iter == *(pathToSplit.rbegin()))) { mode++; }
        // Заполняем первую чать firstEdge.
        if (mode == 1) { newEdge->push_back(*iter); }
    }
    // Ссылаемся на тот эл-т, на котором mode стал == 2. (до этого ссылались на следующий за ним эл-т.)
    --iter;
    fulfilEdge(pathToSplit, *newEdge, *iter);
    return newEdge;
}

// Метод, принимающий на вход набор граней, и список вершин, которые нужно "вписать". Находит нужную грань
// (достаёт из сегмента по номеру segmentId). Так же меняет для всех граней, связанных с удаляемым сегментом, список сегментов,
// которым они принадлежат (удаляет соотв. segmentId). Любая грань при вписывании делится на две =>
// одна грань записывается на место родительской, одна дозаписывается в конец набора граней. Возвращаает хеш-таблицу сегментов, которые лежали в разрезанной грани.
unordered_set<int>* splitEgde(vector<Edge*>& listOfEdges, list<int>* pathToSplit, vector<Segment*>& listOfSegments, int segmentId) {
    // Узнаём номер грани, которую мы будем резать. (оффтоп - любая такая грань должна содержать все "граничные" вершины)
    int edgeId = *(listOfSegments[segmentId]->containingEdges->begin());

    // Инициализируем итератор на начало списка.
    list<int>::iterator iter = (*listOfEdges[edgeId]).verticlesInEdge->begin();
    // Запоминаем все сегменты, которые могли быть вписаны в разрезаемую грань.
    unordered_set<int>* segmentsToChange = listOfEdges[edgeId]->segmentsInEdge;
    // Инициализируем 2 новые грани.
    Edge* firstEdge = new Edge(fillNewEdge(*(*listOfEdges[edgeId]).verticlesInEdge, *pathToSplit, iter));
    Edge* secondEdge = new Edge(fillNewEdge(*(*listOfEdges[edgeId]).verticlesInEdge, *pathToSplit, iter));
    // Перезаписываем новые грани в список граней.
    listOfEdges[edgeId] = firstEdge;
    listOfEdges.push_back(secondEdge);
    // Проходимся по всем граням, ссылавшимся на segmentID, который мы вырезали. Он больше не существует => ссылки на него нужно убрать.
    for (const int & i: *listOfSegments[segmentId]->containingEdges) { listOfEdges[i]->segmentsInEdge->erase(segmentId); }
    return segmentsToChange;
}

// Метод, проверяющий, можно ли вписать сегмент в соответствующую грань.
bool isPuttable(Segment& segment, Edge& edge) {
    // Проходимся по всем "граничным" вершинам выбранного сегмента.
    for (const int & i: *segment.specialVerticles) {
        // Если хоть какая-то из граничных вершин не лежит в сегменте - его нельзя вписать!!!!
        if (!edge.verticlesInEdgeCharacteristic->count(i)) { return false; }
    }
    return true;
}

// Единственная причина, по которой юзали хеш-таблицу в Segment
// - хочется искать грани, в которых лежит сегмент + добавлять + удалять их за 0(1) (таков фукционал метода) =>
// В этом нам помошник только хеш-таблица.

// Метод, меняющий в сегментах из containingSegments, edgeId на id одного из 2х новых сегментов / удаляющий его вообще (если сегмент некуда вставить).
// Каждый раз при добавлении Грани edgeId / edges.size() - 1: пополняем containingSegments соответствующей грани.
void resetConnections(vector<Segment*>& listOfSegments, vector<Edge*>& edges, unordered_set<int>& containingSegments, int edgeId, int deletedSegment) {
    for (const int & i: containingSegments) {
        // Если этот сегмент удалялся, то ничего с ним не делаем. Иначе - выполняем следующие операции.
        if (i != deletedSegment) {
            // Если можно уложить сегмент i в грань edgeId, то добавляем в список сегментов этой грани номер соотв. грани.
            if (isPuttable(*listOfSegments[i], *edges[edgeId])) { edges[edgeId]->segmentsInEdge->insert(i); }
                // А если нельзя - пытаемся уложить сегмент в дописанную в конец списка грань.
            else {
                // В любом случае удаляем у сегмента ссылку на edgeId.
                listOfSegments[i]->containingEdges->erase(edgeId);
                // Если получается уложить сегмент в дописанную в конец списка грань - записываем новую связь.
                if (isPuttable(*listOfSegments[i], *edges[edges.size() - 1])) {
                    edges[edges.size() - 1]->segmentsInEdge->insert(i);
                    listOfSegments[i]->containingEdges->insert(edges.size() - 1);
                }
            }
        }
    }
}

// Каждый сегмент из "вырезавшихся" должен:
// 1) Иметь хотя бы одну связь с "соплёй", по которой резали. (которая стала частью одной из новых граней)
// 2) Связываться с ней не в SpecialVErticles => иметь связь с новой "стенкой"
// => В связи с этим, НОВЫЙ (только что вырезанный) сегмент точно может быть вписан только в какое-то из 2 новых рёбер. =>
// пользуемся этим. (в очередной раз упрощаем ассимптотику)
// Метод, заполняющий containingEdges в масиве сегментов listOfSegments. Если нашёлся хотя бы 1 новый сегмент, который
// никуда нельзя уложить - возвращаем false. Инача - true.
bool fillEgdesInSegments(vector<Segment*>& listOfSegments, vector<Edge*>& edges, int edgeId,  int oldSegmentsSize) {
    // Проходимся по всем сегментам из listOfSegments.
    for (int i = 0; i < listOfSegments.size(); ++i) {
        // Если сегмент можно положить в грань edgeId, то создаём новую связь.
        if (isPuttable(*listOfSegments[i], *edges[edgeId])) {
            // Так как этот сегмент мы потом впишем в список старых сегментов, его новый индекс будет не i, а oldSegmentsSize + i.
            edges[edgeId]->segmentsInEdge->insert(oldSegmentsSize + i);
            listOfSegments[i]->containingEdges->insert(edgeId);
        }
        // Также пытаемся положить сегмент в последнюю добавленную грань (вторая вырезавшаяся).
        else if (isPuttable(*listOfSegments[i], *edges[edges.size() - 1])) {
            // Так как этот сегмент мы потом впишем в список старых сегментов, его новый индекс будет не i, а oldSegmentsSize + i.
            edges[edges.size() - 1]->segmentsInEdge->insert(oldSegmentsSize + i);
            listOfSegments[i]->containingEdges->insert(edges.size() - 1);
        }
            // Если данный сегмент никуда не вписался - ругаемся.
        else { return false; }
    }
    // Если ни разу не ругались, то всё прошло хорошо.
    return true;
}

// Метод, вставляющий массив сегментов в конец существующего массива сегментов. Последнеий сегмент записываается на место "родительского".
void insertSegments(vector<Segment*>& oldListOfSegments, vector<Segment*>& newListOfSegments, vector<Edge*>& edges, int fatherId) {
    // Чтобы много раз не аллоцироваать память - проверяем, сможем ли вписать без перезаписывания весь массив сегментов.
    // Если не сможем - сразу занимаем с запасом память (чтобы много раз не вызыввалось реаллокации при вывполнении insertSegments)
    if (oldListOfSegments.capacity() < oldListOfSegments.size() + newListOfSegments.size()) {
        oldListOfSegments.reserve(2 * (oldListOfSegments.size() + newListOfSegments.size()));
    }
    // Проходимся по списку всех сегментов, и добавляем их в конец списка старых сегментов.
    for (Segment* & i : newListOfSegments) { oldListOfSegments.push_back(i); }
    // В самом конце нужно переложить последний сегмент на место fatherId, и изменить все ссылки в segmentsInEdge его containingEdges.
    // Оффтоп - если newListOfSegments пуст, то всё так же будет работать.

    // Проходимся по всем граням, ссылающимся на последний записанный сегмент.
    for (const int & i: *oldListOfSegments[oldListOfSegments.size() - 1]->containingEdges) {
        // Важное уточнение - oldSegmentsSize + i из предыдущего метода отвечал за сохранение "правильных" индексов =>
        // У всех таких граней должны иметься ссылки на сегмент с таким индексом. Эту ссылку нужно удалить)))
        if (edges[i]->segmentsInEdge->count(oldListOfSegments.size() - 1)) {
            edges[i]->segmentsInEdge->erase(oldListOfSegments.size() - 1);
            // Наш сегмент положится на место fatherId => записываем "ссылку" на эту позицию.
            edges[i]->segmentsInEdge->insert(fatherId);
        }
    }
    // Теперь ставим на место отца соотв. сегмент.
    oldListOfSegments[fatherId] = oldListOfSegments[oldListOfSegments.size() - 1];
    // Уменьшаем размер вектора на 1.
    oldListOfSegments.pop_back();
}

// ----------------------------------------------------------Место, где все собрались-----------------------------------------------------------------

// Метод, проверяющий, планарен ли граф linkTable.
bool isPlanar(vector<list<int>*>* linkTable) {
    // Пришлось использовать сслыки, чтобы не использовалось излишнее копирование при выполнении оператора присвоения.
    // Так как структура очень объёмная, копирование портит скорость работы напрочь.
    vector<Segment*>* Segments;
    vector<Segment*>* buffer;
    vector<Edge*> Edges;
    list<int>* pathToDelete;

    //---------------------------------------Создаём первую укладку---------------------------------------

    Segment* seg = new Segment(linkTable);
    // В цикле будет повторяться начальная и конечная вершина. Удаляем её.
    pathToDelete = findPath(0, 0, *seg);
    pathToDelete->pop_back();
    // Создём первые 2 ребра (то-что-внутри цикла, и что снаружи).
    Edges.push_back(new Edge(pathToDelete));
    Edges.push_back(new Edge(Edges[0]));
    // Режем сегмент на "куси"
    Segments = separateSegment(*Edges[0]->verticlesInEdge, *seg);

    // Каждый первый сегмент может быть вписан, как во "внутреннее", так и во "внешнее" ребро.
    for (Segment* & segment : *Segments) {
        segment->containingEdges->insert(0);
        segment->containingEdges->insert(1);
    }
    // Каждая грань содержит каждый из получившихся сегментов.
    for (Edge* & edge: Edges) {
        for (int i = 0; i < Segments->size(); ++i) { edge->segmentsInEdge->insert(i); }
    }

    //---------------------------------------Перваая укладка создана---------------------------------------

    // Выполняем алгоритм, пока есть сегменты в очереди.
    while (Segments->size() > 0) {
        // Выбираем сегмент, который нужно вписать.
        int segToErase = segmentToPut(*Segments, Edges.size());
        // Если нашёлся сегмент, который нельзя вписать (segmentToPut вернул -1) - прееращаем выполнение гаммы.
        if (segToErase == -1) { return false; }
        // Выбираем грань, в которую будем вписывать.
        int edgeToErase = (*(*Segments)[segToErase]->containingEdges->begin());

        //---------------------------------------"Тело" гамма-алгоритма---------------------------------------

        // Запоминаем путь из этого сегмента от 0 до 1 "специальной" вершины.
        pathToDelete = findPath(*((*Segments)[segToErase]->specialVerticles->begin()),
                                *(++(*Segments)[segToErase]->specialVerticles->begin()), *(*Segments)[segToErase]);

        // Вырезаем из вписываемого сегмента найденный ранее путь. На выход получаем список сегментов.
        buffer = separateSegment(*pathToDelete, *(*Segments)[segToErase]);

        // Вставляем вырезанный путь в какую-то из граней. (в какую получится) + для всех "старых" сегментов "перенаправляем" ссылки.
        // На этот момент segToErase ОБЯЗАН помнить, в каких гранях лежал!!!!!
        resetConnections(*Segments, Edges, *splitEgde(Edges, pathToDelete, *Segments, segToErase), edgeToErase, segToErase);

        // Для нововырезанных сегментов определяем, в какой из новых граней они должны лежать. (в описании метода пояснено, почему лежат только в новых)
        // Если метод вернул false, то существует новый сегмент, который не получается куда-лиюо вписать => граф не планарен.
        if (!fillEgdesInSegments(*buffer, Edges, (*(*Segments)[segToErase]->containingEdges->begin()), Segments->size())) {
            return false;
        }
        // Добавлям нововырезанные сегменты к общему набору сегментов.
        insertSegments(*Segments, *buffer, Edges, segToErase);

        //---------------------------------------"Конец произвольной итерации"---------------------------------------
    }
    // Если весь while смог отработать - укладка существует.
    return true;
}

//-----------------------------------------------------------------Тело прогрммы.--------------------------------------------------------------------------

// Метод, проверяющий - планарен ли произвольный введённый граф.
bool GammaAlgorithm(vector<list<int>*>& linkTable) {
    bool checker = true;
    for (vector<list<int>*>* & table : separateLinkTable(linkTable, *new vector<bool>(linkTable.size(), false))) {
        // Так как каждое table описывавет связный граф, рёбер в нём может быть меньше вершин только, если граф - дерево (его уложить в пространстве точно можно!)
        // => Рассматриваем только НЕ деревья.
        if (table->size() <= countEdges(*table)) {
            //  Делим граф на компоненты двойной связанности, и ...
            for (vector<list<int>*>* & segment : separateLinkTable(*table, *findBridges(*table))) {
                // Если компонента - не дерево, то запускаем гаммма-алгоритм.
                if (segment->size() <= countEdges(*segment)) {
                    // ... проверяем на планарность их. Если хотя бы одна такая компонента не планарна - весь граф не планарен.
                    if (!isPlanar(segment)) { checker = false; }
                }
            }
        }
    }
    // Если все части графа получилось уложить - он планарен!
    return checker;
}

int main() {
    int amountOfEdges, buffer;
    string path, line;
    vector<string> stringGraph;

    writeRules();
    cout << "Введите имя вышего файла!" << endl;
    cin >> path;
    ifstream in;
    in.open(path);
    // Считываем файл.
    while (getline(in, line)) { stringGraph.push_back(line); }
    in.close();
    vector<list<int>*>* randomGraph = stringToGraph(stringGraph);
    if (GammaAlgorithm(*randomGraph)) {cout << "Граф планарен!"; }
    else { cout << "Граф не планарен!!!"; }
    return 0;
}