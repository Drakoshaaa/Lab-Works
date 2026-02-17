#include <iostream>
#include <fstream>

using namespace std;

const char FILEIN[] = "string1.txt";
const char FILEOUT[] = "out.txt";

struct String{
    char* data;
    char mark;  
};

void printstr(String *Text);
unsigned char getmark(ifstream *filein);
void stopmark_analyse(ifstream *filein, unsigned char mark, unsigned char stop);
int calcsize(ifstream *filein, unsigned char mark, unsigned char stop);
void str_in(ifstream *filein, String *Text, int size);
void string_merge(String *Text, String *temp, char target, int tempsize);

int main() {
    ifstream filein(FILEIN); 

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
    unsigned char target;
    int tempsize, gapsize, idx1, idx2, idx3 = 0;
    bool found = false;

    String temp;
    temp.mark = mark;
   
    for (int m = 0; Text.data[m] != Text.mark;){
        if (found){
            idx3 = 0;
        }

        // Вычисляем размер первого стоящего слова и его последнюю букву
        for (int i = idx3, j = 0; Text.data[i] != ' ' && Text.data[i] != '\n' && Text.data[i] != Text.mark ; i++, j++){
            target = Text.data[i];
            tempsize = j + 1;
        }

        // Индекс начала следующего слова
        idx1 = idx3 + tempsize + 1;
    
        // Выделяем память у временного объекта, хранящего первое слово
        temp.data = new char[tempsize + 1];

        // Заполняем массив символами
        for (int i = idx3, j = 0; j < tempsize; i++, j++){
            temp.data[j] = Text.data[i];
            if (j == tempsize - 1) temp.data[tempsize] = temp.mark;
        }

        cout << endl;
        printstr(&temp);
        cout << endl;

        // Ищем ближайшее справа слово, которое начинается на ту же букву
        found = false;
        for (int i = idx1 ; i < size && Text.data[i] != Text.mark; i++){
            if ((Text.data[i] == ' ' || Text.data[i] == '\n') && Text.data[i+1] == target && i+1 < size && Text.data[i+1] != Text.mark){
                idx2 = i + 1; // Индекс начала подходящего слова
                found = true;
                
                // Смещаем вcё, вплоть до подходящего слова в самое начало массива
                for (int j = idx1, k = 0; j < idx2; j++, k++){
                    Text.data[k] = Text.data[j];
                }

                gapsize = idx2 - idx1; // Размер только что смещённого текста (c правым пробелом)

                // Вставляем содержимое массива temp после только что смещённого текста
                for (int j = gapsize, k = 0; j < gapsize + tempsize; j++, k++){
                    Text.data[j] = temp.data[k]; 
                }

                // Смещаем всё оставшееся справа на 2 символа влево(чтобы произошло слияние двух подходящих слов)
                for (int j = idx2 ; Text.data[j] != Text.mark ; j++){
                    Text.data[j-2] = Text.data[j]; 
                }

                
                // Добавляем в конец маркер
                size-=2;
                Text.data[size] = Text.mark;
                
                break;
            } 

            // Вычисляем текущий индекс конца смещённого текста
            idx3 = gapsize;
        }

        m = idx3;

        printstr(&Text);
        cout << endl;
        delete[] temp.data;
    }

    printstr(&Text);

    delete[] Text.data;
}

void printstr(String *Text){
    for (int i = 0; Text->data[i] != Text->mark; i++){
        cout << Text->data[i];    
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