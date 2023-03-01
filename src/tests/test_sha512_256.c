#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

int str2hex(char* str, unsigned char* out, unsigned int* outlen, unsigned maxlen) {
	char* p = str;
	char high = 0, low = 0;
	int tmplen = strlen(p), cnt = 0;
	tmplen = strlen(p);
	while (cnt < (tmplen / 2) && cnt < maxlen)
	{
		high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
		low = (*(++p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p)-48 - 7 : *(p)-48;
		out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
		p++;
		cnt++;
	}
	if (tmplen % 2 != 0) out[cnt] = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;

	if (outlen != NULL) *outlen = tmplen / 2 + tmplen % 2;
	return tmplen / 2 + tmplen % 2;
}

int main(int argc, char *argv[])
{
    unsigned char hash[SHA512_DIGEST_LENGTH];
    // char message[] = "This is a message to be hashed.";
    char message[] = "";
    int i;

    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    sha512.h[0] = 0x22312194FC2BF72C;
    sha512.h[1] = 0x9F555FA3C84C64C2;
    sha512.h[2] = 0x2393B86B6F53B151;
    sha512.h[3] = 0x963877195940EABD;
    sha512.h[4] = 0x96283EE2A88EFFE3;
    sha512.h[5] = 0xBE5E1E2553863992;
    sha512.h[6] = 0x2B0199FC2C85B8AA;
    sha512.h[7] = 0x0EB72DDC81C52CA2;
    SHA512_Update(&sha512, message, strlen(message));
    SHA512_Final(hash, &sha512);

    //c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a
    for (i = 0; i < SHA512_DIGEST_LENGTH/2; i++) {
        printf("%02x", hash[i]);
    }

    printf("\n");

    char hex_msg[] = "00000020a59d7e60ea6ec23700d2f0e27503930701916cf66cc257ea7306000000000000f84733f5b3cdf6a6da59f490cd8688b44db9e7a84d89d37eb1cfddc521131ced51cdd463ae9b071a41f430db";
    char msg[80];

    str2hex(hex_msg, msg, NULL, 80);
    SHA512_Init(&sha512);
    sha512.h[0] = 0x22312194FC2BF72C;
    sha512.h[1] = 0x9F555FA3C84C64C2;
    sha512.h[2] = 0x2393B86B6F53B151;
    sha512.h[3] = 0x963877195940EABD;
    sha512.h[4] = 0x96283EE2A88EFFE3;
    sha512.h[5] = 0xBE5E1E2553863992;
    sha512.h[6] = 0x2B0199FC2C85B8AA;
    sha512.h[7] = 0x0EB72DDC81C52CA2;
    SHA512_Update(&sha512, msg, 80);
    SHA512_Final(hash, &sha512);
    for (i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    SHA512_Init(&sha512);
    sha512.h[0] = 0x22312194FC2BF72C;
    sha512.h[1] = 0x9F555FA3C84C64C2;
    sha512.h[2] = 0x2393B86B6F53B151;
    sha512.h[3] = 0x963877195940EABD;
    sha512.h[4] = 0x96283EE2A88EFFE3;
    sha512.h[5] = 0xBE5E1E2553863992;
    sha512.h[6] = 0x2B0199FC2C85B8AA;
    sha512.h[7] = 0x0EB72DDC81C52CA2;
    SHA512_Update(&sha512, hash, 32);
    SHA512_Final(hash, &sha512);
    for (i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}
