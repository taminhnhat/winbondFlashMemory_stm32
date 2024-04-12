#include <Arduino.h>
#include <SPI.h>
#include <winbond.h>

#define _begin digitalWrite(W25Q_SS_PIN, LOW)
#define _end digitalWrite(W25Q_SS_PIN, HIGH)
#define _wait                  \
    while (readBusyBit() == 1) \
    {                          \
        delay(10);             \
    }

Winbond flash;

Winbond::Winbond()
{
    this->maxSPIFrequency = DEFAULT_MAX_SPI_FREQUENCY;
}
void Winbond::readManufacturer()
{
    uint8_t readStr[6];
    _begin;
    readStr[0] = SPI.transfer(0x90, SPI_TRANSMITRECEIVE); // Transfer the 8-bit value of data to shift register, remembering that the least significant bit goes first
    readStr[1] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    readStr[2] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    readStr[3] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    readStr[4] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    readStr[5] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    _end;
    Serial.print("Manufactorer ID: ");
    Serial.print(readStr[4]);
    Serial.print("   Device ID: ");
    Serial.println(readStr[5]);
}
void Winbond::readUniqueID()
{
    uint8_t readStr[13];
    _begin;
    ;
    readStr[0] = SPI.transfer(0x4b, SPI_TRANSMITRECEIVE);  //
    readStr[1] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);  //
    readStr[2] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);  //
    readStr[3] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);  //
    readStr[4] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);  //
    readStr[5] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);  //
    readStr[6] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);  //
    readStr[7] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);  //
    readStr[8] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);  //
    readStr[9] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);  //
    readStr[10] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    readStr[11] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    readStr[12] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);
    _end;
    String strout = "Serial Numer: ";
    for (int i = 5; i <= 11; i++)
    {
        strout += readStr[i];
        strout += "-";
    }
    strout += readStr[12];
    Serial.println(strout);
}
void Winbond::readJedecID()
{
    uint8_t readStr[4];
    _begin;
    readStr[0] = SPI.transfer(0x9f, SPI_TRANSMITRECEIVE); //
    readStr[1] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    readStr[2] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    readStr[3] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE); //
    _end;
    Serial.printf("Manufacturer ID: %d   Memory Type: %d   Capacity: %d\r\n", readStr[1], readStr[2], readStr[3]);
}
void Winbond::readStatus(statusRegisterIndex resgisterIndex = STATUS_REGISTER_1)
{
    uint8_t data[3];
    _begin;
    (resgisterIndex == STATUS_REGISTER_1) ? SPI.transfer(0x05, SPI_TRANSMITRECEIVE) : SPI.transfer(0x35, SPI_TRANSMITRECEIVE);
    data[1] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);
    data[2] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);
    _end;
    Serial.printf("Status Register %d: %d %d\r\n", resgisterIndex + 1, data[1], data[2]);
}
uint8_t Winbond::readBusyBit()
{
    uint8_t data[3];
    _begin;
    data[0] = SPI.transfer(0x05, SPI_TRANSMITRECEIVE);
    data[1] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);
    data[2] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);
    _end;
    return data[2] & 0b00000001;
}
void Winbond::readData(uint32_t address, uint16_t data_lenght)
{
    if (data_lenght == 0 || data_lenght > 256)
        return;
    uint8_t msb_address = (address >> 16) & 0xff;
    uint8_t mid_address = (address >> 8) & 0xff;
    uint8_t lsb_address = address & 0xff;
    uint8_t data[5];
    _begin;
    data[0] = SPI.transfer(0x03, SPI_TRANSMITRECEIVE);
    data[1] = SPI.transfer(msb_address, SPI_TRANSMITRECEIVE);
    data[2] = SPI.transfer(mid_address, SPI_TRANSMITRECEIVE);
    data[3] = SPI.transfer(lsb_address, SPI_TRANSMITRECEIVE);
    for (uint8_t i = 0; i < data_lenght; i++)
        Serial.printf("%d ", SPI.transfer(0x00, SPI_TRANSMITRECEIVE));
    Serial.println("");
    data[4] = SPI.transfer(0x00, SPI_TRANSMITRECEIVE);
    _end;
}
void Winbond::readPage(uint32_t address)
{
    uint8_t msb_address = (address >> 16) & 0xff;
    uint8_t mid_address = (address >> 8) & 0xff;
    uint8_t lsb_address = address & 0xff;
    uint8_t data[5];
    _begin;
    data[0] = SPI.transfer(0x03, SPI_TRANSMITRECEIVE);
    data[1] = SPI.transfer(msb_address, SPI_TRANSMITRECEIVE);
    data[2] = SPI.transfer(mid_address, SPI_TRANSMITRECEIVE);
    data[3] = SPI.transfer(lsb_address, SPI_TRANSMITRECEIVE);
    for (int i = 1; i <= 255; i++)
        Serial.printf("%d ", SPI.transfer(0x00, SPI_TRANSMITRECEIVE));
    Serial.println(SPI.transfer(0x00, SPI_TRANSMITRECEIVE));
    _end;
}
void Winbond::writeData(uint32_t address, uint8_t data)
{
    uint8_t msb_address = (address >> 16) & 0xff;
    uint8_t mid_address = (address >> 8) & 0xff;
    uint8_t lsb_address = address & 0xff;
    // enable write
    _begin;
    SPI.transfer(0x06, SPI_TRANSMITRECEIVE);
    _end;
    // write process
    _begin;
    SPI.transfer(0x02, SPI_TRANSMITRECEIVE);
    SPI.transfer(msb_address, SPI_TRANSMITRECEIVE);
    SPI.transfer(mid_address, SPI_TRANSMITRECEIVE);
    SPI.transfer(lsb_address, SPI_TRANSMITRECEIVE);
    SPI.transfer(data, SPI_TRANSMITRECEIVE);
    _end;
    // disable write
    _begin;
    SPI.transfer(0x04, SPI_TRANSMITRECEIVE);
    _end;
}
void Winbond::writeData(uint32_t address, uint8_t *data, uint16_t data_lenght)
{
    uint8_t msb_address = (address >> 16) & 0xff;
    uint8_t mid_address = (address >> 8) & 0xff;
    uint8_t lsb_address = address & 0xff;
    // enable write
    _begin;
    SPI.transfer(0x06, SPI_TRANSMITRECEIVE);
    _end;
    // write process
    _begin;
    SPI.transfer(0x02, SPI_TRANSMITRECEIVE);
    SPI.transfer(msb_address, SPI_TRANSMITRECEIVE);
    SPI.transfer(mid_address, SPI_TRANSMITRECEIVE);
    SPI.transfer(lsb_address, SPI_TRANSMITRECEIVE);
    for (int i = 0; i < data_lenght - 1; i++)
        SPI.transfer(data[i], SPI_TRANSMITRECEIVE);
    SPI.transfer(data[data_lenght - 1], SPI_TRANSMITRECEIVE);
    _end;
    // disable write
    _begin;
    SPI.transfer(0x04, SPI_TRANSMITRECEIVE);
    _end;
}
void Winbond::eraseSector(uint32_t address)
{
    uint8_t msb_address = (address >> 16) & 0xff;
    uint8_t mid_address = (address >> 8) & 0xff;
    uint8_t lsb_address = address & 0xff;
    // enable write
    _begin;
    SPI.transfer(0x06, SPI_TRANSMITONLY);
    _end;
    // erase
    _begin;
    SPI.transfer(0x20, SPI_TRANSMITONLY);
    SPI.transfer(msb_address, SPI_TRANSMITONLY);
    SPI.transfer(mid_address, SPI_TRANSMITONLY);
    SPI.transfer(lsb_address, SPI_TRANSMITONLY);
    _end;
    // disable write
    _begin;
    SPI.transfer(0x04, SPI_TRANSMITONLY);
    _end;
    // wait
    _wait;
}
void Winbond::erase32KBBlock(uint32_t address)
{
    uint8_t msb_address = (address >> 16) & 0xff;
    uint8_t mid_address = (address >> 8) & 0xff;
    uint8_t lsb_address = address & 0xff;
    // enable write
    _begin;
    SPI.transfer(0x06, SPI_TRANSMITRECEIVE);
    _end;
    // erase
    _begin;
    SPI.transfer(0x52, SPI_TRANSMITRECEIVE);
    SPI.transfer(msb_address, SPI_TRANSMITRECEIVE);
    SPI.transfer(mid_address, SPI_TRANSMITRECEIVE);
    SPI.transfer(lsb_address, SPI_TRANSMITRECEIVE);
    _end;
    // disable write
    _begin;
    SPI.transfer(0x04, SPI_TRANSMITRECEIVE);
    _end;
    _wait;
}
void Winbond::erase64KBBlock(uint32_t address)
{
    uint8_t msb_address = (address >> 16) & 0xff;
    uint8_t mid_address = (address >> 8) & 0xff;
    uint8_t lsb_address = address & 0xff;
    // enable write
    _begin;
    SPI.transfer(0x06, SPI_TRANSMITRECEIVE);
    _end;
    // erase
    _begin;
    SPI.transfer(0xd8, SPI_TRANSMITRECEIVE);
    SPI.transfer(msb_address, SPI_TRANSMITRECEIVE);
    SPI.transfer(mid_address, SPI_TRANSMITRECEIVE);
    SPI.transfer(lsb_address, SPI_TRANSMITRECEIVE);
    _end;
    // disable write
    _begin;
    SPI.transfer(0x04, SPI_TRANSMITRECEIVE);
    _end;
    _wait;
}

void Winbond::eraseChip()
{
    // enable write
    _begin;
    SPI.transfer(0x06, SPI_TRANSMITONLY);
    _end;
    // erase
    _begin;
    SPI.transfer(0x60, SPI_TRANSMITONLY);
    _end;
    // disable write
    _begin;
    SPI.transfer(0x04, SPI_TRANSMITONLY);
    _end;
    _wait;
}

void Winbond::wait()
{
    _wait;
    return;
}