#ifndef AW9523B_H
#define AW9523B_H

#include <Wire.h>
#include <stdint.h>
#include <stdbool.h>
#include <Adafruit_I2CDevice.h>

// Device Address
#define AW9523B_I2C_BASE_ADDRESS 0x58 // 7-bit Address
// Register Address
#define AW9523B_ADDR_INPUT0 0x00
#define AW9523B_ADDR_INPUT1 0x01
#define AW9523B_ADDR_OUTPUT0 0x02
#define AW9523B_ADDR_OUTPUT1 0x03
#define AW9523B_ADDR_CONFIG0 0x04
#define AW9523B_ADDR_CONFIG1 0x05
#define AW9523B_ADDR_INT0 0x06
#define AW9523B_ADDR_INT1 0x07
#define AW9523B_ADDR_ID 0x10
#define AW9523B_ADDR_GCR 0x11
typedef enum {
    DRV_MODE_OPEN_DRAIN = 0, 
    DRV_MODE_PUSH_PULL = 1, 
} GPIO_DRV_MODE_Enum; 
#define AW9523B_ADDR_LEDMODE0 0x12
#define AW9523B_ADDR_LEDMODE1 0x13
#define AW9523B_ADDR_DIM_BASE 0x20
#define AW9523B_ADDR_RESET 0x7F

// Uncomment to enable debug messages
class AW9523B
{
public: 
    AW9523B(TwoWire *i2c, int8_t ad0, int8_t ad1); 
    virtual ~AW9523B(); 

    void begin(); 
    
    void reset(); 

    void setAddress(uint8_t _ad0, uint8_t _ad1); 

    uint8_t readPort(uint8_t port); 
    uint8_t readPin(uint8_t port, uint8_t pinIndex);
    void writePort(uint8_t port, uint8_t data); 
    void writePin(uint8_t port, uint8_t pinIndex, uint8_t data); 
    void setConfig(uint8_t port, uint8_t mode); 
    void setConfig(uint8_t port, uint8_t pinMask, uint8_t mode); 
    void setInterrupt(uint8_t port, uint8_t flag); 
    void setInterrupt(uint8_t port, uint8_t pinMask, uint8_t flag); 

    uint8_t readID(); 

    void setGlobalControl(GPIO_DRV_MODE_Enum mode, uint8_t range); 
    
    void setLedMode(uint8_t port, uint8_t flag); 
    void setGpioMode(uint8_t port, uint8_t flag); 
    void setPinMode(uint8_t port, uint8_t pinMask, uint8_t flag);
    void setDimmer(uint8_t port, uint8_t subport, uint8_t dim); 

    static AW9523B* getInstance(){return instance;};

private: 
    static AW9523B* instance;
protected:
    Adafruit_I2CDevice *i2cdevice;
    int8_t ad0 = -1; 
    int8_t ad1 = -1; 
    
    uint8_t readByte(uint8_t addr); 
    void writeByte(uint8_t addr, uint8_t data); 

}; 

#endif