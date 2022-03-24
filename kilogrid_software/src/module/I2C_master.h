//this file comes from the I2C master library available at https://github.com/g4lvanix/I2C-master-lib


#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#define I2C_READ 0x01
#define I2C_WRITE 0x00

void I2C_init(void);
uint8_t I2C_start(uint8_t);
uint8_t I2C_write(uint8_t);
uint8_t I2C_read_ack(void);
uint8_t I2C_read_nack(void);
void I2C_stop(void);

#endif // I2C_MASTER_H
