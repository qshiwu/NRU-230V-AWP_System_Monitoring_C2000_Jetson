#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 1024
int Is_Simple = 0;
int Heartbeat_Thr = 100;
int Heartbeat_Period = 10;
int Host_Period = 5;
int Host_Loss = 0;
pthread_mutex_t mutex;

#pragma pack(push, 1)
typedef union H_Float
{
    unsigned long data;
    float value;
}H_Float;

typedef struct
{
    uint16_t ID;         // 2 bytes
    uint32_t Data;       // 4 bytes
    uint16_t Checksum;   // 2 bytes
}UARTFormat;

typedef enum
{
    HEARTBEAT = 0x0000U,
    PC_RAM  = 0x0001U, 
    PC_CPU0  = 0x0002U,
    PC_CPU1  = 0x0003U,
    PC_CPU2  = 0x0004U,
    PC_CPU3  = 0x0005U,
    PC_CPU4  = 0x0006U,
    PC_CPU5  = 0x0007U,
    PC_CPU6  = 0x0008U,
    PC_CPU7  = 0x0009U,
    PC_CPU_TEMP = 0x000BU,
    PC_GPU_TEMP,
    IS_SIMPLE,
    HEARTBEAT_THR,
    HEARTBEAT_PERIOD,
} CommandID;

int read_ini_file(const char *filename, const char *key_to_find, int *value_found);
unsigned long Float2IEEE754(float fInput);
void msleep(int ms);
void sleep_ms(unsigned long milliseconds);
void splitString(const char *input, char delimiter, char **result, int *resultSize) ;
int chkLimit(int upLimit, int lowLimit, int value);
void send_Hearbeat_Init(int fd);
unsigned short calculateChecksum(UARTFormat *packet);
void createPacket(UARTFormat* Format, unsigned short id, unsigned long data);
void sendPacket(int serialPort, UARTFormat *packet);
void sendMultiPacket(int serialPort, UARTFormat *packet, int cnt);
int AnalyzeString(char *inputString, UARTFormat* Format, int *retSize);
int countCharacter(const char *str, char target);

#endif