#include "main.h"
#include "UART.h"
#include "RunScript.h"

/*---------------Thread Function Start----------------*/
void *thread_function0(void *arg) {
    char *content = NULL;
    int fd = *((int*)arg);
    UARTFormat UARTArry[20];
    int cmdSize = 0;
    int AnalyzResult = 0;
    while(1)
    {
        executeScript("sudo ./GetPCStatus_Simple.sh", &content, MAX_BUFFER_SIZE) ;
        printf("%s", content);

        AnalyzResult = AnalyzeString(content, UARTArry, &cmdSize);
        for(int i = 0; i < cmdSize; i++)
        {
            if(AnalyzResult != 0)
                break;
            sendPacket(fd, &UARTArry[i]);
            sleep_ms(13);
        }

        free(content);
        sleep_ms(1 * 1000); //1s
    }

    return NULL;
}

void *thread_function1(void *arg) {
    char *content = NULL;
    char delimiter = '\n';
    int arrsize = 0;
    char *strarr[2];
    int fd = *((int*)arg);
    char *ptr;
    UARTFormat UARTArry[20];
    int cmdSize = 0;
    int AnalyzResult = 0;
    while(1)
    {
        executeScript("sudo ./GetPCStatus.sh", &content, MAX_BUFFER_SIZE) ;
        printf("%s", content);
        ptr = strchr(content, '\n');
        if(ptr != NULL)
        {
            splitString(content, delimiter, strarr, &arrsize);
            for (int i = 0; i < arrsize; i++)
            {
                AnalyzResult = AnalyzeString(strarr[i], UARTArry, &cmdSize);
                if(AnalyzResult != 0)
                {
                    printf("AnalyzResult : %d\n", AnalyzResult);
                    break;
                }
                for(int i = 0; i < cmdSize; i++)
                {
                    sendPacket(fd, &UARTArry[i]);
                    sleep_ms(13);
                }
            }
            free(content);
        }
        sleep_ms(1 * 1000); //1s
    }

    return NULL;
}


void *thread_function2(void *arg) {
    int i = 0;
    int fd = *((int*)arg);
    UARTFormat HeartbeatUR;
    createPacket(&HeartbeatUR, HEARTBEAT, ('G' << 24) | ('O' << 16) | ('O' << 8) | 'D');

    while(1)
    {   
        i++;
        sendPacket(fd, &HeartbeatUR);
        sleep_ms(Host_Period);        
    }
    return NULL;
}
/*---------------Thread Function End----------------*/

void sendPacket(int serialPort, UARTFormat *packet)
{
    pthread_mutex_lock(&mutex); 
    write(serialPort, (const void *)packet, sizeof(UARTFormat));
    pthread_mutex_unlock(&mutex);
}

void sendMultiPacket(int serialPort, UARTFormat *packet, int cnt)
{
    char *UARTChar;
    int SendSize = 0;
    int index = 0;
    unsigned char *bytePtr;

    SendSize = cnt * sizeof(UARTFormat);
    UARTChar = malloc( SendSize);
    for(int i = 0; i < cnt; i++)
    {
        index = i * sizeof(UARTFormat);
        bytePtr = (unsigned char *) &packet[i];
        for(int j = 0; j < sizeof(UARTFormat); j++)
        {
            UARTChar[index + j] = bytePtr[j];
        }
    }
    
    pthread_mutex_lock(&mutex); 
    write(serialPort, UARTChar, SendSize);
    pthread_mutex_unlock(&mutex);
    free(UARTChar);
}

void printStructBytes(int fd, UARTFormat *packet)
{
    printf("ID : %d\n", packet->ID);
    unsigned char *bytePtr = (unsigned char *)packet;
    for (size_t i = 0; i < sizeof(UARTFormat); i++)
    {
        printf("Byte %zu: 0x%02X\n", i, bytePtr[i]);
        
    }
    printf("---------------------\n");
    write(fd, packet, sizeof(UARTFormat));
}

int main(int argc, char **argv) 
{
    int fd;
    pthread_t thread0, thread1, thread2;
    int res0, res1, res2;

    read_ini_file("CANDataFormat.ini", "Is_Simple", &Is_Simple);
    read_ini_file("CANDataFormat.ini", "Heartbeat_Period", &Heartbeat_Period);
    read_ini_file("CANDataFormat.ini", "Host_Period", &Host_Period);
    
    if(Is_Simple > 0 && Host_Period < 5)
    {
        Host_Period = 5;
    }
    if(Host_Period >= Heartbeat_Period || Host_Period == 0)
    {
        printf("Host_Period %d Error\n", Host_Period);
        return -1;
    }

    printf("CAN Format : %d\n", Is_Simple);
    printf("Heartbeat Frequency : %d\n", Heartbeat_Period);
    printf("Host Frequency : %d\n", Host_Period);

    pthread_mutex_init(&mutex, NULL); 

    printf("uart_start\n");
    if (uart_start(&fd) != 0) {
        return 1;
    }
    sleep(1);

    send_Hearbeat_Init(fd);

    res2 = pthread_create(&thread2, NULL, thread_function2, &fd);
    if (res2 != 0) {
        perror("Create thread2 fail");
        return 1;
    }

    if(Is_Simple == 0)
    {
        //CAN bus don't send anything
    }
    else if(Is_Simple == 1)
    {
        res0 = pthread_create(&thread0, NULL, thread_function0, &fd);
        if (res0 != 0) {
            perror("Create thread0 fail");
            return 1;
        }

        pthread_join(thread0, NULL);
    }
    else
    {
        res1 = pthread_create(&thread1, NULL, thread_function1, &fd);
        if (res1 != 0) {
            perror("Create thread1 fail");
            return 1;
        }
        pthread_join(thread1, NULL);
    }
  
    pthread_join(thread2, NULL);
    sleep(1);
    uart_stop(fd);
    pthread_mutex_destroy(&mutex);

    return 0;
}

/*---------------Normal Function Start----------------*/

int read_ini_file(const char *filename, const char *key_to_find, int *value_found) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 0;
    }

    char line[MAX_BUFFER_SIZE];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == ';' || line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        char *key = strtok(line, "=");
        char *value_str = strtok(NULL, "\n");

        if (key != NULL && value_str != NULL && strcmp(key, key_to_find) == 0) {
            *value_found = atoi(value_str);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

unsigned long Float2IEEE754(float fInput)
{
    H_Float tmp;
    tmp.value = fInput;
    return tmp.data;
}

void msleep(int ms)
{
     usleep(1000 * ms);
}

void sleep_ms(unsigned long milliseconds) 
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
}

void splitString(const char *input, char delimiter, char **result, int *resultSize) 
{
    char *inputCopy = strdup(input);
    char *token = strtok(inputCopy, &delimiter);
    *resultSize = 0;

    while (token != NULL) {
        result[*resultSize] = strdup(token);
        (*resultSize)++;

        token = strtok(NULL, &delimiter);
    }

    free(inputCopy);
}

int chkLimit(int upLimit, int lowLimit, int value)
{
    if(value > upLimit)
    {
        value = upLimit;
    }
    else if(value < lowLimit)
    {
        value = lowLimit;
    }
    else
    {
        value = value;
    }
    
    return value;
}

void send_Hearbeat_Init(int fd)
{
    UARTFormat IsSimpleUR, HeartbeatThrUR, HeartbeatPeriodUR;

    Heartbeat_Period = chkLimit(1000, 11, Heartbeat_Period);

    createPacket(&IsSimpleUR, IS_SIMPLE, (unsigned long)Is_Simple);
    createPacket(&HeartbeatPeriodUR, HEARTBEAT_PERIOD, (unsigned long)Heartbeat_Period);

    sendPacket(fd, &IsSimpleUR);
    sleep_ms(1);
    sendPacket(fd, &HeartbeatThrUR);
    sleep_ms(1);
    sendPacket(fd, &HeartbeatPeriodUR);
    sleep_ms(1);
    sleep_ms(Host_Period);
}

unsigned short calculateChecksum(UARTFormat *packet) {
    unsigned int sum = 0;

    sum += (packet->ID & 0xFF);
    sum += ((packet->ID >> 8) & 0xFF);

    sum += (packet->Data & 0xFF);
    sum += ((packet->Data >> 8) & 0xFF);
    sum += ((packet->Data >> 16) & 0xFF);
    sum += ((packet->Data >> 24) & 0xFF);

    return sum;
}

void createPacket(UARTFormat* Format, unsigned short id, unsigned long data) 
{
    Format->ID = id;
    Format->Data = data;
    Format->Checksum = calculateChecksum(Format);
}

void ParameterSetting(UARTFormat* Format, int num, char **Parameter, int cnt)
{
    unsigned long Datatmp = 0;
    float ftmp;
    char *endptr;
    int i = 0;
    if(num == 1)
    {
        for(i = 0; i < cnt ; i++)
        {
            if(i == 0)
                continue;
            
            char *param = Parameter[i];
            if (param[strlen(param) - 1] == '%') {
                param[strlen(param) - 1] = '\0';
            }
            Datatmp = atoi(param);
            createPacket(&Format[i - 1], (PC_RAM + (i - 1)), Datatmp);
        }
    }
    else if (num == 2)
    {
        for(i = 0; i < cnt ; i++)
        {
            if(i == 0)
                continue;

            char *param = Parameter[i];
            if (param[strlen(param) - 1] == 'C') {
                param[strlen(param) - 1] = '\0';
            }
            ftmp = strtof(param, &endptr);
            if(endptr != NULL)
            {
                Datatmp = Float2IEEE754(ftmp);
                createPacket(&Format[i - 1], (PC_CPU_TEMP + (i - 1)), Datatmp);
            }
            else
            {
                printf("strtof Error");
            }
        }
    }
    
}

int AnalyzeString(char *inputString, UARTFormat* Format, int *retSize)
{
    char delimiter = ',';
    char *result[20];
    int resultSize = 0;
    *retSize = 0;
    int retResult = 0;

    splitString(inputString, delimiter, result, &resultSize);
    if(!strncmp(result[0], "PC_STATUS1", strlen("PC_STATUS1")))
    {
        if(resultSize - countCharacter(inputString, delimiter) == 1)
            ParameterSetting(Format, 1, result, resultSize);
        else
        {
            printf("PC_STATUS1 Error %d, resultSize : %d\n", countCharacter(inputString, delimiter), resultSize);
            retResult = -1;
        }
            
    }
    else if(!strncmp(result[0], "PC_STATUS2", strlen("PC_STATUS2")))
    {
        if(resultSize - countCharacter(inputString, delimiter) == 1)
            ParameterSetting(Format, 2, result, resultSize);
        else
        {
            printf("PC_STATUS2 Error %d, resultSize : %d\n", countCharacter(inputString, delimiter), resultSize);
            retResult = -1;
        }
            
    }

    for (int i = 0; i < resultSize; i++) {
        free(result[i]);
    }

    *retSize = resultSize - 1;
    return retResult;
}

int countCharacter(const char *str, char target)
{
    int count = 0;
    while (*str) {
        if (*str == target) {
            count++;
        }
        str++;
    }
    return count;
}
/*---------------Normal Function End----------------*/