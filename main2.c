#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

cJSON *createJSONObjectFromCSV(char *csvRow, char **columnHeaders, int numColumns) { 
    cJSON *object = cJSON_CreateObject();//создаем пустой объект json

    char *token = strtok(csvRow, ",");// разбиваем строку на подстроки запятой 
    int columnIndex = 0;

    while (token != NULL && columnIndex < numColumns - 1) {
        cJSON *value;

        int intValue = atoi(token);//преобразование токена в число 
        if (intValue != 0 || strcmp(token, "0") == 0) {//проверяем число это или строка 
            value = cJSON_CreateNumber(intValue);// тип числа 
        } else {
            value = cJSON_CreateString(token);//тип строки
        }

        cJSON_AddItemToObject(object, columnHeaders[columnIndex], value); //добавление пары, где columnHeaders[columnIndex] массив с заголовками столбцов и токена 

        token = strtok(NULL, ",");//переход к следующему токену 
        columnIndex++;//след столбик 
    }

    return object;
}

cJSON *parseCSV(const char *csvFilename) {//возвращает тип json
    //открываем файл 
    FILE *csvFile = fopen(csvFilename, "r");
    if (csvFile == NULL) {
        return NULL;
    }

    // Чтение заголовков столбцов
    char line[512];//массив для строк 
    if (fgets(line, sizeof(line), csvFile) == NULL) {//читаем 1 строчку (заголовки), сохраняем ее в массив 
        fclose(csvFile);
        return NULL;
    }

    // Разделение заголовков столбцов по запятой
    char **columnHeaders = NULL;
    int numColumns = 0;//кол-во заголовков 
    char *token = strtok(line, ","); //разбиваем на токены 
    while (token != NULL) {
        numColumns++;
        columnHeaders = realloc(columnHeaders, numColumns * sizeof(char *)); //добавляем память
        columnHeaders[numColumns - 1] = strdup(token);//заголовок в массив columnHeaders, strdup копирует токен и сохраняет 
        token = strtok(NULL, ",");//след заголовок
    }

    cJSON *jsonArray = cJSON_CreateArray();//создаем новый json массив 

   
    while (fgets(line, sizeof(line), csvFile) != NULL) {//пока мы можем считать строку 
        cJSON *jsonObject = createJSONObjectFromCSV(line, columnHeaders, numColumns);//массив заголовков и количество столбцов и сохраняет в указатель 
        cJSON_AddItemToArray(jsonArray, jsonObject);//добавляем объект в массив 
    }

    fclose(csvFile);

    // Освобождение памяти 
    for (int i = 0; i < numColumns; i++) {
        free(columnHeaders[i]);
    }
    free(columnHeaders);

    return jsonArray;
}

int main() {
    printf("Enter CSV-file: ");
    char filename[100];
    fgets(filename, sizeof(filename), stdin);//прочитываем название файла 

    size_t len = strlen(filename);//длина названия 
    while (len > 0 && filename[len - 1] == '\n')
    {
        filename[len - 1] = '\0';//удаляет символ конца меняя его на нулевой 
    }


    cJSON *json = parseCSV(filename);
    if (json == NULL) {
        printf("Error\n");
        return 1;
    }
    char *jsonString = cJSON_Print(json);//преобразование JSON в строку

    // Создание выходного файла JSON
    char outputFilename[100];
    snprintf(outputFilename, sizeof(outputFilename), "%s.json", filename);//принимает строку, ее размер и имя файла 
    FILE *outputFile = fopen(outputFilename, "w");// открываем файл, w файл будет создан или перезаписан
    if (outputFile == NULL) {
        printf("Error\n");
        //cJSON_Delete(json);
        free(jsonString);
        return 1;
    }

    // Запись JSON-строки в файл
    fprintf(outputFile, "%s", jsonString);
    fclose(outputFile);

    printf("JSON-file is created: %s", outputFilename);

    // Освобождение памяти
    //cJSON_Delete(json);
    //free(jsonString);

    return 0;
}