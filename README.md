# MGP
智慧型报告厅——我的毕业设计项目

    智慧型报告厅的设计分为五部分，分别是：网络视频会议、APP、服务器、集成控制面板、控制设备。
    我主要负责集成控制面板部分的代码编写工作。
    
系统设计框架：
    APP <==>  服务器 <==> 集成控制面板 <==> 控制设备

集成控制面板设计框架：
    WIFI网络 <==> ESP8266 <==> STM32F103ZET6 <==> CAN总线网络 <==> 控制设备