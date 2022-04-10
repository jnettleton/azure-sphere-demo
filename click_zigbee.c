// https://libstock.mikroe.com/projects/view/3826/zigbee-click
// file:C:\Program%20Files%20(x86)\Microsoft%20Azure%20Sphere%20SDK\Sysroots\11\usr\include\applibs

#include "click_zigbee.h"

//#include <fcntl.h>
#include <unistd.h>


bool zigbee_found;

//static inline void UART_InitConfig(UART_Config* uartConfig);
//static inline int UART_Open(UART_Id uartId, const UART_Config* uartConfig);

uint8_t dev_mode;
uint8_t app_mode;

const uint8_t DEV_HOST = 1;
const uint8_t DEV_USER = 2;

const uint8_t APP_TASK = 11;
const uint8_t APP_INIT = 12;

char AT_BCAST_MSG[15] = ":00,MikroE";

char AT_HOST_CFG1[10] = "00=6314";
char AT_HOST_CFG2[20] = "0A=0914;password";
char AT_HOST_CFG3[50] = "09=5A6967426565416C6C69616E63653039;password";

int zigbee_uart_fd = -1;
UART_Id zigbee_uart_id = 0;

UART_Config zigbee_uart_config = {
    .blockingMode = UART_BlockingMode_NonBlocking,
    .baudRate = 115200,
    .dataBits = UART_DataBits_Eight,
    .parity = UART_Parity_None,
    .stopBits = UART_StopBits_One,
    .flowControl = UART_FlowControl_RTSCTS
};

void zigbee_open(void)
{
    zigbee_uart_id = SAMPLE_CLICK1_UART;
    UART_InitConfig(&zigbee_uart_config);
    zigbee_uart_fd = UART_Open(zigbee_uart_id, &zigbee_uart_config);
}

void zigbee_close(void)
{
    if (zigbee_uart_fd >= 0)
    {
        close(zigbee_uart_fd);
        zigbee_uart_fd = -1;
    }
}

ssize_t zigbee_read(char *buffer, size_t size)
{
    //ssize_t read(int, void*, size_t);
    return read(zigbee_uart_fd, buffer, size);
}

ssize_t zigbee_write(const char *buffer, size_t size)
{
    //ssize_t write(int, const void*, size_t);
    return write(zigbee_uart_fd, buffer, size);
}

//void log_write(uint8_t* str_buf, uint8_t str_type)
//{
//    char* temp_buf = str_buf;
//    mikrobus_logWrite(temp_buf, str_type);
//}
//
//void resp_wait()
//{
//    uint8_t resp_flag;
//    for (; ; )
//    {   //Cheks library buffer
//        resp_flag = zigbee_resp();
//
//        if ((APP_TASK == app_mode) && (DEV_USER == dev_mode))
//        {   //If there is anything in buffer logs it 
//            if ((ZIGBEE_WAIT != resp_flag))
//            {
//                zigbee_log_buffer();
//                zigbee_clear_dev_buffer();
//                break;
//            }
//        }
//        else
//        {   //Checks if there is OK or ERROR message in buffer
//            if ((ZIGBEE_OK == resp_flag) ||
//                (ZIGBEE_ERROR == resp_flag))
//            {
//                break;
//            }
//        }
//    }
//}

//void system_init()
//{
//    mikrobus_gpioInit(_MIKROBUS1, _MIKROBUS_INT_PIN, _GPIO_INPUT);
//    mikrobus_gpioInit(_MIKROBUS1, _MIKROBUS_RST_PIN, _GPIO_OUTPUT);
//    mikrobus_gpioInit(_MIKROBUS1, _MIKROBUS_AN_PIN, _GPIO_OUTPUT);
//
//    mikrobus_uartInit(_MIKROBUS1, &ZIGBEE_UART_CFG[0]);
//
//    mikrobus_logInit(_LOG_USBUART, 115200);
//
//    Delay_ms(100);
//}

//void application_init()
//{
//    mikrobus_logWrite(" ", _LOG_LINE);
//    mikrobus_logWrite(" ***** APP INIT ***** ", _LOG_LINE);
//    zigbee_uart_driver_init((zigbee_obj_t)&_MIKROBUS1_GPIO,
//        (zigbee_obj_t)&_MIKROBUS1_UART);
//
//    //Selects device mode                        
//    dev_mode = DEV_HOST;
//    app_mode = APP_INIT;
//
//    //Init library
//    zigbee_driver_init(&log_write);
//    //Init of UART ISR
//    zigbee_init_uart_isr();
//    Delay_ms(500);
//    //Restarts device and clears library friver
//    zigbee_restart();
//    zigbee_clear_dev_buffer();
//    //Sends AT command for checking communication
//    zigbee_send_cmd(ZIGBEE_AT);
//    resp_wait();
//    //Disconnects from previous PAN if connected
//    zigbee_send_cmd(ZIGBEE_AT_DASSL);
//    resp_wait();
//    //Software reset
//    zigbee_send_cmd(ZIGBEE_ATZ);
//    resp_wait();
//    //Checks device information
//    zigbee_send_cmd(ZIGBEE_ATI);
//    resp_wait();
//    //Chhecks if device is connected to some PAN network
//    zigbee_send_cmd(ZIGBEE_AT_N);
//    resp_wait();
//
//    if (DEV_HOST == dev_mode)
//    {
//        //Configures device and creates new PAN network
//        zigbee_send_at(ZIGBEE_ATS, &AT_HOST_CFG1[0]);
//        resp_wait();
//        zigbee_send_at(ZIGBEE_ATS, &AT_HOST_CFG2[0]);
//        resp_wait();
//        zigbee_send_at(ZIGBEE_ATS, &AT_HOST_CFG3[0]);
//        resp_wait();
//        zigbee_send_cmd(ZIGBEE_AT_EN);
//        resp_wait();
//    }
//    else if (DEV_USER == dev_mode)
//    {
//        //Joins to some PAN network if there is one
//        zigbee_send_cmd(ZIGBEE_AT_JN);
//        resp_wait();
//    }
//    //Checks if it's connected to some network and returns OK if it's connected
//    zigbee_send_cmd(ZIGBEE_AT_IDREQ);
//    resp_wait();
//    //Checks again if it's connected to some network
//    zigbee_send_cmd(ZIGBEE_AT_N);
//    resp_wait();
//
//    Delay_ms(1000);
//    app_mode = APP_TASK;
//    mikrobus_logWrite(" ***** APP TASK ***** ", _LOG_LINE);
//}

//void application_task()
//{
//    if (DEV_HOST == dev_mode)
//    {
//        //Broadcasts message 'MikroE' over it's PAN network
//        zigbee_send_at(ZIGBEE_AT_BCAST, &AT_BCAST_MSG[0]);
//        resp_wait();
//        Delay_ms(3000);
//    }
//    else if (DEV_USER == dev_mode)
//    {
//        //Checks if there is something received
//        resp_wait();
//    }
//}

//void main()
//{
//    system_init();
//    application_init();
//
//    for (; ; )
//    {
//        application_task();
//    }
//}
