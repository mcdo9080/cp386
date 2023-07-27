/*
------------------------------------------
CP386 Assignemnt 5 2023-7-26
Mark McDonald 
------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#define BUFFER_SIZE BUFSIZ

void print_matrix(int C, int R, int arr[][R])
{
    int i, j;
    for (i = 0; i < C; i++)
    {
        for (j = 0; j < R; j++)
        {
            printf("%d", arr[i][j]);
            if (j != R - 1)
                printf(" ");
        }
        printf("\n");
    }
}

void print_array(int R, int arr[R])
{
    int i;
    for (i = 0; i < R; i++)
    {
        printf("%d", arr[i]);
        if (i != R - 1)
            printf(" ");
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    char buffer[BUFFER_SIZE];
    int customers = 0, resources = argc - 1, available[resources];

    for (int i = 0; i < resources; i++)
        available[i] = atoi(argv[i + 1]);

    FILE *f;
    if ((f = fopen("sample_in_banker.txt", "r")) == NULL)
        exit(1);

    // find out number of customers
    while (fscanf(f, "%s", buffer) == 1)
        customers++;

    // add information to maximum array

    rewind(f);

    int c = 0, r = 0, maximum[customers][resources], allocated[customers][resources], need[customers][resources];

    while (fscanf(f, "%s", buffer) == 1)
    {
        char *token = strtok(buffer, ",");

        while (token != NULL)
        {
            if (r < resources)
            {
                maximum[c][r] = atoi(token);
                need[c][r] = atoi(token);
                allocated[c][r] = 0;
            }
            r++;
            token = strtok(NULL, ",");
        }
        c++;
        r = 0;
    }

    printf("Number of Customers: %d\n", customers);
    printf("Currently Available resources: ");
    print_array(resources, available);
    printf("maximum Resources from file:\n");
    print_matrix(customers, resources, maximum);

    fclose(f);

    printf("Enter Command: ");

    fgets(buffer, BUFSIZ, stdin);

    while (strcmp(buffer, "Exit\n") != 0)
    {

        if (strncmp(buffer, "RQ", 2) == 0)
        {
            c = 0;
            r = 0;

            int to_add[resources];

            char *token = strtok(buffer, " ");

            token = strtok(NULL, " ");

            c = atoi(token);

            token = strtok(NULL, " ");

            while (token != NULL)
            {

                to_add[r] = atoi(token);

                if (available[r] - atoi(token) < 0)
                {
                    printf("State is NOT safe, request denied\n");
                    c = -1;
                    break;
                }

                r++;
                if (r > resources)
                {
                    printf("ERROR: too many inputs!\n");
                    c = -1;
                    break;
                }
                token = strtok(NULL, " ");
            }

            if (c != -1)
            {
                for (r = 0; r < resources; r++)
                {
                    allocated[c][r] = to_add[r];
                    available[r] = available[r] - to_add[r];
                    need[c][r] = maximum[c][r] - to_add[r];
                }
                printf("State is safe, and request is satisfied\n");
            }
            // print_matrix(customers, resources, allocated);
        }
        else if (strncmp(buffer, "RL", 2) == 0)
        {
            c = 0;
            r = 0;
            char *token = strtok(buffer, " ");

            token = strtok(NULL, " ");

            c = atoi(token);

            token = strtok(NULL, " ");

            while (token != NULL)
            {
                allocated[c][r] -= atoi(token);
                available[r] = available[r] + atoi(token);
                need[c][r] = maximum[c][r] + atoi(token);
                // printf("%d", atoi(token));
                r++;
                token = strtok(NULL, " ");
            }
        }
        else if (strcmp(buffer, "Run\n") == 0)
        {
            int f[customers], ans[customers], ind = 0;

            for (int i = 0; i < customers; i++)
                f[i] = 0;
            int y = 0;

            for (int k = 0; k < 5; k++)
            {
                for (int i = 0; i < customers; i++)
                {
                    if (f[i] == 0)
                    {

                        int flag = 0;
                        for (int j = 0; j < resources; j++)
                        {
                            if (need[i][j] > available[j])
                            {
                                flag = 1;
                                break;
                            }
                        }

                        if (flag == 0)
                        {
                            ans[ind++] = i;
                            for (y = 0; y < resources; y++)
                                available[y] += allocated[i][y];
                            f[i] = 1;
                        }
                    }
                }
            }
            for (int i = 0; i < customers; i++)
                for (int j = 0; j < resources; j++)
                    available[j] = available[j] - allocated[i][j];

            printf("Safe sequence is: ");
            print_array(customers, ans);

            for (int i = 0; i < customers; i++)
            {
                printf("--> Customer Thread %d\n", ans[i]);
                printf("    Allocated resources:  ");
                print_array(resources, allocated[ans[i]]);
                printf("    Needed: ");
                print_array(resources, need[ans[i]]);
                printf("    Available:  ");
                print_array(resources, available);

                printf("    Thread has started\n");

                for (int j = 0; j < resources; j++)
                {
                    available[j] = available[j] + allocated[ans[i]][j];
                    need[ans[i]][j] = maximum[ans[i]][j];
                    allocated[ans[i]][j] = 0;
                }
                printf("    Thread has finished\n");
                printf("    Thread is releasing resources\n");
                printf("    New Available:  ");
                print_array(resources, available);
            }
        }
        else if (strcmp(buffer, "Status\n") == 0)
        {
            printf("Available Resources:\n");
            print_array(resources, available);
            printf("Maximum Resources:\n");
            print_matrix(customers, resources, maximum);
            printf("Allocated Resources:\n");
            print_matrix(customers, resources, allocated);
            printf("Need Resources:\n");
            print_matrix(customers, resources, need);
        }
        else
        {
            printf("ERROR: command not recognized\n");
        }

        printf("Enter Command: ");
        fgets(buffer, BUFSIZ, stdin);
    }
}