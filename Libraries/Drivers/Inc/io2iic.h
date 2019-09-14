#ifndef __IO2IIC_H_
#define __IO2IIC_H_

#include "gpio.h"

/* IO口模拟IIC端口定义 */
#define IIC_SCL_PORT	HW_GPIOC
#define IIC_SCL_PIN		(7)
#define IIC_SDA_PORT	HW_GPIOC
#define IIC_SDA_PIN		(6)

void i2c_start(void);

void i2c_stop(void);

int get_ack(void);

int i2c_send_byte(uint8_t byte);

/* IO模拟IIC写数据 */
int i2c_write_byte(uint8_t dev_addr_wr, uint8_t word_addr, uint8_t mydata);

/* IO模拟IIC读数据 */
uint8_t i2c_read_byte(uint8_t dev_addr_wr, uint8_t word_addr);

#endif
