#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>

#include "external/cJSON.h"

void parse_json(char *str);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <stop_number>\n", argv[0]);
        return 1;
    }

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        fprintf(stderr, "Couldn't create a socket\n");
        return 1;
    } 

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(443),
    };

    if (inet_aton("46.41.138.133", &addr.sin_addr) == 0)
    {
        fprintf(stderr, "Couldn't convert IP address\n");
        goto err_close_fd;
    }
    if (connect(fd, (const struct sockaddr *)&addr, sizeof(addr)))
    {
        fprintf(stderr, "Couldn't open connection on socket %d\n", fd); 
        goto err_close_fd;
    }

    SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ctx)
    {
        fprintf(stderr, "Couldn't create context for SSL\n");
        goto err_close_fd;
    }

    SSL *ssl = SSL_new(ctx);
    if (!ssl)
    {
        fprintf(stderr, "Couldn't create SSL\n");
        goto err_ctx_free;
    }

    if (SSL_set_fd(ssl, fd) != 1 || SSL_connect(ssl) != 1)
    {
        fprintf(stderr, "Couldn't set socket for SSL\n");
        goto err_free_ssl;
    }

    char *request_template =
        "GET /api/v1/displays/%s HTTP/1.1\r\n"
        "Host: www.zditm.szczecin.pl\r\n"
        "Connection: close\r\n"
        "accept: text/xml\r\n"
        "\r\n";
    char request[256] = { 0 };
    char *stop_number = argv[1];
    snprintf(request, sizeof(request), request_template, stop_number);
    SSL_write(ssl, request, strlen(request));

    char response[4096] = { 0 };
    char *start = response;
    int count = 0, available = sizeof(response) - 1;
    do {
        count = SSL_read(ssl, start, available);
        start += count;
        available -= count;
    } while (count > 0 && available > 0);

    char *json = strstr(response, "{");
    if (json != NULL)
    {
        parse_json(json);
    }

err_free_ssl:
    SSL_free(ssl);
err_ctx_free:
    SSL_CTX_free(ctx);
err_close_fd:
    close(fd);

    return 0;
}

void parse_json(char *str)
{
	cJSON *root = cJSON_Parse(str);
	if (!root)
		return;
	if (!cJSON_IsObject(root))
		goto err_json;

    cJSON *message = cJSON_GetObjectItem(root, "message");
    if (cJSON_IsString(message) && strcmp(message->valuestring, "Stop not found.") == 0)
    {
        fprintf(stderr, "Stop not found\n");
        goto err_json;
    }

	cJSON *departures = cJSON_GetObjectItem(root, "departures");
	if (!cJSON_IsArray(departures))
		goto err_json;

	cJSON *vehicle;
	cJSON_ArrayForEach(vehicle, departures) {
		if (!cJSON_IsObject(vehicle))
			goto err_json;

		cJSON *line_number = cJSON_GetObjectItem(vehicle, "line_number");
		if (!cJSON_IsString(line_number))
			goto err_json;

        cJSON *direction = cJSON_GetObjectItem(vehicle, "direction");
        if (!cJSON_IsString(direction))
            goto err_json;

		cJSON *time_real = cJSON_GetObjectItem(vehicle, "time_real");
		if (!cJSON_IsNumber(time_real) && !cJSON_IsNull(time_real))
			goto err_json;

		cJSON *time_scheduled = cJSON_GetObjectItem(vehicle, "time_scheduled");
		if (!cJSON_IsString(time_scheduled) && !cJSON_IsNull(time_scheduled))
			goto err_json;

        if (!cJSON_IsNull(time_real))
        {
		    printf("%3s %2d min  %s\n",
                    line_number->valuestring,
                    time_real->valueint,
                    direction->valuestring);
        } else {
            printf("%3s  %s  %s\n",
                    line_number->valuestring,
                    time_scheduled->valuestring,
                    direction->valuestring);
        }
	}

err_json:
	cJSON_Delete(root);
}

