// It should be run after server file

// To Run this code follow the commands written below:
// __________________________________________________
// gcc client.c -o client
// ./client 127.0.0.1 6200
// __________________________________________________
// 6200 can be replaced with any valid port number in the range of [1024, 65353]

// But port number should be same as that of server


// Complete flow is explained in readme.txt file


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

void error(char *error_message)
{
    perror(error_message);
    exit(1);
}

// Validating Command
int validate_command(char *buffer)
{
    int q1, q2, q3, q4, q5, q6, q7, q8;
    q1 = strcmp("/user\n", buffer);
    q2 = strcmp("/files\n", buffer);
    q3 = strncmp("/upload", buffer, 7);
    q4 = strncmp("/download", buffer, 9);
    q5 = strncmp("/invite", buffer, 7);
    q6 = strncmp("/read", buffer, 5);
    q7 = strncmp("/insert", buffer, 7);
    q8 = strncmp("/delete", buffer, 7);
    if(q1==0 || q2==0 || q3==0 || q4==0 || q5==0 || q6==0 || q7==0 || q8==0)
    {
        if(q1==0)
            return 1;
        else if(q2==0)
            return 2;
        else if(q3==0)
        {
            if(buffer[7] == ' ')
            {
                int i = strlen(buffer);
                int m = strncmp(".txt", buffer+i-5, 4);
                if(m == 0)
                    return 3;
                else
                    return 0;
            }
            else
                return 0;
        }
        else if(q4==0)
        {
            if(buffer[9] == ' ')
            {
                int i = strlen(buffer);
                int m = strncmp(".txt", buffer+i-5, 4);
                if(m == 0)
                    return 4;
                else
                    return 0;
            }
            else
                return 0;
        }
        else if(q5==0)
        {
            if(buffer[7] == ' ')
            {
                int i;
                for(i=8; i<strlen(buffer); i++)
                {
                    if(buffer[i] == ' ')
                        break;
                }
                int m = strncmp(".txt", buffer+i-4, 4);
                if(m == 0)
                {
                    if(buffer[i+6] == ' ')
                    {
                        for(int j=i+1; j<i+6; j++)
                        {
                            if((int)(buffer[j]) >=48 && (int)(buffer[j])<=57)
                                continue;
                            else
                                return 0;
                        }
                        if(buffer[i+7] == 'E' || buffer[i+7] == 'V')
                        {
                            if(strlen(buffer) == i+9)
                                return 5;
                            else
                                return 0;
                        }
                        else 
                            return 0;
                    }
                    else
                        return 0;
                }
                else
                    return 0;
            }
            else
                return 0;
        }
        else if(q6==0)
        {
            if(buffer[5] == ' ')
            {
                int i;
                for(i=6; i<strlen(buffer); i++)
                {
                    if(buffer[i] == ' ' || buffer[i] == '\n')
                        break;
                }
                
                int m = strncmp(".txt", buffer+i-4, 4);
                if(m==0)
                {
                    if(i+1 == strlen(buffer))
                        return 6;
                    int j;
                    if(buffer[i+1] == '-')
                    {
                        for(j=i+2; j<strlen(buffer); j++)
                        {
                            if(buffer[j] == ' ' || buffer[j] == '\n')
                                break;
                            else
                            {
                                if((int)(buffer[j]) >=48 && (int)(buffer[j])<=57)
                                    continue;
                                else
                                    return 0;
                            }
                        }
                        if(j+1 == strlen(buffer))
                            return 6;
                    }
                    else
                    {
                        for(j=i+1; j<strlen(buffer); j++)
                        {
                            if(buffer[j] == ' ' || buffer[j] == '\n')
                                break;
                            else
                            {
                                if((int)(buffer[j]) >=48 && (int)(buffer[j])<=57)
                                    continue;
                                else
                                    return 0;
                            }
                        }
                        if(j+1 == strlen(buffer))
                            return 6;
                    }
                    if(buffer[j+1] == '-')
                    {
                        int k;
                        for(k=j+2; k<strlen(buffer); k++)
                        {
                            if(buffer[k] == '\n')
                                break;
                            else
                            {
                                if((int)(buffer[k]) >=48 && (int)(buffer[k])<=57)
                                    continue;
                                else
                                    return 0;
                            }
                        }
                        if(k+1 == strlen(buffer))
                            return 6;
                    }
                    else
                    {
                        int k;
                        for(k=j+1; k<strlen(buffer); k++)
                        {
                            if(buffer[k] == '\n')
                                break;
                            else
                            {
                                if((int)(buffer[k]) >=48 && (int)(buffer[k])<=57)
                                    continue;
                                else
                                    return 0;
                            }
                        }
                        if(k+1 == strlen(buffer))
                            return 6;
                    }
                }
                else
                    return 0;
            }
            else
                return 0;
        }
        else if(q7==0)
        {
            if(buffer[7] == ' ')
            {
                int k;
                for(k=8; k<strlen(buffer); k++)
                {
                    if(buffer[k] == ' ')
                        break;
                }
                if(!(strncmp(buffer+k-4, ".txt", 4) == 0))
                {
                    return 0;
                }
                if(buffer[k+1] == '"')
                {
                    if(buffer[strlen(buffer)-2] == '"')
                    {
                        return 7;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else if(buffer[k+1] == '-')
                {
                    int j;
                    for(j=k+2; j<strlen(buffer); j++)
                    {
                        if(buffer[j] == ' ')
                            break;
                        if((int)(buffer[j]) >=48 && (int)(buffer[j])<=57)
                            continue;
                        else
                            return 0;
                    }
                    if(buffer[j+1] == '"')
                    {
                        if(buffer[strlen(buffer)-2] == '"')
                            return 7;
                        else
                            return 0;
                    }
                    else
                        return 0;
                }
                else
                {
                    int j;
                    for(j=k+1; j<strlen(buffer); j++)
                    {
                        if(buffer[j] == ' ')
                            break;
                        if((int)(buffer[j]) >=48 && (int)(buffer[j])<=57)
                            continue;
                        else
                            return 0;
                    }
                    if(buffer[j+1] == '"')
                    {
                            
                        if(buffer[strlen(buffer)-2] == '"')
                            return 7;
                        else
                            return 0;
                    }
                    else
                        return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else if(q8==0)
        {
            if(buffer[7] == ' ')
            {
                int i;
                for(i=8; i<strlen(buffer); i++)
                {
                    if(buffer[i] == ' ' || buffer[i] == '\n')
                        break;
                }
                
                int m = strncmp(".txt", buffer+i-4, 4);
                if(m==0)
                {
                    if(i+1 == strlen(buffer))
                        return 8;
                    int j;
                    if(buffer[i+1] == '-')
                    {
                        for(j=i+2; j<strlen(buffer); j++)
                        {
                            if(buffer[j] == ' ' || buffer[j] == '\n')
                                break;
                            else
                            {
                                if((int)(buffer[j]) >=48 && (int)(buffer[j])<=57)
                                    continue;
                                else
                                    return 0;
                            }
                        }
                        if(j+1 == strlen(buffer))
                            return 8;
                    }
                    else
                    {
                        for(j=i+1; j<strlen(buffer); j++)
                        {
                            if(buffer[j] == ' ' || buffer[j] == '\n')
                                break;
                            else
                            {
                                if((int)(buffer[j]) >=48 && (int)(buffer[j])<=57)
                                    continue;
                                else
                                    return 0;
                            }
                        }
                        if(j+1 == strlen(buffer))
                            return 8;
                    }
                    if(buffer[j+1] == '-')
                    {
                        int k;
                        for(k=j+2; k<strlen(buffer); k++)
                        {
                            if(buffer[k] == '\n')
                                break;
                            else
                            {
                                if((int)(buffer[k]) >=48 && (int)(buffer[k])<=57)
                                    continue;
                                else
                                    return 0;
                            }
                        }
                        if(k+1 == strlen(buffer))
                            return 8;
                    }
                    else
                    {
                        int k;
                        for(k=j+1; k<strlen(buffer); k++)
                        {
                            if(buffer[k] == '\n')
                                break;
                            else
                            {
                                if((int)(buffer[k]) >=48 && (int)(buffer[k])<=57)
                                    continue;
                                else
                                    return 0;
                            }
                        }
                        if(k+1 == strlen(buffer))
                            return 8;
                    }
                }
                else
                    return 0;
            }
            else
                return 0;
            return 0;
        }
    }
    else
    {
        return 0;
    }
    return 0;
}

// Counting Number of lines in a file
int NLINEX(char *file_name)
{
    int ans = 0;
    FILE *file_pointer;
    char c;

    // File is open with read only access
    file_pointer = fopen(file_name, "r");
    
    if (file_pointer == NULL)
        return -1;
    
    int flag = 0;
    // Prasing the whole file character by character and do increment, when newline character '\n' is found
    for(c = getc(file_pointer); c != EOF; c = getc(file_pointer))
    {
        if (c == '\n')
            ans++;
        flag = 1;
    }
        
    
    fclose(file_pointer);
    if(flag == 0)
        return ans;
    ans++;
    return ans;
}

int main(int argc, char const *argv[])
{
    int socket_file_descriptor, port_number;
    int n;
    char buffer[10005];
    char timepass[1000];
    struct sockaddr_in server_address;
    struct hostent *server;

    if(argc < 3)
    {
        // 3 parameter are passed from the command line: file name, derver ip address and port number
        // if argc is less than 3, then one of them is absent
        fprintf(stderr, "Programm is Terminated, Input is not complete!! \n");
        exit(0);
    }

    port_number = atoi(argv[2]);
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    // Error generated while opening the socket
    if(socket_file_descriptor < 0)
        error("There is an error in opening of Socket!! \n");
    
    server = gethostbyname(argv[1]);
    
    if(server == NULL)
        fprintf(stderr, "No Server Available or No Host with such name is available!! \n");

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    // Connection stablishment
    if(connect(socket_file_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        error("Connection is Failed!! \n");

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bzero(buffer, 10005);

    // Read Welcome message

    n = read(socket_file_descriptor, buffer, 10005);
    // Error came in the read command
    if(n < 0)
        error("Error while Reading!! \n");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////      
    printf("Server: %s", buffer);
    
    int nm = strcmp("There are already 5 clients under the server, Hence your request to join is denied!! \n", buffer);
    if(nm==0)
    {
        close(socket_file_descriptor);
        return 0;
    }

    bzero(buffer, 10005);

    while(1)
    {
        bzero(buffer, 10005);
// ---------------------------------------------------------------------------------------------------------------
        // Read of invite status
        n = read(socket_file_descriptor, buffer, 10005);
        // Error came in the read command
        if(n < 0)
            error("Error while Reading!!\n");
        // printf("%s\n", buffer);
        if(strcmp(buffer, "Yes") == 0)
        {
            printf("\n--------------------------------------------------------------------------\n\n");
            printf("You got an Invite!!\n");
            printf("\n--------------------------------------------------------------------------\n\n");
        }
        
        bzero(buffer, 10005);
// --------------------------------------------------------------------------------------------------------------
        printf("\n##########################################################################\n\n");
        printf("Please Enter the Command:\t");
        
        fgets(buffer, 10005, stdin);
        
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Write the command on terminal
        n = write(socket_file_descriptor, buffer, strlen(buffer));
        // Error came in the write command
        if(n < 0)
            error("Error while Writing!! \n");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        int m = strncmp("/exit", buffer, 5);
        if(m==0)
            break;
        
        int v = validate_command(buffer);

        bzero(timepass, 1000);
// ---------------------------------------------------------------------------------------------------------------
        // Read ack of recieving command
        n = read(socket_file_descriptor, timepass, 1000);
        // Error came in the read command
        if(n < 0)
            error("Error while Reading!!\n");
        bzero(timepass, 1000);
// --------------------------------------------------------------------------------------------------------------
        char num[10];
        bzero(num, 10);
        sprintf(num, "%d", v);

// -----------------------------------------------------------------------------------------------------------------
        // Writing Validation Status
        n = write(socket_file_descriptor, num, 10);
        // Error came in the write command
        if(n < 0)
            error("Error while Writing!! \n");
//----------------------------------------------------------------------------------------------------------------
        bzero(num, 10);

        if(v==0)
        {
            printf("Invalid Command!!\n");
            continue;
        }
        else
        {
            if(v!=0)
            {
                // User command
                if(v==1)
                {
                    bzero(buffer, 10005);
                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the output
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    //////////////////////////////////////////////////////////////////////////////
                    printf("%s",buffer);
                    bzero(buffer, 10005);
                }
                // Files Command
                else if(v==2)
                {
                    bzero(buffer, 10005);
                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the output
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    //////////////////////////////////////////////////////////////////////////////
                    printf("%s",buffer);
                    bzero(buffer, 10005);
                }
                // upload command
                else if(v==3)
                {
                    char file_name_upload[50];
                    strncpy(file_name_upload, buffer+8, strlen(buffer)-9);

                    bzero(buffer, 10005);

                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the Error or timepass
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    
                    if(strcmp(buffer, "Error: A file with same name has already been uploaded!!\n") == 0)
                    {
                        printf("%s",buffer);
                        bzero(buffer, 10005);
                        continue;
                    }
                    //////////////////////////////////////////////////////////////////////////////

                    int no_lines = NLINEX(file_name_upload);

                    bzero(num, 10);
                    sprintf(num, "%d", no_lines);

                    // -----------------------------------------------------------------------------------------------------------------
                    // Writing no. of lines
                    n = write(socket_file_descriptor, num, 10);
                    // Error came in the write command
                    if(n < 0)
                        error("Error while Writing!! \n");
                    
                    //-------------------------------------------------------------------------------------------------------------------
                    bzero(num, 10);
                    bzero(buffer, 10005);
                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the Error or timepass
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    //////////////////////////////////////////////////////////////////////////////
                    
                    bzero(num, 10);
                    bzero(buffer, 10005);
                    FILE *file_pointer;
                    file_pointer = fopen(file_name_upload, "r");
                    if (file_pointer == NULL)
                    {
                        bzero(buffer, 10005);
                        strcpy(buffer, "No");
                        // -----------------------------------------------------------------------------------------------------------------
                        // Writing no. of lines
                        n = write(socket_file_descriptor, buffer, 10005);
                        // Error came in the write command
                        if(n < 0)
                            error("Error while Writing!! \n");
                        
                        //-------------------------------------------------------------------------------------------------------------------
                        bzero(buffer, 10005);
                        //////////////////////////////////////////////////////////////////////////////
                        // Reading the Error or timepass
                        n = read(socket_file_descriptor, buffer, 10005);
                        // Error came in the read command
                        if(n < 0)
                            error("Error while Reading!!\n");
                        //////////////////////////////////////////////////////////////////////////////
                        bzero(buffer, 10005);
                        printf("File Not Found!! \n");
                        continue;
                        // return 0;
                    }

                    bzero(buffer, 10005);
                    strcpy(buffer, "Yes");
                    // -----------------------------------------------------------------------------------------------------------------
                    // Writing no. of lines
                    n = write(socket_file_descriptor, buffer, 10005);
                    // Error came in the write command
                    if(n < 0)
                        error("Error while Writing!! \n");
                        
                    //-------------------------------------------------------------------------------------------------------------------
                    bzero(buffer, 10005);
                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the Error or timepass
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    //////////////////////////////////////////////////////////////////////////////
                    bzero(buffer, 10005);

                    char c;
                    for(c = getc(file_pointer); c != EOF; c = getc(file_pointer))
                    {
                        size_t len = strlen(buffer);
                        char *str = malloc(len + 2);

                        strcpy(str, buffer);
                        str[len] = c;
                        str[len + 1] = '\0';
                        
                        strcpy(buffer, str);
                        free(str);
                        
                        if(c == '\n')
                        {
                            // -----------------------------------------------------------------------------------------------------------------
                            // Writing line
                            n = write(socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            //-------------------------------------------------------------------------------------------------------------------
                            bzero(buffer, 10005);
                            
                            //////////////////////////////////////////////////////////////////////////////
                            // Reading the timepass
                            n = read(socket_file_descriptor, buffer, 10005);
                            // Error came in the read command
                            if(n < 0)
                                error("Error while Reading!!\n");
                            //////////////////////////////////////////////////////////////////////////////
                            
                            bzero(buffer, 10005);
                            
                        }
                    }
                    
                    // -----------------------------------------------------------------------------------------------------------------
                    // Writing line
                    n = write(socket_file_descriptor, buffer, 10005);
                    // Error came in the write command
                    if(n < 0)
                        error("Error while Writing!! \n");
                    //-------------------------------------------------------------------------------------------------------------------
                    
                    bzero(buffer, 10005);
                    
                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the timepass
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    
                    //////////////////////////////////////////////////////////////////////////////
                    bzero(buffer, 10005);
                    // // -----------------------------------------------------------------------------------------------------------------

                    fclose(file_pointer);
                    
                    printf("File Uploaded Successfully!!\n");
                    bzero(buffer, 10005);
                }
                // download command
                else if(v==4)
                {
                    char file_name_download[50];
                    bzero(file_name_download, 50);
                    strncpy(file_name_download, buffer+10, strlen(buffer)-11);

                    bzero(buffer, 10005);
                    ////////////////////////////////////////////////////////////////////
                    // Read no of lines or Error
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    ///////////////////////////////////////////////////////////////////

                    if((strcmp(buffer, "Error: You don't have access to downlaod the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                    {
                        printf("%s", buffer);
                        bzero(buffer, 10005);
                        continue;
                    }

                    int no_of_lines = atoi(buffer);
                    //////////////////////////////////////////////////////////////////////////////////////
                    // Write timepass
                    bzero(buffer, 10005);
                    strcpy(buffer, "Ok");
                    n = write(socket_file_descriptor, buffer, 10005);
                    // Error came in the write command
                    if(n < 0)
                        error("Error while Writing!! \n");
                    bzero(buffer, 10005);
                    //////////////////////////////////////////////////////////////////////////////////////////

                    FILE *file_pointer_w = fopen("server_file_new.txt", "w");
                    if (file_pointer_w == NULL)
                    {
                        printf("\nFile Not Found!! \n");
                        return 0;
                    }

                    for(int ind=0; ind < no_of_lines; ind++)
                    {
                        bzero(buffer, 10005);
                        //////////////////////////////////////////////////////////////////////////////
                        // Reading the timepass
                        n = read(socket_file_descriptor, buffer, 10005);
                        // Error came in the read command
                        if(n < 0)
                            error("Error while Reading!!\n");
                        //////////////////////////////////////////////////////////////////////////////
                        fputs(buffer, file_pointer_w);
                        bzero(buffer, 10005);
                        strcpy(buffer, "Ok");
                        
                        // -----------------------------------------------------------------------------------------------------------------
                        // Writing line
                        n = write(socket_file_descriptor, buffer, 10005);
                        // Error came in the write command
                        if(n < 0)
                            error("Error while Writing!! \n");
                        //-------------------------------------------------------------------------------------------------------------------
                        bzero(buffer, 10005);
                    }
                    bzero(buffer, 10005);
                    fclose(file_pointer_w);

                    FILE *file_r, *file_w;
                    file_r = fopen("server_file_new.txt", "r");
                    if (file_r == NULL)
                    {
                        printf("\nError while Executing!! \n");
                        return 0;
                    }
                    file_w = fopen(file_name_download, "w");
                    if (file_w == NULL)
                    {
                        printf("\nError while Executing!! \n");
                        return 0;
                    }
                    char c = fgetc(file_r);
                    while (c != EOF)
                    {
                        fputc(c, file_w);
                        c = fgetc(file_r);
                    }
                    remove("server_file_new.txt");
                    fclose(file_r);
                    fclose(file_w);

                    bzero(buffer, 10005);
                    // -----------------------------------------------------------------------------------------------------------------
                    // Read Final Statement
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the write command
                    if(n < 0)
                        error("Error while Writing!! \n");
                    //-------------------------------------------------------------------------------------------------------------------
                    printf("%s",buffer);
                    bzero(buffer, 10005);
                }
                // Invite command
                else if(v==5)
                {
                    bzero(buffer, 10005);
                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the output
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    //////////////////////////////////////////////////////////////////////////////
                    printf("%s",buffer);
                    bzero(buffer, 10005);
                }
                // read command
                else if(v==6)
                {
                    int lines_read;
                    bzero(buffer, 10005);
                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the output
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    //////////////////////////////////////////////////////////////////////////////
                    
                    if((strcmp(buffer, "Invalid Index!!\n") == 0) || (strcmp(buffer, "Error: You don't have access to read the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                    {
                        printf("%s", buffer);
                        bzero(buffer, 10005);
                        strcpy(buffer, "Ok");
                        // -----------------------------------------------------------------------------------------------------------------
                        // Writing line
                        n = write(socket_file_descriptor, buffer, 10005);
                        // Error came in the write command
                        if(n < 0)
                            error("Error while Writing!! \n");
                        //-------------------------------------------------------------------------------------------------------------------
                        bzero(buffer, 10005);
                    }
                    else
                    {
                        lines_read = atoi(buffer);
                        bzero(buffer, 10005);
                        strcpy(buffer, "Ok");
                        // -----------------------------------------------------------------------------------------------------------------
                        // Writing line
                        n = write(socket_file_descriptor, buffer, 10005);
                        // Error came in the write command
                        if(n < 0)
                            error("Error while Writing!! \n");
                        //-------------------------------------------------------------------------------------------------------------------
                        bzero(buffer, 10005);

                        for(int ind=0; ind < lines_read; ind++)
                        {
                            bzero(buffer, 10005);
                            //////////////////////////////////////////////////////////////////////////////
                            // Reading the timepass
                            n = read(socket_file_descriptor, buffer, 10005);
                            // Error came in the read command
                            if(n < 0)
                                error("Error while Reading!!\n");
                            //////////////////////////////////////////////////////////////////////////////
                            printf("%s", buffer);
                            bzero(buffer, 10005);
                            strcpy(buffer, "Ok");
                            
                            // -----------------------------------------------------------------------------------------------------------------
                            // Writing line
                            n = write(socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            //-------------------------------------------------------------------------------------------------------------------
                            bzero(buffer, 10005);
                        }
                        bzero(buffer, 10005);
                    }
                }
                // Insert command
                else if(v==7)
                {
                    bzero(buffer, 10005);
                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the output
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    //////////////////////////////////////////////////////////////////////////////
                    if(strcmp(buffer, "Message is added Succefully!!\n") == 0)
                    {
                        bzero(buffer, 10005);
                        strcpy(buffer, "Ok");    
                        // -----------------------------------------------------------------------------------------------------------------
                        // Writing timepass
                        n = write(socket_file_descriptor, buffer, 10005);
                        // Error came in the write command
                        if(n < 0)
                            error("Error while Writing!! \n");
                        //-------------------------------------------------------------------------------------------------------------------
                        bzero(buffer, 10005);
                        //////////////////////////////////////////////////////////////////////////////
                        // Reading the line
                        n = read(socket_file_descriptor, buffer, 10005);
                        // Error came in the read command
                        if(n < 0)
                            error("Error while Reading!!\n");
                        //////////////////////////////////////////////////////////////////////////////
                        int lines_read = atoi(buffer);
                        bzero(buffer, 10005);
                        strcpy(buffer, "Ok");
                        // -----------------------------------------------------------------------------------------------------------------
                        // Writing timepass
                        n = write(socket_file_descriptor, buffer, 10005);
                        // Error came in the write command
                        if(n < 0)
                            error("Error while Writing!! \n");
                        //-------------------------------------------------------------------------------------------------------------------
                        bzero(buffer, 10005);

                        for(int ind=0; ind < lines_read; ind++)
                        {
                            bzero(buffer, 10005);
                            //////////////////////////////////////////////////////////////////////////////
                            // Reading the timepass
                            n = read(socket_file_descriptor, buffer, 10005);
                            // Error came in the read command
                            if(n < 0)
                                error("Error while Reading!!\n");
                            //////////////////////////////////////////////////////////////////////////////
                            printf("%s", buffer);
                            bzero(buffer, 10005);
                            strcpy(buffer, "Ok");
                            
                            // -----------------------------------------------------------------------------------------------------------------
                            // Writing line
                            n = write(socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            //-------------------------------------------------------------------------------------------------------------------
                            bzero(buffer, 10005);
                        }
                        bzero(buffer, 10005);
                    }
                    else
                    {
                        printf("%s",buffer);
                    }
                    
                    bzero(buffer, 10005);
                }
                // delete command
                else if(v==8)
                {
                    int lines_del;
                    bzero(buffer, 10005);
                    
                    //////////////////////////////////////////////////////////////////////////////
                    // Reading the output
                    n = read(socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    //////////////////////////////////////////////////////////////////////////////
                    
                    if((strcmp(buffer, "Invalid Index!!\n") == 0) || (strcmp(buffer, "Error: You don't have access to perform delete in the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                    {
                        printf("%s", buffer);
                        bzero(buffer, 10005);
                        strcpy(buffer, "Ok");
                        // -----------------------------------------------------------------------------------------------------------------
                        // Writing timepass
                        n = write(socket_file_descriptor, buffer, 10005);
                        // Error came in the write command
                        if(n < 0)
                            error("Error while Writing!! \n");
                        //-------------------------------------------------------------------------------------------------------------------
                        bzero(buffer, 10005);
                    }
                    else
                    {
                        lines_del = atoi(buffer);
                        bzero(buffer, 10005);
                        strcpy(buffer, "Ok");
                        // -----------------------------------------------------------------------------------------------------------------
                        // Writing timepass
                        n = write(socket_file_descriptor, buffer, 10005);
                        // Error came in the write command
                        if(n < 0)
                            error("Error while Writing!! \n");
                        //-------------------------------------------------------------------------------------------------------------------
                        bzero(buffer, 10005);
                        if(lines_del == 0)
                        {
                            printf("%s\n", buffer);
                            printf("Whole data of the file is deleted!!\n");
                            bzero(buffer, 10005);
                        }
                        else
                        {
                            for(int ind=0; ind < lines_del; ind++)
                            {
                                bzero(buffer, 10005);
                                //////////////////////////////////////////////////////////////////////////////
                                // Reading the lines
                                n = read(socket_file_descriptor, buffer, 10005);
                                // Error came in the read command
                                if(n < 0)
                                    error("Error while Reading!!\n");
                                //////////////////////////////////////////////////////////////////////////////
                                printf("%s", buffer);
                                bzero(buffer, 10005);
                                strcpy(buffer, "Ok");
                                
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing timepass
                                n = write(socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                bzero(buffer, 10005);
                            }
                            bzero(buffer, 10005);
                        }
                        bzero(buffer, 10005);
                    }
                }
            }
        }
        
        bzero(buffer, 10005);
    }
    close(socket_file_descriptor);
    return 0;
}
