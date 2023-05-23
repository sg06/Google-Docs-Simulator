// First run this

// To Run this code follow the commands written below:
// __________________________________________________
// gcc server.c -o server
// ./server 6200
// __________________________________________________
// 6200 can be replaced with any valid port number in the range of [1024, 65353]

// Length Buffer will always be length of given input + 1 (Because we always Press Enter to give input, and it is stored as newline character in buffer)

// Complete flow is explained in readme.txt file


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <limits.h>

// structure stoding the file
struct file
{
	int id;
	char file_name[100];
	char permission[5];
    int number_of_line;
};

void error(char *error_message)
{
    perror(error_message);
    exit(1);
}

// Count number of lines
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

// Main Function
int main(int argc, char const *argv[])
{
    pid_t pid;
    if(argc < 2)
    {
        // 2 parameter are passed from the command line: file name and port number
        // if argc is less than 2, then one of them is absent
        fprintf(stderr, "Programm is Terminated, Input is Not Complete!! \n");
        exit(1);
    }
    int socket_file_descriptor, new_socket_file_descriptor, port_number;
    int n;
    char buffer[10005];
    struct sockaddr_in server_address, client_address;
    socklen_t client_length;

    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    
    // Error generated while opening the socket
    if(socket_file_descriptor < 0)
        error("There is an error in opening of Socket!! \n");
    
    bzero((char *) &server_address, sizeof(server_address));
    port_number = atoi(argv[1]);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    // Error Generated when binding gets failed
    if(bind(socket_file_descriptor, (struct sockaddr *) &server_address, sizeof(server_address))<0)
        error("Binding is failed!! ");

    listen(socket_file_descriptor, 5);
    // 5 is the maximum number of clients that can connect to the server

    client_length = sizeof(client_address);

    // Shared Memory used to count active clients
    int shmid, *count_p, *count_c;
    shmid = shmget( IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT);
    count_p = (int *) shmat( shmid, 0, 0 );
    count_c = (int *) shmat( shmid, 0, 0 );
    count_p[0] = 0;

    // Array of shared memory to store active clients
    int shared_user, *user_p, *user_c;
    shared_user = shmget(IPC_PRIVATE, 5*sizeof(int), 0777|IPC_CREAT);
    user_p = (int *) shmat(shared_user, 0, 0);
    user_c = (int *) shmat(shared_user, 0, 0);

    // Array storing list of invited clients
    int invited_user, *invite_p, *invite_c;
    invited_user = shmget(IPC_PRIVATE, 10*sizeof(int), 0777|IPC_CREAT);
    invite_p = (int *) shmat(invited_user, 0, 0);
    invite_c = (int *) shmat(invited_user, 0, 0);

    // Number of invited clients
    int count_invite, *c_invite_p, *c_invite_c;
    count_invite = shmget(IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT);
    c_invite_p = (int *) shmat(count_invite, 0, 0);
    c_invite_c = (int *) shmat(count_invite, 0, 0);

    // Array of structure of file, storing details about all the files
    struct file *file_c, *file_p;
	int file_shmid;
    file_shmid = shmget(IPC_PRIVATE, 100*sizeof(struct file *), 0777|IPC_CREAT);
    file_c = (struct file *) shmat(file_shmid, 0, 0);
    file_p = (struct file *) shmat(file_shmid, 0, 0);

    // Size of array of file structure
    int no_file_shmid, *file_count_p, *file_count_c;
    no_file_shmid = shmget( IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT);
    file_count_p = (int *) shmat( no_file_shmid, 0, 0 );
    file_count_c = (int *) shmat( no_file_shmid, 0, 0 );
    file_count_p[0] = 0;

    char user[1000];
    char command_type[10];
    
    user_p[0] = INT_MAX;
    user_p[1] = INT_MAX;
    user_p[2] = INT_MAX;
    user_p[3] = INT_MAX;
    user_p[4] = INT_MAX;

    invite_p[0] = INT_MAX;
    invite_p[1] = INT_MAX;
    invite_p[2] = INT_MAX;
    invite_p[3] = INT_MAX;
    invite_p[4] = INT_MAX;
    invite_p[5] = INT_MAX;
    invite_p[6] = INT_MAX;
    invite_p[7] = INT_MAX;
    invite_p[8] = INT_MAX;
    invite_p[9] = INT_MAX;

    c_invite_p[0] = 0;
    
    int t_count = 0;
    int m;
    printf("Welcome, Server is started!!\n");
    while(1)
    {
        new_socket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *) &client_address, &client_length);

        // Error in accepting command
        if(new_socket_file_descriptor < 0)
            error("Error while Accepting!! \n");

        if(count_p[0] >= 5)
        {
        ////////////////////////////////////////////////////////////////////////////////
            //Write Welcome Message
            n = write(new_socket_file_descriptor, "There are already 5 clients under the server, Hence your request to join is denied!! \n", strlen("There are already 5 clients under the server, Hence your request to join is denied!! \n"));
            // Error came in the write command
            if(n < 0)
                error("Error while Writing!! \n");
        /////////////////////////////////////////////////////////////////////////////////
        }
        else
        {
            t_count++;
            char welcome[100];
            strcat(welcome, "Welcome as a Client ");
            char num[20];
            sprintf(num, "%d", t_count);
            strcat(welcome, num);
            strcat(welcome, "!!\n");
            ///////////////////////////////////////////////////////////////////////////
            //Write Welcome Message
            n = write(new_socket_file_descriptor, welcome, strlen(welcome));
            bzero(welcome, 100);
            bzero(num, 20);
            // Error came in the write command
            if(n < 0)
                error("Error while Writing!! \n");
            ///////////////////////////////////////////////////////////////////////////
            count_p[0]++;
            // user_arr[count_p[0]-1] = 10000 + t_count;
            user_p[count_p[0]-1] = 10000 + t_count;
            
            if((pid = fork()) == 0)
            {
                // Server will continue to run, until the last client is out (in this case we have single client)
                while(1)
                {
                    bzero(buffer, 10005);

                    //---------------------------------------------------------------------------------------------------------------------------------
                    if(c_invite_c[0] <= 0)
                        strcpy(buffer, "No");
                    else
                    {
                        for(int ijk=0; ijk<c_invite_c[0]; ijk++)
                        {
                            if(invite_c[ijk] == t_count+10000)
                            {
                                strcpy(buffer, "Yes");
                                invite_c[ijk] = INT_MAX;
                                c_invite_c[0]--;
                                break;
                            }
                            else
                            {
                                strcpy(buffer, "No");
                            }
                        }
                        if(strcmp(buffer, "Yes") == 0)
                        {
                            for(int ijk=0; ijk<10; ijk++)
                            {
                                for(int pqr=ijk+1; pqr<10; pqr++)
                                {
                                    if(invite_c[pqr] < invite_c[ijk])
                                    {
                                        int temp = invite_c[ijk];
                                        invite_c[ijk] = invite_c[pqr];
                                        invite_c[pqr] = temp;
                                    }
                                }
                            }
                        }
                    }
                    n = write(new_socket_file_descriptor, buffer, 10005);
                    // Error came in the write command
                    if(n < 0)
                        error("Error while Writing!! \n");
                    //-----------------------------------------------------------------------------------------------------------------------------------
                    bzero(buffer, 10005);
                    ////////////////////////////////////////////////////////////////////
                    // Read command from the terminal
                    n = read(new_socket_file_descriptor, buffer, 10005);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");
                    ///////////////////////////////////////////////////////////////////
                    printf("Client %d: %s", t_count, buffer);

                    //////////////////////////////////////////////////////////////////////////
                    //////////////////////////////////////////////////////////////////////////
                    /////////////////////////////// exit /////////////////////////////////////
                    //////////////////////////////////////////////////////////////////////////
                    //////////////////////////////////////////////////////////////////////////
                    m = strncmp("/exit", buffer, 5);
                    if(m==0)
                    {
                        for(int i=0; i<5; i++)
                        {
                            if(user_c[i] == 10000+t_count)
                            {
                                user_c[i] = INT_MAX;
                                break;
                            }
                        }
                        for(int i=0; i<5; i++)
                        {
                            for(int j=i+1; j<5; j++)
                            {
                                if(user_c[j] < user_c[i])
                                {
                                    int temp = user_c[i];
                                    user_c[i] = user_c[j];
                                    user_c[j] = temp;
                                }
                            }
                        }
                        for(int i=0; i<file_count_c[0]; i++)
                        {
                            if((file_c[i].id == 10000 + t_count) && (strncmp(file_c[i].permission, "O", 1) == 0))
                            {
                                for(int j=i+1; j<file_count_c[0]; j++)
                                {
                                    if(strcmp(file_c[i].file_name, file_c[j].file_name) == 0)
                                    {
                                        file_c[j].id = INT_MAX;
                                        file_c[j].number_of_line = 0;
                                        bzero(file_c[j].permission, 5);
                                        bzero(file_c[j].file_name, 100);
                                    }
                                }
                                file_c[i].id = INT_MAX;
                                file_c[i].number_of_line = 0;
                                bzero(file_c[i].permission, 5);
                                bzero(file_c[i].file_name, 100);
                            }
                            else if(file_c[i].id == 10000 + t_count)
                            {
                                file_c[i].id = INT_MAX;
                                file_c[i].number_of_line = 0;
                                bzero(file_c[i].permission, 5);
                                bzero(file_c[i].file_name, 100);
                            }
                        }
                        for(int i=0; i<file_count_c[0]; i++)
                        {
                            for(int j=i+1; j<file_count_c[0]; j++)
                            {
                                if(file_c[j].id < file_c[i].id)
                                {
                                    int temp = file_c[i].id;
                                    file_c[i].id = file_c[j].id;
                                    file_c[j].id = temp;

                                    int temp2 = file_c[i].number_of_line;
                                    file_c[i].number_of_line = file_c[j].number_of_line;
                                    file_c[j].number_of_line = temp2;

                                    char temp_arr[100];
                                    strcpy(temp_arr, file_c[i].file_name);
                                    strcpy(file_c[i].file_name, file_c[j].file_name);
                                    strcpy(file_c[j].file_name, temp_arr);

                                    strcpy(temp_arr, file_c[i].permission);
                                    strcpy(file_c[i].permission, file_c[j].permission);
                                    strcpy(file_c[j].permission, temp_arr);
                                }
                            }
                        }
                        int size = file_count_c[0];
                        for(int i=0; i<size; i++)
                        {
                            if(file_c[i].id == INT_MAX)
                            {
                                file_count_c[0]--;
                            }
                        }
                        count_c[0]--;
                        break;
                    }

                    n = write(new_socket_file_descriptor, "Command Read Succesfully!!\n", strlen("Command Read Succesfully!!\n"));
                    // Error came in the write command
                    if(n < 0)
                        error("Error while Writing!! \n");

                    bzero(command_type, 10);

                    n = read(new_socket_file_descriptor, command_type, 10);
                    // Error came in the read command
                    if(n < 0)
                        error("Error while Reading!!\n");

                    if(strncmp(command_type, "0", 1) == 0)
                    {
                        continue;
                    }

                    else
                    {
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        /////////////////////////////// user /////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        if(strncmp(command_type, "1", 1) == 0)
                        {
                            bzero(buffer, 10005);
                            for(int i=0; i<count_p[0]; i++)
                            {
                                bzero(num, 20);
                                sprintf(num, "%d", user_c[i]);
                                strcat(buffer, num);
                                strcat(buffer, "\t");
                            }
                            strcat(buffer, "\n");
                            //////////////////////////////////////////////////////////////////////////////////////
                            // Write final Output
                            n = write(new_socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            //////////////////////////////////////////////////////////////////////////////////////////
                        }
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        /////////////////////////////// files ////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        else if(strncmp(command_type, "2", 1) == 0)
                        {
                            bzero(buffer, 10005);
                            char collab[1000];
                            bzero(collab, 1000);
                            
                            for(int i=0; i<file_count_c[0]; i++)
                            {
                                if(strncmp(file_c[i].permission, "O", 1) == 0)
                                {
                                    strcat(buffer, "Filename: ");
                                    strcat(buffer, file_c[i].file_name);
                                    strcat(buffer, "\nOwner: ");
                                    bzero(num, 20);
                                    sprintf(num, "%d", file_c[i].id);
                                    strcat(buffer, num);
                                    bzero(num, 20);
                                    strcat(buffer, "\n");
                                    strcat(buffer, "Number of Lines: ");
                                    sprintf(num, "%d", file_c[i].number_of_line);
                                    strcat(buffer, num);
                                    bzero(num, 20);
                                    strcat(buffer, "\n");
                                    strcat(buffer, "Collaborators: ");
                                    bzero(collab, 1000);
                                    for(int j=i+1; j<file_count_c[0]; j++)
                                    {
                                        if(strcmp(file_c[i].file_name, file_c[j].file_name) == 0)
                                        {
                                            bzero(num, 20);
                                            sprintf(num, "%d", file_c[j].id);
                                            strcat(collab, num);
                                            bzero(num, 20);
                                            strcat(collab, " - ");
                                            strncat(collab, file_c[j].permission, 1);
                                            strcat(collab, "\t");
                                        }
                                    }
                                    strcat(buffer, collab);
                                    strcat(buffer, "\n");
                                    bzero(collab, 1000);
                                }
                            }
                            if(file_count_c[0] == 0)
                                strcpy(buffer, "Error: No file is uploaded till now!!\n");
                            //////////////////////////////////////////////////////////////////////////////////////
                            // Write final Output
                            n = write(new_socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            //////////////////////////////////////////////////////////////////////////////////////////
                        }
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        ////////////////////////////// upload ////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        else if(strncmp(command_type, "3", 1) == 0)
                        {
                            char file_name_upload[50];
                            bzero(file_name_upload, 50);
                            strncpy(file_name_upload, buffer+8, strlen(buffer)-9);
                            // strcat(file_name_upload, "t");

                            int flag_flag=0;
                            for(int abc=0; abc<file_count_c[0]; abc++)
                            {
                                if(strcmp(file_c[abc].file_name, file_name_upload) == 0)
                                {
                                    flag_flag = 1;
                                    break;
                                }
                            }
                            
                            if(flag_flag == 1)
                            {
                                //////////////////////////////////////////////////////////////////////////////////////
                                // Write Error
                                bzero(buffer, 10005);
                                strcpy(buffer, "Error: A file with same name has already been uploaded!!\n");
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                bzero(buffer, 10005);
                                continue;
                                //////////////////////////////////////////////////////////////////////////////////////////
                            }

                            //////////////////////////////////////////////////////////////////////////////////////
                            // Write timepass
                            bzero(buffer, 10005);
                            strcpy(buffer, "Ok");
                            n = write(new_socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            bzero(buffer, 10005);
                            //////////////////////////////////////////////////////////////////////////////////////////
                            
                            char no_lines[10];
                            bzero(no_lines, 10);
                            ////////////////////////////////////////////////////////////////////
                            // Read number of lines
                            n = read(new_socket_file_descriptor, no_lines, 10);
                            // Error came in the read command
                            if(n < 0)
                                error("Error while Reading!!\n");
                            ///////////////////////////////////////////////////////////////////

                            int no_of_lines = atoi(no_lines);

                            bzero(no_lines, 10);
                            
                            bzero(buffer, 10005);
                            strcpy(buffer, "Ok");
                            // -----------------------------------------------------------------------------------------------------------------
                            // Writing timepass
                            n = write(new_socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            //-------------------------------------------------------------------------------------------------------------------
                            bzero(buffer, 10005);

                            ////////////////////////////////////////////////////////////////////
                            // Read number of lines
                            n = read(new_socket_file_descriptor, buffer, 10005);
                            // Error came in the read command
                            if(n < 0)
                                error("Error while Reading!!\n");
                            ///////////////////////////////////////////////////////////////////

                            if(strcmp(buffer, "No") == 0)
                            {
                                bzero(buffer, 10005);
                                strcpy(buffer, "Ok");
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing timepass
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                bzero(buffer, 10005);
                                
                                continue;
                            }
                            
                            bzero(buffer, 10005);
                            strcpy(buffer, "Ok");
                            // -----------------------------------------------------------------------------------------------------------------
                            // Writing timepass
                            n = write(new_socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            //-------------------------------------------------------------------------------------------------------------------
                            bzero(buffer, 10005);
                            

                            FILE *file_pointer_w = fopen("server_file_new.txt", "w");
                            if (file_pointer_w == NULL)
                            {
                                printf("File Not Found!!\n");
                                return 0;
                            }

                            for(int ind=0; ind < no_of_lines; ind++)
                            {
                                //////////////////////////////////////////////////////////////////////////////
                                // Reading line
                                n = read(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the read command
                                if(n < 0)
                                    error("Error while Reading!!\n");
                                //////////////////////////////////////////////////////////////////////////////
                                fputs(buffer, file_pointer_w);
                                bzero(buffer, 10005);
                                // bzero(buffer, 10005);
                                strcpy(buffer, "Ok");
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing timepass
                                n = write(new_socket_file_descriptor, buffer, 10005);
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
                                printf("Error while Executing!! \n");
                                return 0;
                            }
                            file_w = fopen(file_name_upload, "w");
                            if (file_w == NULL)
                            {
                                printf("Error while Executing!! \n");
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

                            file_c[file_count_c[0]].id = 10000 + t_count;
                            file_c[file_count_c[0]].number_of_line = no_of_lines;
                            strcpy(file_c[file_count_c[0]].permission, "O");
                            strcpy(file_c[file_count_c[0]].file_name, file_name_upload);
                            file_count_c[0]++;

                            bzero(buffer, 10005);
                        }
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////// download ////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        else if(strncmp(command_type, "4", 1) == 0)
                        {
                            char file_name_download[50];
                            bzero(file_name_download, 50);
                            strncpy(file_name_download, buffer+10, strlen(buffer)-11);
                            // strcat(file_name_download, "t");

                            int flaggg = 0;

                            if(file_count_c[0] == 0)
                            {
                                strcpy(buffer, "Error: File not found!!\n");
                            }
                            else
                            {
                                for(int abc=0; abc<file_count_c[0]; abc++)
                                {
                                    if(strcmp(file_c[abc].file_name, file_name_download) == 0)
                                    {
                                        flaggg = 1;
                                    }
                                    if((strcmp(file_c[abc].file_name, file_name_download) == 0) && (file_c[abc].id == (10000 + t_count)))
                                    {
                                        int no_lines = NLINEX(file_name_download);
                                        bzero(buffer, 10005);
                                        sprintf(buffer, "%d", no_lines);
                                        break;
                                    }
                                    else
                                    {
                                        strcpy(buffer, "Error: You don't have access to downlaod the file!!\n");
                                    }
                                }
                            }
                            if(flaggg == 0)
                            {
                                strcpy(buffer, "Error: File not found!!\n");
                            }
                            // -----------------------------------------------------------------------------------------------------------------
                            // Writing no. of lines or error
                            n = write(new_socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            //-------------------------------------------------------------------------------------------------------------------
                            // bzero(num, 20);

                            if((strcmp(buffer, "Error: You don't have access to downlaod the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                            {
                                bzero(buffer, 10005);
                                continue;
                            }
                            else
                            {
                                bzero(buffer, 10005);
                                //////////////////////////////////////////////////////////////////////////////
                                // Reading the timepass
                                n = read(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the read command
                                if(n < 0)
                                    error("Error while Reading!!\n");
                                //////////////////////////////////////////////////////////////////////////////
                                bzero(buffer, 10005);

                                
                                FILE *file_pointer;
                                file_pointer = fopen(file_name_download, "r");
                                if (file_pointer == NULL)
                                {
                                    printf("File Not Found!!\n");
                                    return 0;
                                }
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
                                        n = write(new_socket_file_descriptor, buffer, 10005);
                                        // Error came in the write command
                                        if(n < 0)
                                            error("Error while Writing!! \n");
                                        //-------------------------------------------------------------------------------------------------------------------
                                        bzero(buffer, 10005);
                                        //////////////////////////////////////////////////////////////////////////////
                                        // Reading the timepass
                                        n = read(new_socket_file_descriptor, buffer, 10005);
                                        // Error came in the read command
                                        if(n < 0)
                                            error("Error while Reading!!\n");
                                        //////////////////////////////////////////////////////////////////////////////
                                        bzero(buffer, 10005);
                                    }
                                }
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing line
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                bzero(buffer, 10005);
                                // Reading the timepass
                                n = read(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the read command
                                if(n < 0)
                                    error("Error while Reading!!\n");
                                //////////////////////////////////////////////////////////////////////////////
                                bzero(buffer, 10005);
                                //////////////////////////////////////////////////////////////////////////////
                                // Reading the final output
                                fclose(file_pointer);
                                bzero(buffer, 10005);
                                strcpy(buffer, "File Downloaded Successfully!!\n");
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the read command
                                if(n < 0)
                                    error("Error while Reading!!\n");
                                //////////////////////////////////////////////////////////////////////////////
                                bzero(buffer, 10005);
                            }
                        }
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        ////////////////////////////// invite ////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        else if(strncmp(command_type, "5", 1) == 0)
                        {
                            int ab;
                            for(ab=8; ab<strlen(buffer); ab++)
                            {
                                if(buffer[ab] == ' ')
                                    break;
                            }
                            char file_collab[20];
                            strncpy(file_collab, buffer+8, ab-8);
                            char id_collab[10];
                            strncpy(id_collab, buffer+ab+1, 5);
                            char permission_collab[5];
                            strncpy(permission_collab, buffer+ab+7, 1);
                            int user_id_collab = atoi(id_collab);

                            if(user_id_collab == 10000 + t_count)
                            {
                                strcpy(buffer, "You can not send invitation to yourself!!\n");
                            }
                            else
                            {
                                int flag_invite = 0;
                                int flaggggggg = 0;
                                int xy;
                                
                                for(xy=0; xy<file_count_c[0]; xy++)
                                {
                                    if((strcmp(file_collab, file_c[xy].file_name) == 0) && (strncmp("O", file_c[xy].permission, 1) == 0) && (file_c[xy].id == 10000 + t_count))
                                    {
                                        flag_invite = 1;
                                        break;
                                    }
                                }
                                bzero(buffer, 10005);
                                
                                if(flag_invite == 1)
                                {
                                    for(int abcd=0; abcd<5; abcd++)
                                    {
                                        if(user_c[abcd] == user_id_collab)
                                        {
                                            flaggggggg = 1;
                                        }
                                    }
                                    if(flaggggggg == 1)
                                    {
                                        int hill=0;
                                        for(int abcd=0; abcd<file_count_c[0]; abcd++)
                                        {
                                            if((file_c[abcd].id == user_id_collab) && (strcmp(file_c[abcd].file_name, file_collab)==0))
                                            {
                                                hill = 1;
                                                if(strncmp(file_c[abcd].permission, permission_collab, 1) == 0)
                                                {
                                                    strcpy(buffer, "This client is already been invited with same Permission!!\n");
                                                    break;
                                                }
                                                else
                                                {
                                                    strncpy(file_c[abcd].permission, permission_collab, 1);
                                                    strcpy(buffer, "This client is already been invited, Now its permission is updated!!\n");
                                                    int diss = 0;
                                                    for(int uio=0; uio<c_invite_c[0]; uio++)
                                                    {
                                                        if(user_id_collab == invite_c[uio])
                                                        {
                                                            diss = 1;
                                                            break;
                                                        }
                                                    }
                                                    if(diss == 0)
                                                    {
                                                        invite_c[c_invite_c[0]] = user_id_collab;
                                                        c_invite_c[0]++;
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                        if(hill == 0)
                                        {
                                            file_c[file_count_c[0]].id = user_id_collab;
                                            file_c[file_count_c[0]].number_of_line = file_c[xy].number_of_line;
                                            strncpy(file_c[file_count_c[0]].permission, permission_collab, 1);
                                            strcpy(file_c[file_count_c[0]].file_name, file_collab);
                                            file_count_c[0]++;
                                            strcpy(buffer, "Client Invited Successfully!!\n");
                                            
                                            invite_c[c_invite_c[0]] = user_id_collab;
                                            c_invite_c[0]++;
                                        }
                                        
                                    }
                                    else
                                    {
                                        strcpy(buffer, "You are not allowed to Invite this client (as it is not active)!!\n");
                                    }
                                }
                                else
                                {
                                    strcpy(buffer, "You are not allowed to Invite client for this file!!\n");
                                }
                            }
                            // -----------------------------------------------------------------------------------------------------------------
                            // Writing line
                            n = write(new_socket_file_descriptor, buffer, 10005);
                            // Error came in the write command
                            if(n < 0)
                                error("Error while Writing!! \n");
                            //-------------------------------------------------------------------------------------------------------------------
                            bzero(buffer, 10005);
                            bzero(file_collab, 20);
                            bzero(id_collab, 10);
                            bzero(permission_collab, 5);

                        }
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////  read  ////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        else if(strncmp(command_type, "6", 1) == 0)
                        {
                            char file_read[50];
                            bzero(file_read, 50);
                            char ind[20];
                            int ind1 = 0;
                            int ind2 = 0;

                            int abcd = 0;
                            int count_space = 0;
                            int read_space[5];
                            while(buffer[abcd] != '\n')
                            {
                                if(buffer[abcd] == ' ')
                                {
                                    read_space[count_space] = abcd;
                                    count_space++;
                                }
                                abcd++;
                            }
                            read_space[count_space] = abcd;
                            
                            int flg = count_space - 1;
                            if(count_space == 1)
                            {
                                bzero(file_read, 50);// clear array
                                strncpy(file_read, buffer + read_space[0] + 1, read_space[1]-read_space[0]-1);
                            }

                            else if(count_space == 2)
                            {
                                bzero(file_read, 50);// clear array
                                strncpy(file_read, buffer + read_space[0] + 1, read_space[1]-read_space[0]-1);

                                bzero(ind, 20);
                                strncpy(ind, buffer + read_space[1] + 1, read_space[2]-read_space[1]-1);
                                ind1 = atoi(ind);
                                bzero(ind, 20);
                            }
                            else if(count_space == 3)
                            {
                                bzero(file_read, 50);// clear array
                                strncpy(file_read, buffer + read_space[0] + 1, read_space[1]-read_space[0]-1);

                                bzero(ind, 20);
                                strncpy(ind, buffer+read_space[1]+1, read_space[2]-read_space[1]-1);
                                ind1 = atoi(ind);
                                bzero(ind, 20);

                                strncpy(ind, buffer+read_space[2]+1, read_space[3]-read_space[2]-1);
                                ind2 = atoi(ind);
                                bzero(ind, 20);
                            }

                            int no_of_lines_read;
                            if(flg == 0)
                            {
                                int flagggg = 0;
                                if(file_count_c[0] == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_read) == 0)
                                    {
                                        flagggg = 1;
                                    }
                                    if((strcmp(file_c[i].file_name, file_read) == 0) && (file_c[i].id == (10000 + t_count)))
                                    {
                                        no_of_lines_read = NLINEX(file_read);
                                        bzero(buffer, 10005);
                                        sprintf(buffer, "%d", no_of_lines_read);
                                        break;
                                    }
                                    else
                                    {
                                        strcpy(buffer, "Error: You don't have access to read the file!!\n");
                                    }
                                }
                                if(flagggg == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                
                                if((strcmp(buffer, "Error: You don't have access to read the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    continue;
                                }
                                else
                                {
                                    bzero(buffer, 10005);

                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);

                                    /////////////////////////////////////////////////////////////////////////////////////
                                    
                                    FILE *file_pointer;
                                    file_pointer = fopen(file_read, "r");
                                    if (file_pointer == NULL)
                                    {
                                        printf("File Not Found!!\n");
                                        return 0;
                                    }
                                    char c;
                                    // int count = 0;
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
                                            n = write(new_socket_file_descriptor, buffer, 10005);
                                            // Error came in the write command
                                            if(n < 0)
                                                error("Error while Writing!! \n");
                                            //-------------------------------------------------------------------------------------------------------------------
                                            bzero(buffer, 10005);
                                            //////////////////////////////////////////////////////////////////////////////
                                            // Reading the timepass
                                            n = read(new_socket_file_descriptor, buffer, 10005);
                                            // Error came in the read command
                                            if(n < 0)
                                                error("Error while Reading!!\n");
                                            //////////////////////////////////////////////////////////////////////////////
                                            bzero(buffer, 10005);
                                        }
                                    }
                                    // -----------------------------------------------------------------------------------------------------------------
                                    // Writing line
                                    strcat(buffer, "\n");
                                    n = write(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the write command
                                    if(n < 0)
                                        error("Error while Writing!! \n");
                                    //-------------------------------------------------------------------------------------------------------------------
                                    bzero(buffer, 10005);
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the final output
                                    fclose(file_pointer);

                                    /////////////////////////////////////////////////////////////////////////////////////
                                }
                            }
                            else if(flg == 1)
                            {
                                int flagggg = 0;
                                if(file_count_c[0] == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_read) == 0)
                                    {
                                        flagggg = 1;
                                    }
                                    if((strcmp(file_c[i].file_name, file_read) == 0) && (file_c[i].id == (10000 + t_count)))
                                    {
                                        no_of_lines_read = NLINEX(file_read);
                                        bzero(buffer, 10005);
                                        if(ind1<0)
                                            ind1 = no_of_lines_read + ind1;
                                        if(ind1>=no_of_lines_read || ind1<0)
                                        {
                                            strcpy(buffer, "Invalid Index!!\n");
                                        }
                                        else
                                        {
                                            no_of_lines_read = no_of_lines_read - ind1;
                                            sprintf(buffer, "%d", no_of_lines_read);
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        strcpy(buffer, "Error: You don't have access to read the file!!\n");
                                    }
                                }
                                if(flagggg == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                
                                if((strcmp(buffer, "Invalid Index!!\n") == 0) || (strcmp(buffer, "Error: You don't have access to read the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    continue;
                                }
                                else
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);


                                    FILE *file_pointer;
                                    file_pointer = fopen(file_read, "r");
                                    if (file_pointer == NULL)
                                    {
                                        printf("File Not Found!!\n");
                                        return 0;
                                    }
                                    char c;
                                    int count_read = 0;
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
                                            if(count_read >= ind1)
                                            {
                                                // -----------------------------------------------------------------------------------------------------------------
                                                // Writing line
                                                n = write(new_socket_file_descriptor, buffer, 10005);
                                                // Error came in the write command
                                                if(n < 0)
                                                    error("Error while Writing!! \n");
                                                //-------------------------------------------------------------------------------------------------------------------
                                                bzero(buffer, 10005);
                                                //////////////////////////////////////////////////////////////////////////////
                                                // Reading the timepass
                                                n = read(new_socket_file_descriptor, buffer, 10005);
                                                // Error came in the read command
                                                if(n < 0)
                                                    error("Error while Reading!!\n");
                                                //////////////////////////////////////////////////////////////////////////////
                                                bzero(buffer, 10005);
                                            }
                                            count_read++;
                                            bzero(buffer, 10005);
                                        }
                                    }
                                    // -----------------------------------------------------------------------------------------------------------------
                                    // Writing line
                                    strcat(buffer, "\n");
                                    n = write(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the write command
                                    if(n < 0)
                                        error("Error while Writing!! \n");
                                    //-------------------------------------------------------------------------------------------------------------------
                                    bzero(buffer, 10005);
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the final output
                                    fclose(file_pointer);

                                }
                            }
                            else if(flg == 2)
                            {
                                int flagggg = 0;
                                if(file_count_c[0] == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_read) == 0)
                                    {
                                        flagggg = 1;
                                    }
                                    if((strcmp(file_c[i].file_name, file_read) == 0) && (file_c[i].id == (10000 + t_count)))
                                    {
                                        no_of_lines_read = NLINEX(file_read);
                                        bzero(buffer, 10005);
                                        if(ind1<0)
                                            ind1 = no_of_lines_read + ind1;
                                        if(ind2<0)
                                            ind2 = no_of_lines_read + ind2;
                                        if(ind1>=no_of_lines_read || ind1<0 || ind2>=no_of_lines_read || ind2<0 || ind2 <= ind1)
                                        {
                                            strcpy(buffer, "Invalid Index!!\n");
                                        }
                                        else
                                        {
                                            no_of_lines_read = ind2 - ind1;
                                            sprintf(buffer, "%d", no_of_lines_read);
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        strcpy(buffer, "Error: You don't have access to read the file!!\n");
                                    }
                                }
                                if(flagggg == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                
                                if((strcmp(buffer, "Invalid Index!!\n") == 0) || (strcmp(buffer, "Error: You don't have access to read the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    continue;
                                }
                                else
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);


                                    FILE *file_pointer;
                                    file_pointer = fopen(file_read, "r");
                                    if (file_pointer == NULL)
                                    {
                                        printf("File Not Found!!\n");
                                        return 0;
                                    }
                                    char c;
                                    int count_read = 0;
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
                                            if(count_read >= ind1 && count_read<ind2)
                                            {
                                                // -----------------------------------------------------------------------------------------------------------------
                                                // Writing line
                                                n = write(new_socket_file_descriptor, buffer, 10005);
                                                // Error came in the write command
                                                if(n < 0)
                                                    error("Error while Writing!! \n");
                                                //-------------------------------------------------------------------------------------------------------------------
                                                bzero(buffer, 10005);
                                                //////////////////////////////////////////////////////////////////////////////
                                                // Reading the timepass
                                                n = read(new_socket_file_descriptor, buffer, 10005);
                                                // Error came in the read command
                                                if(n < 0)
                                                    error("Error while Reading!!\n");
                                                //////////////////////////////////////////////////////////////////////////////
                                                bzero(buffer, 10005);
                                            }
                                            count_read++;
                                            bzero(buffer, 10005);
                                        }
                                    }
                                    
                                    fclose(file_pointer);
                                }
                            }
                            else
                            {
                                bzero(buffer, 10005);
                                strcpy(buffer, "Invalid Index!!\n");
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                bzero(buffer, 10005);
                                //////////////////////////////////////////////////////////////////////////////
                                // Reading the timepass
                                n = read(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the read command
                                if(n < 0)
                                    error("Error while Reading!!\n");
                                //////////////////////////////////////////////////////////////////////////////
                                bzero(buffer, 10005);
                                continue;
                            }
                            
                            bzero(buffer, 10005);
                        }
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        ////////////////////////////// insert ////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        else if(strncmp(command_type, "7", 1) == 0)
                        {
                            char file_insert[50];
                            bzero(file_insert, 50);
                            char insert_msg[100];
                            bzero(insert_msg, 100);
                            char ind_insert[20];
                            bzero(ind_insert, 20);
                            int insert_ind;
                            int abcd = 0;
                            int count_space_insert = 0;
                            int read_space[5];
                            while(buffer[abcd] != '"')
                            {
                                if(buffer[abcd] == ' ')
                                {
                                    read_space[count_space_insert] = abcd;
                                    count_space_insert++;
                                }
                                abcd++;
                            }
                            if(count_space_insert == 2)
                            {
                                strncpy(file_insert, buffer+read_space[0]+1, read_space[1]-read_space[0]-1);
                                int num_line_in_file = NLINEX(file_insert);
                                
                                int flagggg = 0;
                                int big_flag = 0;
                                if(file_count_c[0] == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_insert) == 0)
                                    {
                                        flagggg = 1;
                                    }
                                    if((strcmp(file_c[i].file_name, file_insert) == 0) && (file_c[i].id == (10000 + t_count)) && ((strncmp(file_c[i].permission, "E", 1) == 0) || (strncmp(file_c[i].permission, "O", 1) == 0)))
                                    {
                                        
                                        big_flag = 1;
                                        break;
                                    }
                                    else
                                    {
                                        strcpy(buffer, "Error: You don't have access to insert message in this file!!\n");
                                    }
                                }
                                if(flagggg == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                if(big_flag == 1)
                                {
                                    if(num_line_in_file >0)
                                    {
                                        strcpy(insert_msg, "\n");
                                    }
                                    strncat(insert_msg, buffer+read_space[1]+2, strlen(buffer)-read_space[1]-4);
                                    // strcat(insert_msg, "\n");
                                    int xy=0;
                                    while(xy < strlen(insert_msg))
                                    {
                                        if(insert_msg[xy] == '\\')
                                        {
                                            if(xy+1 < strlen(insert_msg) && insert_msg[xy+1] == 'n')
                                            {
                                                insert_msg[xy+1] = '\n';
                                                insert_msg[xy] = ' ';
                                            }
                                        }
                                        xy++;
                                    }
                                    // if
                                    bzero(buffer, 10005);
                                    FILE *file_pointer;
                                    char c;
                                    file_pointer = fopen(file_insert, "a");
                                    if (num_line_in_file == -1 || file_pointer == NULL)
                                    {
                                        strcat(buffer, "File Not Found!!\n");
                                        fclose(file_pointer);
                                    }
                                    else
                                    {
                                        fputs(insert_msg, file_pointer);
                                        strcat(buffer, "Message is added Succefully!!\n");
                                        fclose(file_pointer);
                                    }
                                }
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                
                                if(strcmp(buffer, "Message is added Succefully!!\n") == 0)
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    int no_line_ins = NLINEX(file_insert);
                                    sprintf(buffer, "%d", no_line_ins);
                                    // -----------------------------------------------------------------------------------------------------------------
                                    // Writing number of lines or error
                                    n = write(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the write command
                                    if(n < 0)
                                        error("Error while Writing!! \n");
                                    //-------------------------------------------------------------------------------------------------------------------
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);


                                    FILE *file_pointer;
                                    file_pointer = fopen(file_insert, "r");
                                    if (file_pointer == NULL)
                                    {
                                        printf("File Not Found!!\n");
                                        return 0;
                                    }
                                    char c;
                                    // int count = 0;
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
                                            n = write(new_socket_file_descriptor, buffer, 10005);
                                            // Error came in the write command
                                            if(n < 0)
                                                error("Error while Writing!! \n");
                                            //-------------------------------------------------------------------------------------------------------------------
                                            bzero(buffer, 10005);
                                            //////////////////////////////////////////////////////////////////////////////
                                            // Reading the timepass
                                            n = read(new_socket_file_descriptor, buffer, 10005);
                                            // Error came in the read command
                                            if(n < 0)
                                                error("Error while Reading!!\n");
                                            //////////////////////////////////////////////////////////////////////////////
                                            bzero(buffer, 10005);
                                        }
                                    }
                                    // -----------------------------------------------------------------------------------------------------------------
                                    // Writing line
                                    strcat(buffer, "\n");
                                    n = write(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the write command
                                    if(n < 0)
                                        error("Error while Writing!! \n");
                                    //-------------------------------------------------------------------------------------------------------------------
                                    bzero(buffer, 10005);
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the final output
                                    fclose(file_pointer);

                                    /////////////////////////////////////////////////////////////////////////////////////
                                }
                                // update number of lines
                                int updated_lines = NLINEX(file_insert);
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_insert) == 0)
                                    {
                                        file_c[i].number_of_line = updated_lines;
                                    }
                                }
                                bzero(buffer, 10005);
                            }
                            else if(count_space_insert == 3)
                            {
                                strncpy(file_insert, buffer+read_space[0]+1, read_space[1]-read_space[0]-1);
                                strncpy(ind_insert, buffer+read_space[1]+1, read_space[2]-read_space[1]-1);
                                insert_ind = atoi(ind_insert);
                                // strcpy(insert_msg, "\n");
                                strncpy(insert_msg, buffer+read_space[2]+2, strlen(buffer)-read_space[2]-4);
                                strcat(insert_msg, "\n");


                                int flagggg = 0;
                                int big_flag = 0;
                                if(file_count_c[0] == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_insert) == 0)
                                    {
                                        flagggg = 1;
                                    }
                                    if((strcmp(file_c[i].file_name, file_insert) == 0) && (file_c[i].id == (10000 + t_count)) && ((strncmp(file_c[i].permission, "E", 1) == 0) || (strncmp(file_c[i].permission, "O", 1) == 0)))
                                    {
                                        
                                        big_flag = 1;
                                        break;
                                    }
                                    else
                                    {
                                        strcpy(buffer, "Error: You don't have access to insert message in this file!!\n");
                                    }
                                }
                                if(flagggg == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }

                                if(big_flag == 1)
                                {
                                    int no_of_lines_insert = NLINEX(file_insert);
                                    bzero(buffer, 10005);
                                    if(insert_ind<0)
                                        insert_ind = no_of_lines_insert + insert_ind;
                                    if(insert_ind>=no_of_lines_insert || insert_ind<0)
                                    {
                                        strcpy(buffer, "Invalid Index!!\n");
                                    }
                                    else
                                    {
                                        int xy=0;
                                        while(xy < strlen(insert_msg))
                                        {
                                            if(insert_msg[xy] == '\\')
                                            {
                                                if(xy+1 < strlen(insert_msg) && insert_msg[xy+1] == 'n')
                                                {
                                                    insert_msg[xy+1] = '\n';
                                                    insert_msg[xy] = ' ';
                                                }
                                            }
                                            xy++;
                                        }

                                        FILE *file_pointer_r, *file_pointer_w;
                                        char c;
                                        // Original File
                                        file_pointer_r = fopen(file_insert, "r");
                                        if (file_pointer_r == NULL)
                                        {
                                            strcpy(buffer, "File Not Found!! \n");
                                            fclose(file_pointer_r);
                                            fclose(file_pointer_w);
                                        }
                                        else
                                        {
                                            // Temporary file
                                            file_pointer_w = fopen("server_file_new.txt", "w");
                                            if (file_pointer_w == NULL)
                                            {
                                                strcpy(buffer, "File Not Found!! \n");
                                                fclose(file_pointer_r);
                                                fclose(file_pointer_w);
                                            }
                                            else
                                            {
                                                int count = 0;
                                                int flag = 0;

                                                // Algorithm : Explained Above
                                                for(c = getc(file_pointer_r); c != EOF; c = getc(file_pointer_r))
                                                {
                                                    if(count == insert_ind)
                                                    {
                                                        if(flag == 0)
                                                        {
                                                            fputs(insert_msg, file_pointer_w);
                                                            flag = 1;
                                                        }
                                                        fputc(c, file_pointer_w);
                                                    }
                                                    else
                                                        fputc(c, file_pointer_w);
                                                    if(c == '\n')
                                                        count++;
                                                }

                                                fclose(file_pointer_r);
                                                fclose(file_pointer_w);

                                                FILE *file_r, *file_w;
                                                file_r = fopen("server_file_new.txt", "r");
                                                if (file_r == NULL)
                                                {
                                                    strcpy(buffer, "Error while Executing!! \n");
                                                    fclose(file_r);
                                                    fclose(file_w);
                                                }
                                                else
                                                {
                                                    file_w = fopen(file_insert, "w");
                                                    if (file_w == NULL)
                                                    {
                                                        strcpy(buffer, "Error while Executing!! \n");
                                                        fclose(file_r);
                                                        fclose(file_w);
                                                    }
                                                    else
                                                    {
                                                        c = fgetc(file_r);
                                                        while (c != EOF)
                                                        {
                                                            fputc(c, file_w);
                                                            c = fgetc(file_r);
                                                        }
                                                        remove("server_file_new.txt");
                                                        fclose(file_r);
                                                        fclose(file_w);
                                                        strcpy(buffer, "Message is added Succefully!!\n");

                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                if(strcmp(buffer, "Message is added Succefully!!\n") == 0)
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    int no_line_ins = NLINEX(file_insert);
                                    sprintf(buffer, "%d", no_line_ins);
                                    // -----------------------------------------------------------------------------------------------------------------
                                    // Writing number of lines or error
                                    n = write(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the write command
                                    if(n < 0)
                                        error("Error while Writing!! \n");
                                    //-------------------------------------------------------------------------------------------------------------------
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);


                                    FILE *file_pointer;
                                    file_pointer = fopen(file_insert, "r");
                                    if (file_pointer == NULL)
                                    {
                                        printf("File Not Found!!\n");
                                        return 0;
                                    }
                                    char c;
                                    // int count = 0;
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
                                            n = write(new_socket_file_descriptor, buffer, 10005);
                                            // Error came in the write command
                                            if(n < 0)
                                                error("Error while Writing!! \n");
                                            //-------------------------------------------------------------------------------------------------------------------
                                            bzero(buffer, 10005);
                                            //////////////////////////////////////////////////////////////////////////////
                                            // Reading the timepass
                                            n = read(new_socket_file_descriptor, buffer, 10005);
                                            // Error came in the read command
                                            if(n < 0)
                                                error("Error while Reading!!\n");
                                            //////////////////////////////////////////////////////////////////////////////
                                            bzero(buffer, 10005);
                                        }
                                    }
                                    // -----------------------------------------------------------------------------------------------------------------
                                    // Writing line
                                    strcat(buffer, "\n");
                                    n = write(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the write command
                                    if(n < 0)
                                        error("Error while Writing!! \n");
                                    //-------------------------------------------------------------------------------------------------------------------
                                    bzero(buffer, 10005);
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the final output
                                    fclose(file_pointer);

                                    /////////////////////////////////////////////////////////////////////////////////////
                                }
                                // update number of lines
                                int updated_lines = NLINEX(file_insert);
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_insert) == 0)
                                    {
                                        file_c[i].number_of_line = updated_lines;
                                    }
                                }
                                bzero(buffer, 10005);
                            }
                            else
                            {
                                strcpy(buffer, "Incorrect Input!!\n");
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                bzero(buffer, 10005);
                            }
                            bzero(buffer, 10005);
                            bzero(file_insert, 50);
                            bzero(insert_msg, 100);
                            bzero(ind_insert, 20);
                        }
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        ////////////////////////////// delete ////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        //////////////////////////////////////////////////////////////////////////
                        else if(strncmp(command_type, "8", 1) == 0)
                        {
                            char file_delete[50];
                            bzero(file_delete, 50);
                            char ind[20];
                            int ind1 = 0;
                            int ind2 = 0;

                            int abcd = 0;
                            int count_space = 0;
                            int read_space[5];
                            while(buffer[abcd] != '\n')
                            {
                                if(buffer[abcd] == ' ')
                                {
                                    read_space[count_space] = abcd;
                                    count_space++;
                                }
                                abcd++;
                            }
                            read_space[count_space] = abcd;
                            
                            int flg = count_space - 1;
                            if(count_space == 1)
                            {
                                
                                bzero(file_delete, 50);// clear array
                                strncpy(file_delete, buffer + read_space[0] + 1, read_space[1]-read_space[0]-1);
                            }

                            else if(count_space == 2)
                            {
                                bzero(file_delete, 50);// clear array
                                strncpy(file_delete, buffer + read_space[0] + 1, read_space[1]-read_space[0]-1);

                                bzero(ind, 20);
                                strncpy(ind, buffer + read_space[1] + 1, read_space[2]-read_space[1]-1);
                                ind1 = atoi(ind);
                                bzero(ind, 20);
                            }
                            else if(count_space == 3)
                            {
                                bzero(file_delete, 50);// clear array
                                strncpy(file_delete, buffer + read_space[0] + 1, read_space[1]-read_space[0]-1);

                                bzero(ind, 20);
                                strncpy(ind, buffer+read_space[1]+1, read_space[2]-read_space[1]-1);
                                ind1 = atoi(ind);
                                bzero(ind, 20);

                                strncpy(ind, buffer+read_space[2]+1, read_space[3]-read_space[2]-1);
                                ind2 = atoi(ind);
                                bzero(ind, 20);
                            }

                            int no_of_lines_del;
                            if(flg == 0)
                            {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                int flagggg = 0;
                                if(file_count_c[0] == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_delete) == 0)
                                    {
                                        flagggg = 1;
                                    }
                                    if((strcmp(file_c[i].file_name, file_delete) == 0) && (file_c[i].id == (10000 + t_count)) && ((strncmp(file_c[i].permission, "E", 1) == 0) || (strncmp(file_c[i].permission, "O", 1) == 0)))
                                    {
                                        no_of_lines_del = 0;
                                        bzero(buffer, 10005);
                                        sprintf(buffer, "%d", no_of_lines_del);
                                        break;
                                    }
                                    else
                                    {
                                        strcpy(buffer, "Error: You don't have access to perform delete in the file!!\n");
                                    }
                                }
                                if(flagggg == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }

                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                
                                if((strcmp(buffer, "Invalid Index!!\n") == 0) || (strcmp(buffer, "Error: You don't have access to perform delete in the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    continue;
                                }
                                else
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);

                                    /////////////////////////////////////////////////////////////////////////////////////
                                    
                                    FILE *file_pointer;
                                    file_pointer = fopen(file_delete, "w");
                                    if(file_pointer == NULL)
                                    {
                                        strcpy(buffer, "File not found!!\n");
                                        fclose(file_pointer);
                                    }
                                    else
                                    {
                                        bzero(buffer, 10005);
                                        fputs(buffer, file_pointer);
                                        fclose(file_pointer);
                                    }
                                }
                                int updated_lines = NLINEX(file_delete);
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_delete) == 0)
                                    {
                                        file_c[i].number_of_line = updated_lines;
                                    }
                                }
                                bzero(buffer, 10005);
                            }
                            else if(flg == 1)
                            {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                int flagggg = 0;
                                if(file_count_c[0] == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_delete) == 0)
                                    {
                                        flagggg = 1;
                                    }
                                    if((strcmp(file_c[i].file_name, file_delete) == 0) && (file_c[i].id == (10000 + t_count)) && ((strncmp(file_c[i].permission, "E", 1) == 0) || (strncmp(file_c[i].permission, "O", 1) == 0)))
                                    {
                                        no_of_lines_del = NLINEX(file_delete);
                                        bzero(buffer, 10005);
                                        if(ind1<0)
                                            ind1 = no_of_lines_del + ind1;
                                        if(ind1>=no_of_lines_del || ind1<0)
                                        {
                                            strcpy(buffer, "Invalid Index!!\n");
                                        }
                                        else
                                        {
                                            bzero(buffer, 10005);
                                            no_of_lines_del = ind1;
                                            sprintf(buffer, "%d", no_of_lines_del);
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        strcpy(buffer, "Error: You don't have access to perform delete in the file!!\n");
                                    }
                                }
                                if(flagggg == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                
                                if((strcmp(buffer, "Invalid Index!!\n") == 0) || (strcmp(buffer, "Error: You don't have access to perform delete in the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    continue;
                                }
                                else
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);

                                    FILE *file_pointer_r, *file_pointer_w;
                                    char c;
                                    // Original File
                                    file_pointer_r = fopen(file_delete, "r");
                                    if (file_pointer_r == NULL)
                                    {
                                        bzero(buffer, 10005);
                                        fclose(file_pointer_r);
                                        fclose(file_pointer_w);
                                        error("File Not Found!! \n");
                                    }
                                    else
                                    {
                                        // Temporary file
                                        file_pointer_w = fopen("server_file_new.txt", "w");
                                        if (file_pointer_w == NULL)
                                        {
                                            bzero(buffer, 10005);
                                            fclose(file_pointer_r);
                                            fclose(file_pointer_w);
                                            error("File Not Found!! \n");
                                        }
                                        else
                                        {
                                            int count = 0;
                                            int flag = 0;

                                            // Algorithm : Explained Above
                                            for(c = getc(file_pointer_r); c != EOF; c = getc(file_pointer_r))
                                            {
                                                if(c == '\n')
                                                    count++;
                                                if(count >= ind1)
                                                {    
                                                    break;
                                                }
                                                else
                                                    fputc(c, file_pointer_w);
                                                // if(c == '\n')
                                                //     count++;
                                            }

                                            fclose(file_pointer_r);
                                            fclose(file_pointer_w);

                                            FILE *file_r, *file_w;
                                            file_r = fopen("server_file_new.txt", "r");
                                            if (file_r == NULL)
                                            {
                                                bzero(buffer, 10005);
                                                
                                                fclose(file_r);
                                                fclose(file_w);
                                                error("Error while Executing!! \n");
                                            }
                                            else
                                            {
                                                file_w = fopen(file_delete, "w");
                                                if (file_w == NULL)
                                                {
                                                    
                                                    fclose(file_r);
                                                    fclose(file_w);
                                                    error("Error while Executing!! \n");
                                                }
                                                else
                                                {
                                                    c = fgetc(file_r);
                                                    while (c != EOF)
                                                    {
                                                        fputc(c, file_w);
                                                        c = fgetc(file_r);
                                                    }
                                                    remove("server_file_new.txt");
                                                    fclose(file_r);
                                                    fclose(file_w);

                                                    FILE *file_pointer;
                                                    file_pointer = fopen(file_delete, "r");
                                                    if (file_pointer == NULL)
                                                    {
                                                        printf("File Not Found!!\n");
                                                        return 0;
                                                    }
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
                                                            n = write(new_socket_file_descriptor, buffer, 10005);
                                                            // Error came in the write command
                                                            if(n < 0)
                                                                error("Error while Writing!! \n");
                                                            //-------------------------------------------------------------------------------------------------------------------
                                                            bzero(buffer, 10005);
                                                            //////////////////////////////////////////////////////////////////////////////
                                                            // Reading the timepass
                                                            n = read(new_socket_file_descriptor, buffer, 10005);
                                                            // Error came in the read command
                                                            if(n < 0)
                                                                error("Error while Reading!!\n");
                                                            //////////////////////////////////////////////////////////////////////////////
                                                            bzero(buffer, 10005);
                                                        }
                                                    }
                                                    // -----------------------------------------------------------------------------------------------------------------
                                                    // Writing line
                                                    strcat(buffer, "\n");
                                                    n = write(new_socket_file_descriptor, buffer, 10005);
                                                    // Error came in the write command
                                                    if(n < 0)
                                                        error("Error while Writing!! \n");
                                                    //-------------------------------------------------------------------------------------------------------------------
                                                    bzero(buffer, 10005);
                                                    //////////////////////////////////////////////////////////////////////////////
                                                    // Reading the timepass
                                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                                    // Error came in the read command
                                                    if(n < 0)
                                                        error("Error while Reading!!\n");
                                                    //////////////////////////////////////////////////////////////////////////////
                                                    // bzero(buffer, 10005);
                                                    bzero(buffer, 10005);
                                                    fclose(file_pointer);
                                                    ////////////////////////////////////////////////////////////////////////////////////
                                                }
                                            }
                                        }
                                    }
                                }
                                int updated_lines = NLINEX(file_delete);
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_delete) == 0)
                                    {
                                        file_c[i].number_of_line = updated_lines;
                                    }
                                }
                                bzero(buffer, 10005);
                            }
                            else if(flg == 2)
                            {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                int flagggg = 0;
                                if(file_count_c[0] == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_delete) == 0)
                                    {
                                        flagggg = 1;
                                    }
                                    if((strcmp(file_c[i].file_name, file_delete) == 0) && (file_c[i].id == (10000 + t_count)) && ((strncmp(file_c[i].permission, "E", 1) == 0) || (strncmp(file_c[i].permission, "O", 1) == 0)))
                                    {
                                        no_of_lines_del = NLINEX(file_delete);
                                        bzero(buffer, 10005);
                                        if(ind1<0)
                                            ind1 = no_of_lines_del + ind1;
                                        if(ind2<0)
                                            ind2 = no_of_lines_del + ind2;
                                        if(ind1>=no_of_lines_del || ind1<0 || ind2>=no_of_lines_del || ind2<0 || ind2 <= ind1)
                                        {
                                            strcpy(buffer, "Invalid Index!!\n");
                                        }
                                        else
                                        {
                                            bzero(buffer, 10005);
                                            no_of_lines_del = no_of_lines_del - ind2 + ind1;
                                            sprintf(buffer, "%d", no_of_lines_del);
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        strcpy(buffer, "Error: You don't have access to perform delete in the file!!\n");
                                    }
                                }
                                if(flagggg == 0)
                                {
                                    strcpy(buffer, "Error: File not found!!\n");
                                }
                                
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                
                                if((strcmp(buffer, "Invalid Index!!\n") == 0) || (strcmp(buffer, "Error: You don't have access to perform delete in the file!!\n")==0) || (strcmp(buffer, "Error: File not found!!\n")==0))
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);
                                    continue;
                                }
                                else
                                {
                                    bzero(buffer, 10005);
                                    //////////////////////////////////////////////////////////////////////////////
                                    // Reading the timepass
                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                    // Error came in the read command
                                    if(n < 0)
                                        error("Error while Reading!!\n");
                                    //////////////////////////////////////////////////////////////////////////////
                                    bzero(buffer, 10005);


                                    FILE *file_pointer_r, *file_pointer_w;
                                    char c;
                                    // Original File
                                    file_pointer_r = fopen(file_delete, "r");
                                    if (file_pointer_r == NULL)
                                    {
                                        strcpy(buffer, "File Not Found!! \n");
                                        fclose(file_pointer_r);
                                        fclose(file_pointer_w);
                                    }
                                    else
                                    {
                                        // Temporary file
                                        file_pointer_w = fopen("server_file_new.txt", "w");
                                        if (file_pointer_w == NULL)
                                        {
                                            strcpy(buffer, "File Not Found!! \n");
                                            fclose(file_pointer_r);
                                            fclose(file_pointer_w);
                                        }
                                        else
                                        {
                                            int count = 0;
                                            int flag = 0;

                                            // Algorithm : Explained Above
                                            for(c = getc(file_pointer_r); c != EOF; c = getc(file_pointer_r))
                                            {
                                                if(c == '\n')
                                                    count++;
                                                if(count >= ind1 && count < ind2)
                                                {

                                                }
                                                else
                                                {
                                                    fputc(c, file_pointer_w);
                                                }
                                            }

                                            fclose(file_pointer_r);
                                            fclose(file_pointer_w);

                                            FILE *file_r, *file_w;
                                            file_r = fopen("server_file_new.txt", "r");
                                            if (file_r == NULL)
                                            {
                                                strcpy(buffer, "Error while Executing!! \n");
                                                fclose(file_r);
                                                fclose(file_w);
                                            }
                                            else
                                            {
                                                file_w = fopen(file_delete, "w");
                                                if (file_w == NULL)
                                                {
                                                    strcpy(buffer, "Error while Executing!! \n");
                                                    fclose(file_r);
                                                    fclose(file_w);
                                                }
                                                else
                                                {
                                                    c = fgetc(file_r);
                                                    while (c != EOF)
                                                    {
                                                        fputc(c, file_w);
                                                        c = fgetc(file_r);
                                                    }
                                                    remove("server_file_new.txt");
                                                    fclose(file_r);
                                                    fclose(file_w);
                                                    // strcpy(buffer, "Message is added Succefully!!\n");

                                                    FILE *file_pointer;
                                                    file_pointer = fopen(file_delete, "r");
                                                    if (file_pointer == NULL)
                                                    {
                                                        printf("File Not Found!!\n");
                                                        return 0;
                                                    }
                                                    char c;
                                                    // int count = 0;
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
                                                            n = write(new_socket_file_descriptor, buffer, 10005);
                                                            // Error came in the write command
                                                            if(n < 0)
                                                                error("Error while Writing!! \n");
                                                            //-------------------------------------------------------------------------------------------------------------------
                                                            bzero(buffer, 10005);
                                                            //////////////////////////////////////////////////////////////////////////////
                                                            // Reading the timepass
                                                            n = read(new_socket_file_descriptor, buffer, 10005);
                                                            // Error came in the read command
                                                            if(n < 0)
                                                                error("Error while Reading!!\n");
                                                            //////////////////////////////////////////////////////////////////////////////
                                                            bzero(buffer, 10005);
                                                        }
                                                    }
                                                    // -----------------------------------------------------------------------------------------------------------------
                                                    // Writing line
                                                    strcat(buffer, "\n");
                                                    n = write(new_socket_file_descriptor, buffer, 10005);
                                                    // Error came in the write command
                                                    if(n < 0)
                                                        error("Error while Writing!! \n");
                                                    //-------------------------------------------------------------------------------------------------------------------
                                                    bzero(buffer, 10005);
                                                    // Reading the timepass
                                                    n = read(new_socket_file_descriptor, buffer, 10005);
                                                    // Error came in the read command
                                                    if(n < 0)
                                                        error("Error while Reading!!\n");
                                                    //////////////////////////////////////////////////////////////////////////////
                                                    bzero(buffer, 10005);
                                                    //////////////////////////////////////////////////////////////////////////////
                                                    // Reading the final output
                                                    fclose(file_pointer);
                                                    ////////////////////////////////////////////////////////////////////////////////////
                                                }
                                            }
                                        }
                                    }
                                }
                                int updated_lines = NLINEX(file_delete);
                                for(int i=0; i<file_count_c[0]; i++)
                                {
                                    if(strcmp(file_c[i].file_name, file_delete) == 0)
                                    {
                                        file_c[i].number_of_line = updated_lines;
                                    }
                                }
                                bzero(buffer, 10005);
                            }
                            else
                            {
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                bzero(buffer, 10005);
                                strcpy(buffer, "Invalid Index!!\n");
                                // -----------------------------------------------------------------------------------------------------------------
                                // Writing number of lines or error
                                n = write(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the write command
                                if(n < 0)
                                    error("Error while Writing!! \n");
                                //-------------------------------------------------------------------------------------------------------------------
                                bzero(buffer, 10005);
                                //////////////////////////////////////////////////////////////////////////////
                                // Reading the timepass
                                n = read(new_socket_file_descriptor, buffer, 10005);
                                // Error came in the read command
                                if(n < 0)
                                    error("Error while Reading!!\n");
                                //////////////////////////////////////////////////////////////////////////////
                                bzero(buffer, 10005);
                                continue;
                            }
                            bzero(buffer, 10005);
                        }
                    }
                    bzero(buffer, 10005);
                }
            }
            else
            {
                continue;
            }
        }
    }
    

    close(new_socket_file_descriptor);
    close(socket_file_descriptor);
    return 0;
}
