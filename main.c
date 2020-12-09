#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void help() {
    printf("Somehting wrong. Pleace check that you are correct. \nprogramm_name [options] [source_file [resulting_file]] \nOptions:\n\t-s: subnet in CIDR format (****:****:****:****:****:****:****:****/...) \n\t-r : phrase to exchange IPv6 in the tekst\nIf you dont specify sours/resulting file the source/resulting text will be written/read from/into standard output stream\nExample of starting programm: \n./laba -s 000f:f00f:0000:1234:abfd:0000:0000:0101/127 -r error\n");
    return;
}//Вывод мануала

void bufferSize(char **str, int size, int cns) {
    *str = (char *) realloc(*str, (size + cns) * sizeof(char *));
}

void newBufferSize(char **str, int cns) {
    *str = (char *) malloc(cns * sizeof(char));
}

char *check(char *str) {//Проверка на то, что поданная строчка явялется IPv6 адресом
    char *buf = NULL, *dubl = NULL;
    buf = (char *) malloc(1);
    dubl = (char*)malloc(1);
    int count = 0, pos = -1, k = 0;
    if (strstr(str, "::") != NULL) { //Проверка, что в строке только один раз встречается ::
        dubl = (char *) realloc(dubl,strlen(strstr(str, "::"))*sizeof(char*));
        dubl = strstr(str, "::");
        buf = (char *) realloc(buf, strlen(dubl) - 2);
        for (int i = 2; i < strlen(strstr(str, "::")); i++)
            buf[i - 2] = dubl[i];
        if (strstr(buf, "::") != NULL) {
            return NULL;
        }
        buf = NULL;
    }
    if (strstr(str, ":::") != NULL) {//Проверка, что в строке встречается не более двух подрядидущих :
        return NULL;
    }
    while (str[k] != ':') {//Проверка символов в начале строки на соответствие IPv6 критериям
        if ((!((str[k] >= '0') && (str[k] <= '9'))) && (!((str[k] >= 'a') && (str[k] <= 'f')))) {
            pos = k;
        }
        k++;
    }
    if (pos != -1) {//Удаление символов, не соответствующих требованиям IPv6, в начале строки
        buf = (char *) realloc(buf, strlen(str) - pos - 1);
        for (int i = pos + 1; i < (int) strlen(str); i++) {
            buf[i - (pos + 1)] = str[i];
        }
        str = NULL;
        str = (char *) realloc(str, strlen(buf));
        str = buf;
        buf = NULL;
    }
    if (str[0] == ':') {//Проверка, что первый символ не :
        return NULL;
    }
    for (int i = 0; i < strlen(str); i++)//Проверка, что каждый символ принадлежит шестнадцатеричной системе счисления или :
        if ((!((str[i] >= '0') && (str[i] <= '9'))) && (!((str[i] >= 'a') && (str[i] <= 'f'))) && (str[i] != ':')) {
            buf = (char *) realloc(buf, i);
            for (int j = 0; j < i; j++)
                buf[j] = str[j];
            str = NULL;
            str = (char *) realloc(str, strlen(buf));
            str = buf;
            buf = NULL;
            break;
        }
    pos = 0;
    for (int i = 0; i < strlen(str); i++) {//Проверка, что между каждыми двумя :, кроме :: стоит не больше 4 символов
        if (str[i] != ':')
            pos++;
        if (str[i] == ':')
            pos = 0;
        if (pos > 4) {//Если символов больше, обрезать строку до требований IPv6
            buf = (char *) realloc(buf, i);
            for (int j = 0; j < i; j++)
                buf[j] = str[j];
            str = NULL;
            str = (char *) realloc(str, strlen(buf));
            str = buf;
            buf = NULL;
        }
    }
    for (int i = 0; i < strlen(str); i++) {//Посчет колличества : в строке, если больше 7, обрезать по требованиям IPv6
        if (str[i] == ':')
            count++;
        if ((count == 7) && ((strlen(str) - i - 1) > 4)) {
            buf = (char *) realloc(buf, i + 5);
            for (int j = 0; j < i + 5; j++)
                buf[j] = str[j];
            str = NULL;
            str = (char *) realloc(str, strlen(buf));
            str = buf;
            buf = NULL;
        }
        if (count > 7) {
            buf = (char *) realloc(buf, i);
            for (int j = 0; j < i; j++)
                buf[j] = str[j];
            str = NULL;
            str = (char *) realloc(str, strlen(buf));
            str = buf;
            buf = NULL;
        }
    }
    if (str[strlen(str) - 1] == ':')//Если последний символ : возврат отрицания того, что строка принадлежит IPv6 адресам
        return NULL;
    if ((count < 7) && (strstr(str, "::") == NULL)) {//Если : меньше 7 и нет ::, возврат отрицания принадлежности к IPv6 адресам
        return NULL;
    }
    free(buf);
    return str;//Возврат модифицированной IPv6 строки
}

char *cidr(char *str) {//Преобразование IPv6 адресса в формат CIDR
    char *buf = NULL, *buf1 = NULL;
    buf = (char *) malloc(1);
    buf1 = (char *) malloc(1);
    int count = 0;
    if (strstr(str, "::") != NULL) {//Если есть :: посчет  :, для подсчета колличества дополнительных блоков 0000
        for (int i = 0; i < strlen(str); i++)
            if ((str[i] == ':') && (str[i + 1] != ':'))
                count++;
    }
    if ((count <= 6) && (count > 0)) {//Вставить блоки 0000 вместо ::
        buf1 = (char *) realloc(buf1, strlen(strstr(str, "::")));
        buf1 = strstr(str, "::");
        buf = (char *) realloc(buf, strstr(str, "::") - str + 1);
        memcpy(buf, str, strstr(str, "::") - str);
        for (count; count < 7; count++) {
            buf = (char *) realloc(buf, strlen(buf) + 5);
            strcat(buf, ":0000");
        }
        int poz = strlen(buf);
        buf = (char *) realloc(buf, poz + strlen(strstr(str, "::")) - 1);
        for (int i = poz, j = 1; j <= strlen(strstr(str, "::")); i++, j++)
            buf[i] = buf1[j];
        str = NULL;
        str = (char *) realloc(str, strlen(buf));
        str = buf;
        buf = NULL;
        buf1 = NULL;
    }
    buf = (char *) realloc(buf, 39);
    char *arr = strtok(str, ":");//Разбитие строки на блоки между :
    int ch = 0;
    while (arr != NULL) {//Модифицировать каждый блок, чтобы он состоял из 4 шестнадцатиричных символов путем добавления 0 вначало блока
        ch++;
        buf1 = (char *) realloc(buf1, 5);
        memset(buf1, '0', 4 - strlen(arr));
        strcat(buf1, arr);
        if (ch != 8)
            strcat(buf1, ":");
        strcat(buf, buf1);
        buf1 = NULL;
        arr = strtok(NULL, ":\0");
    }
    str = NULL;
    str = (char *) realloc(str, strlen(buf));
    strcpy(str, buf);
    free(buf);
    return str;//Возврат отредактированного текста
}

int func(FILE *f1, FILE *f2, char *substr, char *new) { //Функция чтения, посика и замены строк
    int cur = 0;
    char c, *str = NULL, **arrstr = NULL;
    int i = 0, j = 0, pam = 0, cns = 1, stolb = 0, position = 0;
    char *buf = NULL, *buf1 = NULL;
    newBufferSize(&str, cns);
    buf = (char *) malloc(1);
    buf1 = (char *) malloc(1);
    if (strstr(substr, "/") == NULL) {//Проверка подсети на соответствование треблованиям
        printf("No subnet mask\n");
        help();
        exit(1);
    }
    buf = (char *) realloc(buf, strlen(strstr(substr, "/")) - 1);
    buf1 = (char *) realloc(buf, strlen(strstr(substr, "/")));
    buf1 = strstr(substr, "/");
    for (i = 1; i < strlen(buf1); i++)
        buf[i - 1] = buf1[i];
    sscanf(buf, "%d", &i);
    if ((i > 128) || (i <= 0)) {//Проверка подсети на соответствование треблованиям
        printf("Bad subnet mask\n");
        help();
        exit(1);
    }
    for (int num = 4; num < 29; num += 5)//Проверка подсети на соответствование треблованиям
        if (substr[num] != ':') {
            printf("Not CIDR\n");
            help();
            exit(1);
        };
    for (int num = 0; num < 39; num++)//Проверка подсети на соответствование треблованиям
        if ((!((substr[num] >= '0') && (substr[num] <= '9'))) && (!((substr[num] >= 'a') && (substr[num] <= 'f'))) &&
            (substr[num] != ':')) {
            printf("Not IPv6\n");
            help();
            exit(1);
        }
    substr = check(substr);
    if (substr == NULL) {//Проверка подсети на соответствование треблованиям
        printf("Not IPv6\n");
        help();
        exit(1);
    }
    i=0;
    if (str == NULL)
        return 1;
    while ((c = getc(f1)) != EOF) {//Посимвольное считывание
        if (c == '\n') {//Найден символ конца строки, считывание строки в массив
            i++;
            str[i - 1] = '\0';
            pam = pam + i;
            arrstr = (char **) realloc(arrstr, pam * (sizeof(char*)));
            if (arrstr == NULL)
                return 1;
            stolb++;
            arrstr[stolb - 1] = str;
            str = NULL;
            newBufferSize(&str, cns);
            if (str == NULL)
                return 1;
            i = 0;

        } else {
            i++;//Посимвольное считывание в буфер
            if ((i % cns) == 0) {
                bufferSize(&str, i, cns);
                if (str == NULL)
                    return 1;
            }
            str[i - 1] = c;
        }
    }
    str[i] = '\0';//Считывание последней строки
    pam = pam + i - 1;
    arrstr = (char **) realloc(arrstr, pam * (sizeof(char *)));
    if (arrstr == NULL)
        return 1;
    if(strlen(str)!=0){
        arrstr[stolb] = str;
        stolb++;
    }
    sscanf(buf,"%d", &i);
    j = 0;
    while (i >= 4) {//Создание маски подсети на основе исходных данных
        if (substr[j] != ':')
            i -= 4;
        j++;
    }
    int k, step;
    if (substr[j] == ':')
        j++;
    k = 0;
    sscanf(&substr[j], "%x", &k);
    while (k >= 16)
        k /= 16;
    step = 0;
    while (i > 0) {
        step += pow(2, 4 - i);
        i--;
    }
    k &= step;
    position = j;
    char *arr = NULL;
    for (i = 0; i < stolb; i++) {//Поиск строк и их замена
        int prev = -1, start = 0, end = 0;
        arr=NULL;
        arr = (char *) realloc(arr,strlen(arrstr[i]));
        arr = arrstr[i];
        int r=-1;
        while (1){//Работа со строкой
            r++;
            if(arr[r] =='\0')
                break;
            if (arr[r] == ':') {//Поис символа :
                if (prev == -1) {
                    if (r < 4) {//Установка границ для предполагаемого IPv6 адресса
                        start = 0;
                    } else {
                        start = r - 4;
                    }
                } else {
                    if (r - prev < 5) {
                        start = prev + 1;
                    } else {
                        start = r - 4;
                    }
                }
                prev = r;
                str = NULL;
                str = (char *) realloc(str, 39);
                if (strlen(arr) - 1 - start < 39) {
                    end = strlen(arrstr[i]);
                } else {
                    end = start + 39;
                }
                for (int q = start, y = 0; q <= end; q++, y++)
                    str[y] = arr[q];
                str = check(str);//Проверка предлолагаемого адресса
                if (str != NULL) {
                    int size = strlen(str);//Установка новых границ для замены
                    int remember = start, prov = 0;
                    while (arr[remember] != ':') {
                        prov++;
                        remember++;
                    }
                    remember = 0;
                    while (str[remember] != ':') {
                        prov--;
                        remember++;
                    }
                    start += prov;
                    str = cidr(str);//Модификация адресса под CIDR
                    int every = 0;
                    for (int h = 0; h < position; h++)//Проверка принадлежит ли адресс подсети
                        if (str[h] != substr[h])
                            every = 1;
                    if (every == 0) {
                        int l;
                        sscanf(&str[position], "%x", &l);
                        while (l >= 16)
                            l /= 16;
                        if (!((l >= k) && (l <= k + 15 - step)))
                            every = 1;
                    }
                    if (every == 0) {//Если принадлежит, замена адреса на считываемое значение
                        buf = NULL;
                        buf = (char *) realloc(buf, strlen(arr) + strlen(new) - size);
                        memcpy(buf, arr, start);
                        strcat(buf, new);
                        buf1 = NULL;
                        buf1 = (char *) realloc(buf1, strlen(arr) - start - size);
                        for (int q = 0, q1 = start + size; q1 < strlen(arr); q1++, q++)
                            buf1[q] = arr[q1];
                        strcat(buf, buf1);
                        arr = NULL;
                        arr = (char *) realloc(arr, strlen(buf));
                        strcpy(arr, buf);
                        r = 0;
                        prev = -1;
                        start = 0;
                        end = 0;
                        buf=NULL;
                        buf1=NULL;
                    }
                }
            }
        }
        fprintf(f2, "%s\n", arr);
    }
    free(arrstr);
    free(str);
    free(buf);
    return 0;
}

int main(int argc, char *argv[]) {
    int ch;
    FILE *f1;
    FILE *f2;
    const char *fname1;
    const char *fname2;
    char *substr, *new;
    int opt;
    if (argc == 1) {
        help();
        return 0;
    }
    while ((opt = getopt(argc, argv, ":s:r:")) != -1) {//Считывание опций из консоли
        switch (opt) {
            case 's':
                if ((optarg == "-s") || (optarg == "-r")) {
                    help();
                    return 0;
                } else {
                    substr = optarg;
                }
                break;
            case 'r':
                if ((optarg == "-s") || (optarg == "-r")) {
                    help();
                    return 0;
                } else {
                    new = optarg;
                }
                break;
            case ':':
                printf("Options -s and -r needs a valuen \n");
                help();
                return 0;
            case '?':
                printf("unknown option: %c\n", optopt);
                help();
                return 0;
        }
    }
    int res = argc - optind;//Если агрументов много, выводится мануал
    if (res > 2) {
        help();
        return 0;
    }
    if (res > 0) {//Проверка на наличие файлов для чтения/хаписи
        fname1 = argv[optind];
        f1 = fopen(fname1, "rb");
        if (res == 2) {
            fname2 = argv[optind + 1];
            f2 = fopen(fname2, "wb");
        } else {
            f2 = stdout;
        }
    } else {
        f1 = stdin;
        f2 = stdout;
    }
    if (f1 == NULL) {//Проверка файла на существование
        help();
        return 0;
    }
    ch = func(f1, f2, substr, new);
    if (ch == 1) {
        printf("Error! memory not allocated\n");
        return 0;
    }
    return 0;
}

