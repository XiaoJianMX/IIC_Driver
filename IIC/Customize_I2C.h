#pragma once
#include "ch32v20x.h"
#include <stdint.h>
#include "config.h"

#define   SDA_GPIO_RCC      RCC_APB2Periph_GPIOB
#define   SCL_GPIO_RCC      RCC_APB2Periph_GPIOB

#if     SOFT_I2C

#define SCL_H               I2C_SCL_H()
#define SCL_L               I2C_SCL_L()
#define SDA_H               I2C_SDA_H()
#define SDA_L               I2C_SDA_L()
#define SDA_READ            I2C_SDA_READ()
#define SDA_IN()            I2C_SDA_IN()
#define SDA_OUT()           I2C_SDA_OUT()



#endif

/* I2C  返回状态 */
typedef enum
{
    I2C_OK = 0,
    I2C_ERROR
} I2C_Status;
/* 硬件I2C 设置的速率 */
typedef enum
{
    I2C_SPEED_100K,
    I2C_SPEED_200K,
    I2C_SPEED_300K,
    I2C_SPEED_400K,
} I2C_Speed;
/* 硬件I2C 主从模式 */
typedef enum
{
    I2C_MODE_MASTER,
    I2C_MODE_SLAVE,
} I2C_Mode;
/* 应答是否正确 */
typedef enum
{
    I2C_Ack_OK = 0,
    I2C_Ack_ERROR,
} I2C_Ack_Status;
/* 使用的I2C GPIO口 */
typedef struct __I2C_GPIO
{
    GPIO_TypeDef *SCL_GPIO_Port;
    GPIO_TypeDef *SDA_GPIO_Port;
    uint16_t SCL_GPIO_Pin;
    uint16_t SDA_GPIO_Pin;
} I2C_GPIO_TypeDef;
/* I2C 结构体 */
typedef struct __I2C
{   
    I2C_GPIO_TypeDef I2C_GPIO;
    void (*I2C_GPIO_Init)(void);//GPIO 初始化函数
#if SOFT_I2C //软件I2C的 封装函数
    I2C_Status (*I2C_Start)(void);
    I2C_Status (*I2C_Stop)(void);
    I2C_Status (*I2C_SendByte)(uint8_t data);
    uint8_t (*I2C_ReadByte)(void);
    I2C_Ack_Status (*I2C_Wait_Ack)(void);
    void (*I2C_Ack)(void);
    void (*I2C_NAck)(void);
    void (*I2C_Delay)(volatile uint32_t delay_para);
#else
    void (*I2C_Init)(I2C_Speed speed, I2C_Mode mode,I2C_GPIO_TypeDef *I2C_GPIO);//硬件I2C初始化函数
#endif
    /* 封装的I2C 读写函数 可自定义 */
    I2C_Status (*ReadOne)(uint8_t devAddr,uint8_t reg, uint8_t *data);
    I2C_Status (*WriteOne)(uint8_t devAddr,uint8_t reg, uint8_t data);
    I2C_Status (*ReadMulti)(uint8_t devAddr,uint8_t reg , uint8_t len, uint8_t *data);
    I2C_Status (*WriteMulti)(uint8_t devAddr,uint8_t reg, uint8_t len, uint8_t *data);
} Customize_I2C_TypeDef;


Customize_I2C_TypeDef *MY_I2C_Init(I2C_GPIO_TypeDef *I2C_GPIO);
