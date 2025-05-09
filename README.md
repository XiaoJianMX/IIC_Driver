


          
# IIC 自定义接口库

## 项目概述

这是一个用于STM32单片机的自定义I2C（IIC）接口库，支持软件I2C和硬件I2C两种模式。该库提供了一套完整的I2C通信接口，使用户能够方便地进行I2C设备的读写操作。

## 特性

- 支持软件I2C和硬件I2C两种模式
- 可通过配置文件轻松切换模式
- 提供单字节和多字节读写功能
- 简洁的API接口设计
- 可自定义GPIO引脚配置

## 文件结构

- `Customize_I2C.c` - I2C接口实现源文件
- `Customize_I2C.h` - I2C接口头文件
- `config.h` - 配置文件，用于设置I2C模式和延时参数

## 配置说明

在`config.h`文件中可以进行以下配置：

```c
#define DALAY_TIME          (10)        // 延时时间
#define SOFT_I2C            1           // 软件I2C   使能：1  关闭：0       
#define Delay_Us(x)         Delay_Us(x) // 延时函数
```

- `DALAY_TIME`: 定义I2C通信中的延时时间
- `SOFT_I2C`: 设置为1启用软件I2C模式，设置为0启用硬件I2C模式
- `Delay_Us`: 微秒级延时函数，需要用户自行实现

## 使用方法

### 初始化

```c
// 定义I2C GPIO配置
I2C_GPIO_TypeDef i2c_gpio;
i2c_gpio.SDA_GPIO_Port = GPIOB;
i2c_gpio.SCL_GPIO_Port = GPIOB;
i2c_gpio.SDA_GPIO_Pin = GPIO_Pin_7;
i2c_gpio.SCL_GPIO_Pin = GPIO_Pin_6;

// 初始化I2C
Customize_I2C_TypeDef *my_i2c = MY_I2C_Init(&i2c_gpio);
```

### 读取数据

```c
// 读取单个字节
uint8_t data;
my_i2c->ReadOne(DEVICE_ADDR, REG_ADDR, &data);

// 读取多个字节
uint8_t buffer[10];
my_i2c->ReadMulti(DEVICE_ADDR, REG_ADDR, 10, buffer);
```

### 写入数据

```c
// 写入单个字节
uint8_t data = 0x55;
my_i2c->WriteOne(DEVICE_ADDR, REG_ADDR, data);

// 写入多个字节
uint8_t buffer[3] = {0x01, 0x02, 0x03};
my_i2c->WriteMulti(DEVICE_ADDR, REG_ADDR, 3, buffer);
```

## 主要函数

- `MY_I2C_Init()`: 初始化I2C接口
- `ReadOne()`: 读取单个字节
- `ReadMulti()`: 读取多个字节
- `WriteOne()`: 写入单个字节
- `WriteMulti()`: 写入多个字节

## 注意事项

1. 使用软件I2C模式时，需要确保`Delay_Us()`函数已正确实现
2. 硬件I2C模式需要正确配置STM32的I2C外设
3. 在使用前请确保GPIO时钟已经使能



        
