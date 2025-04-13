# slim
A Typescript Platform

## Building
$ autoreconf -vfi  

$ ./configure --prefix=/path/to/instal/root --with-google-v8-dir=/path/to/google/v8 --with-librdkafka=/path/to/installed/librdkafka

$ make


### build librdkafka
./configure --enable-static --prefix=/home/greergan/product/.local