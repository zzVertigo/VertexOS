BINUTILS_VERSION=2.34
GCC_VERSION=9.2.0

set -e

function main {
    downloadSources
}

function downloadSources {
    echo "Downloading all sources..."

    downloadAndExtract "binutils" $BINUTILS_VERSION
    downloadAndExtract "gcc" $GCC_VERSION "http://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz"

    mkdir build-gcc build-binutils

    mv gcc-$GCC_VERSION gcc
    mv binutils-$BINUTILS_VERSION binutils

    cd gcc

    ./contrib/download_prerequisites

    cd ..

    compileBinutils
    compileGCC
}

function downloadAndExtract {
    name=$1
    version=$2
    override=$3

    pwd

    echo "Processing $name-$version"

    if [ ! -f $name-$version.tar.gz ] 
    then
        echo "Downloading $name-$version.tar.gz"

        if [ -z $3 ]
        then
            wget http://ftp.gnu.org/gnu/$name/$name-$version.tar.gz
        else
            wget $override
        fi
    else
        echo "$name-$version.tar.gz already exists"
    fi

    tar -xf $name-$version.tar.gz
}

function compileBinutils {
    echo "Compiling Biniutls $BINUTILS_VERSION"

    cd build-binutils

    configArgs="--target=i686-elf --with-sysroot --disable-nls --disable-werror --prefix=../../vertex-toolchain/"

    ../binutils/configure $configArgs

    cd ..
}

function compileGCC {
    echo "Compiling GCC $GCC_VERSION"

    configArgs="--target=i686-elf --disable-nls --enable-languages=c,c++ --without-headers --prefix=../../vertex-toolchain/"

    cd build-gcc

    ../gcc/configure $configArgs
}

main