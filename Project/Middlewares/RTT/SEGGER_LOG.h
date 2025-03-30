

#ifndef LOG_H
#define LOG_H


#include "SEGGER_RTT.h"




//#include "Data_information.h"
#if defined(USE_HAL_DRIVER)
extern uint32_t HAL_GetTick(void);
extern uint32_t HAL_GetUs(void);
    #define SYS_MS HAL_GetTick()
    #define SYS_US HAL_GetUs()
#else
    
    #define SYS_MS   log_getTick() //log_getUs() // 如果不是使用HAL库 则使用自定义的时间源  ///-mdfc_40325
    #define SYS_US   log_getTick()
#endif




#define USE_LOG_DEBUG   1
#define PRINT_TIMESTAMP 1

typedef enum {
    LOG_TERMINAL0,
    LOG_TERMINAL1,
    LOG_TERMINAL2,
    LOG_TERMINAL3,
    LOG_TERMINAL4,
    LOG_TERMINAL5,
} LogTerminal_e;

typedef enum {
    LOG_ARR_BYTE1,
    LOG_ARR_BYTE2,
    LOG_ARR_BYTE4,
    LOG_ARR_FLOAT,
    LOG_ARR_INT,
} LogArrayType_e;





#if USE_LOG_DEBUG


    #if PRINT_TIMESTAMP     ///-mdfc_40325

      // #if  !APP
           #define LOG_PROTO(type, color, format, ...)    SEGGER_RTT_printf(0, "%s""[%02d:%03d:%03d]" "%s" format,color,(SYS_MS / 1000) % 60, SYS_MS % 1000, SYS_US%1000,type,##__VA_ARGS__);
      // #else
//           #define LOG_PROTO(type, color, format, ...)   \
//             SEGGER_RTT_printf(0, "%s" "[%02d:%02d:%02d:%03d]" "%s" format,color,(Sys_Time.hour),(Sys_Time.min),(SYS_MS%60),(SYS_MS%1000),type,##__VA_ARGS__);
     //  #endif

        //            #if PRINT_TIMESTAMP
        //                #define LOG_PROTO(type, color, format, ...)                                                                                        \
        //                    SEGGER_RTT_printf(0, "[%02d:%03d:%03d] %s%s" format "%s\r\n", (SYS_MS / 1000) % 60, SYS_MS % 1000, SYS_US % 1000, color, type, \
        //                                      ##__VA_ARGS__, RTT_CTRL_RESET);

        // #define LOG_PROTO(type, color, format, ...)                                                                                                \
        //     SEGGER_RTT_printf(0, "[%02d:%02d:%02d:%03d:%03d] %s%s" format "%s\r\n", (SYS_MS / (60 * 60 * 1000)) % 24, (SYS_MS / (60 * 1000)) % 60, \
        //                       (SYS_MS / 1000) % 60, SYS_MS % 1000, SYS_US % 1000, color, type, ##__VA_ARGS__, RTT_CTRL_RESET);

    #else
        #define LOG_PROTO(type, color, format, ...) SEGGER_RTT_printf(0, "%s%s" format "%s\r\n", color, type, ##__VA_ARGS__, RTT_CTRL_RESET);
    #endif


///-mdfc_40325
#if  1
    #define LOG_I(format, ...)     LOG_PROTO("[I]:",RTT_CTRL_TEXT_BRIGHT_WHITE, format, ##__VA_ARGS__)      // 白色日志输出
    #define LOG_D(format, ...)     LOG_PROTO("[D]:",RTT_CTRL_TEXT_BRIGHT_GREEN,format, ##__VA_ARGS__)  // 绿色日志输出
    #define LOG_W(format, ...)     LOG_PROTO("[W]:", RTT_CTRL_TEXT_BRIGHT_YELLOW,format, ##__VA_ARGS__)  // 黄色日志输出
    #define LOG_E(format, ...)     LOG_PROTO("[E]:", RTT_CTRL_TEXT_BRIGHT_RED, format, ##__VA_ARGS__)    // 红色日志输出
    #define LOG_CLEAR()            SEGGER_RTT_WriteString(0, "\r\n" RTT_CTRL_CLEAR)                          // 清屏
#else
    #define LOG_I(format, ...)  LOG_PROTO("[INFO]:", "", format, ##__VA_ARGS__)                           // 无颜色日志输出
    #define LOG_D(format, ...) LOG_PROTO("[DEBUG]:", RTT_CTRL_TEXT_BRIGHT_GREEN, format, ##__VA_ARGS__)  // 绿色日志输出
    #define LOG_W(format, ...)  LOG_PROTO("[WARN]:", RTT_CTRL_TEXT_BRIGHT_YELLOW, format, ##__VA_ARGS__)  // 黄色日志输出
    #define LOG_E(format, ...) LOG_PROTO("[ERROR]:", RTT_CTRL_TEXT_BRIGHT_RED, format, ##__VA_ARGS__)    // 红色日志输出
    #define LOG_CLEAR()            SEGGER_RTT_WriteString(0, "\r\n" RTT_CTRL_CLEAR)                          // 清屏
#endif

    // 打印数组
    #define LOG_ARRAY(pArr, len, logArrayType, terminal)                        \
        do {                                                                    \
            SEGGER_RTT_SetTerminal(terminal);                                   \
            for (int i = 0; i < len; i++) {                                     \
                switch (logArrayType) {                                         \
                    case LOG_ARR_BYTE1:                                         \
                        SEGGER_RTT_printf(0, "%02x ", *(uint8_t *)(pArr + i));  \
                        break;                                                  \
                    case LOG_ARR_BYTE2:                                         \
                        SEGGER_RTT_printf(0, "%04x ", *(uint16_t *)(pArr + i)); \
                        break;                                                  \
                    case LOG_ARR_BYTE4:                                         \
                        SEGGER_RTT_printf(0, "%08x ", *(uint32_t *)(pArr + i)); \
                        break;                                                  \
                    case LOG_ARR_FLOAT:                                         \
                        SEGGER_RTT_printf(0, "%f ", *(float *)(pArr + i));      \
                        break;                                                  \
                    case LOG_ARR_INT:                                           \
                        SEGGER_RTT_printf(0, "%d ", *(int *)(pArr + i));        \
                        break;                                                  \
                    default:                                                    \
                        break;                                                  \
                }                                                               \
            }                                                                   \
            SEGGER_RTT_printf(0, "\r\n");                                       \
            SEGGER_RTT_SetTerminal(0);                                          \
        } while (0)

    // 初始化
    #define LOG_INIT()               \
        do {                         \
            SEGGER_RTT_Init();       \
            LOG_CLEAR();             \
            LOG_I("RTT Inited!");    \
        } while (0)

#else
    #define LOG_INFO(format, ...)
    #define LOG_DEBUG(format, ...)
    #define LOG_WARN(format, ...)
    #define LOG_ERROR(format, ...)
    #define LOG_CLEAR()
    #define LOG_ARRAY(pArr, len, logArrayType, terminal)
    #define LOG_INIT()
#endif

#endif


