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
#include <stdarg.h>
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
void saveChartToFile(const char* filename, const Category categories[], const Scaled values[], int numCategories, const char* title, const char* xAxisLabel);
void cancelData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void addData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void changeCategoryName(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void changeCategoryValue(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);
void changeTitle(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel);
void changeXLabel(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel);
int getValidatedInteger(int* output, const int validNumbers[], int validCount);
void clearInputBuffer();
void clearInputBufferfile();
int isValidWindowsFilename(char *filename);
bool parseInteger(const char *input, int *number);
bool isValidNumber(int number, const int validNumbers[], int validCount);
void generateChartWithNaturalLanguage(const char* instruction, Category categories[], int* numCategories, char* title, char* xAxisLabel);
void parseApiResponse(const char* response, Category categories[], int* numCategories, char* title, char* xAxisLabel);
size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);
void removeTrailingWeirdCharacters(char* str);
void enableANSIProcessing();
void my_printf(const char* format, ...);
void readBarChartFromFile(const char* filename, Category categories[], int* numCategories, char* title, char* xAxisLabel, int* sortOption);

int main()
{
	#define printf my_printf
	enableANSIProcessing(); // Enable ANSI color processing for Windows
	int exitProgram = 0; // Flag to control the loop

	// Variables to store user inputs
	Category categories[12];
	Scaled values[12];
	int numCategories = 0;
	char title[101], xAxisLabel[101];
	int sortOption;

	// initialize long string
	const char* welcomeMessage =
		"\n"
		"\x1b[34m   _____  \x1b[35m ____                _____ _                _      _____                           _             _ \n"
		"\x1b[34m  / ____| \x1b[35m|  _ \\              / ____| |              | |    / ____|                         | |           | |\n"
		"\x1b[34m | |      \x1b[35m| |_) | __ _ _ __  | |    | |__   __ _ _ __| |_  | |  __  ___ _ __   ___ _ __ __ _| |_ ___  _ __| |\n"
		"\x1b[34m | |      \x1b[35m|  _ < / _` | '__| | |    | '_ \\ / _` | '__| __| | | |_ |/ _ \\ '_ \\ / _ \\ '__/ _` | __/ _ \\| '__| |\n"
		"\x1b[34m | |____  \x1b[35m| |_) | (_| | |    | |____| | | | (_| | |  | |_  | |__| |  __/ | | |  __/ | | (_| | || (_) | |  |_|\n"
		"\x1b[34m  \\_____| \x1b[35m|____/ \\__,_|_|     \\_____|_| |_|\\__,_|_|   \\__|  \\_____|\\___|_| |_|\\___|_|  \\__,_|\\__\\___/|_|  (_)\n"
		"                                                                                                             \n"
		"                                                                                                             \n"
		"";
	

	printf("%s", welcomeMessage);


 // Prompt the user for a file to read
    char filename[101];
    printf("Do you have a file to read? (y/n): ");
    char choice;
    while (scanf(" %c", &choice) != 1 || (choice != 'y' && choice != 'n')) {
        printf("Invalid input. Please enter 'y' or 'n': ");
        clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
    }
    clearInputBuffer();

    if (choice == 'y') {
        printf("Enter the file path: ");
        scanf("%100s", filename);
        clearInputBuffer();

        // Read data from the file
        readBarChartFromFile(filename, categories, &numCategories, title, xAxisLabel, &sortOption);
    } else {

	printf("Would you like to generate a chart using natural language? (y/n): ");
	char choice;
	while (scanf(" %c", &choice) != 1 || (choice != 'y' && choice != 'n'))	//loops if input is not 'y' or 'n'
	{
		printf("Invalid input. Please enter 'y' or 'n': ");
		clearInputBuffer();
	}
	clearInputBuffer();

	if (choice == 'y')
	{
		char instruction[1024];
		char message[4096];
		const char* longString =
			"I'm about to give you some instructions for creating a horizontal bar chart. There are only 3 different attributes of the table: title, x-axis, and rows. Do not reply with extra text like \"Here's the JSON output for your instruction:\" and do not surround the output in code blocks like ```json ```. You are only to return the output in pure JSON format. If you are unsure about a specific value in the json, just make up something for the json.\n"
			"for example, if the instruction is: \"Create a graph for a b c given the values are 100,200,300\" you will only reply something like the following:\n"
			"{\"create\":{\"title\": \"Bar chart of a, b, c\", \"x-axis\": \"values\", \"rows\": [{\"name\": \"a\", \"value\": 100},{\"name\": \"b\", \"value\": 200},{\"name\": \"c\", \"value\": 300}]}}\n"
			"\n"
			"If the instruction is: \"rank top 3 real universities in singapore and their power level in numbers, give me exact names and exact power level. if you don't know just make it up\" you will return something like the following:\n"
			"{\"create\": {\"title\": \"Top 3 Universities in Singapore and Their Power Levels\",\"x-axis\": \"Power Level\",\"rows\": [{\"name\": \"National University of Singapore (NUS)\", \"value\": 95},{\"name\": \"Nanyang Technological University (NTU)\", \"value\": 92},{\"name\": \"Singapore Management University (SMU)\", \"value\": 88}]}}\n"
			"\n"
			"If the instruction is: \"electricity price in singapore in years 2015-2020\" you will return something like the following:\n"
			"{\"create\": {\"title\": \"Electricity Price in Singapore (2015-2020)\", \"x-axis\": \"Cents per kWh\", \"rows\": [{\"name\": \"2015\", \"value\": 26},{\"name\": \"2016\", \"value\": 23},{\"name\": \"2017\", \"value\": 24},{\"name\": \"2018\", \"value\": 26},{\"name\": \"2019\", \"value\": 27},{\"name\": \"2020\", \"value\": 26}]}}\n"
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
		generateChartWithNaturalLanguage(message, categories, &numCategories, title, xAxisLabel);
	}
	else
	{
		// Collect user input manually
		getInput(categories, &numCategories, title, xAxisLabel, &sortOption);
		// Sort categories based on user's choice
		sortCategories(categories, values, numCategories, sortOption);
	}
	}
	// Optionally scale values to fit the chart
	scaleValues(categories, values, numCategories);

	

	// Draw the chart
	drawChart(categories,values, numCategories, title, xAxisLabel);
	
	while (!exitProgram)
	{
		// Ask if the user wants to save the chart and proceed if so
		int options;
		printf("What would you like to do with the chart? Choose an option below:\n1.Save the chart.\n2.Modify the chart.\n3.Use natural language to modify the chart.\n4.Exit Program\n");
		while (getValidatedInteger(&options, (int[]){ 1, 2, 3, 4 }, 4) == 0) {	//loops if input is not 1,2,3 or 4
			printf("Invalid input. Please enter 1, 2, 3 or 4: ");
		}
		if (options == 1)	//option to save chart
		{
			char filename[101];
			printf("Enter filename to save: ");
			while (scanf(" %100[^\n]", filename) != 1 || !isValidWindowsFilename(filename))	//loops if number of characters is more than 100
			{
				printf("Invalid input. Please enter less than 100 characters: ");
				clearInputBuffer();
			}
			clearInputBuffer();

			saveChartToFile(filename, categories, values, numCategories, title, xAxisLabel);
		}
		else if (options == 2)	//option to modify chart
		{
			int modifyOptions = 0;
			printf("Choose an option to modify the chart. Input an option from below:\n1. Remove data\n2. Add data\n3. Modify a category name\n4. Modify a category value\n5. Change chart title\n6. Change x-axis label\n7. Re-sort Chart\n8. Exit Program\n");
			while (getValidatedInteger(&modifyOptions, (int[]){ 1, 2, 3, 4, 5, 6, 7, 8}, 8) == 0) { //loops if input is not 1,2,3,4,5,6,7 or 8
				printf("Invalid input. Please enter 1, 2, 3, 4, 5, 6 ,7 or 8: ");
				clearInputBuffer();
			}

			if (modifyOptions == 1)
			{
				cancelData(categories, values, &numCategories, title, xAxisLabel, &sortOption);	// Function to remove a category [Option 1]
			}
			else if (modifyOptions == 2)
			{
				addData(categories, values, &numCategories, title, xAxisLabel,&sortOption);	// Function to add a category [Option 2]
            }
            else if (modifyOptions == 3) {
                changeCategoryName(categories, values, &numCategories, title, xAxisLabel, &sortOption);	// Function to modify a category name [Option 3]
            }
            else if (modifyOptions == 4) {
                changeCategoryValue(categories, values, &numCategories, title, xAxisLabel, &sortOption);	// Function to modify a category value [Option 4]
            }
			else if (modifyOptions == 5) {
				changeTitle(categories, values, &numCategories, title, xAxisLabel);	// Function to change chart title [Option 5]
			}
			else if (modifyOptions == 6)
			{
				changeXLabel(categories, values, &numCategories, title, xAxisLabel);	// Function to change x-axis label [Option 6]
			}
			else if (modifyOptions == 7)
			{

				printf("Choose new sorting method : Sort by name (0) or by bar length (1)? ");
						while (getValidatedInteger(&sortOption, (int[]){0, 1}, 2) == 0)	//loops if input is not 0 or 1
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
		else if (options == 3) {	//option to use natural langauge to modify the chart
			char instruction[1024];
			char message[4096];
			const char* longString =
				"I'm about to give you some instructions for changing the parameters of a bar chart. There are only 3 different attributes of the table: title, x-axis, and row. Do not reply with extra text like \"Here's the JSON output for your instruction:\" and do not surround the output in code blocks. I want you to return the output in pure JSON format. \n"
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
				"{\"change\":{\"x-axis\": \"good night punpun\", \"row\": {\"name\": \"morning punpun\", \"changed_value\": 6586}}}\n"
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

			// int sortOption = 0; // Assume sorting option is decided here or passed in some way
			// sortCategories(categories,values, numCategories, sortOption); // Sort categories based on user's choice

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

void str_trim(char* str) {
	int start = 0;
	while (isspace(str[start])) {
		start++;
	}

	int end = strlen(str) - 1;
	while (end > start && isspace(str[end])) {
		end--;
	}

	str[end + 1] = '\0';
	if (end > start) {
		memmove(str, str + start, end - start + 2);
	}
}

void my_printf(const char* format, ...) {
	va_list args;

	// Print the ANSI color reset code directly to stdout to avoid recursion
	fputs(ANSI_COLOR_RESET, stdout);

	va_start(args, format);
	vprintf(format, args); // Use vprintf to print the original message with variadic arguments
	va_end(args);

	// Print the ANSI color yellow code directly to stdout to avoid recursion
	fputs(ANSI_COLOR_YELLOW, stdout);
}


void enableANSIProcessing() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) {
		return;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) {
		return;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode)) {
		// Handle the error if needed
	}
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

// Helper function to find the index of a substring in a string
int find_substring(const char* str, const char* substr) {
	char* pos = strstr(str, substr);
	if (pos == NULL)
		return -1;
	return (int)(pos - str);
}

// Parse JSON from the input string and update the string in place
void parse_and_replace_json(char* input_string) {
	// Find the beginning of the JSON block
	int start_index = find_substring(input_string, "```json");
	if (start_index == -1) {
		printf("JSON block not found.\n");
		return;
	}
	start_index += 6; // Move past "```json"

	// Find the end of the JSON block
	int end_index = find_substring(input_string + start_index, "```");
	if (end_index == -1) {
		printf("End of JSON block not found.\n");
		return;
	}

	// Adjust the end index to consider the start of JSON block
	end_index += start_index;

	// Extract JSON string
	int json_length = end_index - start_index;
	char* json_string = (char*)malloc(json_length + 1);
	strncpy(json_string, input_string + start_index, json_length);
	json_string[json_length] = '\0';

	// Process the parsed JSON data (printing in this example)
	printf("Parsed JSON data:\n%s\n", json_string);

	// Example: Modify the parsed JSON (in this case, simply append "Modified" to it)
	strcat(json_string, " Modified");

	// Replace the original JSON block with the modified JSON data
	strcpy(input_string + start_index, json_string);

	free(json_string);
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

	// Parse the JSON block from the content string
	// parse_and_replace_json(content->valuestring);

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
						strncpy(categories[i].name, changed_name->valuestring, 15);
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
				// only copy the first 15 characters of the name into the categories array
				strncpy(categories[*numCategories].name, name->valuestring, 15);
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

	// check if it's a create action
	cJSON* create = cJSON_GetObjectItemCaseSensitive(instruction, "create");
	if (create) {
		cJSON* titleItem = cJSON_GetObjectItemCaseSensitive(create, "title");
		if (titleItem && cJSON_IsString(titleItem)) {
			// Make sure not to write more characters than the array can hold and set the null terminator.
			size_t max_size = 100; // Reserving space for the null terminator
			strncpy(title, titleItem->valuestring, max_size);
			title[max_size] = '\0'; // Explicitly set the null terminator
		}
		cJSON* xAxisItem = cJSON_GetObjectItemCaseSensitive(create, "x-axis");
		if (xAxisItem && cJSON_IsString(xAxisItem)) {
			// Make sure not to write more characters than the array can hold and set the null terminator.
			size_t max_size = 100; // Reserving space for the null terminator
			strncpy(xAxisLabel, xAxisItem->valuestring, max_size);
			xAxisLabel[max_size] = '\0'; // Explicitly set the null terminator
		}
		cJSON* rows = cJSON_GetObjectItemCaseSensitive(create, "rows");
		if (rows) {
			*numCategories = cJSON_GetArraySize(rows);
			for (int i = 0; i < *numCategories; i++) {
				cJSON* row = cJSON_GetArrayItem(rows, i);
				cJSON* name = cJSON_GetObjectItemCaseSensitive(row, "name");
				cJSON* value = cJSON_GetObjectItemCaseSensitive(row, "value");
				if (name && value) {
					strncpy(categories[i].name, name->valuestring, 15);
					// add null terminator to prevent buffer overflow
					categories[i].name[15] = '\0';
					categories[i].value = value->valueint;
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

int isUniqueCategoryName(Category categories[], int index)
{
	for (int i = 0; i < index; i++)
	{
		if (strcmp(categories[i].name, categories[index].name) == 0)
		{
			return 0; // Not unique
		}
	}
	return 1; // Unique
}

// Implementations of getInput, scaleValues, sortCategories, drawChart, and saveChartToFile follow...
void getInput(Category categories[], int *numCategories, char *title, char *xAxisLabel, int *sortOption)
{
	// Enter bar chart title
	printf("Enter the title of the bar chart: ");
	while (scanf(" %100[^\n]", title) != 1) //Check if input is within 100 characters
	{
		printf("Invalid input. Please enter a title with 100 characters or less: ");
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	// Enter x-axis label value
	printf("Enter the x-axis label: ");
	while (scanf(" %100[^\n]", xAxisLabel) != 1)
	{
		printf("Invalid input. Please enter an x-axis label with 100 characters or less: "); //Check if input is within 100 characters
		clearInputBuffer(); // Assuming clearInputBuffer is uncommented and available
	}
	clearInputBuffer();

	// Enter number of categories
	printf("How many categories (max 12)? ");
	while (getValidatedInteger(numCategories, (int[]){1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, 12) == 0) // Check if category value is an integer within 1 to 12
	{
		printf("Invalid input. Please enter a number between 1 and 12: ");
	}

	for (int i = 0; i < *numCategories; i++)
	{
		printf("Enter category %d name: ", i + 1);
		while (scanf(" %15[^\n]", categories[i].name) != 1)
		{
			printf("Invalid input. Please enter a category name with 15 characters or less: "); // Check if category name is within 15 characters
			clearInputBuffer(); // Clears the buffer to remove invalid input
		}
		while (isUniqueCategoryName(categories, i) != 1)
		{
			printf("Category name must be unique. Please enter a different name: ");
			while (scanf(" %15[^\n]", categories[i].name) != 1)
			{
				printf("Invalid input. Please enter a category name with 15 characters or less: ");
				clearInputBuffer();
			}
		}
		// remove leading and trailing whitespace
		str_trim(categories[i].name);

		clearInputBuffer();

		// Enter category value
		printf("Enter %s value: ", categories[i].name);
		while (getValidatedInteger(&categories[i].value, NULL, 0) == 0) //Check if category value is an integer and is not 0
		{
			printf("Invalid input. Please enter an integer value for %s: ", categories[i].name);
		}
	}

	// Sort by name or by bar length
	printf("Sort by name (0) or by bar length (1)? ");
	while (getValidatedInteger(sortOption, (int[]){0, 1}, 2) == 0) //Check if the Sort Option is either 0 or 1
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
			max = categories[i].value; // Store the highest value obtain from all categories into max
	}

	// Always scale values to fit into the max display size of 80
	for (int i = 0; i < numCategories; i++)
	{
		strcpy(values[i].name, categories[i].name); // Copy category name
		if (max != 0) // Avoid division by zero if max is 0
			values[i].value = (categories[i].value * 80) / max; // Scaled value obtained
		else
			values[i].value = 0; // If max is 0, set scaled value to 0
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
	int width = 120; // Max width for bars
	printf(ANSI_COLOR_BLUE "%*s\n\n", width / 2 + (int)strlen(title) / 2, title); // Center the title

	int max_bar_length = 0;
	int axis_values = 0;
	for (int i = 0; i < numCategories; i++) {
		printf(ANSI_COLOR_YELLOW "%-16s |", values[i].name); // Print category name
		if (values[i].value > max_bar_length) {
			axis_values = categories[i].value;
			max_bar_length = values[i].value;
		}
		for (int j = 0; j < values[i].value; j++) {
			printf(ANSI_COLOR_GREEN "X" ANSI_COLOR_RESET); // Print bar
			Sleep(5); // Delay for 5 milliseconds to animate
		}
		printf("\n%17s|\n", "                ");
	}
	// for the x-axis
	for (int k = 0; k <= max_bar_length; k++) {
		if (k == 0) {
			printf("%17s+", "                ");
		}
		else if (k == max_bar_length / 2 || k == max_bar_length) {
			printf(ANSI_COLOR_MAGENTA "+" ANSI_COLOR_RESET);
		}
		else {
			printf(ANSI_COLOR_RED "-" ANSI_COLOR_RESET);
		}
		Sleep(5); // Delay for 5 milliseconds to animate
	}
	// count number of digits in axis_values
	int axis_length = 0;
	int temp = axis_values;
	while (temp != 0) {
		temp /= 10;
		axis_length++;
	}
	int half_axis_length = 0;
	int temp2 = axis_values / 2;
	while (temp2 != 0) {
		temp2 /= 10;
		half_axis_length++;
	}
	for (int n = 0; n <= max_bar_length; n++) {

		if (n == 0) {
			printf(ANSI_COLOR_CYAN "\n%17s0", "                ");
		}
		else if (n == ((max_bar_length / 2) - (half_axis_length/2))) {
			printf(ANSI_COLOR_CYAN "%d", axis_values / 2);
		}
		else if (n == max_bar_length - (axis_length/2) - half_axis_length + 1) {
			printf(ANSI_COLOR_CYAN "%d", axis_values);
		}
		else {
			printf(" ");
		}
	}
	printf(ANSI_COLOR_BLUE "\n%*s\n\n" ANSI_COLOR_RESET, width / 2 + (int)strlen(xAxisLabel) / 2, xAxisLabel);
}

void saveChartToFile(const char* filename, const Category categories[], const Scaled values[], int numCategories, const char* title, const char* xAxisLabel) {
	FILE* file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error opening file!\n");
		return;
	}

	int width = 120; // Max width for bars
	fprintf(file, "%*s\n\n", width / 2 + (int)strlen(title) / 2, title); // Center the title

	int max_bar_length = 0;
	int axis_values = 0;
	for (int i = 0; i < numCategories; i++) {
		fprintf(file, "%-16s |", values[i].name); // Print category name
		if (values[i].value > max_bar_length) {
			axis_values = categories[i].value;
			max_bar_length = values[i].value;
		}
		for (int j = 0; j < values[i].value; j++) {
			fprintf(file, "X"); // Print bar
		}
		fprintf(file, "\n%17s|\n", "                ");
	}
	// for the x-axis
	for (int k = 0; k <= max_bar_length; k++) {
		if (k == 0) {
			fprintf(file, "%17s+", "                ");
		}
		else if (k == max_bar_length / 2 || k == max_bar_length) {
			fprintf(file, "+");
		}
		else {
			fprintf(file, "-");
		}
	}
	// count number of digits in axis_values
	int axis_length = 0;
	int temp = axis_values;
	while (temp != 0) {
		temp /= 10;
		axis_length++;
	}
	for (int n = 0; n <= max_bar_length; n++) {
		if (n == 0) {
			fprintf(file, "\n%17s0", "                ");
		}
		else if (n == ((max_bar_length / 2) - (axis_length / 2))) {
			fprintf(file, "%d", axis_values / 2);
		}
		else if (n == max_bar_length - axis_length) {
			fprintf(file, "%d", axis_values);
		}
		else {
			fprintf(file, " ");
		}
	}
	fprintf(file, "\n%*s\n\n", width / 2 + (int)strlen(xAxisLabel) / 2, xAxisLabel);

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

    // Confirmation prompt
    int confirm = 0;
    while (1)
    {
        printf("Are you sure you want to remove category %d? (1 for yes, 2 for no): ", choice);
        if (getValidatedInteger(&confirm, (int[]){1, 2}, 2) == 1)
        {
            break; // Valid input
        }
        else
        {
            printf("Invalid choice. Please enter 1 for yes or 2 for no.\n");
        }
    }

    // If the user confirms the removal, proceed with the removal
    if (confirm == 1)
    {
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
        
    }
    else
    {
        printf("Operation cancelled. No data was removed.\n");
    }
	drawChart(categories,values, *numCategories, title, xAxisLabel);
}

// Assuming the definition of Category and Scaled structures
// and other relevant functions (scaleValues, sortCategories, drawChart) are available

// Function to add a category [Option 2]
void addData(Category categories[], Scaled values[], int* numCategories, char* title, char* xAxisLabel, int* sortOption)
{
    if (*numCategories == MAX_CATEGORIES) {
        printf("Maximum number of categories reached (%d).\n", MAX_CATEGORIES);
        drawChart(categories,values, *numCategories, title, xAxisLabel); // Redraw the chart
        return;
    }

    char newCategoryName[16]; // Buffer to hold the new category name
    bool isDuplicate = false; // Flag to check for duplicate names

    do {
        printf("Enter the name of the new category: ");
        while (scanf(" %15[^\n]", newCategoryName) != 1) {
			printf("Invalid input. Please enter a category name with 15 characters or less: ");
			clearInputBuffer(); // Clear the input buffer
		}
		// trim leading and trailing whitespace
		str_trim(newCategoryName);

        clearInputBuffer(); // Clear the input buffer

        isDuplicate=false; // Reset the flag to false for each new input
        // Check for duplicate names
        for (int i = 0; i < *numCategories; i++) {
            if (strcmp(categories[i].name, newCategoryName) == 0) {
                printf("Category name already exists. Please enter a unique name.\n");
                isDuplicate = true; // Set the flag to true if a duplicate is found
                break;
            }
        }
    } while (isDuplicate); // Loop until a unique name is entered

    printf("Enter the value of the new category (numeric only): ");
    while (getValidatedInteger(&categories[*numCategories].value, NULL, 0) == 0) {
        printf("Invalid input. Please enter a numeric value: ");
    }

    // Confirmation prompt
    int confirm = 0;
    while (1)
    {
        printf("Are you sure you want to add category %s with value %d? (1 for yes, 2 for no): ", newCategoryName, categories[*numCategories].value);
        if (getValidatedInteger(&confirm, (int[]){1, 2}, 2) == 1)
        {
            break; // Valid input
        }
        else
        {
            printf("Invalid choice. Please enter 1 for yes or 2 for no.\n");
        }
    }

    // If the user confirms the addition, proceed with the addition
    if (confirm == 1)
    {
        // Copy the new category name to the categories array
        strcpy(categories[*numCategories].name, newCategoryName, 15);

        (*numCategories)++; // Update the number of categories

        scaleValues(categories, values, *numCategories); // Optionally scale values to fit the chart

        sortCategories(categories,values, *numCategories, *sortOption); // Sort categories based on user's choice
    }
    else
    {
        printf("Operation cancelled. No data was added.\n");
    }

    // Redraw the chart regardless of whether data was added or not
    drawChart(categories,values, *numCategories, title, xAxisLabel);
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

	// trim leading and trailing whitespace
	str_trim(categories[modifyIndex].name);

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
	while (scanf(" %100[^\n]", title) != 1)	//loops if number of characters is more than 100
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
	while (scanf(" %100[^\n]", xAxisLabel) != 1)	//loops if number of characters is more than 100
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
//function to read the file 
void readBarChartFromFile(const char* filename, Category categories[], int* numCategories, char* title, char* xAxisLabel, int* sortOption) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file! Please ensure the file path is correct and the file is accessible.\n");
        exit(EXIT_FAILURE); // Exit the program if the file cannot be opened
    }

    // Read the title
    if (fscanf(file, "Title of bar chart : %[^\n]", title) != 1) {
        printf("Error reading title. Please ensure the title is a string.\n");
        fclose(file);
        exit(EXIT_FAILURE); // Exit the program if the title is not a string
    }
    fgetc(file); // Consume the newline character

    // Read the x-axis label
    if (fscanf(file, "X-axis label : %[^\n]", xAxisLabel) != 1) {
        printf("Error reading x-axis label. Please ensure the label is a string.\n");
        fclose(file);
        exit(EXIT_FAILURE); // Exit the program if the x-axis label is not a string
    }
    fgetc(file); // Consume the newline character

    // Read the number of categories
    if (fscanf(file, "number of categories(s) :%d", numCategories) != 1) {
        printf("Error reading number of categories. Please ensure it is an integer.\n");
        fclose(file);
        exit(EXIT_FAILURE); // Exit the program if the number of categories is not an integer
    }
    fgetc(file); // Consume the newline character
	
// Check if the number of categories exceeds the limit
    if (*numCategories > 12) {
        printf("Error: The number of categories exceeds the limit of 12.\n");
        fclose(file);
        exit(EXIT_FAILURE); // Exit the program if the number of categories is more than 12
    }
    // Read each category's name and value
    for (int i = 0; i < *numCategories; i++) {
        char tempName[16]; // Temporary buffer to hold the category name
        if (fscanf(file, "%15[^\n]", tempName) != 1) {
            printf("Error reading category name. Please ensure it is a string.\n");
            fclose(file);
            exit(EXIT_FAILURE); // Exit the program if the category name is not a string
        }
        clearInputBufferfile(file); // Clear the input buffer for the file

        // Truncate the category name if it's longer than 15 characters
        tempName[15] = '\0'; // Ensure null-termination
		// trim leading and trailing whitespace
		str_trim(tempName);
		
        strncpy(categories[i].name, tempName, sizeof(categories[i].name));
        
        // Read the category value
        if (fscanf(file, "%d", &categories[i].value) != 1) {
            printf("Error reading category value. Please ensure it is an integer.\n");
            fclose(file);
            exit(EXIT_FAILURE); // Exit the program if the category value is not an integer
        }
        fgetc(file); // Consume the newline character
    }

    // Read the sort option
    if (fscanf(file, "sort option (0) for by name (1) for by bar length: %d", sortOption) != 1) {
        printf("Error reading sort option. Please ensure it is an integer.\n");
        fclose(file);
        exit(EXIT_FAILURE); // Exit the program if the sort option is not an integer
    }
    fgetc(file); // Consume the newline character

    fclose(file);
}

void clearInputBufferfile(FILE* file) {
    int ch;
    while ((ch = fgetc(file)) != '\n' && ch != EOF);
}