#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <curl/curl.h>
#include "cJSON.h"
#define MAX_CATEGORIES 12
#define BUFFER_SIZE 1024

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
void changeTitle(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel);
void changeXLabel(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel);
int getValidatedInteger(int* output, const int validNumbers[], int validCount);
void clearInputBuffer();
int isValidWindowsFilename(char* filename);
bool parseInteger(const char* input, int* number);
bool isValidNumber(int number, const int validNumbers[], int validCount);
void generateChartWithNaturalLanguage(const char* instruction, Category categories[], int* numCategories, char* title, char* xAxisLabel);
void parseApiResponse(const char* response, Category categories[], int* numCategories, char* title, char* xAxisLabel);
void interpretNaturalLanguageInstruction(const char* jsonInstruction, Category categories[], int* numCategories, char* title, char* xAxisLabel);


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
		printf("What would you like to do with the chart? Choose an option below:\n1.Save the chart.\n2.Modify the chart.\n3. Use natural language to modify the chart.\n4.Exit Program\n");
		while (getValidatedInteger(&options, (int[]){ 1, 2, 3, 4 }, 4) == 0) {
			printf("Invalid input. Please enter 1, 2, or 3: ");
		}
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
			int modifyOptions = 0;
			printf("Choose an option to modify the chart. Input an option from below:\n1. Remove data\n2. Add data\n5. Change chart title\n6. Change x-axis label\n8. Exit Program\n");
			while (getValidatedInteger(&modifyOptions, (int[]){ 1, 2, 5, 6, 8 }, 5) == 0) {
				printf("Invalid input. Please enter 1, 2, 5, 6 or 8: ");
				clearInputBuffer();
			}

			if (modifyOptions == 1) {
				cancelData(categories, values, &numCategories, title, xAxisLabel, &sortOption);
			}
			else if (modifyOptions == 2) {
				addData(categories, values, &numCategories, title, xAxisLabel);
			}
			else if (modifyOptions == 5) {
				changeTitle(categories, values, &numCategories, title, xAxisLabel);
			}
			else if (modifyOptions == 6) {
				changeXLabel(categories, values, &numCategories, title, xAxisLabel);
			}
			else if (modifyOptions == 8) {
				exitProgram = 1; // Exit the loop and program
			}
		}
		else if (options == 3) {
			char instruction[1024];
			printf("Enter your instruction in natural language: ");
			fgets(instruction, sizeof(instruction), stdin);

			// Call the function that handles the natural language instruction
			generateChartWithNaturalLanguage(instruction, categories, &numCategories, title, xAxisLabel);

			// Redraw the chart after interpreting the natural language command
			drawChart(values, numCategories, title, xAxisLabel);
		}
		else if (options == 4) {
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

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;
	char** response = (char**)userp;
	char* ptr = realloc(*response, strlen(*response) + realsize + 1);
	if (!ptr) {
		printf("Not enough memory to save HTTP response.\n");
		return 0;
	}

	*response = ptr;
	memcpy(&(ptr[strlen(*response)]), contents, realsize);
	ptr[strlen(*response) + realsize] = 0;
	return realsize;
}

void generateChartWithNaturalLanguage(const char* instruction, Category categories[], int* numCategories, char* title, char* xAxisLabel) {
	CURL* curl;
	CURLcode res;
	char* response = calloc(1, 1); // The response data from the server will be saved here

	// Initialize a libcurl handle
	curl = curl_easy_init();
	if (curl) {
		// Construct the JSON payload
		cJSON* json_payload = cJSON_CreateObject();
		cJSON* json_messages = cJSON_CreateArray();
		cJSON* json_message = cJSON_CreateObject();
		cJSON_AddItemToArray(json_messages, json_message);
		cJSON_AddStringToObject(json_message, "role", "user");
		cJSON_AddStringToObject(json_message, "content", instruction);
		cJSON_AddItemToObject(json_payload, "messages", json_messages);
		char* json_string = cJSON_PrintUnformatted(json_payload);

		// Set the libcurl options
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/v1/chat/completions");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_string);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		// Headers
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// Perform the HTTP POST request
		res = curl_easy_perform(curl);

		// Check for errors
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else {
			// Use the response data
			parseApiResponse(response, categories, numCategories, title, xAxisLabel);
		}

		// Clean up
		free(json_string);
		cJSON_Delete(json_payload);
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
		free(response);
	}
}

void parseApiResponse(const char* response, Category categories[], int* numCategories, char* title, char* xAxisLabel) {
	// Parse the JSON response using cJSON
	cJSON* json_response = cJSON_Parse(response);
	if (json_response == NULL) {
		const char* error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL) {
			fprintf(stderr, "Error before: %s\n", error_ptr);
		}
		return;
	}

	// Extract data from the response (this is highly dependent on the response format)
	// Assuming the response has a field "text" which contains the generated instruction
	const cJSON* text = cJSON_GetObjectItemCaseSensitive(json_response, "text");
	if (cJSON_IsString(text) && (text->valuestring != NULL)) {
		// For example, the response might be a command like "Add a category 'Food' with a value of 40"
		// You would need to write a custom parser to interpret this text and modify your categories
		interpretNaturalLanguageInstruction(text->valuestring, categories, numCategories, title, xAxisLabel);
	}

	cJSON_Delete(json_response);
}

void interpretNaturalLanguageInstruction(const char* jsonInstruction, Category categories[], int* numCategories, char* title, char* xAxisLabel) {
	cJSON* json = cJSON_Parse(jsonInstruction);
	if (json == NULL) {
		const char* error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL) {
			fprintf(stderr, "Error in JSON Parse: %s\n", error_ptr);
		}
		return;
	}

	// Here, we assume the JSON has a specific format. For instance:
	// { "action": "add", "category": { "name": "Food", "value": 40 } }
	cJSON* actionItem = cJSON_GetObjectItemCaseSensitive(json, "action");
	cJSON* categoryItem;

	if (cJSON_IsString(actionItem) && (actionItem->valuestring != NULL)) {
		if (strcmp(actionItem->valuestring, "add") == 0) {
			// Add a new category
			categoryItem = cJSON_GetObjectItemCaseSensitive(json, "category");
			if (cJSON_IsObject(categoryItem)) {
				cJSON* name = cJSON_GetObjectItemCaseSensitive(categoryItem, "name");
				cJSON* value = cJSON_GetObjectItemCaseSensitive(categoryItem, "value");

				if (cJSON_IsString(name) && cJSON_IsNumber(value)) {
					strncpy(categories[*numCategories].name, name->valuestring, sizeof(categories[0].name) - 1);
					categories[*numCategories].value = value->valuedouble; // assuming the value is not an integer
					(*numCategories)++;
				}
			}
		}
		else if (strcmp(actionItem->valuestring, "changeTitle") == 0) {
			// Change chart title
			cJSON* titleItem = cJSON_GetObjectItemCaseSensitive(json, "title");
			if (cJSON_IsString(titleItem)) {
				strncpy(title, titleItem->valuestring, 100);
			}
		}
		// ... Handle other actions like "changeXLabel", "removeCategory", etc.
	}

	cJSON_Delete(json);
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


bool parseInteger(const char* input, int* number) {
	char* endPtr;
	errno = 0; // To detect overflow or underflow

	long val = strtol(input, &endPtr, 10); // Convert string to long

	// Check conversion errors and ensure the entire string was consumed
	if (input == endPtr || *endPtr != '\n' || errno == ERANGE || val < INT_MIN || val > INT_MAX) {
		return false; // Not a valid integer or out of int range
	}

	*number = (int)val; // Store the converted value
	return true; // Success
}

// Function to check if the integer is within the validNumbers array
bool isValidNumber(int number, const int validNumbers[], int validCount) {
	if (validCount == 0) {
		return true; // If validCount is 0, all numbers are considered valid
	}

	for (int i = 0; i < validCount; ++i) {
		if (number == validNumbers[i]) {
			return true; // Number is in the validNumbers list
		}
	}

	return false; // Number not found in validNumbers list
}

int getValidatedInteger(int* output, const int validNumbers[], int validCount) {
	char buffer[BUFFER_SIZE];

	// Prompt user for input
	if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
		return 0; // Error or end-of-file encountered
	}

	int number;
	if (!parseInteger(buffer, &number)) {
		return 0; // Input was not a valid integer
	}

	if (!isValidNumber(number, validNumbers, validCount)) {
		return 0; // Number is not in the list of valid numbers (if applicable)
	}

	// Passed all checks, set output and return success
	*output = number;
	return 1;
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
	}

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
		}
	}

	printf("Sort by name (0) or by bar length (1)? ");
	while (getValidatedInteger(sortOption, (int[]){ 0, 1 }, 2) == 0) {
		printf("Invalid input. Please enter 0 for name or 1 for bar length: ");
	}
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
	else{
		for (int i = 0; i < numCategories; i++)
		{
			strcpy(values[i].name, categories[i].name);
			values[i].value=categories[i].value;
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
		printf("Invalid input. Please enter a numeric value: ");
	}

	(*numCategories)++; // Update the number of categories

	scaleValues(categories, values, *numCategories); // Optionally scale values to fit the chart

	int sortOption = 0; // Assume sorting option is decided here or passed in some way
	sortCategories(values, *numCategories, sortOption); // Sort categories based on user's choice

	drawChart(values, *numCategories, title, xAxisLabel); // Draw the chart
}

void changeTitle(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel) {
	printf("Enter the new title of the bar chart: ");
	while (scanf(" %100[^\n]", title) != 1) {
		printf("Invalid input. Please enter a title with 100 characters or less: ");
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	scaleValues(categories, values, *numCategories);

	int sortOption = 0; // Assume sorting option is decided here or passed in some way
	sortCategories(values, *numCategories, sortOption);

	drawChart(values, *numCategories, title, xAxisLabel);
}

void changeXLabel(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel) {
	printf("Enter the new x-axis label: ");
	while (scanf(" %100[^\n]", xAxisLabel) != 1) {
		printf("Invalid input. Please enter an x-axis label with 100 characters or less: ");
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	scaleValues(categories, values, *numCategories);

	int sortOption = 0; // Assume sorting option is decided here or passed in some way
	sortCategories(values, *numCategories, sortOption);

	drawChart(values, *numCategories, title, xAxisLabel);
}