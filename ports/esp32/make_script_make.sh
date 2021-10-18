# . ../../../esp-idf/export.sh
make  BOARD=M5STICKC_PLUS USER_C_MODULES=../../../usercmodules/usercmodules/micropython.cmake
make  BOARD=ESP32_CAM USER_C_MODULES=../../../usercmodules/usercmodules/micropython.cmake

