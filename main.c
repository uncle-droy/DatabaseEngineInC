#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

// Define maximum length of input line
#define MAX_LINE_LENGTH 256
// Define maximum number of words
#define MAX_WORDS 20
// Define maximum length of a single word (optional, but good practice)
#define MAX_WORD_LENGTH 50


bool running = true;

//this function processes user commands and will be called multiple times as the user wishes
void process_command(char* input){
    
    //declare variables to store all file related data
    //using static to preserve values between function calls
    static char filename[100];
    static FILE* db_file = NULL;
    static char *contents;
    static int num_records = 0;
    static int num_fields = 0;
    static char contents_array[1000][1000][100];
    static char lines[1000][1000];

    //separate main command keyword from rest of input
    char *command = strlwr(strtok(input, " \n"));
    char *arg_str = strtok(NULL, "\n");   // everything after command


    //open command logic
    if (strcmp(command, "open") == 0){
        if (contents) {
            free(contents);
            contents = NULL;
        }
        if (db_file) {
            fclose(db_file);
            db_file = NULL;
        }
        num_records = 0;
        num_fields = 0;
        strcpy(filename, arg_str);
        db_file = fopen(filename, "r+");
        //error handling for file open
        if (db_file == NULL){
            printf("Error opening file: %s\n", filename);
            return;
        }
        //If file opened successfully, read contents into memory
        else{
            printf("File %s opened successfully.\n", filename);

            //obtain file size
            fseek(db_file, 0, SEEK_END);

            long fsize = ftell(db_file); // get current file pointer
            fseek(db_file, 0, SEEK_SET); // set file pointer to beginning
            //read entire contents of file into contents variable
            contents = malloc(fsize + 1);
            fread(contents, 1, fsize, db_file);
            contents[fsize] = 0;
            // printf("File contents:\n%s\n", contents);

            //parse contents into 2D array, ie, separate by lines
            char *line;
            line = strtok(contents, "\n");
            while (line != NULL){
                strcpy(lines[num_records], line);
                num_records++;
                line = strtok(NULL, "\n");
            }


            //parse each line into fields, separated by commas
            //parse each line into fields, separated by commas
            char *word;
            for (int i = 0; i < num_records; i++){
                word = strtok(lines[i], ",");
                int j = 0;
                while (word != NULL){
                    strcpy(contents_array[i][j], word); //fill contents array
                    word = strtok(NULL, ",");
                    j++;
                }
                if (i == 0) {
                    num_fields = j;   // header defines number of fields
                }
            }
            
            // num_records--; //exclude header line from record count
            printf("Parsed %d records with %d fields each.\n", num_records, num_fields);
            
        }
    }

    else if (strcmp(command, "create") == 0){
        printf("Processing CREATE command...\n");
        if (contents) {
            free(contents);
            contents = NULL;
        }
        if (db_file) {
            fclose(db_file);
            db_file = NULL;
        }
        // Add logic for CREATE command
        printf("Enter filename for new database:");
        char filename[100];
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = 0; // Remove trailing newline
        db_file = fopen(filename, "w+");
        if (db_file == NULL) {
            printf("Error creating database file: %s\n", filename);
        } else {
            printf("Database file created: %s\n", filename);
        }
        
        printf("Define corresponding field names (serparated by commas):\n");
        char field_names[256];
        fgets(field_names, sizeof(field_names), stdin);
        field_names[strcspn(field_names, "\n")] = 0; // Remove trailing newline
        
        printf("Define fields types (serparated by commas):\n");
        char field_definitions[256];
        fgets(field_definitions, sizeof(field_definitions), stdin);
        field_definitions[strcspn(field_definitions, "\n")] = 0; // Remove trailing newline
        
        printf("Define lengths for each field (serparated by commas):\n");
        char field_lengths[256];
        fgets(field_lengths, sizeof(field_lengths), stdin);
        field_lengths[strcspn(field_lengths, "\n")] = 0; // Remove trailing newline

        fprintf(db_file, "%s\n", field_definitions);
        fprintf(db_file, "%s\n", field_lengths);
        fprintf(db_file, "%s\n", field_names);

        fclose(db_file);
        printf("Database schema written to file successfully.\nOpen the database using the 'open' command to start adding records.\n");
    }


    else if (strcmp(command, "insert") == 0 && db_file != NULL && contents != NULL){
        char *value = strtok(arg_str, ",");
        // printf("DEBUG INSERT: arg_str = '%s'\n", arg_str);
        // printf("DEBUG INSERT: num_fields = %d\n", num_fields);
        int insert_successful = 1;
        for (int i = 0; i < num_fields; i++){
            
            if (value == NULL) {
            printf("%s", value);
            printf("Not enough values provided for INSERT command.\n");
            break;
            }
    
            char field_def_copy[100];
            strcpy(field_def_copy, contents_array[0][i]);
            
            char type[10]; // Buffer to hold the extracted type
            // sscanf reads the first string token into 'type'
            if (sscanf(field_def_copy, "%9s", type) != 1) { 
                printf("Error parsing field definition.\n");
                insert_successful = 0;
                break;
            }
            // --- End of Fix ---

            while (*value == ' ') value++; // Trim leading spaces
            
            if (strcmp(type, "int") == 0){
                //validate integer input
                // value = strtok(value, " "); //remove leading/trailing spaces
                char* endptr;
                long val;
                val = strtol(value, &endptr, 10);
                if (*endptr != '\0' || endptr == value || (errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (val > INT_MAX || val < INT_MIN)){
                    printf("Invalid integer input: %s\n", value);
                    break;
                }
                else{
                    if (strlen(value) <= strtol(contents_array[1][i], NULL, 10)){
                        strcpy(contents_array[num_records][i], value);
                        printf("Length of limit: %lu and length of input:%lu.\n", strtol(contents_array[1][i], NULL, 10),strlen(value));
                    }
                    else{
                        printf("Input length exceeds defined length: %lu\n", strtol(contents_array[1][i], NULL, 10));
                        break;
                    }
                }
            }
            
            else if (strcmp(type, "str") == 0){
                //validate string input
                if (strlen(value) <= strtol(contents_array[1][i], NULL, 10)){
                    strcpy(contents_array[num_records][i], value);
                    printf("Length of limit: %lu and length of input:%lu.\n", strtol(contents_array[1][i], NULL, 10),strlen(value));
                }
                else{
                    printf("Input length exceeds defined length: %lu\n", strtol(contents_array[1][i], NULL, 10));
                    break;
                }
                

            }

            else if (strcmp(type, "bool") == 0){
                //validate boolean input
                if (strcmp(value, "true") != 0 && strcmp(value, "false") != 0){
                    printf("Invalid boolean input: %s\n", value);
                    break;
                }
                else{
                    strcpy(contents_array[num_records][i], value);
                }
            }
            else{
                printf("Unknown field type: %s\n", contents_array[0][i]);
                break;
            }
            
            if (i == num_fields - 1){
                num_records++;
            
            }
            value = strtok(NULL, ",");  
        }
        
    }
    
    else if (strcmp(command, "rowdelete") == 0 && db_file != NULL && contents != NULL){
        //parse row number from arg_str
        int row_to_delete = atoi(arg_str) + 2;
        if (row_to_delete <= 0 || row_to_delete >= num_records){
            printf("Invalid row number: %d\n", row_to_delete);
            return;
        }
        //shift all rows after the deleted row up by one
        for (int r = row_to_delete; r < num_records - 1; r++){
            for (int f = 0; f < num_fields; f++){
                strcpy(contents_array[r][f], contents_array[r + 1][f]);
            }
        }
        num_records--;
        printf("Row %d deleted successfully.\n", row_to_delete+1);
    }
    
    else if (strcmp(command, "fielddelete") == 0 && db_file != NULL && contents != NULL){
        //parse field number from arg_str
        int field_to_delete = atoi(arg_str);
        if (field_to_delete <= 0 || field_to_delete > num_fields){
            printf("Invalid field number: %d\n", field_to_delete);
            return;
        }
        //shift all fields after the deleted field left by one
        for (int r = 0; r < num_records; r++){
            for (int f = field_to_delete - 1; f < num_fields - 1; f++){
                strcpy(contents_array[r][f], contents_array[r][f + 1]);
            }
        }
        num_fields--;
        printf("Field %d deleted successfully.\n", field_to_delete);
    }

    else if (strcmp(command, "length") == 0 && db_file != NULL && contents != NULL){
        //parse field name from arg_str
        char *field_name = strtok(arg_str, " ");
        int field_index = 0;
        char *field_size = strtok(NULL, " \n");
        
        while (strcmp(field_name, contents_array[2][field_index]) != 0) {
            if (field_index<=num_fields -1){
                field_index++;
            }
            else{
                printf("Field %s not found in database.\n", field_name);
                return;
            }
        }
        strcpy(contents_array[1][field_index], field_size);
        printf("Field %s length updated to %s successfully.\n", field_name, field_size);
        
    }

    else if (strcmp(command,"edit") ==0 && db_file != NULL && contents != NULL){
        //parse row number and field name from arg_str
        char *row_str = strtok(arg_str, " ");
        char *field_str = strtok(NULL, " ");
        char *new_value = strtok(NULL, "\n");
        int user_row_index = atoi(row_str);
        int array_row_index = user_row_index + 2;

        if (user_row_index <= 0 || user_row_index > num_records - 2){
            printf("Invalid row number: %d\n", user_row_index);
            return;
        }
        
        int i = 0;
        printf("Processing EDIT command for Row: %d, Field: %s...\n", user_row_index, field_str);
        while (strcmp(field_str, contents_array[2][i]) != 0) {
            printf("Checking field %s at index %d\n", contents_array[1][i], i);
            if (i<=num_fields -1){
                i++;
            }
            else{
                printf("Field %s not found in database.\n", field_str);
                return;
            }
        }
        
        //Check data type of field to validate new value
        char field_def_copy[100];
        strcpy(field_def_copy, contents_array[0][i]);
        char type[10]; 
        if (sscanf(field_def_copy, "%9s", type) != 1) { 
             printf("Error parsing field definition.\n");
             return;
        }
        if (strcmp(type, "int") == 0){
            //validate integer input
            char* endptr;
            long val;
            val = strtol(new_value, &endptr, 10);
            if (*endptr != '\0' || endptr == new_value || (errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (val > INT_MAX || val < INT_MIN)){
                printf("Invalid integer input: %s\n", new_value);
                return;
            }
        }
        else if (strcmp(type, "str") == 0){
            //validate string input
            if (strlen(new_value) > strtol(contents_array[1][i], NULL, 10)){
                printf("Input length exceeds defined length: %lu\n", strtol(contents_array[1][i], NULL, 10));
                return;
            }
        }
        else if (strcmp(type, "bool") == 0){
            //validate boolean input
            if (strcmp(new_value, "true") != 0 && strcmp(new_value, "false") != 0){
                printf("Invalid boolean input: %s\n", new_value);
                return;
            }
        }
        //update value in contents_array
        strcpy(contents_array[array_row_index][i], new_value);
        printf("Row %d, Field %s updated successfully.\n", user_row_index, field_str);
    }


    
    
    else if (strcmp(command, "rename") == 0){
        // printf("Enter old field name: ");
        char old_field_name[100];
        // fgets(old_field_name, sizeof(old_field_name), stdin);
        // old_field_name[strcspn(old_field_name, "\n")] = 0;
        // printf("Enter new field name: ");
        char new_field_name[100];
        // fgets(new_field_name, sizeof(new_field_name), stdin);
        // new_field_name[strcspn(new_field_name, "\n")] = 0;
        char *old_field_name_arg = strtok(arg_str, " ");
        char *new_field_name_arg = strtok(NULL, "\n");
        strcpy(old_field_name, old_field_name_arg);
        strcpy(new_field_name, new_field_name_arg);
        int i = 0;
        while (strcmp(old_field_name, contents_array[2][i]) != 0) {
            printf("Checking field %s at index %d\n", contents_array[2][i], i);
            if (i<=num_fields -1){
                i++;
            }
            else{
                printf("Field %s not found in database.\n", old_field_name);
                return;
            }
        }
        strcpy(contents_array[2][i], new_field_name);
        printf("Field name '%s' renamed to '%s' successfully.\n", old_field_name, new_field_name);
    }

    else if (strcmp(command, "addfield") == 0){
        //Command to add new field to database
        printf("Enter new field definition (name): ");
        char field_def[100];
        fgets(field_def, sizeof(field_def), stdin);
        field_def[strcspn(field_def, "\n")] = 0;
        
        printf("Enter field type (int/str/bool): ");
        char field_type[10];
        fgets(field_type, sizeof(field_type), stdin);
        field_type[strcspn(field_type, "\n")] = 0;
        
        printf("Enter field length: ");
        char field_length[10];
        fgets(field_length, sizeof(field_length), stdin);
        field_length[strcspn(field_length, "\n")] = 0;

        printf("Adding new field: %s\n", field_def);
        //Update contents_array to add new field
        if (strcmp(field_type, "int") == 0 || strcmp(field_type, "str") == 0 ){
            strcpy(contents_array[0][num_fields], field_type);
        }
        else if(strcmp(field_type, "bool") == 0){
            strcpy(contents_array[0][num_fields], "4");
        }
        else{
            printf("Invalid field type: %s\n", field_type);
            return;
        }
        strcpy(contents_array[1][num_fields], field_length);
        strcpy(contents_array[2][num_fields], field_def);
        char def[10] = "";
        if (strcmp(field_type, "str") == 0){
            strcpy(def, "NULL"); //default empty string for string fields
        }
        else if (strcmp(field_type, "bool") == 0){
            strcpy(def, "false"); //default false for boolean fields
        }
        else if (strcmp(field_type, "int") == 0){
            strcpy(def, "0"); //default 0 for integer fields
        }

        for (int r = 3; r < num_records; r++){
            strcpy(contents_array[r][num_fields], def); //initialize new field to empty string
        }
        //default values of all fields in existing records to empty string
        num_fields++; // Increment the number of fields
    }

    else if (strcmp(command, "overview") == 0 && db_file != NULL && contents != NULL){
        printf("Current Database Contents:\n");
        for (int r = 2; r < num_records; r++){
            for (int f = 0; f < num_fields; f++){
                printf("%-*s ", strtol(contents_array[1][f], NULL, 10) + 3, contents_array[r][f]);
            }
            printf("\n");
        }
    }
    

    else if (strcmp(command, "select") == 0){ //select particular number of rows range and particular fields names and show the data
        char field_names_input[2048];
        char field_names_array[20][128];
        int cur_field_index = 0;
        int field_indexes[20];
        int mapped_field_index = 0;
        
        printf("Enter name of fields to select (comma-separated): ");
        fgets(field_names_input, sizeof(field_names_input), stdin);
        strtok(field_names_input, " \n"); // Remove trailing newline
        char *field = strtok(field_names_input, " ,");
        
        while (field != NULL){
            printf("Selected field: %s\n", field);
            for(int i=0;i<num_fields;i++){
                if(strcmp(field,contents_array[2][i])==0){
                    field_indexes[mapped_field_index]=i;
                    mapped_field_index++;
                    strcpy(field_names_array[cur_field_index], field);
                    cur_field_index++;
                    printf("Mapping field %s to index %d\n", field, i);
                    break;
                }
                else{
                    continue;
                }
            }
            field = strtok(NULL, " ,");
        }

        printf("Enter row range to select (start end): ");
        int row_start_index;
        int row_end_index;
        scanf("%d %d", &row_start_index, &row_end_index);
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        //Write column headers
        for (int i = 0; i < mapped_field_index; i++){
            printf("%-*s ", strtol(contents_array[1][field_indexes[i]], NULL, 10) + 3, field_names_array[i]);
        }
        printf("\n");
        //Write data rows
        for (int r = row_start_index + 2; r <= row_end_index + 2; r++){
            for (int f = 0; f < mapped_field_index; f++){
                printf("%-*s ", strtol(contents_array[1][field_indexes[f]], NULL, 10) + 3, contents_array[r][field_indexes[f]]);
            }
            printf("\n");
        }
    }



    else if (strcmp(command,"commit") ==0){
        if (db_file == NULL || contents == NULL){
            printf("No database is currently open. Use the 'open' command first.\n");
            return;
        }
        //rewind file pointer to beginning
        fclose(db_file);
        db_file = fopen(filename, "w+"); //clear file contents
        
        //write all records back to file
        for (int r = 0; r < num_records; r++){
            for (int f = 0; f < num_fields; f++){
                fprintf(db_file, "%s", contents_array[r][f]);
                if (f < num_fields - 1){
                    fprintf(db_file, ",");
                }
            }
            if(r!=num_records - 1)fprintf(db_file, "\n");
        }
        fflush(db_file); //ensure all data is written to disk
        
        printf("All changes committed to database file successfully.\n");
    }


    else if (strcmp(command, "help") == 0){
        printf("Available commands:\n");
        printf("open - Open an existing database file\n");
        printf("create - Create a new database file\n");
        printf("insert <values> - Insert a new record with comma-separated values\n");
        printf("overview - Display all records in the current database\n");
        printf("rename - Rename a specific field\n");
        printf("addfield - Add a new field to the database\n");
        printf("length <field_name> <new_length> - Change the length of a specific field\n");
        printf("select - Select specific fields and rows to display\n");
        printf("rowdelete <row_number> - Delete a specific row by its number\n");
        printf("fielddelete <field_number> - Delete a specific field by its number\n");
        printf("edit <row_number> <field_name> <new_value> - Edit data in a specific field in a specific row\n");
        printf("commit - Save all changes to the database file\n");
        printf("exit - Exit the database engine\n");
    }

    else if (strcmp(command, "exit") == 0){
        running = false;
        fclose(db_file);
        free(contents);
        printf("Exiting safely, after processing all transactions...\n");
    }
    else{
        printf("Unknown command: %s\n", command);
    }
    
}

int main(){
    char input[256];
    printf("Welcome to our DB engine!\n");
    while (running){
        printf("> ");
        fgets(input, sizeof(input), stdin);
        process_command(input);
        continue;
    }
    return 0;
}