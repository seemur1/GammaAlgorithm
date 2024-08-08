#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <queue>

using namespace std;

// Публичная переменная текст, позволяющая удобно обрабатывать суффиксы.
string text;
// Публичная переменная currentEnd, позволяющая удобно увеличивать длину всех листовых рёбер за О(1).
int* currentEnd, beginningOfASuffix, pos = 0;

// Структура Суффиксного Дерева.
struct Node {
    // Хеш-таблица рёбер (для быстрого перехода по первой букве) Хранит конец отрезка ссылкой, чтобы обновлять его длину за единицу.
    unordered_map<char, tuple<int, int*, Node*>> edgeList;
    // Суффиксная ссылка.
    Node* suffixLink;
    // Глубина вершины (по буквам относительно начала суффикса).
    int deepness;

    // Оффтоп - некоторые вершины рёберной глубины 1 (НЕ ЛИСТЬЯ!) сслыаются суффиксной ссылкой на корень дерева.
    // Корень же дерева ссылается сам на себя.

    // Конструктор Корня.
    Node(int currentDeepness);
    // Конструктор корня, в котором заполняется суф. ссылка.
    Node(int currentDeepness, Node* suffixLink);
    // Метод, делящий одно из рёбер, новой нодой.
    Node* splitEdge(int pos, char charAtStart);
    // Метод, спускающийся до последней валидной вершины из данной.
    Node* descent();
    // Метод, спускающийся вниз, пока получается, а затем прыгающий по суф ссылке. Останавливается, когда достигает корня,
    // либо когда получается спуститься на "breaking letter". При спуске меняет beginningOfASuffix, добавляет суф. ссылки, и прыгает из вершины
    // в вершину. Возращет ту вершину, в которой мы остановились.
    Node* recursiveReturn();
    // Метод, ищущий все вхождения подстроки в суф дерево по этой вершине.
    vector<int> findAllSubStrings(string subString);
    // Метод, идущий до всех листов от данной вершины, а потом для каждого листа высчитывающий индекс входа.
    vector<int> findAllEntriesBFS();
};

// Вершина, в которой будет храниться информация о том, какой последней вершине нужно обновить суф. ссылку.
Node* nodeTrace = nullptr;

Node::Node(int currentDeepness) : deepness(currentDeepness) {
    suffixLink = this;
    // Всегда в рёбра добавляется "мусорное" ребро (в котором хранится элемент на позиции currentEnd)
    edgeList[text[*currentEnd]] = {*currentEnd, currentEnd, nullptr};
}

Node::Node(int currentDeepness, Node* suffixLink) : Node(currentDeepness) {
    this->suffixLink = suffixLink;
}

Node* Node::splitEdge(int pos, char charAtStart) {
    Node* buffer = get<2>(edgeList[charAtStart]);
    // Записываем новую вершину (Закрепляем на неё ноду, которая раньше висела на конце нашего ребра).
    get<2>(edgeList[charAtStart]) = new Node(deepness + pos, this);
    // Добавляем к новой вершине ответление == отрезанному нашему "хвостику".
    get<2>(edgeList[charAtStart])->edgeList[text[get<0>(edgeList[charAtStart]) + pos]] =
            {get<0>(edgeList[charAtStart]) + pos, get<1>(edgeList[charAtStart]), buffer};
    // Меняем правую границу разрезаемого ребра на позицию до разреза.
    get<1>(edgeList[charAtStart]) = new int(get<0>(edgeList[charAtStart]) + pos - 1);
    return get<2>(edgeList[charAtStart]);
}

Node* Node::descent() {
    // Запоминаем букву, по оторой будем пытаться перейти по ребру.
    char currentLetter = text[beginningOfASuffix + deepness];
    // Если такого ребра попросту нет из текущей вершины - возвращаемся из рекурсии. (пришли в вершину, из которого будем вести новое ребро)
    if (edgeList.count(currentLetter) == 0) {
        return this;
    }
    // Если можно перейти по ребру:
    else {
        // Если глубина вершины, в которое ведёт ребро, всё ещё меньше текущей глубины - переходим по ребру.
        // Так как на ребре может висеть null - используем разность индексов на ребре.
        if (*get<1>(edgeList[currentLetter]) - get<0>(edgeList[currentLetter]) < *currentEnd - beginningOfASuffix - deepness) {
            return get<2>(edgeList[currentLetter])->descent();
        }
            // Иначе - мы нашли ребро, которое будем резать. Обновляем отступ + выходим из рекурсии.
        else {
            pos = get<0>(edgeList[currentLetter]) + *currentEnd - deepness - beginningOfASuffix;
            return this;
        }
    }
}

Node* Node::recursiveReturn() {

    // Если додвигали beginningOfASuffix до конца рассматриваемого подслова - выходим из рекурсии. (что важно - мы сейчас в корне)
    if (*currentEnd == beginningOfASuffix) {
        // Для корректной работы алгоритма - затираем след.
        nodeTrace = nullptr;
        // Если такового ребра нет - Добавляем ребро с мусорной буквой.
        if (edgeList.count(text[*currentEnd]) == 0) {
            edgeList[text[*currentEnd]] = {*currentEnd, currentEnd, nullptr};
            // currentEnd будет увеличен в цикле for, но кто увеличит beginningOfASuffix? Так поможем же ему!
            ++beginningOfASuffix;
            // В корне pos == 0 после итерирования.
            pos = -1;
        }
        // А если таковое ребро уже есть - переходим по нему, не сдвигая последний beginningOfASuffix.
        // Для этого достаточно просто сделать pos == pos соотв. ребра.
        else {
            pos = get<0>(edgeList[text[*currentEnd]]);
        }
        return this;
    }
    // Если после descent стоим в вершине, к которой должна добавляться самая последняя обработанная буква:
    if (*currentEnd - beginningOfASuffix == deepness) {
        // Меняем суффиксную ссылку у предыдущей вершины (если таоквую нужно было менять).
        if (nodeTrace != nullptr) {
            nodeTrace->suffixLink = this;
            // Так как мы стоиим вершине - у неё уже стопудово есть валидная суф ссылка => ничего менять не надо)))
            nodeTrace = nullptr;
        }
        // Если буква есть в одной из вершин - обновляем pos, и возвращаем ссылку на нас.
        // (выходим из рекурсии, т.к. мы - последняя вершина для спуска.)
        if (edgeList.count(text[*currentEnd]) != 0) {
            pos = get<0>(edgeList[text[*currentEnd]]);
            return this;
        }
        // А если соотв. ребра нет:
        else {
            // Добавляем такое ребро.
            edgeList[text[*currentEnd]] = {*currentEnd, currentEnd, nullptr};
            // Двигаем начало обрабатываемого слова на один.
            beginningOfASuffix++;
            // Переходим рекурсивно по своей суффиксной ссылке, по которой предварительно максимально спускаемся.
            return suffixLink->descent()->recursiveReturn();
        }
    }
    // А если самая последняя обработнная буква оказывается в ребре:
    else {
        // Обновляем вершинку в nodeTrace, в которую затем будем вписывать последующие суф. ссылки. В зав-ти от того, была ли она пустой -
        // записываем в неё данные мб.
        nodeTrace = (nodeTrace == nullptr) ? splitEdge((*currentEnd) - beginningOfASuffix - deepness, text[beginningOfASuffix + deepness])
                : nodeTrace->suffixLink = splitEdge((*currentEnd) - beginningOfASuffix - deepness, text[beginningOfASuffix + deepness]);
        // Двигаем beginningOfASuffix на 1.
        beginningOfASuffix++;
        // Спускаемся рекурсивно в свою суф. ссылку.
        return suffixLink->descent()->recursiveReturn();
    }
}

vector<int> Node::findAllSubStrings(string subString) {
    // Если первой буквы из обрбтываемого слова нет ни в одном суффиксе - то и дело с концами, подслово не входит.
    if (edgeList.count(subString[0]) == 0) {
        return {};
    }
    // Если же всё норм - нчинаем спуск:
    else {
        // Инициализируем pos.
        pos = get<0>(edgeList[subString[0]]);
        // Каждый раз нужно помнить позицию, с которой стартовали спуск по ребру.
        beginningOfASuffix = 0;
        // Хитрый факт - чтобы можно было бы гипотетически искать все вхождения у каждого поддерева -
        // Будем поддерживаать относительную глубину (deepness == глубина потомка - глубина батьки).
        // Так как у нас будет меняться обрабатываемая вершина - пока запоминаемся в текущей.
        nodeTrace = this;
        // Проходимся по всем буквам:
        for (int i = 0; i < subString.length(); ++i) {
            // Если pos "Выбежал" за ребро - обновляем pos.
            if (pos > (*get<1>(nodeTrace->edgeList[subString[beginningOfASuffix]]))) {
                nodeTrace = get<2>(nodeTrace->edgeList[subString[beginningOfASuffix]]);
                // Если ребра, начинающегося на нашу букву нет - дропаем пустой масссив.
                if(nodeTrace->edgeList.count(subString[i]) == 0) {
                    return {};
                }
                // А если есть - переходим в соотв. ребро.
                else {
                    pos = get<0>(nodeTrace->edgeList[subString[i]]);
                    // И перемещаем beginningOfASuffix.
                    beginningOfASuffix = nodeTrace->deepness - deepness;
                }
            }
            // Если не закончили идти по ребру - сравниваем текущий pos и i. Если не совпали - БАРДАК! Дропаем пусстой масссив.
            else if (text[pos] != subString[i]) {
                return {};
            }
            ++pos;
        }
        // Будем искать листья в вершине под последним ребром. Если оно само - лист, то мы нашли единственное вхождение.
        if (get<2>(nodeTrace->edgeList[subString[beginningOfASuffix]]) == nullptr) {
            return {(int)(text.length()) - nodeTrace->deepness - *get<1>(nodeTrace->edgeList[subString[beginningOfASuffix]]) +
                    get<0>(nodeTrace->edgeList[subString[beginningOfASuffix]])};
        }
        // Если же под нами хоть кто-то есть:
        else {
            nodeTrace = get<2>(nodeTrace->edgeList[subString[beginningOfASuffix]]);
            // Если после прохода не дропнули пустой массив - интересненько. Теперь Ищем все вхождения. Делать мы это будем, конечно,
            // BFS-ом, так как все листовые потомки будут являться концами суффиксов, в которых префиксом является subString =>
            // subString является подсловом. Так же, за счёт того, что во всех рёбрах хранится l, r, и deepness - можем восстановить
            // из листа индекс вхождения. Ассимтотика = o(p), за счёт того, что ассимтотика bfs = o(n + m), а вершин не больше,
            // чем кол-во листьев => так как листьев в поддереве, по кооторому идём dfs == p, то пройти нужно не более, по p
            // вершинам => m + n <= 2p => Ассимтотика = O(p).
            return nodeTrace->findAllEntriesBFS();
        }
    }
}

// Пояснеение - юзаю BFS, так как не оч хочется лишний раз юзать рекурсию. При реализации Уккоен это было ещё хоть
// как-то оправдно, так как за счёт этого в несколько раз уменьшился объём кода + descent относительно недолго будет
// вниз шагать. Тут же поддерево может быть довольно большим, что может оч подпортить картину.
vector<int> Node::findAllEntriesBFS() {
    Node* buffer;
    // Создаём вектор, хранящий индексы всех входов наашего суффикса.
    vector<int> result;
    // Оффтоп - так как мы - дерево, то циклов нет => можно не хранить, кто посещён, а кто - нет.
    // Для корректного отрбатывания BFS, создаём очередь вершин на посещение.
    queue<Node*> nodesToVisit;
    nodesToVisit.push(this);
    while (!nodesToVisit.empty()) {
        // Достаём первый элемент из очереди.
        buffer = nodesToVisit.front();
        nodesToVisit.pop();
        // Проходимся по всем рёбрам посона.
        for (auto i : buffer->edgeList) {
            // Собираем всех детей в очередь на обработку.
            if ((get<2>(i.second) != nullptr)) {
                nodesToVisit.push(get<2>(i.second));
            }
            // А все листья - закидываем в результат.
            else {
                result.push_back((int)(text.length()) - buffer->deepness - *get<1>(i.second) + get<0>(i.second));
            }
        }
    }
    return result;
}

int main() {
    int amountOfWords;
    string subStringToFind;
    vector<int> amountOfInputs;
    currentEnd = new int(0);
    cout << "Введите строку для анализа." << endl;
    getline(cin, text);
    // Добвляем "лишний" символ, для корректной работы алгоритма.
    text.push_back('$');

    Node* suffixTree = new Node(0), *currentNode = suffixTree;

    ++(*currentEnd);
    beginningOfASuffix = *currentEnd;

    // Потихоньку обноляем currentLength. Вместе с ним направо двигается и счётчик.
    for (; *currentEnd < text.length(); ++(*currentEnd)) {
        // Если искомой буквы из соотв. вершины нет - добавляем её, и спускаемся.
        if (currentNode->edgeList.count(text[beginningOfASuffix + currentNode->deepness]) == 0) {
            currentNode = currentNode->recursiveReturn();
        }
        // Проверяем обновившийся pos. Если с ним что-то не так - меняем режим работы программы.
        else if (pos > (*get<1>(currentNode->edgeList[text[beginningOfASuffix + currentNode->deepness]]))) {
            // Перепрыгиваем в вершину, лежавшую в конце ребра.
            currentNode = get<2>(currentNode->edgeList[text[beginningOfASuffix + currentNode->deepness]]);
            // Если ребра, начинающегося на нашу букву нет - рекурсивно спускаемся вниз, и встаём в вершину, куда получилось подняться.
            if(currentNode->edgeList.count(text[*currentEnd]) == 0) {
                currentNode = currentNode->recursiveReturn();
            }
            // А если есть - обновляем счётчик, по которому идём.
            else {
                pos = get<0>(currentNode->edgeList[text[*currentEnd]]);
            }
        }
        // Если режим работы программы не изменён -
        else {
            // Если не совпали буквы на соответствующих местах - рекурсивно возвращаемся вниз.
            if (text[*currentEnd] != text[pos]) {
                // И встаём в вершину, куда получилось подняться.
                currentNode = currentNode->recursiveReturn();
            }
        }
        pos++;
    }
    // Возвращааем счётчик на прежнее положение.
    --(*currentEnd);
    // Общение с пользователем + ввод кол-ва слов.
    cout << "Suffix Tree is Built!" << endl << "Write the amount of words to find!" << endl;
    cin >> amountOfWords;
    // Проходимся по всем словам на поиск.
    for (int i = 0; i < amountOfWords; ++i) {
        // Считываем слово.
        cout << "Write your " << i << "word!: " << endl;
        cin >> subStringToFind;
        // Пытаемся найти индексы всех входов.
        amountOfInputs = suffixTree->findAllSubStrings(subStringToFind);
        // Если не нашлось ни одного - ругаемся, идём дальше.
        if (amountOfInputs.size() == 0) {
            cout << "Such substring can not be found in your text!" << endl;
        }
        // Если нашлись - выводим все.
        else {
            cout << "The indexes of all entries are:" << endl;
            for (int j = 0; j < amountOfInputs.size(); ++j) {
                cout << amountOfInputs[j] << ' ';
            }
            cout << endl;
        }
    }
    return 0;
}