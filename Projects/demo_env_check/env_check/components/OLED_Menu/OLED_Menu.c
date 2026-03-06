#include "OLED_Menu.h"
#include "OLED.h"
#include "Key_4.h"
#include "Msg.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// 定义menu界面指针
typedef enum
{
    MENU_MAIN,
    MENU_REALTIME,
    MENU_RISK,
    MENU_HISTORY,
    MENU_SETTING

}Menu_Type;

// 定义变量
#define Menu_Item_Num 4             // 菜单一共有四个选择
Menu_Type current_menu = MENU_MAIN; // 菜单当前显示界面
uint8_t menu_index = 0;             // 菜单当前浮标显示位置

// 核心按键逻辑:
/*
    KEY1 → 上
    KEY2 → 下
    KEY3 → 进入
    KEY4 → 返回
*/

// 进入菜单
void Enter_Menu(void) ;

// 按键交互
void Menu_Key_Handle(Key_Status_Typedef key)
{
    switch(key)
    {
        case KEY_1_Single:   // 在main菜单下,浮标上浮
            menu_index--;
            menu_index %= Menu_Item_Num ;
            break;

        case KEY_2_Single:   // 在main菜单下,浮标下浮
            menu_index++;
            menu_index %= Menu_Item_Num ;
            break;

        case KEY_3_Single:   // 确认
            Enter_Menu();    // 在main界面下,进入子菜单,在子菜单按下确认也没用
            break;

        case KEY_4_Single:   // 返回
            current_menu = MENU_MAIN;   // 在任何界面都有用:回到main界面
            break;

        default:
            break;
    }
}

// 进入菜单
void Enter_Menu(void)
{
    if(current_menu == MENU_MAIN)
    {
        switch(menu_index)
        {
            case 0:
                current_menu = MENU_REALTIME;
                break;

            case 1:
                current_menu = MENU_RISK;
                break;

            case 2:
                current_menu = MENU_HISTORY;
                break;

            case 3:
                current_menu = MENU_SETTING;
                break;
        }
        menu_index = 0;
    }
}

// 主菜单显示
void OLED_Show_MainMenu(void)
{
    OLED_Clear();

    OLED_ShowString(0,0,"MENU",OLED_8X16);

    OLED_ShowString(0,16, menu_index==0 ? ">Realtime" : " Realtime", OLED_6X8);
    OLED_ShowString(0,26, menu_index==1 ? ">Risk" : " Risk", OLED_6X8);
    OLED_ShowString(0,36, menu_index==2 ? ">History" : " History", OLED_6X8);
    OLED_ShowString(0,46, menu_index==3 ? ">Setting" : " Setting", OLED_6X8);

    OLED_Update();
}

// 数据显示界面
void OLED_Show_Realtime(void)
{
    OLED_Clear();

    char buf[20];

    sprintf(buf,"Temp: %.2f",Sensor_History[history_head].temp);
    OLED_ShowString(0,0,buf,OLED_6X8);

    sprintf(buf,"Humi: %.2f%%",Sensor_History[history_head].humi);
    OLED_ShowString(0,16,buf,OLED_6X8);

    sprintf(buf,"Light: %.2f",Sensor_History[history_head].Gray);
    OLED_ShowString(0,32,buf,OLED_6X8);

    OLED_Update();
}

// 风险显示界面
void OLED_Show_Risk(void)
{
    OLED_Clear();

    char buf[20];

    sprintf(buf,"Risk: %.2f",Sensor_History[history_head].risk);
    OLED_ShowString(0,0,buf,OLED_8X16);

    OLED_Update();
}

// 展示历史数据(滚动展示)
void OLED_Show_History(void)
{
    OLED_Clear();

    char buf[20];

    static uint8_t check_index = -1 ;
    check_index = (history_head+1) % HISTORY_NUM ;

    sprintf(buf,"Temp: %.2f",Sensor_History[check_index].temp);
    OLED_ShowString(0,0,buf,OLED_6X8);

    sprintf(buf,"Humi: %.2f%%",Sensor_History[check_index].humi);
    OLED_ShowString(0,16,buf,OLED_6X8);

    sprintf(buf,"Light: %.2f",Sensor_History[check_index].Gray);
    OLED_ShowString(0,32,buf,OLED_6X8);

    sprintf(buf,"Risk: %.2f",Sensor_History[check_index].risk);
    OLED_ShowString(0,48,buf,OLED_6X8);

    OLED_Update();
}

// ======================= OLED菜单任务 =======================
void Task_Menu(void *param)
{
    // OLED_Init(); // 已经在main初始化了

    while(1)
    {
        Menu_Key_Handle(Key_4_Get_Status()) ;

        switch(current_menu)
        {
            case MENU_MAIN:
                OLED_Show_MainMenu();
                break;

            case MENU_REALTIME:
                OLED_Show_Realtime();
                break;

            case MENU_RISK:
                OLED_Show_Risk();
                break;

            case MENU_HISTORY:
                OLED_Show_History();
                break;

            case MENU_SETTING:
                // OLED_Show_Setting();
                printf("MENU_SETTING\n") ;
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


