pwd
cd ../..
make -C mpy-cross

cd ports/esp32
pwd
# . ../../../esp-idf/export.sh
source ../../../esp-idf/export.sh
pwd
make BOARD=M5STICKC_PLUS USER_C_MODULES=../../../usercmodules/usercmodules/micropython.cmake clean
make submodules
make BOARD=M5STICKC_PLUS USER_C_MODULES=../../../usercmodules/usercmodules/micropython.cmake

