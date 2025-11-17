#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>

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
    char *command = strtok(input, " \n");
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
        //Gather the filename path and open in R+ mode
        printf("Enter filename in current directory to open:\n");
        fgets(filename, sizeof(filename), stdin); 
        filename[strcspn(filename, "\n")] = 0;
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
                    strcpy(contents_array[i][j], word);
                    word = strtok(NULL, ",");
                    j++;
                }
                if (i == 0) {
                    num_fields = j;   // header defines number of fields
                }
            }

            
            // num_records--; //exclude header line from record count
            printf("Parsed %d records with %d fields each.\n", num_records, num_fields);
            // printf("first one: %s\n", contents_array[1][0]);
            // printf("Last one: %s\n", contents_array[num_records-1][2]);
            
        }
    }

    else if (strcmp(command, "insert") == 0 && db_file != NULL && contents != NULL){
        char *value = strtok(arg_str, ",");
        printf("DEBUG INSERT: arg_str = '%s'\n", arg_str);
        printf("DEBUG INSERT: num_fields = %d\n", num_fields);
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
                    strcpy(contents_array[num_records][i], value);
                }
            }
            else if (strcmp(type, "str") == 0){
                //validate string input
               

                strcpy(contents_array[num_records][i], value);
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
    
    else if (strcmp(command, "overview") == 0 && db_file != NULL && contents != NULL){
        printf("Current Database Contents:\n");
        for (int r = 1; r < num_records; r++){
            for (int f = 0; f < num_fields; f++){
                printf("%s ", contents_array[r][f]);
            }
            printf("\n");
        }
    }
    else if (strcmp(command, "rowdelete") == 0 && db_file != NULL && contents != NULL){
        //parse row number from arg_str
        int row_to_delete = atoi(arg_str) + 1;
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
        printf("Enter filename for new database:\n");
        char filename[100];
        fgets(filename, sizeof(filename), stdin);
        filename[strcspn(filename, "\n")] = 0; // Remove trailing newline
        db_file = fopen(filename, "w+");
        if (db_file == NULL) {
            printf("Error creating database file: %s\n", filename);
        } else {
            printf("Database file created: %s\n", filename);
        }
        
        printf("Define fields formats (serparated by commas):\n");
        char field_definitions[256];
        fgets(field_definitions, sizeof(field_definitions), stdin);
        field_definitions[strcspn(field_definitions, "\n")] = 0; // Remove trailing newline
        
        printf("Define corresponding field names (serparated by commas):\n");
        char field_names[256];
        fgets(field_names, sizeof(field_names), stdin);
        field_names[strcspn(field_names, "\n")] = 0; // Remove trailing newline

        fprintf(db_file, "%s\n", field_definitions);
        fprintf(db_file, "%s\n", field_names);

        fclose(db_file);
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
        strlwr(input);
        // if (strcmp(input, "exit\n") == 0){
        //     running = false;
        //     printf("Exiting safely, after processing all transactions...\n");
        //     break;
        // }
        // else{
            process_command(input);
        // }
        continue;
    }
    return 0;
}

