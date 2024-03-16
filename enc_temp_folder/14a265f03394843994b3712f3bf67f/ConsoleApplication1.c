#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Define a structure to hold category information
typedef struct
{
    char name[16]; // Category name + null terminator
    int value;     // Quantity/Value associated with the category
} Category;

// Function prototypes
void getInput(Category categories[], int *numCategories, char *title, char *xAxisLabel, int *sortOption);
void scaleValues(Category categories[], int numCategories);
void sortCategories(Category categories[], int numCategories, int sortOption);
void drawChart(const Category categories[], int numCategories, const char *title, const char *xAxisLabel);
void saveChartToFile(const char *filename, const Category categories[], int numCategories, const char *title, const char *xAxisLabel);

int main()
{
    // Variables to store user inputs
    Category categories[12];
    int numCategories = 0;
    char title[101], xAxisLabel[101];
    int sortOption;

    // Collect user input
    getInput(categories, &numCategories, title, xAxisLabel, &sortOption);

    // Optionally scale values to fit the chart
    scaleValues(categories, numCategories);

    // Sort categories based on user's choice
    sortCategories(categories, numCategories, sortOption);

    // Draw the chart
    drawChart(categories, numCategories, title, xAxisLabel);

    // Ask if the user wants to save the chart and proceed if so
    char saveOption;
    printf("Do you want to save the chart? (y/n): ");
    scanf(" %c", &saveOption);
    if (saveOption == 'y' || saveOption == 'Y')
    {
        char filename[101];
        printf("Enter filename to save: ");
        scanf("%s", filename);
        saveChartToFile(filename, categories, numCategories, title, xAxisLabel);
    }

    return 0;
}

// void clearInputBuffer()
// {
//     int c;
//     while ((c = getchar()) != '\n' && c != EOF)
//     {
//         // Consume characters until newline or EOF is encountered
//     }
// }

// Implementations of getInput, scaleValues, sortCategories, drawChart, and saveChartToFile follow...
void getInput(Category categories[], int *numCategories, char *title, char *xAxisLabel, int *sortOption)
{
    int x = 1;
    int y = 1;
    printf("Enter the title of the bar chart: ");
    scanf(" %[^\n]", title); // Reads a line of text

    printf("Enter the x-axis label: ");
    scanf(" %[^\n]", xAxisLabel);

    while (x == 1)
    {
        printf("How many categories (max 12)? ");
        scanf("%d", numCategories);
        if (*numCategories > 12)
        {
            printf("The maximum number of categories that can be generated is 12.\n");
            *numCategories = 0;
        }
        else
        {
            for (int i = 0; i < *numCategories; i++)
            {
                printf("Enter category %d name: ", i + 1);
                scanf("%s", &categories[i].name);

                if (strlen(categories[i].name) > 15)
                {
                    printf("Category name must at most be 15 characters only.\n");
                    i--;
                }
                else
                {
                    printf("Enter %s value: ", categories[i].name);
                    scanf("%d", &categories[i].value);
                    // printf("%d", &categories[i].value);
                    // if (isdigit(categories[i].value))
                    // {
                    //     printf("You can only enter an integer value.\n");
                    //     clearInputBuffer();
                    //     i--;
                    // }
                }
            }
            printf("Sort by name (0) or by bar length (1)? ");
            scanf("%d", &sortOption);
            break;
        }
    }
}
void scaleValues(Category categories[], int numCategories)
{
    // Example scaling logic (customize as needed)
    int max = 0;
    for (int i = 0; i < numCategories; i++)
    {
        if (categories[i].value > max)
            max = categories[i].value;
    }

    // Scale if max is too large for display
    if (max > 100)
    {
        for (int i = 0; i < numCategories; i++)
        {
            categories[i].value = (categories[i].value * 100) / max;
        }
    }
}
int compareByName(const void *a, const void *b)
{
    return strcmp(((Category *)a)->name, ((Category *)b)->name);
}

int compareByValue(const void *a, const void *b)
{
    return ((Category *)b)->value - ((Category *)a)->value;
}

void sortCategories(Category categories[], int numCategories, int sortOption)
{
    if (sortOption == 0)
    { // Sort by name
        qsort(categories, numCategories, sizeof(Category), compareByName);
    }
    else
    { // Sort by value
        qsort(categories, numCategories, sizeof(Category), compareByValue);
    }
}
void drawChart(const Category categories[], int numCategories, const char *title, const char *xAxisLabel)
{
    int width = 150;                                              // Max width for bars
    printf("%*s\n\n", width / 2 + (int)strlen(title) / 2, title); // Center the title

    for (int i = 0; i < numCategories; i++)
    {
        printf("%-15s | ", categories[i].name); // Print category name
        for (int j = 0; j < categories[i].value; j++)
        {
            printf("X"); // Print bar
        }
        printf("\n");
    }

    // Print x-axis label centered
    printf("%*s\n\n", width / 2 + (int)strlen(xAxisLabel) / 2, xAxisLabel);
}
void saveChartToFile(const char *filename, const Category categories[], int numCategories, const char *title, const char *xAxisLabel)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return;
    }

    fprintf(file, "%s\n\n", title);
    for (int i = 0; i < numCategories; i++)
    {
        fprintf(file, "%-15s | ", categories[i].name);
        for (int j = 0; j < categories[i].value; j++)
        {
            fprintf(file, "X");
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n%s\n", xAxisLabel);

    fclose(file);
    printf("Chart saved to %s\n", filename);
}