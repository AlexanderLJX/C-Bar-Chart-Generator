#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_CATEGORIES 12
// Define a structure to hold category information
typedef struct
{
    char name[16]; // Category name + null terminator
    int value;     // Quantity/Value associated with the category
} Category;

//structure to hold the scaled values so that it doesnt change the initial values
typedef struct
{
    char name[16]; // Category name + null terminator
    int value;     // Quantity/Value associated with the category
} Scaled;

// Function prototypes
void getInput(Category categories[], int *numCategories, char *title, char *xAxisLabel, int *sortOption);
void scaleValues(Category categories[],Scaled values[], int numCategories);
void sortCategories(Scaled values[], int numCategories, int sortOption);
void drawChart(const Scaled values[], int numCategories, const char *title, const char *xAxisLabel);
void saveChartToFile(const char *filename, const Scaled values[], int numCategories, const char *title, const char *xAxisLabel);
void cancelData(Category categories[],Scaled values[], int *numCategories, char *title, char *xAxisLabel);
void addData(Category categories[],Scaled values[], int *numCategories, char *title, char *xAxisLabel);


int main()
{
    // Variables to store user inputs
    Category categories[12];
    Scaled values[12];
    int numCategories = 0;
    char title[101], xAxisLabel[101];
    int sortOption;

    // Collect user input
    getInput(categories, &numCategories, title, xAxisLabel, &sortOption);

    // Optionally scale values to fit the chart
    scaleValues(categories,values,numCategories);

    // Sort categories based on user's choice
    sortCategories(values, numCategories, sortOption);

    // Draw the chart
    drawChart(values, numCategories, title, xAxisLabel);

    // Ask if the user wants to save the chart and proceed if so
    int options;
    printf("What would you like to do with the chart? Choose an option below\n1.Save the chart.\n2.Modify the chart.\n3.Exit Program\n");
    scanf(" %d", &options);
    if( options == 1 ){
        char filename[101];
        printf("Enter filename to save: ");
        scanf("%s", filename);
        saveChartToFile(filename, values, numCategories, title, xAxisLabel);
    }
    else if (options==2)
    {   int modifyOptions;
        printf("Choose an option to modify the chart. Input an option from below:\n1. Remove data\n2. Add data\n8. Exit Program\n");
        scanf(" %d", &modifyOptions);
        if (modifyOptions==1){

            cancelData(categories,values, &numCategories, title, xAxisLabel);
    }   else if (modifyOptions == 2) {
            addData(categories,values, &numCategories, title, xAxisLabel);
    
    }
        else{

            printf("Exiting....");
            exit(0);
        }
        
        
    }
    else
    {
        printf("Exiting....");
        exit(0);
        
    }
    
    
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
            scanf("%d", sortOption);
            break;
        }
    }
}
void scaleValues(Category categories[],Scaled values[], int numCategories)
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
            strcpy(values[i].name,categories[i].name) ;
            values[i].value = (categories[i].value * 100) / max;
        }
    }
}
int compareByName(const void *a, const void *b)
{
    return strcmp(((Scaled *)a)->name, ((Scaled *)b)->name);
}

int compareByValue(const void *a, const void *b)
{
    return ((Scaled *)b)->value - ((Scaled *)a)->value;
}

void sortCategories(Scaled values[], int numCategories, int sortOption)
{
    if (sortOption == 0)
    { // Sort by name
        qsort(values, numCategories, sizeof(Scaled), compareByName);
    }
    else
    { // Sort by value
        qsort(values, numCategories, sizeof(Scaled), compareByValue);
    }
}
void drawChart(const Scaled values[], int numCategories, const char *title, const char *xAxisLabel)
{
    int width = 150;                                              // Max width for bars
    printf("%*s\n\n", width / 2 + (int)strlen(title) / 2, title); // Center the title
    

    for (int i = 0; i < numCategories; i++)
    {
        printf("%-15s | ", values[i].name); // Print category name
        for (int j = 0; j < values[i].value; j++)
        {
            printf("X"); // Print bar
        }
        printf("\n");
    }

    // Print x-axis label centered
    printf("%*s\n\n", width / 2 + (int)strlen(xAxisLabel) / 2, xAxisLabel);
}

void saveChartToFile(const char *filename, const Scaled values[], int numCategories, const char *title, const char *xAxisLabel)
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
        fprintf(file, "%-15s | ", values[i].name);
        for (int j = 0; j < values[i].value; j++)
        {
            fprintf(file, "X");
        }
        fprintf(file, "\n");
    }
    fprintf(file, "\n%s\n", xAxisLabel);

    fclose(file);
    printf("Chart saved to %s\n", filename);
}

// Function to remove data 
void cancelData(Category categories[],Scaled values[], int *numCategories, char *title, char *xAxisLabel ) 
{
  // 1. Display list of categories
  printf("Select a category to remove (enter category number):\n");
  for (int i = 0; i < *numCategories; i++) {
    printf("%d. %s\n", i + 1, categories[i].name);
  }

  // 2. Get user input for category to remove
  int choice;
  int validChoice = 0;
  while (!validChoice) {
    printf("Your choice: ");
    scanf("%d", &choice);
    if (choice > 0 && choice <= *numCategories) {
      validChoice = 1;
    } else {
      printf("Invalid choice. Please enter a number between 1 and %d.\n", *numCategories);
    }
  }

  // 3. Remove the selected category
  int removeIndex = choice - 1; // Adjust for zero-based indexing
  if (removeIndex < *numCategories - 1) {
    // Shift elements after the removed category to the left
    for (int i = removeIndex; i < *numCategories - 1; i++) {
      categories[i] = categories[i + 1];
      
      
    }

  }
  (*numCategories)--; // Decrement the number of categories


   // Optionally scale values to fit the chart
  scaleValues(categories,values,*numCategories);
  int sortOption;
  // Sort categories based on user's choice
  sortCategories(values, *numCategories, sortOption);
  drawChart(values, *numCategories, title, xAxisLabel);

  
}



// function to add data
void addData(Category categories[], Scaled values[], int *numCategories, char *title, char *xAxisLabel) {

    
  // 1. Check if there's space for a new category
  if (*numCategories == MAX_CATEGORIES) {
    printf("Maximum number of categories reached (%d).\n", MAX_CATEGORIES);
    return;
  }

  // 2. Get user input for new category name and value
  printf("Enter the name of the new category: ");
  scanf(" %[^\n]", categories[*numCategories].name); // Read a line of text

  printf("Enter the value of the new category: ");
  scanf("%d", &categories[*numCategories].value);

  // 3. Update the number of categories
  (*numCategories)++;

  
  // Optionally scale values to fit the chart
    scaleValues(categories,values,*numCategories);

    int sortOption;
    // Sort categories based on user's choice
    sortCategories(values, *numCategories, sortOption);

    // Draw the chart
    drawChart(values, *numCategories, title, xAxisLabel);
}