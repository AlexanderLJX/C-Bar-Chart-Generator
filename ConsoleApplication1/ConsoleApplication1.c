#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_CATEGORIES 12
#define INPUT_BUFFER_SIZE 1024

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
void getInput(Category categories[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void scaleValues(Category categories[], Scaled values[], int numCategories);
void sortCategories(Scaled values[], int numCategories, int sortOption);
void drawChart(const Scaled values[], int numCategories, const char* title, const char* xAxisLabel);
void saveChartToFile(const char* filename, const Scaled values[], int numCategories, const char* title, const char* xAxisLabel);
void cancelData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void addData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel);
int getValidatedInteger(int* output, const int validNumbers[], int validCount);
void clearInputBuffer();
int isValidWindowsFilename(char* filename);

int main()
{
	int exitProgram = 0; // Flag to control the loop

	// Variables to store user inputs
	Category categories[12];
	Scaled values[12];
	int numCategories = 0;
	char title[101], xAxisLabel[101];
	int sortOption;

	// Collect user input
	getInput(categories, &numCategories, title, xAxisLabel, &sortOption);

	// Optionally scale values to fit the chart
	scaleValues(categories, values, numCategories);

	// Sort categories based on user's choice
	sortCategories(values, numCategories, sortOption);

	// Draw the chart
	drawChart(values, numCategories, title, xAxisLabel);

	while (!exitProgram) {
		// Ask if the user wants to save the chart and proceed if so
		int options;
		printf("What would you like to do with the chart? Choose an option below\n1.Save the chart.\n2.Modify the chart.\n3.Exit Program\n");
		while (getValidatedInteger(&options, (int[]){ 1, 2, 3 }, 3) == 0) {
			printf("Invalid input. Please enter 1, 2, or 3: ");
			clearInputBuffer();
		}
		clearInputBuffer();
		if (options == 1) {
			char filename[101];
			printf("Enter filename to save: ");
			while (scanf(" %100[^\n]", filename) != 1 || !isValidWindowsFilename(filename)) {
				printf("Invalid input. Please enter less than 100 characters: ");
				clearInputBuffer();
			}
			clearInputBuffer();

			saveChartToFile(filename, values, numCategories, title, xAxisLabel);
		}
		else if (options == 2) {
			int modifyOptions;
			printf("Choose an option to modify the chart. Input an option from below:\n1. Remove data\n2. Add data\n8. Exit Program\n");
			while (getValidatedInteger(&modifyOptions, (int[]){ 1, 2, 8 }, 3) == 0) {
				printf("Invalid input. Please enter 1, 2, or 8: ");
				clearInputBuffer();
			}
			clearInputBuffer();

			if (modifyOptions == 1) {
				cancelData(categories, values, &numCategories, title, xAxisLabel, &sortOption);
			}
			else if (modifyOptions == 2) {
				addData(categories, values, &numCategories, title, xAxisLabel);
			}
			else if (modifyOptions == 8) {
				exitProgram = 1; // Exit the loop and program
			}
		}
		else if (options == 3) {
			exitProgram = 1; // Exit the loop and program
		}
	}

	printf("Exiting....");
	return 0;
}

void clearInputBuffer()
{
	int c;
	while ((c = getchar()) != '\n' && c != EOF)
	{
		// Consume characters until newline or EOF is encountered
	}
}

int isValidWindowsFilename(char* filename) {
	// Check for invalid characters
	char* invalidChars = "<>:\"/\\|?*";
	for (int i = 0; filename[i] != '\0'; i++) {
		if (strchr(invalidChars, filename[i]) != NULL) {
			return 0; // Found an invalid character
		}
	}

	// Convert filename to uppercase for comparison
	char upperFilename[101];
	for (int i = 0; filename[i] != '\0'; i++) {
		upperFilename[i] = toupper(filename[i]);
	}
	upperFilename[strlen(filename)] = '\0'; // Null-terminate the converted string

	// Check for reserved names
	char* reservedNames[] = {
		"CON", "PRN", "AUX", "NUL",
		"COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
		"LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
	};
	for (int i = 0; i < sizeof(reservedNames) / sizeof(char*); i++) {
		if (strcmp(upperFilename, reservedNames[i]) == 0) {
			return 0; // Filename is a reserved name
		}
	}

	return 1; // Filename is valid
}

int getValidatedInteger(int* output, const int validNumbers[], int validCount) {
	char inputBuffer[INPUT_BUFFER_SIZE];
	int tempNum;
	char checkChar;

	// Read input
	if (fgets(inputBuffer, INPUT_BUFFER_SIZE, stdin) != NULL) {
		// Try to read an integer and check for trailing characters
		if (sscanf(inputBuffer, "%d%c", &tempNum, &checkChar) == 2 && checkChar == '\n') {
			if (validCount > 0) {
				// Check against the list of valid numbers
				for (int i = 0; i < validCount; i++) {
					if (tempNum == validNumbers[i]) {
						*output = tempNum;
						return 1; // Valid number from the list
					}
				}
				return 0; // Number not in the list
			}
			else {
				// If no specific numbers are required, any number is accepted
				*output = tempNum;
				return 1; // Valid as any number is accepted
			}
		}
	}
	return 0; // Failure to read a number
}

// Implementations of getInput, scaleValues, sortCategories, drawChart, and saveChartToFile follow...
void getInput(Category categories[], int* numCategories, char* title, char* xAxisLabel, int* sortOption)
{
	printf("Enter the title of the bar chart: ");
	while (scanf(" %100[^\n]", title) != 1) {
		printf("Invalid input. Please enter a title with 100 characters or less: ");
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	printf("Enter the x-axis label: ");
	while (scanf(" %100[^\n]", xAxisLabel) != 1) {
		printf("Invalid input. Please enter an x-axis label with 100 characters or less: ");
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	printf("How many categories (max 12)? ");
	while (getValidatedInteger(numCategories, (int[]){ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, 12) == 0) {
		printf("Invalid input. Please enter a number between 1 and 12: ");
		clearInputBuffer(); // Clears the buffer to remove invalid input
	}
	clearInputBuffer();

	for (int i = 0; i < *numCategories; i++) {
		printf("Enter category %d name: ", i + 1);
		while (scanf(" %15[^\n]", categories[i].name) != 1) {
			printf("Invalid input. Please enter a category name with 15 characters or less: ");
			clearInputBuffer(); // Clears the buffer to remove invalid input
		}

		clearInputBuffer();

		printf("Enter %s value: ", categories[i].name);
		while (getValidatedInteger(&categories[i].value, NULL, 0) == 0) {
			printf("Invalid input. Please enter an integer value for %s: ", categories[i].name);
			clearInputBuffer();
		}

		clearInputBuffer();
	}

	printf("Sort by name (0) or by bar length (1)? ");
	while (getValidatedInteger(sortOption, (int[]){ 0, 1 }, 2) == 0) {
		printf("Invalid input. Please enter 0 for name or 1 for bar length: ");
		clearInputBuffer();
	}

	clearInputBuffer();
}

void scaleValues(Category categories[], Scaled values[], int numCategories)
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
			strcpy(values[i].name, categories[i].name);
			values[i].value = (categories[i].value * 100) / max;
		}
	}
}
int compareByName(const void* a, const void* b)
{
	return strcmp(((Scaled*)a)->name, ((Scaled*)b)->name);
}

int compareByValue(const void* a, const void* b)
{
	return ((Scaled*)b)->value - ((Scaled*)a)->value;
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
void drawChart(const Scaled values[], int numCategories, const char* title, const char* xAxisLabel)
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

void saveChartToFile(const char* filename, const Scaled values[], int numCategories, const char* title, const char* xAxisLabel)
{
	FILE* file = fopen(filename, "w");
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

void cancelData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption)
{
	printf("Select a category to remove (enter category number):\n");
	for (int i = 0; i < *numCategories; i++) {
		printf("%d. %s\n", i + 1, categories[i].name);
	}

	int choice = 0;
	while (1) {
		printf("Your choice: ");
		if (getValidatedInteger(&choice, (int[]){ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, *numCategories) == 1) {
			break; // Valid input
		}
		else {
			printf("Invalid choice. Please enter a number between 1 and %d.\n", *numCategories);
			clearInputBuffer();
		}
	}

	// Adjust for zero-based indexing
	int removeIndex = choice - 1;

	// Shift elements left if necessary
	for (int i = removeIndex; i < *numCategories - 1; i++) {
		categories[i] = categories[i + 1];
	}
	(*numCategories)--; // Decrement the number of categories

	// Re-scale and sort values according to the current sort option
	scaleValues(categories, values, *numCategories);
	sortCategories(values, *numCategories, *sortOption);
	drawChart(values, *numCategories, title, xAxisLabel);
}



// Assuming the definition of Category and Scaled structures
// and other relevant functions (scaleValues, sortCategories, drawChart) are available

void addData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel) {
	if (*numCategories == MAX_CATEGORIES) {
		printf("Maximum number of categories reached (%d).\n", MAX_CATEGORIES);
		return;
	}

	printf("Enter the name of the new category: ");
	while (scanf(" %15[^\n]", categories[*numCategories].name) != 1) {
		printf("Invalid input. Please enter a category name with 15 characters or less: ");
		clearInputBuffer();
	}
	clearInputBuffer();

	printf("Enter the value of the new category (numeric only): ");

	// Loop until a valid integer is entered
	while (getValidatedInteger(&categories[*numCategories].value, NULL, 0) == 0) {
		clearInputBuffer();
		printf("Invalid input. Please enter a numeric value: ");
	}
	clearInputBuffer();

	(*numCategories)++; // Update the number of categories

	scaleValues(categories, values, *numCategories); // Optionally scale values to fit the chart

	int sortOption = 0; // Assume sorting option is decided here or passed in some way
	sortCategories(values, *numCategories, sortOption); // Sort categories based on user's choice

	drawChart(values, *numCategories, title, xAxisLabel); // Draw the chart
}

