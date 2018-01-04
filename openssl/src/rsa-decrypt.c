#include <string.h>
#include <fcntl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

static int padding_type = RSA_PKCS1_OAEP_PADDING;
static int min_padding = 41;

void check_error(int condition, char *message)
{
    if (condition)
    {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

void check_error_openssl(int condition, char *message)
{
    if (condition)
    {
        fprintf(stderr, "%s: ", message);
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void print(char *lbytes, int llen, char *rbytes, int rlen)
{
    int max = llen > rlen ? llen : rlen;
    int lines = max / 8 + (max % 8 > 0 ? 1 : 0);
    int line;
    for (line = 0; line < lines; line++)
    {
        int i;
        for (i = line * 8; i < (line + 1) * 8; i++)
        {
            if (i < llen)
                printf("%02x:", (unsigned char) lbytes[i]);
            else
                printf("   ");
        }

        printf(" ");

        for (i = line * 8; i < (line + 1) * 8; i++)
        {
            if (i < rlen)
                printf("%02x:", (unsigned char) rbytes[i]);
            else
                break;
        }

        printf("\n");
    }

    printf("\n");
}

void decrypt(char *privfilename, char *infilename, char *outfilename)
{
    // read public key
    FILE *privfile = fopen(privfilename, "r");
    check_error(privfile == NULL, "failed to open privfile");

    RSA *rsa = PEM_read_RSAPrivateKey(privfile, NULL, NULL, NULL);
    check_error_openssl(rsa == NULL, "failed to read privfile");

    fclose(privfile);

    // read input, decrypt, write decrypted
    FILE *infile = fopen(infilename, "r");
    check_error(infile == NULL, "failed to open infile");

    FILE *outfile = fopen(outfilename, "w");
    check_error(outfile == NULL, "failed to open outfile");

    int rsa_size = RSA_size(rsa);
    int max_size = rsa_size - min_padding - 1;

    printf("rsa_size = %d, max_size = %d\n", rsa_size, max_size);

    char encrypted[rsa_size];
    char decrypted[max_size];

    while (1)
    {
        size_t size = fread(encrypted, sizeof(char), rsa_size, infile);
        check_error(ferror(infile), "failed to read infile");
        if (feof(infile))
            break;

        // printf("read %ld\n", size);
        // print(encrypted, size);

        int result = RSA_private_decrypt(size, encrypted, decrypted, rsa, padding_type);
        check_error_openssl(result == -1, "failed to decrypt");

        // printf("write %d\n", result);
        // print(decrypted, result);

        fwrite(decrypted, sizeof(char), result, outfile);
        check_error(ferror(outfile), "failed to write outfile");

        print(encrypted, size, decrypted, result);
    }
    
    fclose(infile);
    fclose(outfile);
    
    RSA_free(rsa);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <private key file> <input file> <output file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *pubkeyfile = argv[1];
    char *inputfile = argv[2];
    char *outputfile = argv[3];

    decrypt(pubkeyfile, inputfile, outputfile);

    return 0;
}