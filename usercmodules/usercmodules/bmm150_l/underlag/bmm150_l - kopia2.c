// Include MicroPython API.

#define __BMM150_VERSION__  "0.1.1"

#include "py/runtime.h"

#include "py/mphal.h"
#include "extmod/machine_i2c.h"
#include "extmod/machine_i2c_extra.h"

#include "bmm150_l.h"

// C functions
STATIC int prod_ints(int a, int b) {
    int result;
	result = a*b;
    return result;
}

STATIC int sum_ints(int a, int b) {
    int result;
	result = a+b;
    return result;
}

STATIC int read_i2c(mp_obj_t i2c_obj, uint16_t addr, uint32_t memaddr) { 
     uint8_t value;
	 uint8_t memaddrsize = 8;
     read_mem(i2c_obj, addr, memaddr, memaddrsize, &value, (size_t)1);
     return (int)value;
}

STATIC void write_i2c(mp_obj_t i2c_obj, uint16_t addr, uint32_t memaddr, uint8_t value) {      
     uint8_t value_local =(uint8_t)value;
	 uint8_t memaddrsize = 8;
     write_mem(i2c_obj, addr, memaddr, memaddrsize, &value_local, (size_t)1);
}

//    bmm150 C code

// Bosch compensation functions
static float _bmm050_compensate_X_float(bmm150_context dev,
                                        int16_t mag_data_x)
{
    assert(dev != NULL);

    uint16_t data_r = dev->hall;
    float inter_retval = 0;

    if (mag_data_x != -4096	/* no overflow */
        ) {
        if ((data_r != 0)
            && (dev->dig_xyz1 != 0)) {
            inter_retval = ((((float)dev->dig_xyz1)
                             * 16384.0 / data_r) - 16384.0);
        } else {
            inter_retval = 0.0f;
            return inter_retval;
        }
        inter_retval = (((mag_data_x * ((((((float)dev->dig_xy2) *
                                           (inter_retval*inter_retval /
                                            268435456.0) +
                                           inter_retval * ((float)dev->dig_xy1)
                                           / 16384.0)) + 256.0) *
                                        (((float)dev->dig_x2) + 160.0)))
                         / 8192.0)
                        + (((float)dev->dig_x1) *
                           8.0)) / 16.0;
    } else {
        inter_retval = 0.0f;
    }
    return inter_retval;
}


static float _bmm050_compensate_Y_float(bmm150_context dev,
                                        int16_t mag_data_y)
{
    assert(dev != NULL);

    uint16_t data_r = dev->hall;
    float inter_retval = 0;

    if (mag_data_y != -4096 /* no overflow */
        ) {
        if ((data_r != 0)
            && (dev->dig_xyz1 != 0)) {
            inter_retval = ((((float)dev->dig_xyz1)
                             * 16384.0
                             /data_r) - 16384.0);
        } else {
            inter_retval = 0.0f;
            return inter_retval;
        }
        inter_retval = (((mag_data_y * ((((((float)dev->dig_xy2) *
                                           (inter_retval*inter_retval
                                            / 268435456.0) +
                                           inter_retval * ((float)dev->dig_xy1)
                                           / 16384.0)) +
                                         256.0) *
                                        (((float)dev->dig_y2) + 160.0)))
                         / 8192.0) +
                        (((float)dev->dig_y1) * 8.0))
            / 16.0;
    } else {
        /* overflow, set output to 0.0f */
        inter_retval = 0.0f;
    }
    return inter_retval;
}

static float _bmm050_compensate_Z_float(bmm150_context dev,
                                        int16_t mag_data_z)
{
    assert(dev != NULL);

    uint16_t data_r = dev->hall;
    float inter_retval = 0;
    /* no overflow */
    if (mag_data_z != -16384) {
        if ((dev->dig_z2 != 0)
            && (dev->dig_z1 != 0)
            && (dev->dig_xyz1 != 0)
            && (data_r != 0)) {
            inter_retval = ((((((float)mag_data_z)-
                               ((float)dev->dig_z4)) * 131072.0)-
                             (((float)dev->dig_z3)*(((float)data_r)
                                                    -((float)dev->dig_xyz1))))
                            /((((float)dev->dig_z2)+
                               ((float)dev->dig_z1)*((float)data_r) /
                               32768.0) * 4.0)) / 16.0;
        }
    } else {
        /* overflow, set output to 0.0f */
        inter_retval = 0.0f;
    }
    return inter_retval;
}


void bmm150_get_magnetometer(const bmm150_context dev,
                             float *x, float *y, float *z) {
    assert(dev != NULL);

    if (x)
        *x = dev->magX;

    if (y)
        *y = dev->magY;

    if (z)
        *z = dev->magZ;
}
							 
//STATIC void update(*dev,



// Python modules functions/definitions  (e.g. classes)

// Python Class definitions
// this is the actual C-structure for our new object
typedef struct _bmm150_BMM150_obj_t {
    mp_obj_base_t base;
    mp_obj_base_t *i2c_obj;
	uint16_t addr;
    int16_t a;
    int16_t b;
	bmm150_context_struct dev;
} bmm150_BMM150_obj_t;

// Just definition (,ust be before locals dict when it´s use there)
STATIC mp_obj_t bmm150_BMM150_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

STATIC void bmm150_BMM150_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    bmm150_BMM150_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_print_str(print, "BMM150 class(");
    mp_obj_print_helper(print, mp_obj_new_int(self->addr), PRINT_REPR);
    mp_print_str(print, ")");
}

// Python class module functions

// This is the function which will be called from Python as bmm150.prod_ints(a, b).

STATIC mp_obj_t bmm150_BMM150_prod_ints(mp_obj_t self_in, mp_obj_t a_obj, mp_obj_t b_obj) {
    bmm150_BMM150_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int a = mp_obj_get_int(a_obj);
    int b = mp_obj_get_int(b_obj);
	self->a = a;
	self->b = b;
    // Call C function
    int result = prod_ints(a, b);
    return mp_obj_new_int(result);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_3(bmm150_BMM150_prod_ints_obj, bmm150_BMM150_prod_ints);

STATIC mp_obj_t bmm150_BMM150_sum_ints(mp_obj_t self_in) {
    bmm150_BMM150_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int a = self->a;
    int b = self->b;
    // Call C function
    int result = sum_ints(a, b);
    return mp_obj_new_int(result);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bmm150_BMM150_sum_ints_obj, bmm150_BMM150_sum_ints);

STATIC mp_obj_t bmm150_BMM150_read_byte(mp_obj_t self_in, mp_obj_t memaddr_obj) {
     bmm150_BMM150_obj_t *self = MP_OBJ_TO_PTR(self_in);
	 uint16_t addr = self->addr;
     uint32_t memaddr = (uint32_t)mp_obj_get_int(memaddr_obj);  
	 // Call C function
	 int result = read_i2c(self->i2c_obj, addr, memaddr);
     return mp_obj_new_int(result);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_2(bmm150_BMM150_read_byte_obj, bmm150_BMM150_read_byte);

STATIC mp_obj_t bmm150_BMM150_write_byte(size_t n_args, const mp_obj_t *args) {
     bmm150_BMM150_obj_t *self = MP_OBJ_TO_PTR(args[0]);
     uint16_t addr = self->addr;
     uint32_t memaddr = (uint32_t)mp_obj_get_int(args[1]);
     uint8_t value =(uint8_t)mp_obj_get_int(args[2]);
	 // Call C function
	 write_i2c(self->i2c_obj, addr, memaddr, value);
     return mp_const_none;
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bmm150_BMM150_write_byte_obj, 3, 3, bmm150_BMM150_write_byte);

//   BMM starts here  update()  init() via make_new

STATIC mp_obj_t bmm150_BMM150_get_magnetometer(mp_obj_t self_in) {
    bmm150_BMM150_obj_t *self = MP_OBJ_TO_PTR(self_in);
    float x;
	float y;
	float z;
    // Call C function
	bmm150_get_magnetometer( &(self->dev), &x, &y, &z);
	mp_obj_t tuple[3] = {
         mp_obj_new_float(x),
         mp_obj_new_float(y),
         mp_obj_new_float(z),
    };
    return mp_obj_new_tuple(3, tuple);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bmm150_BMM150_get_magnetometer_obj, bmm150_BMM150_get_magnetometer);



/* methods end */

// Class modules registration
STATIC const mp_rom_map_elem_t bmm150_BMM150_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_prod_ints),  MP_ROM_PTR(&bmm150_BMM150_prod_ints_obj) },
    { MP_ROM_QSTR(MP_QSTR_sum_ints),   MP_ROM_PTR(&bmm150_BMM150_sum_ints_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_byte),  MP_ROM_PTR(&bmm150_BMM150_read_byte_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_byte), MP_ROM_PTR(&bmm150_BMM150_write_byte_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_magnetometer), MP_ROM_PTR(&bmm150_BMM150_get_magnetometer_obj) },
};
STATIC MP_DEFINE_CONST_DICT(bmm150_BMM150_locals_dict, bmm150_BMM150_locals_dict_table);

const mp_obj_type_t bmm150_BMM150_type = {
    { &mp_type_type },
    .name = MP_QSTR_BMM150,
    .print = bmm150_BMM150_print,
    .make_new = bmm150_BMM150_make_new,
    .locals_dict = (mp_obj_dict_t*)&bmm150_BMM150_locals_dict,
};


STATIC mp_obj_t bmm150_BMM150_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, true);
	
    // create new object
    bmm150_BMM150_obj_t *self = m_new_obj(bmm150_BMM150_obj_t);
    self->base.type = &bmm150_BMM150_type;
	
    // set parameters
//  mp_obj_base_t *spi_obj = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[ARG_spi].u_obj);
    mp_obj_base_t *i2c_obj = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    self->i2c_obj = i2c_obj;
    self->addr = mp_obj_get_int(args[1]);
	self->a = 3;
	self->b = 3;
    return MP_OBJ_FROM_PTR(self);
}





// Modules registration  (e.g. the class)



// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t bmm150_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_bmm150) },
    { MP_ROM_QSTR(MP_QSTR_BMM150), (mp_obj_t)&bmm150_BMM150_type },
};
STATIC MP_DEFINE_CONST_DICT(bmm150_module_globals, bmm150_module_globals_table);

// Define module object.
const mp_obj_module_t bmm150_user_cmodule = {  //mp_module_st7789
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&bmm150_module_globals,
};

// Register the module to make it available in Python.
// Note: the "1" in the third argument means this module is always enabled.
// This "1" can be optionally replaced with a macro like MODULE_c2example_ENABLED
// which can then be used to conditionally enable this module.
MP_REGISTER_MODULE(MP_QSTR_bmm150, bmm150_user_cmodule, 1);
