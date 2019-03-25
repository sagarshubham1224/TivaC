/*
 * MPU9250_REGISTERS.h
 *
 *  Created on: 31-Jul-2018
 *      Author: sagarshubham
 */

#ifndef MPU9250_REGISTERS_H_
#define MPU9250_REGISTERS_H_



//  Registers which store self test output value.
#define MPU9250_SELFTEST_XGYRO_REGISTER                 0x00
#define MPU9250_SELFTEST_YGYRO_REGISTER                 0x01
#define MPU9250_SELFTEST_ZGYRO_REGISTER                 0x02
#define MPU9250_SELFTEST_XACCEL_REGISTER                0x0D
#define MPU9250_SELFTEST_YACCEL_REGISTER                0x0E
#define MPU9250_SELFTEST_ZACCEL_REGISTER                0x0F

// Gyro Offset Registers
#define MPU9250_XG_OFFSET_H_REGISTER                    0x13
#define MPU9250_XG_OFFSET_L_REGISTER                    0x14
#define MPU9250_YG_OFFSET_H_REGISTER                    0x15
#define MPU9250_YG_OFFSET_L_REGISTER                    0x16
#define MPU9250_ZG_OFFSET_H_REGISTER                    0x17
#define MPU9250_ZG_OFFSET_L_REGISTER                    0x18

// Sample Rate Divider Register
#define MPU9250_SAMPLE_RATE_DIVIDER_REGISTER            0x19    // Divides the internal sample rate (see register CONFIG) to generate the
                                                                // sample rate that controls sensor data output rate, FIFO sample rate.
                                                                // NOTE: This register is only effective when Fchoice = 2’b11 (fchoice_b
                                                                // register bits are 2’b00), and (0 < dlpf_cfg < 7), such that the average filter’s
                                                                // output is selected.
                                                                // This is the update rate of sensor register.
                                                                // SAMPLE_RATE = Internal_Sample_Rate / (1 + SMPLRT_DIV)

// Config Register
#define MPU9250_CONFIG_REGISTER                         0x1A    // Register to Set
                                                                // 1. FIFO Mode,
                                                                // 2. FSYNC Data Capture
                                                                // 3. Digital Low Pass Filter Settings(along with Gyro Config Register)

// Gyro Config Register
#define MPU9250_GYRO_CONFIG_REGISTER                    0x1B    // Register to Set
                                                                // 1. Self Test Along X, Y, and Z Gyros.
                                                                // 2. Gyro Full Scale Value.
                                                                // 3. Fchoice_b value for DLPF

// Accel Config Register 1
#define MPU9250_ACCEL_CONFIG_1_REGISTER                 0x1C    // Register to Set
                                                                // 1. Self Test Along X, Y, and Z Accels.
                                                                // 2. Accel Full Scale Values.

// Accel Config Register 2
#define MPU9250_ACCEL_CONFIG_2_REGISTER                 0x1D    // Register to Set
                                                                // 1. DLPF Settings.
                                                                // 2. Accelerometer Low Pass Filter Settings.

// Accel Low Power Output Data Rate Register
#define MPU9250_ACCEL_LOW_PWR_ODR_REGISTER              0x1E    // Register to Set
                                                                // 1. Frequency of wake up to take sample accel value in low power mode

// Wake On Motion Threshold Register
#define MPU9250_WOM_THR_REGISTER                        0x1F    // Register to Save
                                                                // 1. Threshold for wake up of device.

// FIFO Enable Register
#define MPU9250_FIFO_ENABLE_REGISTER                    0x23    // Register to Set
                                                                // 1. Temp Data to FIFO
                                                                // 2. GYRO X Data to FIFO
                                                                // 3. GYRO Y Data to FIFO
                                                                // 4. GYRO Z Data to FIFO
                                                                // 5. ACCEL Data X, Y, Z to FIFO
                                                                // 6. Aux. Slave 2. Data to FIFO
                                                                // 7. Aux. Slave 1. Data to FIFO
                                                                // 8. Aux. Slave 0. Data to FIFO

// I2C Master Control Register
#define MPU9250_I2C_MASTER_CONTROL_REGISTER             0x24    // Register to Set
                                                                // 1. Multi Master Capability.
                                                                // 2. External Sensor wait delay.
                                                                // 3. Aux. Slav 3. Data to FIFO
                                                                // 4. I2C Master Transition.
                                                                // 5. Divider for Internal Clock


// I2C Slave Registers for Aux I2C.
#define MPU9250_I2C_SLAVE_0_ADDRESS_REGISTER            0x25    // Register for Physical Address of I2C Slave 0 ([6:0]) and R/W Bit [7]
#define MPU9250_I2C_SLAVE_0_START_REG_REGISTER          0x26    // I2C Slave 0 register address from where to begin data transfer.
#define MPU9250_I2C_SLAVE_0_CONTROL_REGISTER            0x27    // I2C Slave 0 Control Register.

#define MPU9250_I2C_SLAVE_1_ADDRESS_REGISTER            0x28    // Register for Physical Address of I2C Slave 1 ([6:0]) and R/W Bit [7]
#define MPU9250_I2C_SLAVE_1_START_REG_REGISTER          0x29    // I2C Slave 1 register address from where to begin data transfer.
#define MPU9250_I2C_SLAVE_1_CONTROL_REGISTER            0x2A    // I2C Slave 1 Control Register.

#define MPU9250_I2C_SLAVE_2_ADDRESS_REGISTER            0x2B    // Register for Physical Address of I2C Slave 2 ([6:0]) and R/W Bit [7]
#define MPU9250_I2C_SLAVE_2_START_REG_REGISTER          0x2C    // I2C Slave 2 register address from where to begin data transfer.
#define MPU9250_I2C_SLAVE_2_CONTROL_REGISTER            0x2D    // I2C Slave 2 Control Register.

#define MPU9250_I2C_SLAVE_3_ADDRESS_REGISTER            0x2E    // Register for Physical Address of I2C Slave 3 ([6:0]) and R/W Bit [7]
#define MPU9250_I2C_SLAVE_3_START_REG_REGISTER          0x2F    // I2C Slave 3 register address from where to begin data transfer.
#define MPU9250_I2C_SLAVE_3_CONTROL_REGISTER            0x30    // I2C Slave 3 Control Register.

#define MPU9250_I2C_SLAVE_4_ADDRESS_REGISTER            0x31    // Register for Physical Address of I2C Slave 4 ([6:0]) and R/W Bit [7]
#define MPU9250_I2C_SLAVE_4_START_REG_REGISTER          0x32    // I2C Slave 4 register address from where to begin data transfer.
#define MPU9250_I2C_SLAVE_4_DATA_OUT_REGISTER           0x33    // Data to be written to Slave 4 if enabled.
#define MPU9250_I2C_SLAVE_4_CONTROL_REGISTER            0x34    // I2C Slave 4 Control Register.
#define MPU9250_I2C_SLAVE_4_DATA_IN_REGISTER            0x35    // Data to be read from Slave 4.


// I2C Master Status Register.
#define MPU9250_I2C_MASTER_STATUS_REGISTER              0x36    // Register for receiving states from I2C Slaves on Aux. I2C


// Interrupt Pin and Bypass Enable Configuration Register
#define MPU9250_INT_PIN_BYPASS_CONF_REGISTER            0x37    // Register to configure Int Pin and Bypass configurations.


// Interrupt Enable Register
#define MPU9250_INTERRUPT_ENABLE_REGISTER               0x38    // Register to Enable different Interrupt Types.

// Interrupt Status Register
#define MPU9250_INTERRUPT_STATUS_REGISTER               0x3A    // Register for status of various interrupts.

// Register to store Accelerometer Measurements
#define MPU9250_ACCEL_XOUT_H_REGISTER                   0x3B
#define MPU9250_ACCEL_XOUT_L_REGISTER                   0x3C

#define MPU9250_ACCEL_YOUT_H_REGISTER                   0x3D
#define MPU9250_ACCEL_YOUT_L_REGISTER                   0x3E

#define MPU9250_ACCEL_ZOUT_H_REGISTER                   0x3F
#define MPU9250_ACCEL_ZOUT_L_REGISTER                   0x40

#define MPU9250_TEMP_OUT_H_REGISTER                     0x41
#define MPU9250_TEMP_OUT_L_REGISTER                     0x42

#define MPU9250_GYRO_XOUT_H_REGISTER                    0x43
#define MPU9250_GYRO_XOUT_L_REGISTER                    0x44

#define MPU9250_GYRO_YOUT_H_REGISTER                    0x45
#define MPU9250_GYRO_YOUT_L_REGISTER                    0x46

#define MPU9250_GYRO_ZOUT_H_REGISTER                    0x47
#define MPU9250_GYRO_ZOUT_L_REGISTER                    0x48


// Registers which store external sensor data
#define MPU9250_EXT_SENS_DATA_00_REGISTER               0x49
#define MPU9250_EXT_SENS_DATA_01_REGISTER               0x4A
#define MPU9250_EXT_SENS_DATA_02_REGISTER               0x4B
#define MPU9250_EXT_SENS_DATA_03_REGISTER               0x4C
#define MPU9250_EXT_SENS_DATA_04_REGISTER               0x4D
#define MPU9250_EXT_SENS_DATA_05_REGISTER               0x4E
#define MPU9250_EXT_SENS_DATA_06_REGISTER               0x4F
#define MPU9250_EXT_SENS_DATA_07_REGISTER               0x50
#define MPU9250_EXT_SENS_DATA_08_REGISTER               0x51
#define MPU9250_EXT_SENS_DATA_09_REGISTER               0x52
#define MPU9250_EXT_SENS_DATA_10_REGISTER               0x53
#define MPU9250_EXT_SENS_DATA_11_REGISTER               0x54
#define MPU9250_EXT_SENS_DATA_12_REGISTER               0x55
#define MPU9250_EXT_SENS_DATA_13_REGISTER               0x56
#define MPU9250_EXT_SENS_DATA_14_REGISTER               0x57
#define MPU9250_EXT_SENS_DATA_15_REGISTER               0x58
#define MPU9250_EXT_SENS_DATA_16_REGISTER               0x59
#define MPU9250_EXT_SENS_DATA_17_REGISTER               0x5A
#define MPU9250_EXT_SENS_DATA_18_REGISTER               0x5B
#define MPU9250_EXT_SENS_DATA_19_REGISTER               0x5C
#define MPU9250_EXT_SENS_DATA_20_REGISTER               0x5D
#define MPU9250_EXT_SENS_DATA_21_REGISTER               0x5E
#define MPU9250_EXT_SENS_DATA_22_REGISTER               0x5F
#define MPU9250_EXT_SENS_DATA_23_REGISTER               0x60


// Data Out Registers of I2C Slaves 0, 1, 2, and 3
#define MPU9250_I2C_SLAVE_0_DATA_OUT_REGISTER           0x63
#define MPU9250_I2C_SLAVE_1_DATA_OUT_REGISTER           0x64
#define MPU9250_I2C_SLAVE_2_DATA_OUT_REGISTER           0x65
#define MPU9250_I2C_SLAVE_3_DATA_OUT_REGISTER           0x66


// Register for I2C Master Delay Control
#define MPU9250_MASTER_DELAY_CTRL_REGISTER              0x67

// Register to clear signal paths only. Will need assistance of USER_CONTROL_REGISTER to clear data registers.
#define MPU9250_SIGNAL_PATH_RESET_REGISTER              0x68


// Register to Control Accelerometer Interrupt
#define MPU9250_ACCEL_INTEL_CTRL_REGISTER               0x69

// User Control Register
#define MPU9250_USER_CTRL_REGISTER                      0x6A


// Power Management 1st Register
#define MPU9250_PWR_MGNT_1_REGISTER                     0x6B    // Important for setting Clock Source.

// Power Management 2nd Register
#define MPU9250_PWR_MGNT_2_REGISTER                     0x6C


// FIFO Count High Register for number of bytes written to FIFO (data[12:8] in Register[4:0] space)
#define MPU9250_FIFO_COUNT_H_REGISTER                   0x72

// FIFO Count Low Register for number of bytes written to FIFO (data[7:0] in Register[7:0] space)
#define MPU9250_FIFO_COUNT_L_REGISTER                   0x73

// Register to read and write data to FIFO Buffer.
#define MPU9250_FIFO_READ_WRITE_REGISTER                0x74


// WHO AM I REGISTER
#define MPU9250_WHO_AM_I_REGISTER                       0x75
#define MPU9250_WHO_AM_I_VALUE                          0x71

// Accel Offset Registers
#define MPU9250_XA_OFFSET_H_REGISTER                    0x77
#define MPU9250_XA_OFFSET_L_REGISTER                    0x78
#define MPU9250_YA_OFFSET_H_REGISTER                    0x79
#define MPU9250_YA_OFFSET_L_REGISTER                    0x7A
#define MPU9250_ZA_OFFSET_H_REGISTER                    0x7D
#define MPU9250_ZA_OFFSET_L_REGISTER                    0x7E

/*
// Bits for MPU9250_CONFIG_REGISTER (0x1A)
#define MPU9250_CONFIG_REGISTER_FIFO_MODE_BIT           6


// Bits for MPU9250_FIFO_ENABLE_REGISTER (0x23)
#define MPU9250_FIFO_ENABLE_REGISTER_TEMP_OUT_BIT       7
#define MPU9250_FIFO_ENABLE_REGISTER_GYROX_OUT_BIT      6
#define MPU9250_FIFO_ENABLE_REGISTER_GYROY_OUT_BIT      5
#define MPU9250_FIFO_ENABLE_REGISTER_GYROZ_OUT_BIT      4
#define MPU9250_FIFO_ENABLE_REGISTER_ACCEL_OUT_BIT      3
#define MPU9250_FIFO_ENABLE_REGISTER_AUX_SL2_OUT_BIT    2
#define MPU9250_FIFO_ENABLE_REGISTER_AUX_SL1_OUT_BIT    1
#define MPU9250_FIFO_ENABLE_REGISTER_AUX_SL0_OUT_BIT    0


// Bits for MPU9250_INT_PIN_&_BYPASS_CONF_REGISTER (0x37)
#define MPU9250_INT_PIN_BYPASS_CONF_REGISTER_ACTL_BIT   7
#define MPU9250_INT_PIN_BYPASS_CONF_REGISTER_OPEN_BIT   6
#define MPU9250_INT_PIN_BYPASS_CONF_REGISTER_LEI_BIT    5
#define MPU9250_INT_PIN_BYPASS_CONF_REGISTER_RD2C_BIT   4
#define MPU9250_INT_PIN_BYPASS_CONF_REGISTER_AF_BIT 3
#define MPU9250_INT_PIN_BYPASS_CONF_REGISTER_FIME_BIT   2
#define MPU9250_INT_PIN_BYPASS_CONF_REGISTER_BPE_BIT    1

// Bits for MPU9250_INTERRUPT_ENABLE_REGISTER (0x38)
#define MPU9250_INTERRUPT_ENABLE_REGISTER_WOM_EN_BIT    6
#define MPU9250_INTERRUPT_ENABLE_REGISTER_FIFO_OE_BIT   4
#define MPU9250_INTERRUPT_ENABLE_REGISTER_FSYNC_IE_BIT  3
#define MPU9250_INTERRUPT_ENABLE_REGISTER_DATAR_E_BIT   0


// Bits for MPU9250_INTERRUPT_STATUS_REGISTER (0x3A)
#define MPU9250_INTERRUPT_STATUS_REGISTER_WOM_I_BIT     6
#define MPU9250_INTERRUPT_STATUS_REGISTER_FIFO_I_BIT    4
#define MPU9250_INTERRUPT_STATUS_REGISTER_FSYNC_I_BIT   3
#define MPU9250_INTERRUPT_STATUS_REGISTER_DATAR_I_BIT   0

// Bits for MPU9250_SIGNAL_PATH_RESET_REGISTER (0x68)
#define MPU9250_SIGNAL_PATH_RESET_REGISTER_GYROR_BIT    2
#define MPU9250_SIGNAL_PATH_RESET_REGISTER_ACCELR_BIT   1
#define MPU9250_SIGNAL_PATH_RESET_REGISTER_TEMPR_BIT    0

// Bits for MPU9250_ACCEL_INTEL_CTRL_REGISTER (0x69)
#define MPU9250_ACCEL_INTEL_CTRL_REGISTER_EN_BIT        7
#define MPU9250_ACCEL_INTEL_CTRL_REGISTER_MODE_BIT      6

// Bits for MPU9250_USER_CTRL_REGISTER (0x6A)
#define MPU9250_USER_CTRL_REGISTER_FIFO_ENABLE_BIT      6
#define MPU9250_USER_CTRL_REGISTER_I2C_MST_EN_BIT       5
#define MPU9250_USER_CTRL_REGISTER_I2C_IF_DIS_BIT       4
#define MPU9250_USER_CTRL_REGISTER_FIFO_RST_BIT         2
#define MPU9250_USER_CTRL_REGISTER_I2C_MST_RST_BIt      1
#define MPU9250_USER_CTRL_REGISTER_SIG_COND_RST_BIT     0

// Bits for MPU9250_PWR_MGNT_1_REGISTER (0x6B)
#define MPU9250_PWR_MGNT_1_REGISTER_H_RESET_BIT         7
#define MPU9250_PWR_MGNT_1_REGISTER_SLEEP_BIT           6
#define MPU9250_PWR_MGNT_1_REGISTER_CYCLE_BIT           5
#define MPU9250_PWR_MGNT_1_REGISTER_GYRO_STANDBY_BIT    4
#define MPU9250_PWR_MGNT_1_REGISTER_PD_PTAT_BIT         3
*/



#endif /* MPU9250_REGISTERS_H_ */
