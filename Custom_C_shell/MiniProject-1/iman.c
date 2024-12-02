#include "iman.h"

void fetch_man_page(const char *command)
{
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[8192];  // Buffer to store the response
    char request[1024]; // HTTP request
    int bytes_received;
    char *host = "man.he.net";
    int port = 80;

    // Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("" COLOR_RED "Error creating socket\n" COLOR_RESET);
        return;
    }

    // Get the server's IP address
    server = gethostbyname(host);
    if (server == NULL)
    {
        printf("" COLOR_RED "No such host\n" COLOR_RESET);
        return;
    }
    // printf("%s\n", server->h_name);
    // Configure the server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("" COLOR_RED "Error connecting to server\n" COLOR_RESET);
        return;
    }

    // Form the GET request
    snprintf(request, sizeof(request),
             "GET /?topic=%s&section=all HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             command, host);
    // printf("%s\n", request);
    //  Send the request
    if (write(sockfd, request, strlen(request)) < 0)
    {
        printf("" COLOR_RED "Error sending request\n" COLOR_RESET);
        close(sockfd);
        return;
    }

    // Read the response
    int header_done = 0;
    while ((bytes_received = read(sockfd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_received] = '\0'; // Null-terminate the buffer
        // printf("Received %d bytes:\n", bytes_received); // Debugging: print received data

        if (!header_done)
        {
            // Skip the HTTP headers by finding the first double newline
            remove_first_seven_lines(buffer);
            if (buffer)
            {
                header_done = 1;
                printf("%s\n", buffer);
            }
        }
        else
        {
            // Continue printing the rest of the response
            printf("%s\n", buffer);
        }
    }

    if (bytes_received < 0)
    {
        printf("" COLOR_RED "Error receiving response\n" COLOR_RESET);
    }
    // if(bytes_received == 0){
    //     printf("No manual entry for %s\n", command);
    // }

    // printf("\n---");
    //  Close the socket
    // fflush(stdout);
    close(sockfd);
}
void remove_first_seven_lines(char *input)
{
    int line_count = 0;
    char *line = strtok(input, "\n");

    while (line != NULL)
    {
        line_count++;
        if (line_count > 7)
        {
            printf("%s\n", line); // Print lines after the 7th
        }
        line = strtok(NULL, "\n");
    }
}