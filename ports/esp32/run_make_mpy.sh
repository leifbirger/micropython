
cd ../../micropython
make -C mpy-cross

cd ports/esp32
 . ../../../esp-idf/export.sh
make BOARD=M5STICKC_PLUS USER_C_MODULES=../../../usercmodules/usercmodules/micropython.cmake clean
make submodules
make BOARD=M5STICKC_PLUS USER_C_MODULES=../../../usercmodules/usercmodules/micropython.cmake

