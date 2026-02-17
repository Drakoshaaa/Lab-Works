#include <iostream>
#include <fstream>

using namespace std;

const char FILEIN[] = "string1.txt";
const char FILEOUT[] = "out.txt";

struct String{
    char* data;
    char mark;  
};

void printstr(ofstream *fileout, String *Text);
unsigned char getmark(ifstream *filein);
void stopmark_analyse(ifstream *filein, unsigned char mark, unsigned char stop);
int calcsize(ifstream *filein, unsigned char mark, unsigned char stop);
void str_in(ifstream *filein, String *Text, int size);
void Process(String *Text, int *size);

int main() {
    ifstream filein(FILEIN); 
    ofstream fileout(FILEOUT); 

    if (!filein.is_open()){
        cerr << "Ошибко: Не удалось открыть входной файл.\n";
        return 0;
    }


    cout << "Лабораторная №1, Версия №1 по программированию(второй семестр)\n";
    cout << "Выполнил Решетников Семён, группа 5352.\n\n";


    //Получение маркера конца строки и стоп-символа
    unsigned char mark, stop;

    mark = getmark(&filein);
    stop = getmark(&filein);
    
    if(mark == 0 || stop == 0 || mark == stop) return 0;

    cout << "mark = " << mark << endl;
    cout << "stop = " << stop << endl;

    // Наводим курсор к месту, где начинается строка(стоп-символ)
    stopmark_analyse(&filein, mark, stop);

    // Вычисление размера исходной строки
    int size = calcsize(&filein, mark, stop);
    cout << endl << "size: " << size << endl;

    filein.clear();
    filein.seekg(0, ios::beg);
    mark = getmark(&filein);
    stop = getmark(&filein);

    // Создание объекта String и динамическое выделение памяти под всю строку
    String Text;
    Text.data = new char[size + 1];
    Text.mark = mark;

    // Наводим курсор к месту, где начинается строка(стоп-символ)
    stopmark_analyse(&filein, mark, stop);

    // Заполнение символами поле data у объекта Text
    str_in(&filein, &Text, size);

    // Основная часть программы
    // Слияние строк и перебор всех слов
    Process(&Text, &size);

    printstr(&fileout, &Text);

    delete[] Text.data;
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

void stopmark_analyse(ifstream *filein, unsigned char mark, unsigned char stop){
    unsigned char ch = 0;
    int numofstopchar = 0;

    // Считаем сколько раз в строке встретился стоп-символ
    while (ch != mark && !filein->eof()){
        *filein >> noskipws >> ch;
        if (ch == stop) numofstopchar++;
    }

    filein->clear();
    filein->seekg(0, ios::beg);
    *filein >> ch >> ch;
    ch = 0;

    // Если стоп символов один или более - значит необходимо начинать чтение с первого такого символа
    if (numofstopchar >= 1){
        while (ch != stop) *filein >> noskipws >> ch;
    }

}

int calcsize(ifstream *filein, unsigned char mark, unsigned char stop){
    int size = 0;
    unsigned char ch = 0;
    
    // Если же стоп символов нет - читаем с самого начала
    while (*filein >> noskipws >> ch && ch != stop && ch != mark){
        size++;
    }

    return size;
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
