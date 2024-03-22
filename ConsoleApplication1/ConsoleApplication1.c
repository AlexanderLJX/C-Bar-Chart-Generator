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
#include <windows.h> // For sleep and system functions
#define MAX_CATEGORIES 12
#define BUFFER_SIZE 1024


// Define ANSI color codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Define a structure to hold category information
typedef struct
{
	char name[16]; // Category name + null terminator
	int value;	   // Quantity/Value associated with the category
} Category;

// structure to hold the scaled values so that it doesnt change the initial values
typedef struct
{
	char name[16]; // Category name + null terminator
	int value;	   // Quantity/Value associated with the category
} Scaled;

// Function prototypes
void getInput(Category categories[], int *numCategories, char *title, char *xAxisLabel, int *sortOption);
void scaleValues(Category categories[], Scaled values[], int numCategories);
void sortCategories(Category categories[],Scaled values[], int numCategories, int sortOption);
void drawChart(const Category categories[],const Scaled values[], int numCategories, const char* title, const char* xAxisLabel);
void saveChartToFile(const char* filename, const Scaled values[], int numCategories, const char* title, const char* xAxisLabel);
void cancelData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void addData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void changeCategoryName(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void changeCategoryValue(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void changeTitle(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel);
void changeXLabel(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel);
int getValidatedInteger(int* output, const int validNumbers[], int validCount);
void clearInputBuffer();
int isValidWindowsFilename(char *filename);
bool parseInteger(const char *input, int *number);
bool isValidNumber(int number, const int validNumbers[], int validCount);
void generateChartWithNaturalLanguage(const char* instruction, Category categories[], int* numCategories, char* title, char* xAxisLabel);
void parseApiResponse(const char* response, Category categories[], int* numCategories, char* title, char* xAxisLabel);
size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);
void removeTrailingWeirdCharacters(char* str);


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
	sortCategories(categories,values, numCategories, sortOption);

	// Draw the chart
	drawChart(categories,values, numCategories, title, xAxisLabel);

	while (!exitProgram)
	{
		// Ask if the user wants to save the chart and proceed if so
		int options;
		printf("What would you like to do with the chart? Choose an option below:\n1.Save the chart.\n2.Modify the chart.\n3.Use natural language to modify the chart.\n4.Exit Program\n");
		while (getValidatedInteger(&options, (int[]){ 1, 2, 3, 4 }, 4) == 0) {
			printf("Invalid input. Please enter 1, 2, or 3: ");
		}
		if (options == 1)
		{
			char filename[101];
			printf("Enter filename to save: ");
			while (scanf(" %100[^\n]", filename) != 1 || !isValidWindowsFilename(filename))
			{
				printf("Invalid input. Please enter less than 100 characters: ");
				clearInputBuffer();
			}
			clearInputBuffer();

			saveChartToFile(filename, values, numCategories, title, xAxisLabel);
		}
		else if (options == 2)
		{
			int modifyOptions = 0;
			printf("Choose an option to modify the chart. Input an option from below:\n1. Remove data\n2. Add data\n3. Modify a category name\n4. Modify a category value\n5. Change chart title\n6. Change x-axis label\n7. Re-sort Chart\n8. Exit Program\n");
			while (getValidatedInteger(&modifyOptions, (int[]){ 1, 2, 3, 4, 5, 6, 7, 8}, 8) == 0) {
				printf("Invalid input. Please enter 1, 2, 3, 4, 5, 6 ,7 or 8: ");
				clearInputBuffer();
			}

			if (modifyOptions == 1)
			{
				cancelData(categories, values, &numCategories, title, xAxisLabel, &sortOption);
			}
			else if (modifyOptions == 2)
			{
				addData(categories, values, &numCategories, title, xAxisLabel,&sortOption);
            }
            else if (modifyOptions == 3) {
                changeCategoryName(categories, values, &numCategories, title, xAxisLabel, &sortOption);
            }
            else if (modifyOptions == 4) {
                changeCategoryValue(categories, values, &numCategories, title, xAxisLabel, &sortOption);
            }
			else if (modifyOptions == 5) {
				changeTitle(categories, values, &numCategories, title, xAxisLabel);
			}
			else if (modifyOptions == 6)
			{
				changeXLabel(categories, values, &numCategories, title, xAxisLabel);
			}
			else if (modifyOptions == 7)
			{

				printf("Choose new sorting method : Sort by name (0) or by bar length (1)? ");
						while (getValidatedInteger(&sortOption, (int[]){0, 1}, 2) == 0)
						{
							printf("Invalid input. Please enter 0 for name or 1 for bar length: ");
							clearInputBuffer();
							
						}
						

						sortCategories(categories, values, numCategories, sortOption); // Sort categories based on user's choice

    					drawChart(categories, values, numCategories, title, xAxisLabel); // Draw the chart
			}
			else if (modifyOptions == 8)
			{
				exitProgram = 1; // Exit the loop and program
			}
		}
		else if (options == 3) {
			char instruction[1024];
			char message[4096];
			const char* longString =
				"I'm about to give you some instructions for changing the parameters of a bar chart. Do not reply with extra text like \"Here's the JSON output for your instruction:\" and do not surround the output in code blocks. I want you to return the output in pure JSON format. \n"
				"for example, if the instruction is: \"Change the name of the chart to price of cars and change the xaxis to dollars\" you will return exactly the following:\n"
				"{\"change\":{\"title\": \"price of cars\", \"x-axis\": \"dollars\"}}\n"
				"\n"
				"If the instruction is: \"add a new row named bmw with value of 400,000\" you will return exactly the following:\n"
				"{\"add\": {\"row\": {\"name\": \"bmw\", \"value\": 10}}}\n"
				"\n"
				"If the instruction is: \"delete the row named porche\" you will return exactly the following:\n"
				"{\"delete\":{\"row\": {\"name\": \"porche\"}}}\n"
				"\n"
				"If the instruction is: \"change the name of the table to blah blah black sheep and change the row named bye bye to good bye\" you will return exactly the following:\n"
				"{\"change\":{\"title\": \"price of cars\", \"row\": {\"name\": \"bye bye\", \"changed_name\": \"good bye\"}}}\n"
				"\n"
				"If the instruction is: \"change the xaxis of the table to good night punpun and change the row named morning pupun to to value 6586\" you will return exactly the following:\n"
				"{\"change\":{\"title\": \"good night punpun\", \"row\": {\"name\": \"morning punpun\", \"changed_value\": 6586}}}\n"
				"\n"
				"\n"
				"This is the instruction I want you to reply a json to:\n";
			// Copy the predefined message into the modifiable buffer
			strncpy(message, longString, sizeof(message) - 1);
			message[sizeof(message) - 1] = '\0'; // Ensure null-termination

			printf("Enter your instruction in natural language: ");
			fgets(instruction, sizeof(instruction), stdin);
			printf("Generating chart with natural language instruction...\n");
			// Append the user input to the modifiable buffer
			strncat(message, instruction, sizeof(message) - strlen(message) - 1);

			// Call the function that handles the natural language instruction
			generateChartWithNaturalLanguage(message, categories, &numCategories, title, xAxisLabel);

			scaleValues(categories, values, numCategories); // Optionally scale values to fit the chart

			int sortOption = 0; // Assume sorting option is decided here or passed in some way
			sortCategories(categories,values, numCategories, sortOption); // Sort categories based on user's choice

			// Redraw the chart after interpreting the natural language command
			drawChart(categories,values, numCategories, title, xAxisLabel);
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

size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t real_size = size * nmemb;
	char** response_ptr = (char**)userp;
	char* ptr = realloc(*response_ptr, strlen(*response_ptr) + real_size + 1);

	if (ptr == NULL) {
		// handle realloc failure; possibly return 0 to indicate failure to libcurl
		return 0;
	}

	*response_ptr = ptr;
	memcpy(&((*response_ptr)[strlen(*response_ptr)]), contents, real_size);
	(*response_ptr)[strlen(*response_ptr) + real_size] = '\0';

	return real_size;
}

void removeTrailingWeirdCharacters(char* str) {
	if (str == NULL) return; // Safety check

	char* lastValidChar = strrchr(str, '}'); // Find the last occurrence of '}'
	if (lastValidChar != NULL) {
		// Move one character forward to keep the '}' as part of the valid JSON
		lastValidChar++;
		// Null-terminate the string right after the last valid character
		*lastValidChar = '\0';
	}
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
			// Remove trailing weird characters from the response
			removeTrailingWeirdCharacters(response);
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

	// Navigate through the JSON structure to find the 'content' field
	const cJSON* choices = cJSON_GetObjectItemCaseSensitive(json_response, "choices");
	if (!cJSON_IsArray(choices)) {
		fprintf(stderr, "Error: 'choices' is not an array\n");
		cJSON_Delete(json_response);
		return;
	}

	const cJSON* first_choice = cJSON_GetArrayItem(choices, 0); // Assume we only care about the first choice
	if (first_choice == NULL) {
		fprintf(stderr, "Error: First choice is missing\n");
		cJSON_Delete(json_response);
		return;
	}

	const cJSON* message = cJSON_GetObjectItemCaseSensitive(first_choice, "message");
	if (message == NULL) {
		fprintf(stderr, "Error: 'message' is missing from choice\n");
		cJSON_Delete(json_response);
		return;
	}

	const cJSON* content = cJSON_GetObjectItemCaseSensitive(message, "content");
	if (!cJSON_IsString(content) || content->valuestring == NULL) {
		fprintf(stderr, "Error: 'content' is missing or not a string\n");
		cJSON_Delete(json_response);
		return;
	}

	// debug print the content->valuestring
	printf("[Debug] content->valuestring: %s\n", content->valuestring);

	// Now we have the actual instruction in content->valuestring
	// The instruction should already be in JSON format, so parse it
	cJSON* instruction = cJSON_Parse(content->valuestring);
	if (instruction == NULL) {
		fprintf(stderr, "Error parsing instruction content\n");
		cJSON_Delete(json_response);
		return;
	}

	// Check if it's a change action
	cJSON* change = cJSON_GetObjectItemCaseSensitive(instruction, "change");
	if (change) {
		cJSON* titleItem = cJSON_GetObjectItemCaseSensitive(change, "title");
		if (titleItem && cJSON_IsString(titleItem)) {
			// Make sure not to write more characters than the array can hold and set the null terminator.
			size_t max_size = 100; // Reserving space for the null terminator
			strncpy(title, titleItem->valuestring, max_size);
			title[max_size] = '\0'; // Explicitly set the null terminator
		}
		cJSON* xAxisItem = cJSON_GetObjectItemCaseSensitive(change, "x-axis");
		if (xAxisItem && cJSON_IsString(xAxisItem)) {
			// Make sure not to write more characters than the array can hold and set the null terminator.
			size_t max_size = 100; // Reserving space for the null terminator
			strncpy(xAxisLabel, xAxisItem->valuestring, max_size);
			xAxisLabel[max_size] = '\0'; // Explicitly set the null terminator
		}
		cJSON* row = cJSON_GetObjectItemCaseSensitive(change, "row");
		if (row && *numCategories < MAX_CATEGORIES) {
			cJSON* name = cJSON_GetObjectItemCaseSensitive(row, "name");
			cJSON* changed_name = cJSON_GetObjectItemCaseSensitive(row, "changed_name");
			if (name && changed_name) {
				for (int i = 0; i < *numCategories; i++) {
					if (strcmp(categories[i].name, name->valuestring) == 0) {
						// Update the category name
						strncpy(categories[i].name, changed_name->valuestring, sizeof(categories[0].name) - 1);
						break;
					}
				}
			}
			cJSON* changed_value = cJSON_GetObjectItemCaseSensitive(row, "changed_value");
			if (name && changed_value) {
				for (int i = 0; i < *numCategories; i++) {
					if (strcmp(categories[i].name, name->valuestring) == 0) {
						// Update the category value
						categories[i].value = changed_value->valueint;
						break;
					}
				}
			}
		}
	}

	// Check if it's an add action
	cJSON* add = cJSON_GetObjectItemCaseSensitive(instruction, "add");
	if (add) {
		cJSON* row = cJSON_GetObjectItemCaseSensitive(add, "row");
		if (row && *numCategories < MAX_CATEGORIES) {
			cJSON* name = cJSON_GetObjectItemCaseSensitive(row, "name");
			cJSON* value = cJSON_GetObjectItemCaseSensitive(row, "value");
			if (name && value) {
				strncpy(categories[*numCategories].name, name->valuestring, sizeof(categories[0].name) - 1);
				categories[*numCategories].value = value->valueint;
				(*numCategories)++;
			}
		}
	}

	// Check if it's a delete action
	cJSON* delete = cJSON_GetObjectItemCaseSensitive(instruction, "delete");
	if (delete) {
		cJSON* row = cJSON_GetObjectItemCaseSensitive(delete, "row");
		if (row) {
			cJSON* name = cJSON_GetObjectItemCaseSensitive(row, "name");
			if (name) {
				for (int i = 0; i < *numCategories; i++) {
					if (strcmp(categories[i].name, name->valuestring) == 0) {
						// Shift elements left to delete the category
						for (int j = i; j < *numCategories - 1; j++) {
							categories[j] = categories[j + 1];
						}
						(*numCategories)--;
						break;
					}
				}
			}
		}
	}

	cJSON_Delete(json_response);
}

int isValidWindowsFilename(char* filename) {
	// Check for invalid characters
	char *invalidChars = "<>:\"/\\|?*";
	for (int i = 0; filename[i] != '\0'; i++)
	{
		if (strchr(invalidChars, filename[i]) != NULL)
		{
			return 0; // Found an invalid character
		}
	}

	// Convert filename to uppercase for comparison
	char upperFilename[101];
	for (int i = 0; filename[i] != '\0'; i++)
	{
		upperFilename[i] = toupper(filename[i]);
	}
	upperFilename[strlen(filename)] = '\0'; // Null-terminate the converted string

	// Check for reserved names
	char *reservedNames[] = {
		"CON", "PRN", "AUX", "NUL",
		"COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
		"LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};
	for (int i = 0; i < sizeof(reservedNames) / sizeof(char *); i++)
	{
		if (strcmp(upperFilename, reservedNames[i]) == 0)
		{
			return 0; // Filename is a reserved name
		}
	}

	return 1; // Filename is valid
}

bool parseInteger(const char *input, int *number)
{
	char *endPtr;
	errno = 0; // To detect overflow or underflow

	long val = strtol(input, &endPtr, 10); // Convert string to long

	// Check conversion errors and ensure the entire string was consumed
	if (input == endPtr || *endPtr != '\n' || errno == ERANGE || val < INT_MIN || val > INT_MAX)
	{
		return false; // Not a valid integer or out of int range
	}

	*number = (int)val; // Store the converted value
	return true;		// Success
}

// Function to check if the integer is within the validNumbers array
bool isValidNumber(int number, const int validNumbers[], int validCount)
{
	if (validCount == 0)
	{
		return true; // If validCount is 0, all numbers are considered valid
	}

	for (int i = 0; i < validCount; ++i)
	{
		if (number == validNumbers[i])
		{
			return true; // Number is in the validNumbers list
		}
	}

	return false; // Number not found in validNumbers list
}

int getValidatedInteger(int *output, const int validNumbers[], int validCount)
{
	char buffer[BUFFER_SIZE];

	// Prompt user for input
	if (fgets(buffer, BUFFER_SIZE, stdin) == NULL)
	{
		return 0; // Error or end-of-file encountered
	}

	int number;
	if (!parseInteger(buffer, &number))
	{
		return 0; // Input was not a valid integer
	}

	if (!isValidNumber(number, validNumbers, validCount))
	{
		return 0; // Number is not in the list of valid numbers (if applicable)
	}

	// Passed all checks, set output and return success
	*output = number;
	return 1;
}

// Implementations of getInput, scaleValues, sortCategories, drawChart, and saveChartToFile follow...
void getInput(Category categories[], int *numCategories, char *title, char *xAxisLabel, int *sortOption)
{
	printf("Enter the title of the bar chart: ");
	while (scanf(" %100[^\n]", title) != 1)
	{
		printf("Invalid input. Please enter a title with 100 characters or less: ");
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	printf("Enter the x-axis label: ");
	while (scanf(" %100[^\n]", xAxisLabel) != 1)
	{
		printf("Invalid input. Please enter an x-axis label with 100 characters or less: ");
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	printf("How many categories (max 12)? ");
	while (getValidatedInteger(numCategories, (int[]){1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, 12) == 0)
	{
		printf("Invalid input. Please enter a number between 1 and 12: ");
	}

	for (int i = 0; i < *numCategories; i++)
	{
		printf("Enter category %d name: ", i + 1);
		while (scanf(" %15[^\n]", categories[i].name) != 1)
		{
			printf("Invalid input. Please enter a category name with 15 characters or less: ");
			clearInputBuffer(); // Clears the buffer to remove invalid input
		}

		clearInputBuffer();

		printf("Enter %s value: ", categories[i].name);
		while (getValidatedInteger(&categories[i].value, NULL, 0) == 0)
		{
			printf("Invalid input. Please enter an integer value for %s: ", categories[i].name);
		}
	}

	printf("Sort by name (0) or by bar length (1)? ");
	while (getValidatedInteger(sortOption, (int[]){0, 1}, 2) == 0)
	{
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
		// printf("%d\n", max);

		for (int i = 0; i < numCategories; i++)
		{
			strcpy(values[i].name, categories[i].name);
			values[i].value = (categories[i].value * 100) / max;
		}
	}
	else
	{
		for (int i = 0; i < numCategories; i++)
		{
			strcpy(values[i].name, categories[i].name);
			values[i].value = categories[i].value;
		}
	}
}
//sort functions for scaled values
int compareByName(const void *a, const void *b)
{
	return strcmp(((Scaled *)a)->name, ((Scaled *)b)->name);
	
}

int compareByValue(const void *a, const void *b)
{
	return ((Scaled *)b)->value - ((Scaled *)a)->value;
	
}
//sort functions for non scaled values
int compareByName2(const void *a, const void *b)
{
	
	return strcmp(((Category *)a)->name, ((Category *)b)->name);
}

int compareByValue2(const void *a, const void *b)
{
	
	return ((Category *)b)->value - ((Category *)a)->value; 
}
void sortCategories(Category catergories[],Scaled values[], int numCategories, int sortOption)
{
	if (sortOption == 0)
	{ // Sort by name
		qsort(values, numCategories, sizeof(Scaled), compareByName);
		qsort(catergories, numCategories, sizeof(Category), compareByName2);
	}
	else
	{ // Sort by value
		qsort(values, numCategories, sizeof(Scaled), compareByValue);
		qsort(catergories, numCategories, sizeof(Category), compareByValue2);
	}
}

void drawChart(const Category categories[], const Scaled values[], int numCategories, const char* title, const char* xAxisLabel)
{
	int width = 150; // Max width for bars
	printf(ANSI_COLOR_BLUE);
	printf("%*s\n\n", width / 2 + (int)strlen(title) / 2, title); // Center the title

	int max_bar_length = 0;
	int axis_values = 0;
	for (int i = 0; i < numCategories; i++) {
		printf(ANSI_COLOR_YELLOW);
		printf("%-16s |", values[i].name); // Print category name
		if (values[i].value > max_bar_length) {
			axis_values = categories[i].value;
			max_bar_length = values[i].value;
		}
		for (int j = 0; j < values[i].value; j++) {
			printf(ANSI_COLOR_GREEN);
			printf("X"); // Print bar
			printf(ANSI_COLOR_RESET);
			Sleep(10); // Delay for 50 milliseconds to animate
		}
		printf("\n%17s|\n", "                ");
	}

	for (int k = 0; k <= max_bar_length; k++) {
		if (k == 0) {
			printf("%17s+", "                ");
		}
		else if (k == max_bar_length / 2 || k == max_bar_length) {
			printf(ANSI_COLOR_MAGENTA);
			printf("+");
			printf(ANSI_COLOR_RESET);
		}
		else {
			printf(ANSI_COLOR_RED);
			printf("-");
			printf(ANSI_COLOR_RESET);
		}
	}

	for (int n = 0; n <= max_bar_length; n++) {
		if (n == 0) {
			printf("\n%17s0", "                ");
		}
		else if (n == max_bar_length / 2) {
			printf(ANSI_COLOR_CYAN);
			printf("%d", axis_values / 2);
		}
		else if (n == max_bar_length) {
			printf(ANSI_COLOR_CYAN);
			printf("%d", axis_values);
		}
		else {
			printf(" ");
		}
	}
	printf(ANSI_COLOR_BLUE);
	printf("\n%*s\n\n", width / 2 + (int)strlen(xAxisLabel) / 2, xAxisLabel);
	printf(ANSI_COLOR_RESET);
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

// Function to remove a category [Option 1]
void cancelData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption)
{
	printf("Select a category to remove (enter category number):\n");
	for (int i = 0; i < *numCategories; i++)
	{
		printf("%d. %s\n", i + 1, categories[i].name);
	}

	int choice = 0;
	while (1)
	{
		printf("Your choice: ");
		if (getValidatedInteger(&choice, (int[]){1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, *numCategories) == 1)
		{
			break; // Valid input
		}
		else
		{
			printf("Invalid choice. Please enter a number between 1 and %d.\n", *numCategories);
		}
	}

	// Adjust for zero-based indexing
	int removeIndex = choice - 1;

	// Shift elements left if necessary
	for (int i = removeIndex; i < *numCategories - 1; i++)
	{
		categories[i] = categories[i + 1];
	}
	(*numCategories)--; // Decrement the number of categories

	// Re-scale and sort values according to the current sort option
	scaleValues(categories, values, *numCategories);
	sortCategories(categories,values, *numCategories, *sortOption);
	drawChart(categories,values, *numCategories, title, xAxisLabel);
}

// Assuming the definition of Category and Scaled structures
// and other relevant functions (scaleValues, sortCategories, drawChart) are available

// Function to add a category [Option 2]
void addData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption)
{
    if (*numCategories == MAX_CATEGORIES) {
        printf("Maximum number of categories reached (%d).\n", MAX_CATEGORIES);
        return;
    }

    char newCategoryName[16]; // Buffer to hold the new category name
    bool isDuplicate = false; // Flag to check for duplicate names

    do {
        printf("Enter the name of the new category: ");
        scanf(" %15[^\n]", newCategoryName); // Read the new category name
        clearInputBuffer(); // Clear the input buffer
		
        isDuplicate=false;// Reset the flag to false for each new input
        // Check for duplicate names
        for (int i = 0; i < *numCategories; i++) {
            if (strcmp(categories[i].name, newCategoryName) == 0) {
                printf("Category name already exists. Please enter a unique name.\n");
                isDuplicate = true; // Set the flag to true if a duplicate is found
                break;
            }
        }
    } while (isDuplicate); // Loop until a unique name is entered

    // Copy the new category name to the categories array
    strcpy(categories[*numCategories].name, newCategoryName);

    printf("Enter the value of the new category (numeric only): ");
    while (getValidatedInteger(&categories[*numCategories].value, NULL, 0) == 0) {
        printf("Invalid input. Please enter a numeric value: ");
    }

    (*numCategories)++; // Update the number of categories

    scaleValues(categories, values, *numCategories); // Optionally scale values to fit the chart

    sortCategories(categories,values, *numCategories, *sortOption); // Sort categories based on user's choice

    drawChart(categories,values, *numCategories, title, xAxisLabel); // Draw the chart
}

// Function to modify a category name [Option 3]
void changeCategoryName(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption) {
    printf("Select a category to modify (enter category number):\n");
    for (int i = 0; i < *numCategories; i++) {
        printf("%d. %s\n", i + 1, categories[i].name);
    }

    int choice = 0;
    while (1) {
        if (getValidatedInteger(&choice, (int[]){ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, *numCategories) == 1) {
            break; // Break if valid input
        }
        else {
            printf("Invalid choice. Please enter a number between 1 and %d.\n", *numCategories);
        }
    }

    // Adjust for zero-based indexing
    int modifyIndex = choice - 1;

    printf("Enter the new name for %s: ", categories[modifyIndex].name);
    while (scanf(" %15[^\n]", categories[modifyIndex].name) != 1) {
        printf("Invalid input. Please enter a category name with 15 characters or less: ");
        clearInputBuffer();
    }
    clearInputBuffer();

    scaleValues(categories, values, *numCategories);

    sortCategories(categories,values, *numCategories, *sortOption);

    drawChart(categories,values, *numCategories, title, xAxisLabel);
}

// Function to modify a category value [Option 4]
void changeCategoryValue(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption)
{
    printf("Select a category to modify (enter category number):\n");
    for (int i = 0; i < *numCategories; i++) {
        printf("%d. %s\n", i + 1, categories[i].name);
    }
    int choice = 0;
    while (1) {
        if (getValidatedInteger(&choice, (int[]){ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }, *numCategories) == 1) {
            break; // Break if valid input
        }
        else {
            printf("Invalid choice. Please enter a number between 1 and %d.\n", *numCategories);
        }
    }

    // Adjust for zero-based indexing
    int modifyIndex = choice - 1;
    printf("Enter the new value for %s: ", categories[modifyIndex].name);
    while (getValidatedInteger(&categories[modifyIndex].value, NULL, 0) == 0) {
        printf("Invalid input. Please enter a numeric value: ");
    }

    scaleValues(categories, values, *numCategories);

    sortCategories(categories,values, *numCategories, *sortOption);

    drawChart(categories,values, *numCategories, title, xAxisLabel);
}

// Function to change chart title [Option 5]
void changeTitle(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel) {
	printf("Enter the new title of the bar chart: ");
	while (scanf(" %100[^\n]", title) != 1)
	{
		printf("Invalid input. Please enter a title with 100 characters or less: ");
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	scaleValues(categories, values, *numCategories);

	int sortOption = 0; // Assume sorting option is decided here or passed in some way
	sortCategories(categories,values, *numCategories, sortOption);

	drawChart(categories,values, *numCategories, title, xAxisLabel);
}

// Function to change x-axis label [Option 6]
void changeXLabel(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel) {
	printf("Enter the new x-axis label: ");
	while (scanf(" %100[^\n]", xAxisLabel) != 1)
	{
		printf("Invalid input. Please enter an x-axis label with 100 characters or less: ");
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	scaleValues(categories, values, *numCategories);

	int sortOption = 0; // Assume sorting option is decided here or passed in some way
	sortCategories(categories,values, *numCategories, sortOption);

	drawChart(categories,values, *numCategories, title, xAxisLabel);
}

