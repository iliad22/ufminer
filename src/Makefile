cfpgaminer:clean
	gcc cfpgaminer.c sha256d.c cJSON.c config.c f1.c sk1.c queue_internal.c queue.c radiant.c util.c wiringSerial.c -lcrypto -lcurl -lpthread -pthread -Iinclude -o cfpgaminer_x86
	arm-linux-gnueabi-gcc sha256d.c cfpgaminer.c cJSON.c config.c f1.c sk1.c queue_internal.c queue.c radiant.c util.c wiringSerial.c ../libs/curl-7.68.0/lib/libcurl.a ../libs/openssl-1.1.1f/lib/libcrypto.a ../libs/openssl-1.1.1f/lib/libssl.a ../libs/zlib-1.2.11/lib/libz.a -static -lpthread -pthread -ldl -lssl -lcrypto -DCURL_STATICLIB -I../libs/curl-7.68.0/include -I../libs/openssl-1.1.1f/include -Iinclude -L../libs/openssl-1.1.1f/lib -o cfpgaminer
.PHONY:clean
clean:
	rm -f cfpgaminer
