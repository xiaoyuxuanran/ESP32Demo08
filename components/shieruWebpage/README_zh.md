| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# shieruWebpage介绍

shieruWebpage是使用WiFi的ap和station模式来创建一个http server的demo。

## 如何去使用这个demo

### 加入工程

1.将shieruWebpage放到components文件夹里面。
2.把里面`partitions.csv`分区表放到工程根目录，并进入menuconfig配置flash大小和设置分区表为custom。
3.在main.c里面需要到入`suwp_init.h`头文件，执行`suwp_init()`函数。

### 启用wifi

1.在`suwp_init.cpp`文件中用户代码区域调用`suwp_wifi wifi;`就可以初始化wifi。
2.使用`wifi.wifi_ap_start()`和`wifi.wifi_station_start()`就可以启用wifi的ap模式（热点）和station模式（站点），并且可以初始化wifi名称和密码。默认wifi热点名称和密码分别是`shieru`和`3286811313`,wifi的station模式默认连接的wifi名称和密码分别是`ss`和`3286811313`。

ap示例如下：
``` c++
suwp_wifi wifi;
// wifi.wifi_set_ap_ssid("your ssid"); // 可不设置
// wifi.wifi_set_ap_password("your password"); // 可不设置
wifi.wifi_ap_start();
```
station示例如下：
``` c++
suwp_wifi wifi;
// wifi名称和密码根据需要更改，wifi只能支持2.4G
wifi.wifi_set_station_ssid("your ssid");
wifi.wifi_set_station_password("your password");
wifi.wifi_astation_start();
```

调用`wifi.wifi_ap_start()`和`wifi.wifi_astation_start()`可进行切换，ap和station共存后续会加。

3.开启http_server服务

``` c++
suwp_http_server http_server; // 这句会完成初始化
```

4.编译烧录，复制打印输出的ip地址到浏览器就可以看网页了。修改`shieru.html`和`http_server.cpp`来实现自定义功能。