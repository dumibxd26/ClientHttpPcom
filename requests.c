#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url,
                            char **cookies, int cookies_count
                            , char *jwt_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    // if (query_params != NULL) {
    //     sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
        
    // } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    // }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    memset(line, 0, LINELEN);
    if (cookies != NULL) {
       sprintf(line, "Cookie: ");

        for(int i = 0; i < cookies_count; i++) {
            sprintf(line + strlen(line), "%s; ", cookies[i]);
        }
            
        if (cookies_count > 0) {
            line[strlen(line) - 1] = '\0';
            line[strlen(line) - 1] = '\0';
        }

        // printf("LINE = %s\n", line);

       compute_message(message, line);
    }

    if (jwt_token) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: %s", jwt_token);
        compute_message(message, line);
    }

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

int find_message_length(int fields, char **body) {

    int total_lenth = 0;

    for (int i = 0; i < fields; i++) {
        total_lenth += strlen(body[i]);
    }

    if (fields > 0) {
        total_lenth += fields - 1;
    }

    return total_lenth;
}

char *compute_post_request(char *host, char *url, char *content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count
                            , char *jwt_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    memset(line, 0, LINELEN);
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", find_message_length(body_data_fields_count, body_data));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    memset(line, 0, LINELEN);
    if (cookies != NULL) {
       sprintf(line, "Cookie: ");
        
        for(int i = 0; i < cookies_count; i++) {
          sprintf(line + strlen(line), "%s; ", cookies[i]);
         }

        if (cookies_count > 0) {
            line[strlen(line) - 1] = '\0';
            line[strlen(line) - 1] = '\0';
        }

       compute_message(message, line);
    }

    if (jwt_token) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: %s", jwt_token);
        compute_message(message, line);
    }

    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    // memset(line, 0, LINELEN);
    // strcat(message, body_data_buffer);

    // add the payload data to the message
    for (int i = 0; i < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);
        strcat(body_data_buffer, "&");
    }

    if (body_data_fields_count > 0) {
        body_data_buffer[strlen(body_data_buffer) - 1] = '\0';
    }

    compute_message(message, body_data_buffer);

    free(body_data_buffer);
    free(line);
    return message;
}

char *compute_DELETE_request(char *host, char *url, char *content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count
                            , char *jwt_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    memset(line, 0, LINELEN);
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %d", find_message_length(body_data_fields_count, body_data));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    memset(line, 0, LINELEN);
    if (cookies != NULL) {
       sprintf(line, "Cookie: ");
        
        for(int i = 0; i < cookies_count; i++) {
          sprintf(line + strlen(line), "%s; ", cookies[i]);
         }

        if (cookies_count > 0) {
            line[strlen(line) - 1] = '\0';
            line[strlen(line) - 1] = '\0';
        }

       compute_message(message, line);
    }

    if (jwt_token) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: %s", jwt_token);
        compute_message(message, line);
    }

    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    // memset(line, 0, LINELEN);
    // strcat(message, body_data_buffer);

    // add the payload data to the message
    for (int i = 0; i < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);
        strcat(body_data_buffer, "&");
    }

    if (body_data_fields_count > 0) {
        body_data_buffer[strlen(body_data_buffer) - 1] = '\0';
    }

    compute_message(message, body_data_buffer);

    free(body_data_buffer);
    free(line);
    return message;
}