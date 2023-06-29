#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

void parseJSON(cJSON *json, FILE *csvFile) { //для разбора JSON структуры и записи данных в CSV-файл
    if (json == NULL) { //выполняет проверку является ли json нулевым указателем
        return;
    }

    if (json->type == cJSON_Array) { //проверка типа json - массив  
        cJSON *child = json->child; //Создаем указатель child, json->child это указатель на первый (дочерний) элемент массива
        if (child) { //Если child не равен NULL, то массив не пустой
            
            cJSON *object = child; //создаем указатель, который равен child, для последующего обхода массива 
            cJSON *item = object->child; //равен первому дочернему элементу, для обхода дочерних элементов 
            while (item) { //пока не закончатся дочерние элементы 
                if (item->type == cJSON_String) { //если item строка  
                    fprintf(csvFile, "%s", item->string); //записываем item в csv
                } else if (item->type == cJSON_Number) { //если число
                    fprintf(csvFile, "%s", cJSON_Print(item));// записываем число как строку 
                }

                item = item->next;//переход к след дочернему эл
                if (item) {//если он сущ, то пишем , 
                    fprintf(csvFile, ",");//fprintf записывает то, что требуется в файл 
                }
            }
            fprintf(csvFile, "\n");//переход на новую строку в csv

            // Записываем значения по соответствующим ключам(обходим все объекты и их элементы и пишес в файлик)
            while (object) { //перебор всех объектов 
                item = object->child; //значение указателя на первый дочерний элемент объекта
                while (item) { //перебор элементов 
                    if (item->type == cJSON_Object || item->type == cJSON_Array) { //если тип явл объектом или массивом 
                        fprintf(csvFile, "[Object/Array]"); //сложная структура данных 
                    } else if (item->type == cJSON_String) {//если тип строка 
                        fprintf(csvFile, "%s", item->valuestring);//записываем значение строки 
                    } else if (item->type == cJSON_Number) {//если тип число 
                        fprintf(csvFile, "%s", cJSON_Print(item));//записываем число в виде строки 
                    } else if (item->type == cJSON_True) { //тип правда 
                        fprintf(csvFile, "1");//хаписываем булевую переменную 1 
                    } else if (item->type == cJSON_False) {// противоположно предыдущему 
                        fprintf(csvFile, "0");
                    }

                    item = item->next;
                    if (item) {
                        fprintf(csvFile, ",");
                    }
                }

                fprintf(csvFile, "\n");
                object = object->next;
            }
        }
    }
}

int main() {
    printf("Enter JSON-file: "); //пишем имя файлика 
    char filename[50]; //строка размером 50 для имени
    fgets(filename, sizeof(filename), stdin); //прочитывает строку и сохраняет ее в filename, strdin - стандартный ввод с клавы 

    size_t len = strlen(filename); //считываем длину строки, size_t - целочисленный тип данных
    if (len > 0 && filename[len - 1] == '\n') {//удаляем символ новой строки и заменяем его на нулевой 
        filename[len - 1] = '\0';
    }
    //чтение файлика
    FILE *jsonFile = fopen(filename, "r");
    if (jsonFile == NULL) {
        printf("Error open JSON.\n");
        return 1;
    }

    fseek(jsonFile, 0, SEEK_END); //устанавливается указатель позиции в конец файлика
    long fileSize = ftell(jsonFile);//определяется текущая позиция указателя, для определения размера файлика 
    rewind(jsonFile);//позиуия указателя в начало файлика 

    char *jsonString = (char *)malloc(fileSize + 1);//выделение динамической памяти 
    if (jsonString == NULL) {
        fclose(jsonFile);
        return 1;
    }

    fread(jsonString, 1, fileSize, jsonFile); //читаем строку и ее в jsonString
    jsonString[fileSize] = '\0';//добаляем в конец нулевой элемент 
    fclose(jsonFile);

    cJSON *json = cJSON_Parse(jsonString);//парсим строку jsonString, значения сохраняются в json
    if (json == NULL) {
        printf("Error\n");
        free(jsonString);
        return 1;
    }

    free(jsonString);//освобождаем память 
    //открывание файла csv
    FILE *csvFile = fopen("output.csv", "w");
    if (csvFile == NULL) {
        printf("Error open CSV.\n");
        cJSON_Delete(json);
        return 1;
    }

    parseJSON(json, csvFile); //значения json преобразуются для csv и записываются в файлик 

    //cJSON_Delete(json);
    fclose(csvFile);

    printf("Complete.\n");

    return 0;
}