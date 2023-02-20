// Include MicroPython API.
#include "py/runtime.h"

#include "py/mphal.h"
#include "extmod/machine_i2c.h"
#include "extmod/machine_i2c_extra.h"

// This is the function which will be called from Python as bmm150.prod_ints(a, b).
STATIC mp_obj_t bmm150_prod_ints(mp_obj_t a_obj, mp_obj_t b_obj) {
    // Extract the ints from the micropython input objects.
    int a = mp_obj_get_int(a_obj);
    int b = mp_obj_get_int(b_obj);

    // Calculate the addition and convert to MicroPython object.
    return mp_obj_new_int(a * b);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bmm150_prod_ints_obj, bmm150_prod_ints);



STATIC mp_obj_t bmm150_read(mp_obj_base_t *i2c_obj, uint16_t addr, uint32_t memaddr, uint8_t *buf) {
     read_mem(i2c_obj, addr, memaddr, 1, buf, 1);
     return mp_obj_new_int(buf[0]);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_4(bmm150_read_obj, bmm150_read);
//STATIC int read_mem(mp_obj_t self_in, uint16_t addr, uint32_t memaddr, uint8_t addrsize, uint8_t *buf, size_t len) {
//STATIC mp_obj_t machine_i2c_readfrom_mem_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
//    enum { ARG_addr, ARG_memaddr, ARG_n, ARG_addrsize };

STATIC mp_obj_t bmm150_write(mp_obj_base_t *i2c_obj, uint16_t addr, uint32_t memaddr, const uint8_t *buf) {
     write_mem(i2c_obj, addr, memaddr, 1, buf, 1);
     return mp_const_none;
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_4(bmm150_write_obj, bmm150_write);
//STATIC int write_mem(mp_obj_t self_in, uint16_t addr, uint32_t memaddr, uint8_t addrsize, const uint8_t *buf, size_t len) {
//STATIC mp_obj_t machine_i2c_writeto_mem(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
//    enum { ARG_addr, ARG_memaddr, ARG_buf, ARG_addrsize };





// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t bmm150_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bmm150) },
    { MP_ROM_QSTR(MP_QSTR_prod_ints), MP_ROM_PTR(&bmm150_prod_ints_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&bmm150_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&bmm150_write_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bmm150_module_globals, bmm150_module_globals_table);

// Define module object.
const mp_obj_module_t bmm150_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&bmm150_module_globals,
};

// Register the module to make it available in Python.
// Note: the "1" in the third argument means this module is always enabled.
// This "1" can be optionally replaced with a macro like MODULE_c2example_ENABLED
// which can then be used to conditionally enable this module.
MP_REGISTER_MODULE(MP_QSTR_bmm150, bmm150_user_cmodule, 1);
