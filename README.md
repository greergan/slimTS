# slim
A Typescript Platform

## Building
$ autoreconf -vfi  

$ ./configure --prefix=/path/to/instal/root --with-google-v8-dir=/path/to/google/v8 --with-librdkafka=/path/to/installed/librdkafka

$ make


### build librdkafka
./configure --enable-static --prefix=/home/greergan/product/.local


./configure --prefix=/home/greergan/product/.local --with-google-v8-dir=/home/greergan/product/google/v8 --with-librdkafka=/home/greergan/product/.local

./configure --prefix=$PRODUCT/local --with-google-v8-dir=$PRODUCT/google/v8 --with-librdkafka=$PRODUCT/local