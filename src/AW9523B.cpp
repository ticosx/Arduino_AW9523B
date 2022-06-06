#include "AW9523B.h"
#include "Log.h"

AW9523B* AW9523B::instance = NULL;

/**
 * @brief AW9523B 控制类，会自动保持一个全局实例
 *
 * @param i2c:  AW9523B 挂载的 I2C 总线
 * @param ad0:  AD0 对应的硬件配置，高电平为 1， 低电平为 0
 * @param ad1:  AD1 对应的硬件配置，高电平为 1， 低电平为 0
 * 
 */
AW9523B::AW9523B(TwoWire *i2c, int8_t ad0, int8_t ad1) 
: ad0(ad0), ad1(ad1) {
    logVerbose("Call Contructor");
    // Create an I2C device for the I2C related operations
    i2cdevice = new Adafruit_I2CDevice(AW9523B_I2C_BASE_ADDRESS + (ad1 << 1) + ad0, i2c);
    instance = this;
    // begin();
}

AW9523B::~AW9523B() {

}

void AW9523B::begin() {
    // Initial wait after Power Up
    
    // Reset AW9523
    reset(); 
    delay(5);
    // Default work mode
    setGlobalControl(DRV_MODE_PUSH_PULL, 0);
}

/**
 * @brief 软复位
 */
void AW9523B::reset() {
#ifdef DEBUG
    logVerbose("Resetting...\n"); 
#endif
    writeByte(AW9523B_ADDR_RESET, 0); 
#ifdef DEBUG
    logVerbose("Reset Finished\n"); 
#endif
}

/**
 * @brief 设置 AW9523B 的地址
 *
 * @param ad0: AD0 对应的硬件配置，高电平为 1， 低电平为 0
 * @param ad1: AD1 对应的硬件配置，高电平为 1， 低电平为 0
 * 
 */
void AW9523B::setAddress(uint8_t ad0, uint8_t ad1) {
    this->ad0 = ad0; 
    this->ad1 = ad1; 
} 

/**
 * @brief 读取指定端口的值
 *
 * @param port: 端口号，0 或 1
 * 
 * @return  指定端口的值。如果某引脚为高电平，则该位为 1，低电平则该位为 0
 * 
 */
uint8_t AW9523B::readPort(uint8_t port) {
    logVerbose("Read Port %d\n", port); 
    if(port == 0) {
        return readByte(AW9523B_ADDR_INPUT0); 
    }
    else if(port == 1) {
        return readByte(AW9523B_ADDR_INPUT1); 
    }
    else {
        logErr("Read Port is Invalid"); 
        return 0; 
    }
} 

/**
 * @brief 读取指定引脚的值
 *
 * @param port: 端口号，0 或 1
 * @param pinIndex: 引脚索引，0 ~ 7
 * 
 * @return  指定引脚的值，高电平为 1，低电平为 0
 * 
 */
uint8_t AW9523B::readPin(uint8_t port, uint8_t pinIndex) {
    logVerbose("Read Port %d\n", port); 
    if(port == 0) {
        return (readByte(AW9523B_ADDR_INPUT0)>>pinIndex) & 1; 
    }
    else if(port == 1) {
        return (readByte(AW9523B_ADDR_INPUT1)>>pinIndex) & 1; 
    }
    else {
        logErr("Read Port is Invalid"); 
        return 0; 
    }
} 

/**
 * @brief 向指定端口输出值
 *
 * @param port: 端口号，0 或 1
 * @param data: 要输出的值
 *
 */
void AW9523B::writePort(uint8_t port, uint8_t data) {
    logVerbose("Write Port %d / Data 0x%02x\n", port, data); 
    if(port == 0) {
        writeByte(AW9523B_ADDR_OUTPUT0, data); 
    }
    else if(port == 1) {
        writeByte(AW9523B_ADDR_OUTPUT1, data); 
    }
    else {
        logErr("Write Port is Invalid"); 
    }
} 

/**
 * @brief 向指定引脚输出值
 *
 * @param port: 端口号，0 或 1
 * @param pinIndex: 引脚索引，0 ~ 7
 * @param data: 要输出的值。输出高电平置 1，输出低电平置 0
 *
 */
void AW9523B::writePin(uint8_t port, uint8_t pinIndex, uint8_t data) {
    logVerbose("Write Port %d / Pin %d / Data 0x%02x\n", port, pinIndex, data); 
    // Get the original value
    uint8_t value = readPort(port);
    // Set the proper bit
    uint8_t mask = 1 << pinIndex;
    if(data){
      // 1
      value |= mask;
    } else {
      // 0
      value &= (~mask);
    }
    writePort(port, value);
} 

/**
 * @brief 配置指定端口的输入输出模式
 *
 * @param port: 端口号，0 或 1
 * @param mode: 工作模式，1 表示对应位为输入端口，0 表示对应位为输出端口
 *
 */
void AW9523B::setConfig(uint8_t port, uint8_t mode) {
    if(port == 0) {
        writeByte(AW9523B_ADDR_CONFIG0, mode); 
    }
    else if(port == 1) {
        writeByte(AW9523B_ADDR_CONFIG1, mode); 
    }
}

/**
 * @brief 配置指定端口的输入输出模式
 *
 * @param port: 端口号，0 或 1
 * @param pinMask: 引脚掩码，只有对应位为 1 的引脚才会被配置，其余位保持原有配置不变
 * @param mode: 工作模式，1 表示将 pinMask 指定的引脚配置为输入端口，0 表示将 pinMask 指定的引脚配置为输出端口
 *
 */
void AW9523B::setConfig(uint8_t port, uint8_t pinMask, uint8_t mode) {
    uint8_t config;
    if(port == 0) {
        config = readByte(AW9523B_ADDR_CONFIG0); 
    }
    else if(port == 1) {
        config = readByte(AW9523B_ADDR_CONFIG1); 
    }
    else {
        logErr("Wrong port");
        return; 
    }
    if(mode){
      // Set the masked bits to 1
      config |= pinMask;
    } else {
      // Set the masked bits to 0
      config &= (~pinMask);
    }
    // Write into register
    setConfig(port, config);
}

/**
 * @brief 配置指定端口的中断使能
 *
 * @param port: 端口号，0 或 1
 * @param flag: 中断使能状态，只对工作状态为 GPIO 的端口生效。0 表示对应位中断使能，1 表示对应位中断禁用
 *
 */
void AW9523B::setInterrupt(uint8_t port, uint8_t flag) {
    if(port == 0) {
        writeByte(AW9523B_ADDR_INT0, flag); 
    }
    else if(port == 1) {
        writeByte(AW9523B_ADDR_INT1, flag); 
    }
}

/**
 * @brief 配置指定端口的中断使能
 *
 * @param port: 端口号，0 或 1
 * @param pinMask: 引脚掩码，只有对应位为 1 的引脚才会被配置，其余位保持原有配置不变
 * @param flag: 中断使能状态，只对工作状态为 GPIO 的端口生效。0 表示将 pinMask 指定的引脚中断使能，1 表示将 pinMask 指定的引脚中断禁用
 *
 */
void AW9523B::setInterrupt(uint8_t port, uint8_t pinMask, uint8_t flag) {
    uint8_t config;
    if(port == 0) {
        config = readByte(AW9523B_ADDR_INT0); 
    }
    else if(port == 1) {
        config = readByte(AW9523B_ADDR_INT1); 
    }
    else {
        logErr("Wrong port");
        return; 
    }
    if(flag & 1){
      // Set the masked bits to 1
      config |= pinMask;
    } else {
      // Set the masked bits to 0
      config &= (~pinMask);
    }
    // Write into register
    setInterrupt(port, config);
}

/**
 * @brief 返回芯片 ID
 * 
 * @return  芯片 ID
 * 
 */
uint8_t AW9523B::readID() {
    return readByte(AW9523B_ADDR_ID); 
}

/**
 * @brief 全局控制设置
 *
 * @param mode: GPIO 驱动模式。0 开漏输出，1 推挽输出
 * @param range: LED 电流范围。0: 0 ~ Imax，1: 0 ~ Imax * 3/4, 2: 0 ~ Imax / 2, 3: 0 ~ Imax * 1/4
 *
 */
void AW9523B::setGlobalControl(GPIO_DRV_MODE_Enum mode, uint8_t range) {
    uint8_t data = ((mode & 0x01) << 4) | (range & 0x03); 
    writeByte(AW9523B_ADDR_GCR, data); 
}
    
/**
 * @brief 将引脚设为 LED 工作模式
 *
 * @param port: 端口号，0 或 1
 * @param flag: 工作模式。为 1 的位对应引脚为 LED 模式，为 0 的位对应引脚为 GPIO 模式
 *
 */
void AW9523B::setLedMode(uint8_t port, uint8_t flag) {
    if(port == 0) {
        writeByte(AW9523B_ADDR_LEDMODE0, ~flag); 
    }
    else if(port == 1) {
        writeByte(AW9523B_ADDR_LEDMODE1, ~flag);
    }
}

/**
 * @brief 将引脚设为 GPIO 工作模式
 *
 * @param port: 端口号，0 或 1
 * @param flag: 工作模式。为 1 的位对应引脚为 GPIO 模式，为 0 的位对应引脚为 LED 模式
 *
 */
void AW9523B::setGpioMode(uint8_t port, uint8_t flag) {
    if(port == 0) {
        writeByte(AW9523B_ADDR_LEDMODE0, flag); 
    }
    else if(port == 1) {
        writeByte(AW9523B_ADDR_LEDMODE1, flag); 
    }
}

/**
 * @brief 将引脚设为 GPIO 工作模式
 *
 * @param port: 端口号，0 或 1
 * @param pinMask: 引脚掩码，只有对应位为 1 的引脚才会被配置，其余位保持原有配置不变
 * @param flag: 工作模式。1 表示将 pinMask 指定的引脚配置为 GPIO 模式，0 表示将 pinMask 指定的引脚配置为 LED 模式
 *
 */
void AW9523B::setPinMode(uint8_t port, uint8_t pinMask, uint8_t flag) {
    uint8_t config;
    if(port == 0) {
        config = readByte(AW9523B_ADDR_LEDMODE0); 
    }
    else if(port == 1) {
        config = readByte(AW9523B_ADDR_LEDMODE0); 
    }
    else {
        logErr("Wrong port");
        return; 
    }
    if(flag & 1){
      // Set the masked bits to 1
      config |= pinMask;
    } else {
      // Set the masked bits to 0
      config &= (~pinMask);
    }
    // Write into register
    setGpioMode(port, config);
}

/**
 * @brief 设定指定引脚的 LED 驱动电流
 *
 * @param port: 端口号，0 或 1
 * @param subport: 引脚序号
 * @param dim: 驱动电流，0 ~ 0xFF 分别对应 0 ~ Imax 共256级
 *
 */
void AW9523B::setDimmer(uint8_t port, uint8_t subport, uint8_t dim) {
    if(port == 0) {
        writeByte(AW9523B_ADDR_DIM_BASE + 4 + subport, dim); 
    }
    else if(port == 1) {
        if(subport < 4) {
            writeByte(AW9523B_ADDR_DIM_BASE + subport, dim); 
        }
        else {
            writeByte(AW9523B_ADDR_DIM_BASE + 8 + subport, dim); 
        }
    }
} 

/**
 * @brief 读取指定寄存器的值
 *
 * @param addr: 寄存器地址
 * 
 * @return  指定寄存器的值
 *
 */
uint8_t AW9523B::readByte(uint8_t addr) {
    uint8_t rdData; 
    i2cdevice->write_then_read(&addr, sizeof(addr), &rdData, 1, true);
    return rdData; 

}

/**
 * @brief 设置指定寄存器的值
 *
 * @param addr: 寄存器地址
 * @param data: 要设置的值
 *
 */
void AW9523B::writeByte(uint8_t addr, uint8_t data) {
    i2cdevice->write( &data, sizeof(data), true, &addr, sizeof(addr));
}