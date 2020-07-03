set -e

if [ ! -d brotli ]; then
  echo "Error: this script requires a checkout of https://github.com/google/brotli.git in ./brotli"
  exit 1
fi

function relocate_public_headers {
  sed -r -e 's/<brotli\/(.*)>/".\/\1"/g' -i $*
}

function relocate_common_headers {
  sed -r -e 's/\.\.\/common\//.\//g' -i *.c *.h
}

(
  cd brotli
  git reset --hard
  git checkout .
)

(
  cd brotli/c/include/brotli
  relocate_public_headers *.h
)

(
  cd brotli/c/common
  relocate_common_headers *.c *.h
  relocate_public_headers *.c *.h
)

(
  cd brotli/c/enc
  sed -r -e 's/\.\/prefix.h/.\/enc-prefix.h/g' -i *.c *.h
  relocate_common_headers *.c *.h
  relocate_public_headers *.c *.h
  mv prefix.h enc-prefix.h
)

(
  cd brotli/c/dec
  sed -r -e 's/\.\/prefix.h/.\/dec-prefix.h/g' -i *.c *.h
  relocate_common_headers *.c *.h
  relocate_public_headers *.c *.h
  mv prefix.h dec-prefix.h
)

rm -f *.h *.c
cp brotli/c/include/brotli/*.h .
cp brotli/c/common/{*.c,*.h} .
cp brotli/c/enc/{*.c,*.h} .
cp brotli/c/dec/{*.c,*.h} .
