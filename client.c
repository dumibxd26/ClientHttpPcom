#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    char *cookies[1];
    cookies[0] = (char *)calloc(256, sizeof(char));
    sprintf(cookies[0], "%s", "connect.sid=");

    char *username = "studefdsfdsnt";
    char *password = "student";

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    char *json = json_serialize_to_string_pretty(root_value);

    sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/register", "application/json", &json, 1, NULL, 0, NULL);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    // login
    message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/login", "application/json", &json, 1, NULL, 0, NULL);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    char *p = strstr(response, "Set-Cookie: connect.sid=");

    if (p != NULL) {
        char *c = strstr(response, "; Path");
        snprintf(cookies[0] + strlen("connect.sid="),
            strlen(p) - strlen(c) - strlen("Set-Cookie: connect.sid=") + 1, "%s", p + strlen("Set-Cookie: connect.sid="));
    }

    char *save_this = malloc(280);
    strcpy(save_this, cookies[0]);

    // get access to library

    message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/access", cookies, 1, NULL);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    printf("\n\n\n");

    p = strstr(response, "\"token\":");
    char *token = p + 9;
    token[strlen(token) - 1] = '\0';
    token[strlen(token) - 1] = '\0';

    
    // printf("TOKEN = %s\n", token);

    // copy the token in the cookies

    memset(cookies[0], 0, 256);
    char *BEARER_TOKEN = malloc(2560);
    sprintf(BEARER_TOKEN, "Bearer %s", token);


    sprintf(cookies[0], "%s", BEARER_TOKEN);

    message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/books", cookies, 1, BEARER_TOKEN);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    // add book
    root_value = json_value_init_object();
    root_object = json_value_get_object(root_value);

    char *title = "title";
    char *author = "author";
    char *genre = "genre";
    int page_count = 100;
    char *publisher = "publisher";

    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_number(root_object, "page_count", page_count);
    json_object_set_string(root_object, "publisher", publisher);

    json = json_serialize_to_string_pretty(root_value);

    message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/library/books", "application/json", &json, 1, NULL, 1, BEARER_TOKEN);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    

    char *endpoint_bookid = strdup("/7313");
    char *books_endpoint = strdup("/api/v1/tema/library/books");

    message = compute_get_request("34.254.242.81:8080", strcat(books_endpoint, endpoint_bookid), cookies, 1, BEARER_TOKEN);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    printf("The response was:\n\n\n");

    char *endpoint_delete_bookid = strdup("/7313");
    char *books_delete_endpoint = strdup("/api/v1/tema/library/books");

    message = compute_DELETE_request("34.254.242.81:8080", strcat(books_delete_endpoint, endpoint_delete_bookid),
                                    "application/json", NULL, 0, NULL, 1, BEARER_TOKEN);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    // logout
    strcpy(cookies[0], save_this);

    message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/auth/logout", cookies, 1, BEARER_TOKEN);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    p = strtok(response, " ");
    p = strtok(NULL, " ");

    if (strcmp(p, "200") == 0) {
        printf("Logout successful\n");
    }

    return 0;
}