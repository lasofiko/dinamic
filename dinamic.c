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
int ttime(char* t, char* st, char* en)
{
    if (strcmp(t, st) >= 0 && strcmp(t, en) <= 0)
        return 1;
    return 0;
}
int main()
{
    setlocale(LC_ALL, "Russian");

    FILE* f = NULL;
    int cnt = 0;
    struct Person* c = NULL;

    errno_t error = fopen_s(&f, "in.txt", "r");
    if (error != 0 || f == NULL)
    {
        printf("Файл не открывается\n");
        return 1;
    }

    while (!feof(f))
    {
        struct Person cnew;
        int result;

        result = fscanf(f, "%d", &cnew.id);
        if (result != 1)
            break;

        result = fscanf(f, "%49s %49s %49s",
            cnew.surname,
            cnew.name,
            cnew.otch);
        result = fscanf(f, "%d", &cnew.tr_cnt);

        cnew.inf = (struct Inform*)malloc(cnew.tr_cnt * sizeof(struct Inform));

        for (int i = 0; i < cnew.tr_cnt; ++i)
        {
            char type_str[10];
            result = fscanf(f, "%10s %8s %9s %f",
                cnew.inf[i].day,
                cnew.inf[i].time,
                type_str,
                &cnew.inf[i].summ);

            if (result != 4)
            {
                printf("Ошибка чтения транзакции %d\n", i + 1);
                cnew.inf[i].type = '?';
            }
            else if (strcmp(type_str, "Приход") == 0)
                cnew.inf[i].type = '+';
            else if (strcmp(type_str, "Расход") == 0)
                cnew.inf[i].type = '-';
            else
                cnew.inf[i].type = type_str[0];
            
        }

        struct Person* temp = (struct Person*)realloc(c, (cnt + 1) * sizeof(struct Person));
        c = temp;
        c[cnt] = cnew;
        cnt++;
    }

    fclose(f);
    char sttime[9], edtime[9];

    printf("Начальное время: ");
    scanf("%8s", sttime);

    printf("Конечное время: ");
    scanf("%8s", edtime);

    float mx = 0.0f;
    int* mx_cl = NULL;
    int mx_cnt = 0;

    for (int i = 0; i < cnt; ++i)
    {
        float csp = 0.0f;
        for (int j = 0; j < c[i].tr_cnt; ++j)
            if (c[i].inf[j].type == '-' && ttime(c[i].inf[j].time, sttime, edtime))
            {
                csp += c[i].inf[j].summ;
            }

        printf("Клиент %d: %s %s  расходы: %.2f\n",
            c[i].id, c[i].surname, c[i].name, csp);

        if (csp > mx && csp > 0.0f)
        {
            mx = csp;
            free(mx_cl);
            mx_cl = NULL;
            mx_cnt = 0;

            mx_cl = (int*)malloc(sizeof(int));
            if (mx_cl != NULL)
            {
                mx_cl[0] = i;
                mx_cnt = 1;
            }
        }
        else if (csp == mx && csp > 0.0f)
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
        printf("\nНет расходных операций в период с %s до %s\n", sttime, edtime);

    else
    {
        printf("Максимальные расходы (%.2f руб.) в период %s - %s\n", mx, sttime, edtime);
        printf("Клиенты с максимальными расходами:\n");

        for (int i = 0; i < mx_cnt; ++i)
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

    for (int i = 0; i < cnt; ++i)
        free(c[i].inf);

    free(c);
    free(mx_cl);
    while (getchar() != '\n');
    getchar();
    return 0;
}
