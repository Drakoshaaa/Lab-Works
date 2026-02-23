#include <iostream>
#include <fstream>

using namespace std;

const char FILEIN[] = "assets/v2_string1.txt";
const char FILEOUT[] = "assets/out.txt";

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

    // Контрольный вывод в консоль и в текстовый файл
    cout << "Маркер: " << mark << endl
         << "Данный размер: " << fsize << endl
         << "Реальный размер строки: " << size << endl
         << "Исходная строка: " << endl;
    fileout << "Маркер: " << mark << endl
         << "Данный размер: " << fsize << endl
         << "Реальный размер строки: " << size << endl
         << "Исходная строка: " << endl;
    for (int i = 0; Text.data[i] != Text.mark ; i++){
        cout << Text.data[i];
        fileout << Text.data[i];
    }
    fileout << endl << endl;


    // Основная часть программы
    // Слияние строк и перебор всех слов
    Process(&Text, &size);

    printstr(&fileout, &Text);
    cout << "\n\nПреобразованная строка была занесена в файл " << FILEOUT;

    delete[] Text.data;
    return 0;
}

void printstr(ofstream *fileout, String *Text){
    int space = 0;
    *fileout << "Результат работы программы:\n";
    for (int i = 0; Text->data[i] != Text->mark; i++){
        if (Text->data[i] == ' ' || Text->data[i] == '\n'){
            space++;
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

        while (Text->data[pos] != Text->mark) {

            // пропускаем пробелы и переводы строки
            while (Text->data[pos] != Text->mark && 
                (Text->data[pos] == ' ' || Text->data[pos] == '\n'))
                pos++;

            if (Text->data[pos] == Text->mark) break;

            // определяем первое слово
            int idx1 = pos;
            int end1 = idx1;

            while (Text->data[end1] != Text->mark &&
                Text->data[end1] != ' ' &&
                Text->data[end1] != '\n')
                end1++;

            char target = Text->data[end1 - 1];   // последняя буква

            // ищем подходящее слово справа
            int search = end1;
            int idx2 = -1;
            int end2 = -1;

            while (Text->data[search] != Text->mark) {

                while (Text->data[search] != Text->mark &&
                    (Text->data[search] == ' ' ||
                    Text->data[search] == '\n'))
                    search++;

                if (Text->data[search] == Text->mark) break;

                if (Text->data[search] == target) {
                    idx2 = search;
                    end2 = idx2;

                    while (Text->data[end2] != Text->mark &&
                        Text->data[end2] != ' ' &&
                        Text->data[end2] != '\n')
                        end2++; 

                    break;
                }

                while (Text->data[search] != Text->mark &&
                    Text->data[search] != ' ' &&
                    Text->data[search] != '\n')
                    search++;
            }

            if (idx2 == -1) {
                pos = end1;
                continue;
            }

            // 1. Одновременный сдвиг текста между end1 и idx2 влево
            // и  посимвольное копирование первого слова перед вторым

            for (int i = end1-1; i >= idx1 ; i--, idx2--){

                Text->data[idx2] = Text->data[i];

                for (int k = i; k < idx2-1 ; k++){
                    Text->data[k] = Text->data[k+1];
                }
                
            }
            
            // 2. Свдигаем на один индекс всё, что правее idx2
            for (int i = idx2 ; Text->data[i] != Text->mark ; i++){
                Text->data[i-1] = Text->data[i]; 
            }
            
            (*size)--;  // уменьшаем на 1
            Text->data[*size] = Text->mark;
            

            changed = true;
            
            break;  // начинаем проход заново
        }
    }
}