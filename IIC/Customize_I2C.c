#include "Customize_I2C.h"
 #include "debug.h"
/*  */
static GPIO_InitTypeDef GPIO_InitStruct;
static Customize_I2C_TypeDef I2C = {0};
#if SOFT_I2C == 0
static I2C_TypeDef *Static_I2Cx;
#endif
/* IIC  GPIO初始化函数 */
static void I2C_GPIO_Init(void)
{   
    RCC_APB2PeriphClockCmd(SDA_GPIO_RCC,ENABLE);
    RCC_APB2PeriphClockCmd(SCL_GPIO_RCC,ENABLE);
    GPIO_InitStruct.GPIO_Pin = I2C.I2C_GPIO.SCL_GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C.I2C_GPIO.SCL_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = I2C.I2C_GPIO.SDA_GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C.I2C_GPIO.SDA_GPIO_Port, &GPIO_InitStruct);
}
/* 如果使用软件IIC */
#if SOFT_I2C 
/* SDA_H */
static inline void I2C_SDA_H(void)
{
    GPIO_WriteBit(I2C.I2C_GPIO.SDA_GPIO_Port, I2C.I2C_GPIO.SDA_GPIO_Pin, Bit_SET);
}
/* SDA_L */
static inline void I2C_SDA_L(void)
{
    GPIO_WriteBit(I2C.I2C_GPIO.SDA_GPIO_Port, I2C.I2C_GPIO.SDA_GPIO_Pin, Bit_RESET);
}
/* SCL_H */
static inline void I2C_SCL_H(void)
{
    GPIO_WriteBit(I2C.I2C_GPIO.SCL_GPIO_Port, I2C.I2C_GPIO.SCL_GPIO_Pin, Bit_SET);
}
/* SCL_L */
static inline void I2C_SCL_L(void)
{
    GPIO_WriteBit(I2C.I2C_GPIO.SCL_GPIO_Port, I2C.I2C_GPIO.SCL_GPIO_Pin, Bit_RESET);
}
/* SDA_READ */
static uint8_t I2C_SDA_READ(void)
{
    return GPIO_ReadInputDataBit(I2C.I2C_GPIO.SDA_GPIO_Port, I2C.I2C_GPIO.SDA_GPIO_Pin);
}
/* SDA  IN */
static inline void I2C_SDA_IN(void)
{
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStruct.GPIO_Pin = I2C.I2C_GPIO.SDA_GPIO_Pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C.I2C_GPIO.SDA_GPIO_Port,&GPIO_InitStruct);
}
/* SDA  OUT */
static inline void I2C_SDA_OUT(void)
{
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = I2C.I2C_GPIO.SDA_GPIO_Pin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C.I2C_GPIO.SDA_GPIO_Port,&GPIO_InitStruct);
}
/* 软件IIC的延时函数 */
static inline void I2C_Delay(volatile uint32_t delay_para)
{   
    Delay_Us(delay_para);//此处需要自己的延时函数
}
/* 起始信号 */
static I2C_Status I2C_Start(void)
{
    SDA_H;
    SCL_H;
    I2C.I2C_Delay(DALAY_TIME);
    // SDA_IN();
    // if(!SDA_READ)
    //     return  I2C_ERROR;
    // SDA_OUT();
    SDA_L;
    I2C.I2C_Delay(DALAY_TIME);
    // SDA_IN();
    // if(SDA_READ)
    //     return  I2C_ERROR;
    // SDA_OUT();
    SCL_L;
    //I2C.I2C_Delay(DALAY_TIME);
    return I2C_OK;
}
/* 结束信号 */
static I2C_Status I2C_Stop(void)
{   
    SDA_OUT();
    SCL_L;
    I2C.I2C_Delay(DALAY_TIME);
    SDA_L;
    I2C.I2C_Delay(DALAY_TIME);
    SCL_H;
    I2C.I2C_Delay(DALAY_TIME);
    SDA_H;
    I2C.I2C_Delay(DALAY_TIME);
    return I2C_OK;
}
/* 等待应答 */
static I2C_Ack_Status I2C_Wait_Ack(void)
{   
    uint16_t timeout = 0;
    SCL_L;
    I2C.I2C_Delay(DALAY_TIME);
    SDA_H;
    I2C.I2C_Delay(DALAY_TIME);
    SCL_H;
    I2C.I2C_Delay(DALAY_TIME);
    SDA_IN();
    while(SDA_READ){
        timeout ++;
        if(timeout > 250){
            I2C_Stop();
            return  I2C_Ack_ERROR;
        }
    }
	SDA_OUT();
    SCL_L;
    I2C.I2C_Delay(DALAY_TIME);
    return I2C_Ack_OK;
}
/* 发送应答信号 */
static void I2C_Ack(void)
{
    SCL_L;
    I2C.I2C_Delay(DALAY_TIME);
    SDA_L;
    I2C.I2C_Delay(DALAY_TIME);
    SCL_H;
    I2C.I2C_Delay(DALAY_TIME);
    SCL_L;
    I2C.I2C_Delay(DALAY_TIME);
}
/* 发送不应答信号 */
static void I2C_NAck(void)
{
    SCL_L;
    I2C.I2C_Delay(DALAY_TIME);
    SDA_H;
    I2C.I2C_Delay(DALAY_TIME);
    SCL_H;
    I2C.I2C_Delay(DALAY_TIME);
    SCL_L;
    I2C.I2C_Delay(DALAY_TIME);
}
/* 发送一字节数据 */
static I2C_Status I2C_SendByte(uint8_t data)
{
    uint8_t i = 8;
    while (i--)
    {
        SCL_L;
        I2C.I2C_Delay(DALAY_TIME);
        if (data & 0x80)
            SDA_H;
        else
            SDA_L;
        data <<= 1;
        I2C.I2C_Delay(DALAY_TIME);
        SCL_H;
        I2C.I2C_Delay(DALAY_TIME);
    }
    SCL_L;
    SDA_IN();
    return I2C_OK;
}
/* 读取一字节数据 */
static uint8_t I2C_ReadByte(void)
{   
    uint8_t ReceiveByte = 0;
    uint8_t i = 8;
    SDA_H;
	SDA_IN();
    while (i--)
    {
        ReceiveByte <<= 1;
        SCL_L;
        I2C.I2C_Delay(DALAY_TIME);
        SCL_H;
        I2C.I2C_Delay(DALAY_TIME);
        if (SDA_READ)
        {
            ReceiveByte |= 0x01;
        }
    }
    SCL_L;
	SDA_OUT();

    return ReceiveByte;
}
/* IIC 读取一个字节 */
static I2C_Status I2C_ReadOneByte(uint8_t devAddr,uint8_t reg,uint8_t *data)
{   
#if SOFT_I2C
    if (I2C.I2C_Start() == I2C_ERROR) return I2C_ERROR;
    I2C.I2C_SendByte(devAddr);//发送从机设备地址
    if (I2C.I2C_Wait_Ack() == I2C_Ack_ERROR)
    {
        I2C.I2C_Stop();
        return I2C_ERROR;
    }
    I2C.I2C_SendByte((uint8_t)reg);
    I2C.I2C_Wait_Ack();
    I2C.I2C_Start();
    I2C.I2C_SendByte(devAddr+ 0x01);
    I2C.I2C_Wait_Ack();
    *data = I2C.I2C_ReadByte();
    I2C.I2C_NAck();
    I2C.I2C_Stop();
#else 
    I2C_GenerateSTART(Static_I2Cx, ENABLE);
    if (!I2C_CheckEvent(Static_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
        return 0;
    }
    I2C_Send7bitAddress(Static_I2Cx, I2C.I2C_Addr, I2C_Direction_Receiver);
    if (!I2C_CheckEvent(Static_I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        I2C_GenerateSTOP(Static_I2Cx, ENABLE);
        return 0;
    }
    I2C_AcknowledgeConfig(Static_I2Cx, DISABLE);
    *data = I2C_ReadRegister(Static_I2Cx,reg);
    I2C_AcknowledgeConfig(Static_I2Cx, ENABLE);
#endif
    return I2C_OK;
}
/* IIC 读取多个字节 */
static I2C_Status I2C_ReadMultiByte(uint8_t devAddr,uint8_t reg,uint8_t len,uint8_t *data)
{
    uint8_t i = 0;
    if (I2C.I2C_Start() == I2C_ERROR) return I2C_ERROR;
    I2C.I2C_SendByte(devAddr);
    if (I2C.I2C_Wait_Ack() == I2C_Ack_ERROR)
    {
        I2C.I2C_Stop();
        return I2C_ERROR;
    }
    I2C.I2C_SendByte((uint8_t)reg);
    I2C.I2C_Wait_Ack();
    I2C.I2C_Start();
    I2C.I2C_SendByte(devAddr + 1);
    I2C.I2C_Wait_Ack();
    for (i = 0; i < len; i++)
    {
        
        if (i != len - 1)
        {
            data[i] = I2C.I2C_ReadByte();
            I2C.I2C_Ack();
        }
        else
        {
            data[i] = I2C.I2C_ReadByte();
            I2C.I2C_NAck();
        }
    }
    I2C.I2C_Stop();
    return I2C_OK;
}
/* IIC 发送一个字节 */
static I2C_Status I2C_SendOneByte(uint8_t devAddr,uint8_t reg ,uint8_t data)
{
#if SOFT_I2C
    if (I2C.I2C_Start()) return I2C_ERROR;
    I2C.I2C_SendByte(devAddr + 1);
    if (I2C.I2C_Wait_Ack()) return I2C_ERROR;
    I2C.I2C_SendByte(reg);
    if (I2C.I2C_Wait_Ack()) return I2C_ERROR;
    I2C.I2C_SendByte(data);
    if (I2C.I2C_Wait_Ack()) return I2C_ERROR;
    I2C.I2C_Stop();
#else
    I2C_GenerateSTART(Static_I2Cx, ENABLE);
    if (!I2C_CheckEvent(Static_I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) {
        return I2C_ERROR;
    }
    I2C_Send7bitAddress(Static_I2Cx, I2C.I2C_Addr, I2C_Direction_Transmitter);
    if (!I2C_CheckEvent(Static_I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        I2C_GenerateSTOP(Static_I2Cx, ENABLE);
        return I2C_ERROR;
    }
    I2C_SendData(Static_I2Cx, reg);
    if (!I2C_CheckEvent(Static_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        I2C_GenerateSTOP(Static_I2Cx, ENABLE);
        return I2C_ERROR;
    }
    I2C_SendData(Static_I2Cx, *data);
    if (!I2C_CheckEvent(Static_I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        I2C_GenerateSTOP(Static_I2Cx, ENABLE);
        return I2C_ERROR;
    }
    I2C_GenerateSTOP(Static_I2Cx, ENABLE);
#endif
    return I2C_OK;
}
/* IIC 发送多个字节 */
static I2C_Status I2C_SendMultiByte(uint8_t devAddr ,uint8_t reg,uint8_t len,uint8_t *data)
{
    uint8_t i = 0;
#if SOFT_I2C
    if (I2C.I2C_Start()) return I2C_ERROR;
    I2C.I2C_SendByte(devAddr);
    if(I2C.I2C_Wait_Ack() == I2C_Ack_ERROR)
    {
        I2C.I2C_Stop();
        return I2C_ERROR;
    }
       
    I2C.I2C_SendByte(reg);
    I2C.I2C_Wait_Ack();
    for (i = 0; i < len; i++)
    {
        I2C.I2C_SendByte(data[i]);
        I2C.I2C_Wait_Ack();
    }
    I2C.I2C_Stop();
#else 

#endif
    return I2C_OK;
}
#else /* 使用硬件I2C */
/* 硬件IIC 初始化 */
static void I2C_Hardware_Init(I2C_TypeDef* I2Cx,I2C_GPIO_TypeDef *I2C_GPIO,I2C_Speed Speed)
{   
    if(I2Cx == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
    else if(I2Cx == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
    Static_I2Cx = I2Cx;
    static I2C_InitTypeDef I2C_InitStruct;
    switch(Speed){
        case I2C_SPEED_100K:
            I2C_InitStruct.I2C_ClockSpeed = 100;
            break;
        case I2C_SPEED_200K
            I2C_InitStruct.I2C_ClockSpeed = 200;
            break;
        case I2C_SPEED_300K
            I2C_InitStruct.I2C_ClockSpeed = 300;
            break;
        case I2C_SPEED_400K
            I2C_InitStruct.I2C_ClockSpeed = 400;
            break;
        default:
            I2C_InitStruct.I2C_ClockSpeed = 200;
            break;
    }
    I2C_InitStruct.I2C_ClockSpeed = 400;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStruct.I2C_OwnAddress1 = 0x1234;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_Init(I2Cx,&I2C_InitStruct);
    I2C_Cmd(I2Cx,ENABLE);
}
#endif
/* 初始化IIC结构体，返回的是 I2C_TypeDef 类型的结构体指针 */
Customize_I2C_TypeDef *MY_I2C_Init(I2C_GPIO_TypeDef *I2C_GPIO)
{   
    I2C.I2C_GPIO.SDA_GPIO_Port  = I2C_GPIO->SDA_GPIO_Port;
    I2C.I2C_GPIO.SCL_GPIO_Port  = I2C_GPIO->SCL_GPIO_Port;
    I2C.I2C_GPIO.SCL_GPIO_Pin   = I2C_GPIO->SCL_GPIO_Pin;
    I2C.I2C_GPIO.SDA_GPIO_Pin   = I2C_GPIO->SDA_GPIO_Pin;
    I2C.I2C_GPIO_Init           = I2C_GPIO_Init;
#if SOFT_I2C
    I2C.I2C_Start               = I2C_Start;
    I2C.I2C_Stop                = I2C_Stop;
    I2C.I2C_SendByte            = I2C_SendByte;
    I2C.I2C_ReadByte            = I2C_ReadByte;
    I2C.I2C_Wait_Ack            = I2C_Wait_Ack;
    I2C.I2C_Ack                 = I2C_Ack;
    I2C.I2C_NAck                = I2C_NAck;
    I2C.I2C_Delay               = I2C_Delay;
#else
    I2C.I2C_Init                = I2C_Hardware_Init;
#endif
    I2C.ReadOne                 = I2C_ReadOneByte;
    I2C.ReadMulti               = I2C_ReadMultiByte;
    I2C.WriteOne                = I2C_SendOneByte;
    I2C.WriteMulti              = I2C_SendMultiByte;

    return &I2C;
}