#define _CRT_SECURE_NO_WARNINGS  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

struct Inform
{
    char day[11];
    char time[9];
    char type;
    float summ;
};

struct Person
{
    int id;
    char name[50];
    char surname[50];
    char otch[50];
    int tr_cnt;
    struct Inform* inf;
};

int is_time(char* t, char* st, char* en)
{
    if (strcmp(t, st) >= 0 && strcmp(t, en) <= 0)
        return 1;
    return 0;
}

char* read_non_empty_line(FILE* f, char* buffer, int size)
{
    while (1)
    {
        if (fgets(buffer, size, f) == NULL)
            return NULL;

        // скип символы новой строки
        buffer[strcspn(buffer, "\n")] = '\0';
        buffer[strcspn(buffer, "\r")] = '\0';

        // скип пустые строки
        if (strlen(buffer) > 0)
            return buffer;
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");

    FILE* f = NULL;
    int cnt = 0;
    struct Person* c = NULL;
    float mx = 0.0f;
    int* mx_cl = NULL;
    int mx_cnt = 0;
    char sttime[9] = { 0 };
    char edtime[9] = { 0 };
    int scanf_result = 0;

    errno_t error = fopen_s(&f, "in.txt", "r");

    char buffer[256];

    while (read_non_empty_line(f, buffer, sizeof(buffer)) != NULL)
    {
        struct Person cnew;

        cnew.id = atoi(buffer);

        if (read_non_empty_line(f, buffer, sizeof(buffer)) == NULL)
        {
            printf("нет строки с ФИО %d\n", cnew.id);
            break;
        }

        char* token = strtok(buffer, "\t");
        if (token)
            strncpy(cnew.surname, token, sizeof(cnew.surname) - 1);
        else
            strcpy(cnew.surname, "Неизвестно");

        token = strtok(NULL, "\t");
        if (token)
            strncpy(cnew.name, token, sizeof(cnew.name) - 1);
        else
            strcpy(cnew.name, "Неизвестно");

        token = strtok(NULL, "\t");
        if (token)
            strncpy(cnew.otch, token, sizeof(cnew.otch) - 1);
        else
            strcpy(cnew.otch, "Неизвестно");

        if (read_non_empty_line(f, buffer, sizeof(buffer)) == NULL)
        {
            printf("нет строки с количеством транзакций %d\n", cnew.id);
            break;
        }

        cnew.tr_cnt = atoi(buffer);

        cnew.inf = (struct Inform*)malloc(cnew.tr_cnt * sizeof(struct Inform));
        if (cnew.inf == NULL)
        {
            printf("Ошибка выделения памяти для клиента %d\n", cnew.id);
            break;
        }

        int read_error = 0;
        for (int i = 0; i < cnew.tr_cnt; ++i)
        {
            if (read_non_empty_line(f, buffer, sizeof(buffer)) == NULL)
            {
                printf("не хватает транзакций для клиента %d\n", cnew.id);
                read_error = 1;
                break;
            }

            token = strtok(buffer, "\t");
            if (token)
                strncpy(cnew.inf[i].day, token, sizeof(cnew.inf[i].day) - 1);
            else
                strcpy(cnew.inf[i].day, "01.01.2023");

            token = strtok(NULL, "\t");
            if (token)
                strncpy(cnew.inf[i].time, token, sizeof(cnew.inf[i].time) - 1);
            else
                strcpy(cnew.inf[i].time, "00:00:00");

            token = strtok(NULL, "\t");
            if (token)
            {
                if (strcmp(token, "Приход") == 0)
                    cnew.inf[i].type = '+';
                else if (strcmp(token, "Расход") == 0)
                    cnew.inf[i].type = '-';
                else
                    cnew.inf[i].type = '?';
            }
            else
            {
                cnew.inf[i].type = '?';
            }

            token = strtok(NULL, "\t");
            if (token)
                cnew.inf[i].summ = (float)atof(token);
            else
                cnew.inf[i].summ = 0.0f;
        }

        if (read_error)
        {
            free(cnew.inf);
            break;
        }

        struct Person* temp = (struct Person*)realloc(c, (cnt + 1) * sizeof(struct Person));

        c = temp;
        c[cnt] = cnew;
        cnt++;
    }

    fclose(f);

    printf("\nВсего клиентов прочитано: %d\n\n", cnt);

    printf("Начальное время: ");
    scanf_result = scanf("%8s", sttime);

    printf("Конечное время: ");
    scanf_result = scanf("%8s", edtime);

    printf("\nРасходы клиентов в период с %s до %s:\n", sttime, edtime);
    for (int i = 0; i < cnt; ++i)
    {
        float csp = 0.0f;
        for (int j = 0; j < c[i].tr_cnt; ++j)
        {
            if (c[i].inf[j].type == '-' && is_time(c[i].inf[j].time, sttime, edtime))
            {
                csp += c[i].inf[j].summ;
            }
        }

        printf("Клиент %d: %s %s - расходы: %.2f\n",
            c[i].id, c[i].surname, c[i].name, csp);

        if (csp > mx && csp > 0.0f)
        {
            mx = csp;
            if (mx_cl != NULL)
            {
                free(mx_cl);
                mx_cl = NULL;
            }
            mx_cnt = 0;

            mx_cl = (int*)malloc(sizeof(int));
            if (mx_cl != NULL)
            {
                mx_cl[0] = i;
                mx_cnt = 1;
            }
        }
        else if (csp == mx && csp > 0.0f && mx_cl != NULL)
        {
            int* temp = (int*)realloc(mx_cl, (mx_cnt + 1) * sizeof(int));
            if (temp != NULL)
            {
                mx_cl = temp;
                mx_cl[mx_cnt] = i;
                mx_cnt++;
            }
        }
    }

    if (mx <= 0.0f)
    {
        printf("\nНет расходных операций в период с %s до %s\n", sttime, edtime);
    }
    else
    {
        printf("\nМаксимальные расходы (%.2f руб.) в период %s - %s\n", mx, sttime, edtime);
        printf("Клиенты с максимальными расходами:\n");

        for (int i = 0; i < mx_cnt; ++i)
        {
            if (mx_cl != NULL && i < mx_cnt)
            {
                int idx = mx_cl[i];
                printf("%d. ID: %d, ФИО: %s %s %s\n",
                    i + 1,
                    c[idx].id,
                    c[idx].surname,
                    c[idx].name,
                    c[idx].otch);
            }
        }
    }

    return 0;
}
