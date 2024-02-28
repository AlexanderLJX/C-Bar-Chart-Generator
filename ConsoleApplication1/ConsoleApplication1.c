#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Define a structure to hold category information
typedef struct {
	char name[16]; // Category name + null terminator
	int value;     // Quantity/Value associated with the category
} Category;

// Function prototypes
void getInput(Category categories[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void scaleValues(Category categories[], int numCategories);
void sortCategories(Category categories[], int numCategories, int sortOption);
void drawChart(const Category categories[], int numCategories, const char* title, const char* xAxisLabel);
void saveChartToFile(const char* filename, const Category categories[], int numCategories, const char* title, const char* xAxisLabel);

int main() {
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
	scanf_s(" %c", &saveOption, 1);
	if (saveOption == 'y' || saveOption == 'Y') {
		char filename[101];
		printf("Enter filename to save: ");
		scanf_s("%s", filename, (unsigned)_countof(filename));
		saveChartToFile(filename, categories, numCategories, title, xAxisLabel);
	}

	return 0;
}

// Implementations of getInput, scaleValues, sortCategories, drawChart, and saveChartToFile follow...
void getInput(Category categories[], int* numCategories, char* title, char* xAxisLabel, int* sortOption) {
	printf("Enter the title of the bar chart: ");
	scanf_s(" %[^\n]", title, (unsigned)_countof(title));

	printf("Enter the x-axis label: ");
	scanf_s(" %[^\n]", xAxisLabel, (unsigned)_countof(xAxisLabel));

	printf("How many categories (max 12)? ");
	scanf_s("%d", numCategories);

	for (int i = 0; i < *numCategories; i++) {
		printf("Enter category %d name: ", i + 1);
		scanf_s(" %[^\n]", categories[i].name, (unsigned)_countof(categories[i].name));

		printf("Enter %s value: ", categories[i].name);
		scanf_s("%d", &categories[i].value);
	}

	printf("Sort by name (0) or by value (1)? ");
	scanf_s("%d", sortOption);
}
void scaleValues(Category categories[], int numCategories) {
	// Example scaling logic (customize as needed)
	int max = 0;
	for (int i = 0; i < numCategories; i++) {
		if (categories[i].value > max) max = categories[i].value;
	}

	// Scale if max is too large for display
	if (max > 100) {
		for (int i = 0; i < numCategories; i++) {
			categories[i].value = (categories[i].value * 100) / max;
		}
	}
}
int compareByName(const void* a, const void* b) {
	return strcmp(((Category*)a)->name, ((Category*)b)->name);
}

int compareByValue(const void* a, const void* b) {
	return ((Category*)b)->value - ((Category*)a)->value;
}

void sortCategories(Category categories[], int numCategories, int sortOption) {
	if (sortOption == 0) { // Sort by name
		qsort(categories, numCategories, sizeof(Category), compareByName);
	}
	else { // Sort by value
		qsort(categories, numCategories, sizeof(Category), compareByValue);
	}
}
void drawChart(const Category categories[], int numCategories, const char* title, const char* xAxisLabel) {
	int width = 150; // Max width for bars
	printf("%*s\n\n", width / 2 + (int)strlen(title) / 2, title); // Center the title

	for (int i = 0; i < numCategories; i++) {
		printf("%-15s | ", categories[i].name); // Print category name
		for (int j = 0; j < categories[i].value; j++) {
			printf("X"); // Print bar
		}
		printf("\n");
	}

	// Print x-axis label centered
	printf("%*s\n\n", width / 2 + (int)strlen(xAxisLabel) / 2, xAxisLabel);
}
void saveChartToFile(const char* filename, const Category categories[], int numCategories, const char* title, const char* xAxisLabel) {
	FILE* file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error opening file!\n");
		return;
	}

	fprintf(file, "%s\n\n", title);
	for (int i = 0; i < numCategories; i++) {
		fprintf(file, "%-15s | ", categories[i].name);
		for (int j = 0; j < categories[i].value; j++) {
			fprintf(file, "X");
		}
		fprintf(file, "\n");
	}
	fprintf(file, "\n%s\n", xAxisLabel);

	fclose(file);
	printf("Chart saved to %s\n", filename);
}