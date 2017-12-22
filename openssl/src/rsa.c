#include <string.h>
#include <fcntl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

const int TEST_NUMBER = 10000;

RSA *create_rsa_public(char *keyfile)
{
    FILE *file = fopen(keyfile, "r");
    if (file == NULL)
    {
        perror("create_rsa_public:fopen");
        return NULL;
    }

    RSA *rsa = PEM_read_RSAPublicKey(file, NULL, NULL, NULL);
    if (rsa == NULL)
        ERR_print_errors_fp(stderr);
        
    fclose(file);
    return rsa;
}

RSA *create_rsa_private(char *keyfile)
{
    FILE *file = fopen(keyfile, "r");
    if (file == NULL)
    {
        perror("create_rsa_private:fopen");
        return NULL;
    }

    RSA *rsa = PEM_read_RSAPrivateKey(file, NULL, NULL, NULL);
    if (rsa == NULL)
        ERR_print_errors_fp(stderr);

    fclose(file);
    return rsa;
}

int encrypt(char *data, int len, char *keyfile, char *to)
{
    RSA *rsa = create_rsa_public(keyfile);
    if (rsa == NULL)
        return -1;

    int result = RSA_public_encrypt(len, data, to, rsa, RSA_PKCS1_OAEP_PADDING);
    if (result == -1)
        ERR_print_errors_fp(stderr);

    RSA_free(rsa);
    return result;
}

int decrypt(char *data, int len, char *keyfile, char *to)
{
    RSA *rsa = create_rsa_private(keyfile);
    if (rsa == NULL)
        return -1;

    int result = RSA_private_decrypt(len, data, to, rsa, RSA_PKCS1_OAEP_PADDING);
    if (result == -1)
        ERR_print_errors_fp(stderr);

    RSA_free(rsa);
    return result;
}

int genrand(char *buf, int len)
{
    int length = rand()%len + 1; // 1 to len

    int i;
    for (i = 0; i < length; i++)
        buf[i] = (char) rand();

    return length;
}

int compare(char *a, int alen, char *b, int blen)
{
    if (alen != blen)
        return 1;

    return memcmp(a, b, alen);
}

int max_original_size(char *keyfile)
{
    RSA *rsa = create_rsa_private(keyfile);
    if (rsa == NULL)
        return 0;

    int size = RSA_size(rsa) - 41 - 1; // length must be less than RSA_size(rsa) - 41 for RSA_PKCS1_OAEP_PADDING
    RSA_free(rsa);
    return size;
}

int max_encrypted_size(char *keyfile)
{
    RSA *rsa = create_rsa_private(keyfile);
    if (rsa == NULL)
        return 0;

    int size = RSA_size(rsa); // buffer must be RSA_size(rsa) bytes of memory.
    RSA_free(rsa);
    return size;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <private key file path> <public key file path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    char *prikeyfile = argv[1];
    char *pubkeyfile = argv[2];

    int max_org_size = max_original_size(prikeyfile);
    int max_buf_size = max_encrypted_size(prikeyfile);

    char original[max_org_size];
    char encrypted[max_buf_size];
    char decrypted[max_org_size];

    int olen = genrand(original, sizeof(original));
    int elen = encrypt(original, olen, pubkeyfile, encrypted);
    int dlen = decrypt(encrypted, elen, prikeyfile, decrypted);

    if (compare(original, olen, decrypted, dlen) == 0)
        printf("OK\n");
    else
        printf("NO\n");

    return 0;
}