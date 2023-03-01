cminer:clean
	gcc src/cfpgaminer.c src/sha256d.c libs/cJSON/cJSON.c libs/Queue/queue_internal.c libs/Queue/queue.c src/config.c src/devices/f1/f1.c src/devices/sk1/sk1.c src/coins/radiant/radiant.c src/util.c src/wiringSerial.c -lcrypto -lcurl -lpthread -pthread -Isrc/include -o cfpgaminer_x86
	# arm-linux-gnueabi-gcc sha256d.c cfpgaminer.c cJSON.c config.c f1.c sk1.c queue_internal.c queue.c radiant.c util.c wiringSerial.c ../libs/arm-linux-gnueabi/curl-7.68.0/lib/libcurl.a ../libs/arm-linux-gnueabi/openssl-1.1.1f/lib/libcrypto.a ../libs/arm-linux-gnueabi/openssl-1.1.1f/lib/libssl.a ../libs/arm-linux-gnueabi/zlib-1.2.11/lib/libz.a -static -lpthread -pthread -ldl -lssl -lcrypto -DCURL_STATICLIB -I../libs/arm-linux-gnueabi/curl-7.68.0/include -I../libs/arm-linux-gnueabi/openssl-1.1.1f/include -Iinclude -L../libs/arm-linux-gnueabi/openssl-1.1.1f/lib -o cfpgaminer
.PHONY:clean
clean:
	rm -f cminer
