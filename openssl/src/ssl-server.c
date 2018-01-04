#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int create_server(int port)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s < 0)
        goto PERROR;

    if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        goto CLOSE;

    if (listen(s, 1) < 0)
        goto CLOSE;

    return s;

CLOSE:
    close(s);

PERROR:
    perror("Unable to init socket");
    exit(EXIT_FAILURE);
}

void handle_client(SSL *ssl)
{
    if (SSL_accept(ssl) <= 0)
        goto ERROR;

    char buf[1024];
    while (1)
    {
        int bytes = SSL_read(ssl, buf, sizeof(buf));
        if (bytes > 0)
            SSL_write(ssl, buf, bytes);
        else
            break;
    }

ERROR:
    ERR_print_errors_fp(stderr);
}

int main(int argc, char **argv)
{
    int server = create_server(4433);

    // init OpenSSL
    SSL_library_init();
    SSL_load_error_strings();

    // create context object
    SSL_METHOD *method = TLSv1_2_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (ctx == NULL)
        goto EXIT;

    // configure context object
    SSL_CTX_set_ecdh_auto(ctx, 1);

    if (SSL_CTX_use_certificate_file(ctx, "localhost.crt", SSL_FILETYPE_PEM) <= 0)
        goto EXIT;

    if (SSL_CTX_use_PrivateKey_file(ctx, "localhost.key", SSL_FILETYPE_PEM) <= 0)
        goto EXIT;

    // accept, handshake, echo
    while (1)
    {
        int client = accept(server, NULL, NULL);
        if (client < 0)
            continue;

        // attach the socket descriptor
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        // handshake, echo
        handle_client(ssl);

        SSL_free(ssl);
        close(client);
    }

    close(server);
    SSL_CTX_free(ctx);

    // clean up OpenSSL
    EVP_cleanup();
    ERR_free_strings();

    return 0;

EXIT:
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}
