#ifndef WINBOND_H
#define WINBOND_H

#include <Arduino.h>

#define W25Q_SS_PIN PA4
#define W25Q_CLOCK_PIN PA5
#define W25Q_MISO_PIN PA6
#define W25Q_MOSI_PIN PA7
#define DEFAULT_MAX_SPI_FREQUENCY 1000000

#define BUSY 1

class Winbond
{
public:
    typedef enum
    {
        STATUS_REGISTER_1,
        STATUS_REGISTER_2
    } statusRegisterIndex;

    Winbond();
    void readManufacturer();
    void readUniqueID();
    void readJedecID();
    void readStatus(statusRegisterIndex);
    uint8_t readBusyBit();
    void readData(uint32_t, uint16_t);
    void readPage(uint32_t);
    void writeData(uint32_t, uint8_t);
    void writeData(uint32_t, uint8_t *, uint16_t);
    void eraseSector(uint32_t);
    void erase32KBBlock(uint32_t);
    void erase64KBBlock(uint32_t);
    void eraseChip();
    void wait();

private:
    uint32_t maxSPIFrequency;
};

extern Winbond flash;

#endif