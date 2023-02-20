// Include MicroPython API.

#define __BMM150_VERSION__  "0.1.1"

#include "py/runtime.h"

#include "py/mphal.h"
#include "extmod/machine_i2c.h"
#include "extmod/machine_i2c_extra.h"

#include "bmm150_l.h"

#include <stdio.h>

#define mp_hal_delay_ms(delay)  (mp_hal_delay_us(delay * 1000))
#define upm_delay_ms(delay)  (mp_hal_delay_ms(delay))

#define UPM_ERROR_OPERATION_FAILED 1
#define UPM_SUCCESS 0

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


// These trimming algorithms (bmm050_bosch_compensate_*()) are taken
// from the Bosch BMM050 reference driver code.  See license.txt.

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


// read trim data
static upm_result_t _bmm150_read_trim_data(const bmm150_context dev)
{
    assert(dev != NULL);

    int bufLen = 10;
    uint8_t calibData[bufLen];

    // 2 bytes first
    if (bmm150_read_regs(dev, BMM150_REG_TRIM_DIG_X1, calibData, 2) != 2)
        return UPM_ERROR_OPERATION_FAILED;

    dev->dig_x1 = (int8_t)calibData[0];
    dev->dig_y1 = (int8_t)calibData[1];

    // next block of 4 bytes
    if (bmm150_read_regs(dev, BMM150_REG_TRIM_DIG_Z4_LSB, calibData, 4) != 4)
        return UPM_ERROR_OPERATION_FAILED;

    dev->dig_z4 = (int16_t)((calibData[1] << 8) | calibData[0]);
    dev->dig_x2 = (int8_t)calibData[2];
    dev->dig_y2 = (int8_t)calibData[3];

    // final block of 10 bytes
    if (bmm150_read_regs(dev, BMM150_REG_TRIM_DIG_Z2_LSB, calibData, 10) != 10)
        return UPM_ERROR_OPERATION_FAILED;

    dev->dig_z2 = (int16_t)((calibData[1] << 8) | calibData[0]);
    dev->dig_z1 = (uint16_t)((calibData[3] << 8) | calibData[2]);
    dev->dig_xyz1 = (uint16_t)((calibData[5] << 8) | calibData[4]);
    dev->dig_z3 = (int16_t)((calibData[7] << 8) | calibData[6]);
    dev->dig_xy2 = (int8_t)calibData[8];
    dev->dig_xy1 = calibData[9];

    return UPM_SUCCESS;
}

// init
upm_result_t  bmm150_init(bmm150_context dev)
{
     //  dev init


    // power bit must be on for chip ID to be accessible
    bmm150_set_power_bit(dev, true);

    // not really, just need to set it to something valid until
    // bmm150_set_opmode() is called in bmm150_devinit().
    dev->opmode = BMM150_OPERATION_MODE_SLEEP;

    upm_delay_ms(50);
	//mp_hal_delay_ms(50);

    // check the chip id

    uint8_t chipID = bmm150_get_chip_id(dev);

    if (chipID != BMM150_DEFAULT_CHIPID)
    {
        //printf("%s: invalid chip id: %02x.  Expected %02x\n",
        //       __FUNCTION__, chipID, BMM150_DEFAULT_CHIPID);
        assert(false);
        return UPM_ERROR_OPERATION_FAILED;
    }

    // call devinit with a default high resolution mode
	if (bmm150_devinit(dev, BMM150_USAGE_HIGH_ACCURACY))
    {
        //printf("%s: bmm150_devinit() failed.\n", __FUNCTION__);
        //bmm150_close(dev);
        assert(false);
        return UPM_ERROR_OPERATION_FAILED;
    }
	
    return UPM_SUCCESS;
}



upm_result_t bmm150_devinit(const bmm150_context dev,
                            BMM150_USAGE_PRESETS_T usage)
{
    assert(dev != NULL);

    // just in case...
    if (bmm150_set_power_bit(dev, true))
    {
        //printf("%s: bmm150_set_power_bit() failed.\n", __FUNCTION__);
        assert(false);
        return UPM_ERROR_OPERATION_FAILED;
    }

    // get trim data
    if (_bmm150_read_trim_data(dev))
    {
        //printf("%s: _bmm150_read_trim_data() failed.\n", __FUNCTION__);
        assert(false);
        return UPM_ERROR_OPERATION_FAILED;
    }

    if (bmm150_set_opmode(dev, BMM150_OPERATION_MODE_NORMAL))
    {
        //printf("%s: bmm150_set_opmode() failed.\n", __FUNCTION__);
        assert(false);
        return UPM_ERROR_OPERATION_FAILED;
    }

    upm_delay_ms(50); // 50ms, in case we are waking up

    if (bmm150_set_preset_mode(dev, usage))
    {
        //printf("%s: bmm150_set_preset_mode() failed.\n", __FUNCTION__);
        assert(false);
        return UPM_ERROR_OPERATION_FAILED;
    }

    // settle
    upm_delay_ms(50);

    return UPM_SUCCESS;
}

upm_result_t bmm150_update(const bmm150_context dev)
{
    assert(dev != NULL);

    // special care when in a forced mode - need to trigger a
    // measurement, and wait for the opmode to return to OPMODE_SLEEP,
    // then we can read the values.

    if (dev->opmode == BMM150_OPERATION_MODE_FORCED)
    {
        // trigger measurement
        if (bmm150_set_opmode(dev, BMM150_OPERATION_MODE_FORCED))
        {
            //printf("%s: bmm150_set_opmode() failed.\n", __FUNCTION__);
            assert(false);
            return UPM_ERROR_OPERATION_FAILED;
        }

        // opmode will return to BMM150_OPERATION_MODE_SLEEP after
        // measurement is complete
        do {
            upm_delay_ms(5);
        } while (bmm150_get_opmode(dev) == BMM150_OPERATION_MODE_FORCED);
    }

    const int bufLen = 8;
    uint8_t buf[bufLen];

    if (bmm150_read_regs(dev, BMM150_REG_MAG_X_LSB, buf, bufLen) != bufLen)
    {
        //printf("%s: bmm150_read_regs() failed.\n", __FUNCTION__);
        assert(false);
        return UPM_ERROR_OPERATION_FAILED;
    }

    // we need to get the hall data first, since it's needed for the
    // bosch compensation functions for each of the xyz axes

    dev->hall = (uint16_t)(buf[7] << 8 | (buf[6] &
                                          (_BMM150_MAG_RHALL_LSB_LSB_MASK <<
                                           _BMM150_MAG_RHALL_LSB_LSB_SHIFT)));
    dev->hall /= 4;

    int16_t val;

    // x
    val = (int16_t)(buf[1] << 8 | (buf[0] & (_BMM150_MAG_XY_LSB_LSB_MASK <<
                                             _BMM150_MAG_XY_LSB_LSB_SHIFT)));
    val /= 8;
    dev->magX = _bmm050_compensate_X_float(dev, val);

    // y
    val = (int16_t)(buf[3] << 8 | (buf[2] & (_BMM150_MAG_XY_LSB_LSB_MASK <<
                                             _BMM150_MAG_XY_LSB_LSB_SHIFT)));
    val /= 8;
    dev->magY = _bmm050_compensate_Y_float(dev, val);

    // z
    val = (int16_t)(buf[5] << 8 | (buf[4] & (_BMM150_MAG_Z_LSB_LSB_MASK <<
                                             _BMM150_MAG_Z_LSB_LSB_SHIFT)));
    val /= 2;
    dev->magZ = _bmm050_compensate_Z_float(dev, val);

    return UPM_SUCCESS;
}








uint8_t bmm150_read_reg(const bmm150_context dev, uint8_t reg)
{
    assert(dev != NULL);

     uint8_t value_local;
	 uint8_t memaddrsize = 8;
     read_mem(dev->i2c, dev->addr, (uint16_t)reg, memaddrsize, &value_local, (size_t)1);
     return (int)value_local;
}


int bmm150_read_regs(const bmm150_context dev, uint8_t reg,
                     uint8_t *buffer, int len)
{
    assert(dev != NULL);
    for (int i=0; i<len; i++)
        buffer[i] = bmm150_read_reg(dev, reg+i);

    return len;
}


upm_result_t bmm150_write_reg(const bmm150_context dev,
                              uint8_t reg, uint8_t val) {
     uint8_t value_local =(uint8_t)val;
	 uint8_t memaddrsize = 8;
	 write_mem(dev->i2c, dev->addr, (uint16_t)reg, memaddrsize, &value_local, (size_t)1);
	 return UPM_SUCCESS;
}

uint8_t bmm150_get_chip_id(const bmm150_context dev)
{
    assert(dev != NULL);

    return bmm150_read_reg(dev, BMM150_REG_CHIP_ID);
}


void bmm150_print_debug(const bmm150_context dev) {
	printf("\n BMM150 Debug\n");
	printf("hall: %u \n", (unsigned int)dev->hall);
	printf("dig_x1: %i \n", (int)dev->dig_x1);
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

    if(dev->debug)
        bmm150_print_debug(dev);

}
		


		
upm_result_t bmm150_set_output_data_rate(const bmm150_context dev,
                                         BMM150_DATA_RATE_T odr)
{
    assert(dev != NULL);

    uint8_t reg = bmm150_read_reg(dev, BMM150_REG_OPMODE);

    reg &= ~(_BMM150_OPMODE_DATA_RATE_MASK << _BMM150_OPMODE_DATA_RATE_SHIFT);
    reg |= (odr << _BMM150_OPMODE_DATA_RATE_SHIFT);

    if (bmm150_write_reg(dev, BMM150_REG_OPMODE, reg))
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}

upm_result_t bmm150_set_power_bit(const bmm150_context dev, bool power)
{
    assert(dev != NULL);

    // mask off reserved bits
    uint8_t reg =
        (bmm150_read_reg(dev, BMM150_REG_POWER_CTRL)
         & ~_BMM150_POWER_CTRL_RESERVED_BITS);

    if (power)
        reg |= BMM150_POWER_CTRL_POWER_CTRL_BIT;
    else
        reg &= ~BMM150_POWER_CTRL_POWER_CTRL_BIT;

    if (bmm150_write_reg(dev, BMM150_REG_POWER_CTRL, reg))
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}

upm_result_t bmm150_set_opmode(const bmm150_context dev,
                               BMM150_OPERATION_MODE_T opmode)
{
    assert(dev != NULL);

    uint8_t reg = bmm150_read_reg(dev, BMM150_REG_OPMODE);

    reg &= ~(_BMM150_OPMODE_OPERATION_MODE_MASK
             << _BMM150_OPMODE_OPERATION_MODE_SHIFT);
    reg |= (opmode << _BMM150_OPMODE_OPERATION_MODE_SHIFT);

    if (bmm150_write_reg(dev, BMM150_REG_OPMODE, reg))
        return UPM_ERROR_OPERATION_FAILED;

    dev->opmode = opmode;
    return UPM_SUCCESS;
}

BMM150_OPERATION_MODE_T bmm150_get_opmode(const bmm150_context dev)
{
    assert(dev != NULL);

    uint8_t reg = bmm150_read_reg(dev, BMM150_REG_OPMODE);

    reg &= (_BMM150_OPMODE_OPERATION_MODE_MASK
            << _BMM150_OPMODE_OPERATION_MODE_SHIFT);
    reg >>=  _BMM150_OPMODE_OPERATION_MODE_SHIFT;

    return (BMM150_OPERATION_MODE_T)reg;
}

upm_result_t bmm150_set_repetitions_xy(const bmm150_context dev,
                                       uint8_t reps)
{
    assert(dev != NULL);

    if (bmm150_write_reg(dev, BMM150_REG_REP_XY, reps))
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}

upm_result_t bmm150_set_repetitions_z(const bmm150_context dev,
                                      uint8_t reps)
{
    assert(dev != NULL);

    if (bmm150_write_reg(dev, BMM150_REG_REP_Z, reps))
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}

upm_result_t bmm150_set_preset_mode(const bmm150_context dev,
                                    BMM150_USAGE_PRESETS_T usage)
{
    assert(dev != NULL);

    bool error = false;

    // these recommended presets come from the datasheet, Table 3,
    // Section 4.2
    switch (usage)
    {
    case BMM150_USAGE_LOW_POWER:
        if (bmm150_set_repetitions_xy(dev, 3)
            || bmm150_set_repetitions_z(dev, 3)
            || bmm150_set_output_data_rate(dev, BMM150_DATA_RATE_10HZ))
            error = true;

        break;

    case BMM150_USAGE_REGULAR:
        if (bmm150_set_repetitions_xy(dev, 9)
            || bmm150_set_repetitions_z(dev, 15)
            || bmm150_set_output_data_rate(dev, BMM150_DATA_RATE_10HZ))
            error = true;

        break;

    case BMM150_USAGE_ENHANCED_REGULAR:
        if (bmm150_set_repetitions_xy(dev, 15)
            || bmm150_set_repetitions_z(dev, 27)
            || bmm150_set_output_data_rate(dev, BMM150_DATA_RATE_10HZ))
            error = true;

        break;

    case BMM150_USAGE_HIGH_ACCURACY:
        if (bmm150_set_repetitions_xy(dev, 47)
            || bmm150_set_repetitions_z(dev, 83)
            || bmm150_set_output_data_rate(dev, BMM150_DATA_RATE_20HZ))
            error = true;

        break;

    default:
        printf("%s: Invalid usage value passed.\n", __FUNCTION__);
        error = true;
    }

    if (error)
        return UPM_ERROR_OPERATION_FAILED;

    return UPM_SUCCESS;
}





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
	uint16_t debug;
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

STATIC mp_obj_t bmm150_BMM150_update(mp_obj_t self_in) {
    bmm150_BMM150_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Call C function
	bmm150_update( &(self->dev));
    return mp_const_none;
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bmm150_BMM150_update_obj, bmm150_BMM150_update);

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
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&bmm150_BMM150_update_obj) },
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
    mp_arg_check_num(n_args, n_kw, 2, 3, true);
	
    // create new object
    bmm150_BMM150_obj_t *self = m_new_obj(bmm150_BMM150_obj_t);
    self->base.type = &bmm150_BMM150_type;
	
    // set parameters
//  mp_obj_base_t *spi_obj = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[ARG_spi].u_obj);
    mp_obj_base_t *i2c_obj = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    self->i2c_obj = i2c_obj;
    self->addr = mp_obj_get_int(args[1]);
	self->a = 3;
	self->b = 7;
	
	self->dev.i2c = self->i2c_obj;
	self->dev.addr = self->addr;
	self->debug = 0;
	self->dev.debug = false;
	if(n_args > 2) {
		self->debug = 1;
		self->dev.debug = true;
	}
	bmm150_init( &(self->dev));
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
