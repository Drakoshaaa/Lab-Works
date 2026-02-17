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
void Process(String *Text, int *size);

int main() {
    ifstream filein(FILEIN);
    ofstream fileout(FILEOUT);


    if (!filein.is_open()) {
        cerr << "Ошибка: Не удалось открыть входной файл.\n";
        return 0;
    }

    cout << "Лабораторная №1, Версия №1 по программированию (второй семестр)\n";
    cout << "Выполнил Решетников Семён, группа 5352.\n\n";

    unsigned char mark;
    int size = 0, fsize;

    mark = getmark(&filein);
    if (mark == 0) return 0;


    filein >> fsize;   // читаем количество символов
    filein.get(); 

    
    if (fsize <= 0) {
        cerr << "Ошибка: Некорректный размер строки.\n";
        return 0;
    }

    int pos = filein.tellg();

    // Считаем реальное количество символов
    unsigned char ch;
    while (filein >> noskipws >> ch && size < fsize){
        size++;
    }
    
    String Text;
    Text.data = new char[size + 1];
    Text.mark = mark;

    filein.clear();
    filein.seekg(pos, ios::beg);

    str_in(&filein, &Text, size);
    // Основная часть программы
    // Слияние строк и перебор всех слов
    Process(&Text, &size);

    printstr(&fileout, &Text);


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
    
    // Читаем, пока не встретим маркер ИЛИ не кончится место в массиве
    while (i < size && *filein >> noskipws >> ch && ch != Text->mark) {
        Text->data[i] = ch;
        cout << ch;
        i++;
    }
    
    // Ставим маркер строго в конец реально считанных данных
    Text->data[i] = Text->mark;
}

void Process(String *Text, int *size){

    // Основная часть программы
    // Слияние строк и перебор всех слов
    String temp;
    temp.mark = Text->mark;

    bool changed = true;

    while (changed) {
        changed = false;

        int pos = 0;

        while (pos < *size) {

            // пропускаем пробелы и переводы строки
            while (pos < *size && 
                (Text->data[pos] == ' ' || Text->data[pos] == '\n'))
                pos++;

            if (pos >= *size) break;

            // определяем первое слово
            int idx1 = pos;
            int end1 = idx1;

            while (end1 < *size &&
                Text->data[end1] != ' ' &&
                Text->data[end1] != '\n')
                end1++;

            int len1 = end1 - idx1;
            char target = Text->data[end1 - 1];   // последняя буква

            // ищем подходящее слово справа
            int search = end1;
            int idx2 = -1;
            int end2 = -1;

            while (search < *size) {

                while (search < *size &&
                    (Text->data[search] == ' ' ||
                    Text->data[search] == '\n'))
                    search++;

                if (search >= *size) break;

                if (Text->data[search] == target) {
                    idx2 = search;
                    end2 = idx2;

                    while (end2 < *size &&
                        Text->data[end2] != ' ' &&
                        Text->data[end2] != '\n')
                        end2++;

                    break;
                }

                while (search < *size &&
                    Text->data[search] != ' ' &&
                    Text->data[search] != '\n')
                    search++;
            }

            if (idx2 == -1) {
                pos = end1;
                continue;
            }

            // 1. сохраняем первое слово во временный буфер
            temp.data = new char[len1 + 1];

            for (int i = 0; i < len1; i++)
                temp.data[i] = Text->data[idx1 + i];

            temp.data[len1] = temp.mark;

            // 2. удаляем первое слово (сдвиг влево)
            for (int i = end1; i < *size; i++)
                Text->data[i - len1] = Text->data[i];

            (*size) -= len1;

            // корректируем позицию второго слова
            idx2 -= len1;
            end2 -= len1;

            // 3. освобождаем место перед вторым словом
            for (int i = *size - 1; i >= idx2; i--)
                Text->data[i + len1] = Text->data[i];

            // увеличиваем размер
            (*size) += len1;

            // 4. вставляем первое слово
            for (int i = 0; i < len1; i++)
                Text->data[idx2 + i] = temp.data[i];

            // 5. удаляем первый символ старого второго слова
            for (int i = idx2 + len1; i < *size - 1; i++)
                Text->data[i] = Text->data[i + 1];

            (*size)--;  // уменьшаем на 1
            Text->data[*size] = Text->mark;

            delete[] temp.data;

            changed = true;
            
            break;  // начинаем проход заново
        }
    }
}
