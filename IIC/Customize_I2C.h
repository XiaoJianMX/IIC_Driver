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

/* I2C  ����״̬ */
typedef enum
{
    I2C_OK = 0,
    I2C_ERROR
} I2C_Status;
/* Ӳ��I2C ���õ����� */
typedef enum
{
    I2C_SPEED_100K,
    I2C_SPEED_200K,
    I2C_SPEED_300K,
    I2C_SPEED_400K,
} I2C_Speed;
/* Ӳ��I2C ����ģʽ */
typedef enum
{
    I2C_MODE_MASTER,
    I2C_MODE_SLAVE,
} I2C_Mode;
/* Ӧ���Ƿ���ȷ */
typedef enum
{
    I2C_Ack_OK = 0,
    I2C_Ack_ERROR,
} I2C_Ack_Status;
/* ʹ�õ�I2C GPIO�� */
typedef struct __I2C_GPIO
{
    GPIO_TypeDef *SCL_GPIO_Port;
    GPIO_TypeDef *SDA_GPIO_Port;
    uint16_t SCL_GPIO_Pin;
    uint16_t SDA_GPIO_Pin;
} I2C_GPIO_TypeDef;
/* I2C �ṹ�� */
typedef struct __I2C
{   
    I2C_GPIO_TypeDef I2C_GPIO;
    void (*I2C_GPIO_Init)(void);//GPIO ��ʼ������
#if SOFT_I2C //���I2C�� ��װ����
    I2C_Status (*I2C_Start)(void);
    I2C_Status (*I2C_Stop)(void);
    I2C_Status (*I2C_SendByte)(uint8_t data);
    uint8_t (*I2C_ReadByte)(void);
    I2C_Ack_Status (*I2C_Wait_Ack)(void);
    void (*I2C_Ack)(void);
    void (*I2C_NAck)(void);
    void (*I2C_Delay)(volatile uint32_t delay_para);
#else
    void (*I2C_Init)(I2C_Speed speed, I2C_Mode mode,I2C_GPIO_TypeDef *I2C_GPIO);//Ӳ��I2C��ʼ������
#endif
    /* ��װ��I2C ��д���� ���Զ��� */
    I2C_Status (*ReadOne)(uint8_t devAddr,uint8_t reg, uint8_t *data);
    I2C_Status (*WriteOne)(uint8_t devAddr,uint8_t reg, uint8_t data);
    I2C_Status (*ReadMulti)(uint8_t devAddr,uint8_t reg , uint8_t len, uint8_t *data);
    I2C_Status (*WriteMulti)(uint8_t devAddr,uint8_t reg, uint8_t len, uint8_t *data);
} Customize_I2C_TypeDef;


Customize_I2C_TypeDef *MY_I2C_Init(I2C_GPIO_TypeDef *I2C_GPIO);
