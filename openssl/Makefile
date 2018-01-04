all: rsa ssl

bin/:
	mkdir -p bin

rsa: bin/ bin/rsa-encrypt bin/rsa-decrypt

bin/rsa-encrypt: src/rsa-encrypt.c
	gcc src/rsa-encrypt.c -lssl -lcrypto -o bin/rsa-encrypt

bin/rsa-decrypt: src/rsa-decrypt.c
	gcc src/rsa-decrypt.c -lssl -lcrypto -o bin/rsa-decrypt

ssl: bin/ bin/ssl-server bin/ssl-client

bin/ssl-server: src/ssl-server.c
	gcc src/ssl-server.c -lssl -lcrypto -o bin/ssl-server

bin/ssl-client: src/ssl-client.c
	gcc src/ssl-client.c -lssl -lcrypto -o bin/ssl-client

key: key/ rsa-key ssl-key

key/:
	mkdir -p key

rsa-key: key/key.pem key/key.pub.pem

key/key.pem:
	openssl genrsa -out key/key.pem

key/key.pub.pem: key/key.pem
	openssl rsa -in key/key.pem -out key/key.pub.pem -RSAPublicKey_out

ssl-key:
	openssl req \
          -newkey rsa:2048 -sha256 -nodes -keyout key/localhost.key \
          -x509 -days 365 -out key/localhost.crt

clean:
	rm -rf bin key 
