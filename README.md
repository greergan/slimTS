# slimTS
slimTS is a typescript platform written in C++ for the c++20 standards. It takes in either typescript or javascript ES6 modules which are then type checked and then run.

There is an embedded tsconfig.json object which toggles specific type checking flags.

Use of the 'require' statement is not supported and never will be. This is a module only platform.

slimTS, so far, has only been built on Ubuntu desktop.

## Security
The security model is one of lazy-loaded plugins. Even the console is a plugin. This allows for different behavior from the same "object" under different circumstances.

## Things to understand
The below listed import statement is required otherwise the default "dummy" console is used. If you are not seeing output from your console statements then you probably have not imported the console.
```
import console from 'console'
```

As mentioned above, there is a tsconfig.json object embedded. Not that the "outDir" is an internal virtual filesystem location.
```
const tsconfig = {
    "compilerOptions": {
        "target": "es2024",
        "module": "es2022",
        "types": [],
        "allowJs": true,
        "checkJs": true,
        "outDir": "built",
        "removeComments": true,
        "forceConsistentCasingInFileNames": true,
        "strict": true,
        "noUnusedLocals": true,
        "noUnusedParameters": true,
        "noFallthroughCasesInSwitch": true,
        "noImplicitOverride": true,
        "skipDefaultLibCheck": true
    }
}
```
## Prerequisites
Google v8  
Several steps are required to get the things into a compiled state  
1. Install, configure and fetch the v8 source using [these instructions](https://v8.dev/docs/source-code#using-git)  
2. Build the v8 engine using [the first 4 steps of these instructions](https://v8.dev/docs/embed#run-the-example) This is an extremely long build.
3. Checkout branch 13.1.1 prior to running the build
```
git checkout 13.1.1
```
librdkafa from Confluent Inc.  
1. clone the source from [GitHub](https://github.com/confluentinc/librdkafka)
  librdkafka - compiled and installed, it must be configured as follows along with [these requirements](https://github.com/confluentinc/librdkafka?tab=readme-ov-file#build-from-source). slimTS does not support the optional packages that are listed at this time.
```
./configure --enable-static --prefix=/path/to/install/to
```
[boringssl](https://boringssl.googlesource.com/boringssl) by Google  
The slimTS ssl plugin is linked against the boringssl libraries
```
libssl libpki libcrypto
```
The following header files are used
```
#include <openssl/rsa.h>
#include <openssl/x509.h>
```
The build may work with out of the box [OpenSSL](https://www.openssl.org/) but has not been attempted so far.  
Either way, the plugin does compile but is not in a usable state.

slimTS uses standard installation paths when searching for the required header and library files.

## Building slimTS
slimTS uses the [GNU Autoconf](https://www.gnu.org/software/autoconf/) build configuration tools along with [GNU Make](https://www.gnu.org/software/make/). So far, it has only been built using [GCC](https://gcc.gnu.org/)  
1. Clone or download the slimTS source from [GitHub](https://github.com/greergan/slimTS)
```
git clone git@github.com:greergan/slimTS.git
```
2. Run autoreconf from inside the slimTS root directory
```
autoreconf -vfi
```
3. Run the configure script
```
./configure --prefix=/path/to/install \
    --with-google-v8-dir=/path/to/google/v8 \
			--with-librdkafka=/path/to/installed/librdkafka
```  
4. Run the make command
```
make or make install
```
The output executable is named slim. It expects to be installed properly so that it can find its plugin files. slim will look for the plugin files in a lib directory named slimTS  
```
/usr/local/bin/slim
/usr/local/lib/slimTS
```

5. Run a script
```
slim samples/hello_world.mjs
```

##Plugin modules  
The bare minimum of functionality is present in most plugins. There will be an attempt to match standard Node.js interfaces over time.
- boringssl
- console
- csv
- fs
- kafka
- memoryAdaptor
- network
- os
- path
- process
