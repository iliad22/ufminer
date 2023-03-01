tests:clean
	# gcc test_sha512_256.c sha2big.c -o test_sha512_256
	gcc test_sha512_256.c sha2big.c -I../../libs/openssl-1.1.1f/include -lcrypto -o test_sha512_256
.PHONY:clean
clean:
	rm -f cminer
