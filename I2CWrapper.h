#ifndef I2CWrapper_H
#define I2CWrapper_H

int ExitOnFail=1;
int DisplayFailMessage=1;

void I2CWrapperClose(int handle);
int I2CWrapperOpen(int BUS, int SlaveAddress);
int I2CWrapperSlaveAddress(int handle, int SlaveAddress);
int I2CWrapperReadBlock(int handle, unsigned char cmd, unsigned char  size,  void * array);
int I2CWrapperReadWord(int handle, unsigned char cmd);
int I2CWrapperReadByte(int handle, unsigned char cmd);
int I2CWrapperWriteWord(int handle,unsigned char cmd, unsigned short value);
int I2CWrapperWriteByte(int handle,unsigned char cmd, unsigned char value);

#endif //I2CWrapper_H

