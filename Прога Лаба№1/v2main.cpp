#include <iostream>
#include <fstream>

using namespace std;

const char FILEIN[] = "string2.txt";
const char FILEOUT[] = "out.txt";

struct String{
    char* data;
    char mark;  
};

void printstr(ofstream *fileout, String *Text);
unsigned char getmark(ifstream *filein);
void str_in(ifstream *filein, String *Text, int size);

int main() {
    ifstream filein(FILEIN);


    if (!filein.is_open()) {
        cerr << "Ошибка: Не удалось открыть входной файл.\n";
        return 0;
    }

    cout << "Лабораторная №1, Версия №1 по программированию (второй семестр)\n";
    cout << "Выполнил Решетников Семён, группа 5352.\n\n";

    unsigned char mark;
    int size;

    mark = getmark(&filein);
    if (mark == 0) return 0;

    filein >> size;   // читаем количество символов

    if (size <= 0) {
        cerr << "Ошибка: Некорректный размер строки.\n";
        return 0;
    }

    String Text;
    Text.data = new char[size + 1];
    Text.mark = mark;

    filein >> noskipws;  // чтобы читать пробелы
    filein.get();        // убрать перевод строки после числа

    str_in(&filein, &Text, size);

    bool changed = true;
    while (changed) {
        changed = false;
        int pos = 0; // текущая позиция в строке

        while (pos < size) {
            // Пропускаем пробелы и переводы строк
            while (pos < size && (Text.data[pos] == ' ' || Text.data[pos] == '\n')) {
                pos++;
            }
            if (pos >= size) break;

            int wordStart = pos;
            int wordEnd = wordStart;
            while (wordEnd < size && Text.data[wordEnd] != ' ' && Text.data[wordEnd] != '\n') {
                wordEnd++;
            }
            if (wordEnd >= size) break; // дошли до конца

            char lastChar = Text.data[wordEnd - 1];

            // Поиск ближайшего справа слова, начинающегося с lastChar
            int searchPos = wordEnd;
            int foundStart = -1, foundEnd = -1;
            while (searchPos < size) {
                // Пропускаем разделители
                while (searchPos < size && (Text.data[searchPos] == ' ' || Text.data[searchPos] == '\n')) {
                    searchPos++;
                }
                if (searchPos >= size) break;

                if (Text.data[searchPos] == lastChar) {
                    foundStart = searchPos;
                    foundEnd = foundStart;
                    while (foundEnd < size && Text.data[foundEnd] != ' ' && Text.data[foundEnd] != '\n') {
                        foundEnd++;
                    }
                    break;
                } else {
                    // Пропускаем неподходящее слово
                    while (searchPos < size && Text.data[searchPos] != ' ' && Text.data[searchPos] != '\n') {
                        searchPos++;
                    }
                }
            }

            if (foundStart != -1) {
                // Создаем новую строку
                char* newData = new char[size + (wordEnd - wordStart) + 1]; // достаточно
                int newPos = 0;

                // Копируем часть до начала текущего слова
                for (int i = 0; i < wordStart; i++) {
                    newData[newPos++] = Text.data[i];
                }

                // Копируем промежуток от конца текущего слова до начала найденного
                // Если текущее слово было первым (wordStart == 0), пропускаем первый разделитель
                int startGap = (wordStart == 0) ? wordEnd + 1 : wordEnd;
                for (int i = startGap; i < foundStart; i++) {
                    newData[newPos++] = Text.data[i];
                }

                // Копируем текущее слово
                for (int i = wordStart; i < wordEnd; i++) {
                    newData[newPos++] = Text.data[i];
                }

                // Копируем найденное слово без первого символа
                for (int i = foundStart + 1; i < foundEnd; i++) {
                    newData[newPos++] = Text.data[i];
                }

                // Копируем остаток после найденного слова
                for (int i = foundEnd; i < size; i++) {
                    newData[newPos++] = Text.data[i];
                }

                // Устанавливаем маркер
                newData[newPos] = Text.mark;

                // Заменяем старый массив
                delete[] Text.data;
                Text.data = newData;
                size = newPos;
                changed = true;

                // Начинаем новый проход с начала строки
                break; // выходим из внутреннего while, внешний while начнется заново
            } else {
                // Переходим к следующему слову
                pos = wordEnd + 1;
            }
        }
    }

    // Вывод результата
    ofstream fileout(FILEOUT);

    printstr(&fileout, &Text);
    fileout << endl;

    delete[] Text.data;
    return 0;
}

void printstr(ofstream *fileout, String *Text){
    int space = 0, words = 0;
    for (int i = 0; Text->data[i] != Text->mark; i++){
        if (Text->data[i] == ' ' || Text->data[i] == '\n'){
            space++;
            words++;
        } else{
            if (space > 0){
                *fileout << " ";
            }
            space = 0;
            *fileout << Text->data[i];   
        }
        
    }
}

unsigned char getmark(ifstream *filein){
    unsigned char ch;

    if (!filein->eof()){
        *filein >> ch;

        if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122)){
            cerr << "Ошибко: Вы не задали стоп-символ или символ-маркер.\n";
            return 0;
        }
        else{
            return ch;
        }
    } 
    else{
        cerr << "Ошибко: Входной файл пуст.";
        return 0;
    }
}

void str_in(ifstream *filein, String *Text, int size){
    unsigned char ch = 0;
    int i = 0;

    // Заполняем массив
    while (i < size){
        *filein >> noskipws >> ch;

        Text->data[i] = ch;

        i++;
    }
    
    // Добавляем символ-маркер в конец строки
    Text->data[size] = Text->mark;
}