# . ../../../esp-idf/export.sh
make clean
make USER_C_MODULES=../../../usercmodules/usercmodules/micropython.cmake
make BOARD=GENERIC_CAM
make BOARD=GENERIC_CAM USER_C_MODULES=../../../usercmodules/usercmodules/micropython.cmake

