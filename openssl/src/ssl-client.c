#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"

int connect_server(const char *hostname, int port)
{
    struct hostent *host = gethostbyname(hostname);
    if (host == NULL)
        goto PERROR;

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long *) (host->h_addr);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        goto PERROR;

    int ret = connect(sock, (struct sockaddr *) &addr, sizeof(addr));
    if (ret != 0)
        goto CLOSE;

    return sock;

CLOSE:
    close(sock);

PERROR:
    perror(hostname);
    abort();
}

void print_cipher(SSL *ssl)
{
    printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
}

void print_certificate(SSL *ssl)
{
    X509 *cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if (cert == NULL)
    {
        printf("Info: No client certificates configured.\n");
        return;
    }

    printf("Server certificates:\n");

    char *line;

    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    printf("Subject: %s\n", line);
    free(line);         /* free the malloc'ed string */

    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);         /* free the malloc'ed string */

    X509_free(cert);    /* free the malloc'ed certificate copy */
}

void print_result(char *wbuf, int wlen, char *rbuf, int rlen)
{
    printf("Send %12d bytes  Received %8d bytes\n", wlen, rlen);

    int bytes_per_line = 8;
    int max = wlen > rlen ? wlen : rlen;
    int lines = max / bytes_per_line + (max % bytes_per_line > 0 ? 1 : 0);

    int line;
    for (line = 0; line < lines; line++)
    {
        int from = line * bytes_per_line;
        int to = from + bytes_per_line;

        int index;
        for (index = from; index < to; index++)
        {
            if (index < wlen)
            {
                if (index < rlen && wbuf[index] == rbuf[index])
                    printf("%02x:", (unsigned char) wbuf[index]);
                else
                    printf(RED "%02x" RESET ":", (unsigned char) wbuf[index]);
            }
            else
                printf("   ");
        }

        printf(" ");

        for (index = from; index < to; index++)
        {
            if (index < rlen)
            {
                if (index < wlen && wbuf[index] == rbuf[index])
                    printf("%02x:", (unsigned char) rbuf[index]);
                else
                    printf(RED "%02x" RESET ":", (unsigned char) rbuf[index]);
            }
            else
                break;
        }

        printf("\n");
    }
}

int genrand(char *rbuf, int len)
{
    int length = rand()%len + 1; // 1 to len

    int index;
    for (index = 0; index < length; index++)
        rbuf[index] = (char) rand();

    return length;
}

void work_with_ssl(SSL *ssl)
{
    if (SSL_connect(ssl) == -1)
        goto ERROR;

    print_cipher(ssl);
    printf("\n");

    print_certificate(ssl);
    printf("\n");

    srand(time(NULL));

    char wbuf[256];
    char rbuf[256];

    int wlen = genrand(wbuf, sizeof(wbuf));
    SSL_write(ssl, wbuf, wlen);
    int rlen = SSL_read(ssl, rbuf, sizeof(rbuf));

    print_result(wbuf, wlen, rbuf, rlen);

ERROR:
    ERR_print_errors_fp(stderr);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <hostname> <portnum>\n", argv[0]);
        exit(0);
    }

    char *hostname = argv[1];
    char *portnum = argv[2];
    int server = connect_server(hostname, atoi(portnum));

    // init OpenSSL
    SSL_library_init();
    SSL_load_error_strings();

    // create context object
    SSL_METHOD *method = TLSv1_2_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (ctx == NULL)
        goto ERROR;

    // create SSL connection state
    SSL *ssl = SSL_new(ctx);
    if (ssl == NULL)
        goto ERROR;

    // attach the socket descriptor
    SSL_set_fd(ssl, server);

    // handshake, write, read, compare
    work_with_ssl(ssl);
    
    SSL_free(ssl);      /* release connection state */
    close(server);      /* close socket */
    SSL_CTX_free(ctx);  /* release context */

    // clean up OpenSSL
    EVP_cleanup();
    ERR_free_strings();

    return 0;

ERROR:
    ERR_print_errors_fp(stderr);
    abort();
}