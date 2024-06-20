#include "main.h"
#include "UART.h"
#include "RunScript.h"

/*---------------Thread Function Start----------------*/
void *thread_function0(void *arg) {
    int CharCnt = 0;
    char buf[256];
    char *content = NULL;
    int fd = *((int*)arg);
    while(1)
    {
        executeScript("sudo ./GetPCStatus_Simple.sh", &content, MAX_BUFFER_SIZE) ;
        printf("%s", content);

        strcpy(buf, content);
        CharCnt = strlen(buf) + 1;
        buf[CharCnt] = '\0';
        pthread_mutex_lock(&mutex); 
        //write(fd, buf, CharCnt);
        pthread_mutex_unlock(&mutex);

        free(content);
        sleep_ms(1 * 1000); //1s
    }

    return NULL;
}

void *thread_function1(void *arg) {
    int CharCnt = 0;
    char buf[256];
    char *content = NULL;
    char delimiter = '\n';
    int arrsize = 0;
    char *strarr[2];
    int fd = *((int*)arg);
    char *ptr;
    while(1)
    {
        executeScript("sudo ./GetPCStatus.sh", &content, MAX_BUFFER_SIZE) ;
        printf("%s", content);

        if(strchr(content,'%') != 0)
        {
            ptr = strchr(content, '\n');
            if(ptr != NULL)
            {
                splitString(content, delimiter, strarr, &arrsize);
                for (int i = 0; i < arrsize; i++)
                {
                    strcpy(buf, strarr[i]);
                    CharCnt = strlen(buf) + 1;
                    buf[CharCnt] = '\0';
                    pthread_mutex_lock(&mutex); 
                    //write(fd, buf, CharCnt);
                    printf("%s\n", buf);
                    pthread_mutex_unlock(&mutex);
                    sleep_ms(1);
                }
            } 
        }
        else
            sleep(1);

        free(content);
        sleep_ms(1 * 1000); //1s
    }

    return NULL;
}


void *thread_function2(void *arg) {
    char message[256];
    int GOODCharCnt = 0;
    char *GOOD = "GOOD";
    int fd = *((int*)arg);

    strcpy(message, GOOD);
    GOODCharCnt = strlen(message) + 1;
    printf("GOODCharCnt : %d\n",GOODCharCnt);
    message[GOODCharCnt] = '\0';

    while(1)
    {
        pthread_mutex_lock(&mutex); 
        //write(fd, message, GOODCharCnt);
        printf("%s\n", message);
        pthread_mutex_unlock(&mutex);
        sleep_ms(Host_Period);
    }
    return NULL;
}
/*---------------Thread Function End----------------*/

int TestString()
{
    char *Coamd1String = "PC_STATUS1,1206,0%,0%,0%,0%,2%,0%,2%,0%";
    char *Coamd2String = "PC_STATUS2,53.656C,-256C";
    UARTFormat HeartbeatUR, IsSimpleUR, HeartbeatThrUR, HeartbeatPeriodUR;
    UARTFormat UARTArry[9];
    unsigned long HRBT= ('G' << 24) | ('O' << 16) | ('O' << 8) | 'D'; 
    printf("0x%08X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\n", HRBT, 'G', 'O', 'O', 'D');
    
    AnalyzeString(Coamd1String, UARTArry);
    for(int i = 0; i < 9; i++)
    {
        printf("[%d]ID : 0x%04X, DATA : 0x%08X, Checksum : 0x%04X\n", i, UARTArry[i].ID, UARTArry[i].Data, UARTArry[i].Checksum);
    }
    
    AnalyzeString(Coamd2String, UARTArry);
    for(int i = 0; i < 2; i++)
    {
        printf("[%d]ID : 0x%04X, DATA : 0x%08X, Checksum : 0x%04X\n", i, UARTArry[i].ID, UARTArry[i].Data, UARTArry[i].Checksum);
    }

    createPacket(&HeartbeatUR, HEARTBEAT, ('G' << 24) | ('O' << 16) | ('O' << 8) | 'D');
    createPacket(&IsSimpleUR, IS_SIMPLE, (unsigned long)Is_Simple);
    createPacket(&HeartbeatThrUR, HEARTBEAT_THR, (unsigned long)Heartbeat_Thr);
    createPacket(&HeartbeatPeriodUR, HEARTBEAT_PERIOD, (unsigned long)Heartbeat_Period);

    printf("ID : 0x%04X, DATA : 0x%08X, Checksum : 0x%04X\n", HeartbeatUR.ID, HeartbeatUR.Data, HeartbeatUR.Checksum);
    printf("ID : 0x%04X, DATA : 0x%08X, Checksum : 0x%04X\n", IsSimpleUR.ID, IsSimpleUR.Data, IsSimpleUR.Checksum);
    printf("ID : 0x%04X, DATA : 0x%08X, Checksum : 0x%04X\n", HeartbeatThrUR.ID, HeartbeatThrUR.Data, HeartbeatThrUR.Checksum);
    printf("ID : 0x%04X, DATA : 0x%08X, Checksum : 0x%04X\n", HeartbeatPeriodUR.ID, HeartbeatPeriodUR.Data, HeartbeatPeriodUR.Checksum);


    return 0;
}

int main(int argc, char **argv) 
{
    

    int fd;
    pthread_t thread0, thread1, thread2;
    int res0, res1, res2;

    read_ini_file("CANDataFormat.ini", "Is_Simple", &Is_Simple);
    read_ini_file("CANDataFormat.ini", "Heartbeat_Threshold", &Heartbeat_Thr);
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
    printf("Heartbeat Threshold : %d\n", Heartbeat_Thr);
    printf("Heartbeat Frequency : %d\n", Heartbeat_Period);
    printf("Host Frequency : %d\n", Host_Period);

    TestString();
/*
    pthread_mutex_init(&mutex, NULL); 

    printf("uart_start\n");
    if (uart_start(&fd) != 0) {
        return 1;
    }

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
  
    //pthread_join(thread2, NULL);

    uart_stop(fd);
    pthread_mutex_destroy(&mutex);
*/
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

void sleep_ms(unsigned long milliseconds) {
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
    char message[256];
    int CharCnt = 0;

    Heartbeat_Thr = chkLimit(100, 1, Heartbeat_Thr);
    Heartbeat_Period = chkLimit(1000, 5, Heartbeat_Period);

    sprintf(message, "HB_INIT_FREQ,%d,%d,%d", Heartbeat_Thr, Heartbeat_Period, Is_Simple);
    CharCnt = strlen(message) + 1;
    message[CharCnt] = '\0';

    pthread_mutex_lock(&mutex); 
    write(fd, message, CharCnt);
    pthread_mutex_unlock(&mutex);
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
    if(num == 1)
    {
        for(int i = 0; i < (cnt - 1) ; i++)
        {
            Datatmp = atoi(Parameter[i + 1]);
            createPacket(&Format[i], (PC_RAM + i), Datatmp);
        }
    }
    else if (num == 2)
    {
        for(int i = 0; i < (cnt - 1) ; i++)
        {
            ftmp = strtof(Parameter[i + 1], &endptr);
            Datatmp = Float2IEEE754(ftmp);
            createPacket(&Format[i], (PC_CPU_TEMP + i), Datatmp);
        }
    }
    
}

void AnalyzeString(char *inputString, UARTFormat* Format)
{
    char delimiter = ',';
    char *result[20];
    int resultSize = 0;
    int i = 0;

    splitString(inputString, delimiter, result, &resultSize);
    if(!strncmp(inputString, "PC_STATUS1", strlen("PC_STATUS1")))
    {
        ParameterSetting(Format, 1, result, resultSize);
    }
    else if(!strncmp(inputString, "PC_STATUS2", strlen("PC_STATUS2")))
    {
        ParameterSetting(Format, 2, result, resultSize);
    }

    for (i = 0; i < resultSize; i++) {
        free(result[i]);
    }
}

void TestFunction(int fd)
{
    char message[256];
    int GOODCharCnt = 0;
    char *GOOD = "GOOD";

    strcpy(message, GOOD);
    GOODCharCnt = strlen(message) + 1;
    printf("GOODCharCnt : %d\n",GOODCharCnt);
    message[GOODCharCnt] = '\0';

    for(int i = 0; i < 0xFFFF; i++)
    {

        if((i % 600) != 0)
        {
            write(fd, message, GOODCharCnt);
            sleep_ms(Host_Period);
        }
        else
        {
            printf("%s, %d\n", "Loss", i);
            write(fd, message, GOODCharCnt);
            sleep_ms(20);
        }
    }
    printf("%s", "End\n");
}
/*---------------Normal Function End----------------*/