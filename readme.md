[中文版](https://github.com/TuyaInc/Tuya_ble_lock_sdk_nRF52832) | [English](https://github.com/TuyaInc/Tuya_ble_lock_sdk_nRF52832/blob/master/readme-en.md)


## 1 概述
&emsp;&emsp;**涂鸦蓝牙门锁公版**指的是涂鸦蓝牙门锁的通用版本。
无论你的需求是什么，只要你开发的是**锁类业务应用**，想要通过**低功耗蓝牙**的通信方式接入**涂鸦智能APP和涂鸦云平台**，你都可以基于涂鸦蓝牙门锁公版的sdk做开发。  
&emsp;&emsp;我们的目标是：一份文档解决嵌入式软件开发过程中的所有问题。**所以遇到问题请首先查阅该文档。**  

&emsp;&emsp;**<font color=#FF0000>新手入门</font>请首先跳转至第5章，根据示例流程操作，通过串口模拟硬件，快速体验涂鸦锁类产品，该示例在后续开发过程中也是很好的调试工具。**




## 2 软件架构
&emsp;&emsp;涂鸦低功耗蓝牙产品基于蓝牙通用配网协议，该协议主要实现了蓝牙通用配网流程和基本的数据通信协议，是涂鸦低功耗蓝牙产品的基础协议，协议详述参考《[Tuya_ble_通用_sdk_协议](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_sdk/Tuya_ble_%E9%80%9A%E7%94%A8_sdk_%E5%8D%8F%E8%AE%AE_v3.1_190923.pdf)》，源码位于“[tuya_ble_sdk](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/tree/master/tuya_ble_lock_sdk/src/tuya_ble_sdk)”文件夹内。如果用户的平台有对应的Demo例程，则无需关心该协议，涂鸦已经做好了所有的移植工作；如果用户使用新平台，则需要移植该协议，移植流程参考《[Tuya_ble_通用_sdk_开发指南](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_sdk/Tuya_ble_%E9%80%9A%E7%94%A8_sdk_%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97_v0.1.0_190826.pdf)》。  

&emsp;&emsp;在通用配网协议的基础上，涂鸦定义了一整套锁类产品的dp点（功能点）协议，详细描述了锁类应用业务和手机APP之间的通信逻辑。为了方便用户开发，涂鸦已经封装好了这些dp点协议，源码详见app层的lock_dp_parser和lock_dp_report文件，协议内容参考《[Tuya_ble_锁类_dp 点规范](https://docs.qq.com/sheet/DWlNHUEdIZmF0TGRq?tab=mv4sfq&c=A1A0A0)》。  

&emsp;&emsp;以上为涂鸦蓝牙配网、通信和锁类业务的实现，属于**主框架**。在主框架之外，还有一些重要的**支撑组件**。组件分为两部分：一部分是第三方组件，负责存储、调试等相关的通用功能；一部分是涂鸦的组件，负责授权、产测等相关的专用功能。

&emsp;&emsp;第三方组件主要包括easyFlash、easyLogger和mbedtls。如果用户的平台有对应的Demo例程，则无需关心这些组件，涂鸦已经做好了所有的移植工作；如果用户使用新平台，则只需要对easyFlash和easyLogger组件做简单的移植即可，mbedtls组件（主要为加密接口）无需改动。

&emsp;&emsp;涂鸦的组件是一些通用的源码包，适用于涂鸦的大部分低功耗蓝牙单点产品，此处仅以锁类应用为例介绍，其他应用可参考。主要包括授权产测、ota、本地存储管理和整机产测：授权产测用于烧录和校验通用配网协议所需的mac、authkey和device
id等参数，详情参考《Tuya_ble_通用_授权产测协议_vxx.xx_xxxxxx》；ota实现涂鸦自己的蓝牙ota协议（非Demo平台需移植），详情参考《Tuya_ble_通用_sdk_协议_vxx.xx_xxxxxx》；本地存储管理实现锁类应用在本地的存储逻辑，其他应用可参考；整机产测基于无线的方式实现对产品的整体功能测试，详情参考《[Tuya_ble_通用_整机产测协议](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_%E4%BA%A7%E6%B5%8B/Tuya_ble_%E9%80%9A%E7%94%A8_%E6%95%B4%E6%9C%BA%E4%BA%A7%E6%B5%8B%E5%8D%8F%E8%AE%AE_v1.0_191017.pdf)》。

&emsp;&emsp;以上对主框架和支撑组件都做了介绍。除此之外，还有一个概念需要强调一下：为了提高app层（包括涂鸦组件）的兼容性，将app层和其它层之间做了简单的隔离，隔离层叫做“app_port”。也就是说，在app层只能够见到隔离层封装好的底层api（app_port开头）和app层自己的api接口。

<img src="https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/fe-static/tuya-docs/33386e72-a90e-4a49-9f8e-134ad7032113.png" width = "450"> 


#### 2.1 app
##### 2.1.1 app_lock
- 该文件夹主要实现应用层的锁类业务功能，仅适用于锁类产品。
  - lock_common：未分类的
  - lock_dp_parser：dp点解析，数据方向：手机→设备，《[Tuya_ble_锁类_dp 点规范](https://docs.qq.com/sheet/DWlNHUEdIZmF0TGRq?tab=mv4sfq&c=A1A0A0)》
  - lock_dp_report：dp点上报，数据方向：设备→手机，《[Tuya_ble_锁类_dp 点规范](https://docs.qq.com/sheet/DWlNHUEdIZmF0TGRq?tab=mv4sfq&c=A1A0A0)》
  - lock_hard：客户硬件接口和串口模拟硬件（入门或调试用）
  - lock_test：整机产测，《[Tuya_ble_通用_整机产测协议](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_%E4%BA%A7%E6%B5%8B/Tuya_ble_%E9%80%9A%E7%94%A8_%E6%95%B4%E6%9C%BA%E4%BA%A7%E6%B5%8B%E5%8D%8F%E8%AE%AE_v1.0_191017.pdf)》

##### 2.1.2 app_common
- 该文件夹主要实现应用层的通用功能，适用于各类蓝牙产品。
  - app_common：未分类的
  - app_flash：本地存储管理
  - app_ota：ota功能，《[Tuya_ble_通用_sdk_协议](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_sdk/Tuya_ble_%E9%80%9A%E7%94%A8_sdk_%E5%8D%8F%E8%AE%AE_v3.1_190923.pdf)》中ota章节
  - app_test：授权产测，《[Tuya_ble_通用_授权产测协议](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_%E4%BA%A7%E6%B5%8B/Tuya_ble_%E9%80%9A%E7%94%A8_%E6%8E%88%E6%9D%83%E4%BA%A7%E6%B5%8B%E5%8D%8F%E8%AE%AE_v1.3_190709.pdf)》
  - app_port：如下


#### 2.2 app_port
- 底层接口封装，为app层可能用到的所有接口提供统一的命名和管理，使应用层成为一个独立的模块，方便移植。


#### 2.3 tuya_ble_sdk
- 涂鸦低功耗蓝牙通用配网sdk，实现低功耗蓝牙设备和手机APP之间的配网流程和基础通信协议。
  - 实现协议：《[Tuya_ble_通用_sdk_协议](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_sdk/Tuya_ble_%E9%80%9A%E7%94%A8_sdk_%E5%8D%8F%E8%AE%AE_v3.1_190923.pdf)》
  - 开发指南：《[Tuya_ble_通用_sdk_开发指南](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_sdk/Tuya_ble_%E9%80%9A%E7%94%A8_sdk_%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97_v0.1.0_190826.pdf)》


#### 2.4 芯片原厂sdk
- demo工程使用的是nordic原厂的sdk，版本为： nRF5_SDK_15.3.0_59ac345。

##### 2.4.1 nrfs（nRF52832）
- 对nordic原厂sdk的接口封装：
  - nrfs_common：未分类
  - nrfs_scan_adv：扫描和广播
  - nrfs_ble：ble参数
  - nrfs_svc：服务和特征值
  - nrfs_uart：串口
  - nrfs_timer：定时器
  - nrfs_flash：flash
  - nrfs_gpio：gpio
  - nrfs_test：研发测试用

##### 2.4.2  bk（bk3431q）
- 对bk原厂sdk的接口封装：
  - bk_common：未分类
  - bk_scan_adv：扫描和广播
  - bk_ble：ble参数
  - bk_svc：服务和特征值
  - bk_uart：串口
  - bk_timer：定时器
  - bk_flash：flash
  - bk_gpio：gpio
  - bk_test：研发测试用


#### 2.5 component
- 此处指第三方组件。

##### 2.5.1 cpt_math
- 实现一些常见的数学算法，例如校验和、crc16、crc32、字节反转等。
- 纯c语言库，无论任何平台，客户均无需对该组件进行移植和改动。

##### 2.5.2 cpt_hash
- 实现一些常见的加密算法，例如aes128_ecb、aes128_cbc、md5计算等。
- 主要用于tuya_ble_sdk中的数据加密。
- 纯c语言库，无论任何平台，客户均无需对该组件进行移植和改动。

##### 2.5.3 cpt_mbedtls
- 实现一些常见的加密算法，例如aes128_ecb、aes128_cbc、md5计算等。
- 主要用于tuya_ble_sdk中的数据加密。
- 纯c语言库，无论任何平台，客户均无需对该组件进行移植和改动。

##### 2.5.4 cpt_fpe
- fpe算法，纯c语言库，无论任何平台，客户均无需对该组件进行移植和改动。

##### 2.5.5 cpt_easyflash
- 实现基于Flash的键值存储机制，主要用到如下4个api（详情见注释）：  
  - easyflash_init();
  - ef_set_env_blob(key, buf, size) ;
  - ef_get_env_blob(key, buf, size) ;
  - ef_del_env(key) ;

- 如果使用Demo平台，则已经实现了该组件的移植，客户可以直接使用；如果使用新平台，则需要客户移植该组件，至少需实现如上4个api的功能。
- 源码和教程：https://github.com/armink/EasyFlash

##### 2.5.6 cpt_easylogger
- 实现整个sdk的log打印机制，主要用到如下2个api（详情见注释）：
  - log_d();
  - elog_hexdump(name, width, buf, size);

- 如果使用Demo平台，则已经实现了该组件的移植，客户可以直接使用；如果使用新平台，则需要移植该组件，至少需实现如上2个api的功能。
- 源码和教程：https://github.com/armink/EasyLogger




## 3 重点解析
#### 3.1 本地存储管理
&emsp;&emsp;tuya_ble_sdk占用16k bytes Flash空间用于存储授权和tuya_ble_sdk相关的系统信息，需单独分配，用户无需关心，也不可占用，起始地址为“TUYA_NV_START_ADDR”。

&emsp;&emsp;app层默认占用16k bytes Flash空间用于存储离线记录数据和其他应用数据（key-value存储结构，基于easyFlash），用户可使用剩余空间，只要注意根据使用情况适当调整占用Flash空间的大小即可，起始地址为“EF_START_ADDR”，占用空间大小为“ENV_AREA_SIZE”，相关api如下：
```
uint32_t app_port_kv_set(const char *key, const void *buf, size_t size);
uint32_t app_port_kv_get(const char *key, void *buf, size_t size);
uint32_t app_port_kv_del(const char *key);
```  
&emsp;&emsp;以上提到的Flash空间仅指存储区域，不包含代码区域。  
&emsp;&emsp;本地存储主要指硬件存储、事件存储和设置存储，存储逻辑主要在“app_flash.c”文件中实现。

##### 3.1.1 硬件存储
&emsp;&emsp;app层已经实现了硬件存储的管理逻辑，支持最大256个硬件存储，一个开门方式为一个硬件，目前支持4种开门方式的存储，可根据实际情况调整4种开门方式的最大存储数量，默认每种开门方式为30个，如下图所示：

<img src="https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/fe-static/tuya-docs/b43c27dd-9ca5-4217-b7dc-f89c07d30422.png" width = "620"> 

##### 3.1.2 事件存储
&emsp;&emsp;app层已经实现了事件存储的管理逻辑，支持“EVTID_MAX”个事件存储，用户可根据实际情况调整事件的最大存储数量（若超出已分配Flash空间，需重新分配），默认存储最多64个事件，如下图所示：

<img src="https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/fe-static/tuya-docs/59dc9a28-c7f6-4939-81f6-30d7121d7764.png" width = "500"> 

##### 3.1.3 设置存储
&emsp;&emsp;app层已经实现了设置存储的管理逻辑，用来存储跟门锁相关的设置项目，详见结构体“lock_settings_t”。  
&emsp;&emsp;想要修改门锁的默认设置，请找到api“lock_settings_default”，修改相应参数即可。


#### 3.2 蓝牙数据通道
<img src="https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/fe-static/tuya-docs/95726426-ed86-4e35-b287-1e88790e21d1.png" width = "620"> 


#### 3.3 dp点解析
&emsp;&emsp;dp点解析主要位于lock_dp_parser和lock_dp_report两个文件中，完全按照《[Tuya_ble_锁类_dp 点规范](https://docs.qq.com/sheet/DWlNHUEdIZmF0TGRq?tab=mv4sfq&c=A1A0A0)》协议实现，可以认为是该协议文档的c语言描述。

&emsp;&emsp;“lock_dp_parser.c”文件用于解析蓝牙接收到的锁类dp点数据，如果解析到的数据跟硬件相关，数据会被传到“lock_hard.c”文件中，该文件是dp点数据和本地逻辑的接口，也是用户最应该关心的文件；

&emsp;&emsp;“lock_dp_report.c”文件用于上报dp点数据给蓝牙，本地逻辑中如果有需要上报的数据，都可以通过调用该文件内封装好的api进行数据的上报。


#### 3.4 ota
&emsp;&emsp;ota主要位于app_ota文件中，demo例程中是针对相应平台的实现，如果客户直接使用Demo例程，可直接使用；如果客户使用新平台，可以参考该例程，按照涂鸦提供的通信协议实现新平台上的ota过程。简单说：涂鸦提供ota文件存储平台（iot平台）和数据通信协议，用户需要实现ota在本地的Flash存储逻辑（Demo工程中已帮用户实现好）。  
&emsp;&emsp;协议参考：《[Tuya_ble_通用_sdk_协议](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_sdk/Tuya_ble_%E9%80%9A%E7%94%A8_sdk_%E5%8D%8F%E8%AE%AE_v3.1_190923.pdf)》中ota章节

##### 3.4.1 ota（bk3431q）
&emsp;&emsp;ota类型：分区升级，不支持断点续传  
&emsp;&emsp;[bk3431q_flash分区表](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/bk3431q/bk3431q_flash%E5%88%86%E5%8C%BA%E8%A1%A8.txt)

##### 3.4.2 ota（nRF52832）
&emsp;&emsp;ota类型：分区升级，支持断点续传  


#### 3.5 产测
&emsp;&emsp;涂鸦蓝牙锁类产品的产测主要分为两个部分：

&emsp;&emsp;&emsp;&emsp;1）授权产测——主要包括检测产品信息，烧录Mac地址，device id（即uuid）和authkey等；
	
&emsp;&emsp;&emsp;&emsp;2）整机产测——主要包括射频RSSI测试（强制要求）和其他功能测试（可选）；
	
&emsp;&emsp;授权产测的主要功能是授权，主要包括检测产品信息，烧录Mac地址，device id（即uuid）和authkey等通用配网协议中用到的重要参数，是产品安全性的基础；整机产测的主要功能是测试产品的众多外设，保证产品在产线生产过程中的良品率，涂鸦整机产测的优势在于提供了基于蓝牙无线通信方式的完整产测协议、产测dongle固件和产测上位机。关于产测的详细描述请参考相应协议文档。




## 4 demo讲解
#### 4.1 demo基本信息
- 芯片：nRF52832
- sdk：nRF5_SDK_15.3.0_59ac345
- ide：μVision V5.28.0.0
- project：ble_app_uart
- 串口：rx-8，tx-6（授权测试）
- 调试信息：RTT_viewer


#### 4.2 demo工程架构
<img src="https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/fe-static/tuya-docs/394d0391-8676-40dd-9350-7afe8aeeef53.png" width = "400"> 


#### 4.3 固件烧录
&emsp;&emsp;双击运行“\nRF5_SDK_15.3.0_59ac345\examples\ble_peripheral\ble_app_uart\pca10040\s132\arm5_no_packs\hex\load_softdevice_bootloader_app.bat”

&emsp;&emsp;上述方式用于烧录softdevice + bootloader + app，仅烧录app可直接使用keil的download。


#### 4.4 license申请
&emsp;&emsp;请联系项目经理参考《蓝牙门锁SDKlicense申请流程说明》进行申请。  
&emsp;&emsp;**<font color=#FF0000>正式项目请使用项目经理申请的license进行调试（完全擦除芯片后重新下载固件生效）！</font>**  
&emsp;&emsp;product id、authkey和device id修改的位置如下图所示：  

<img src="https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/fe-static/tuya-docs/8dc61510-69c0-4eec-932d-864c08f19328.png" width = "620"> 


#### 4.5 ota流程
1. 修改固件版本，固件版本规则参考《[Tuya_ble_通用_sdk_协议](https://github.com/TuyaInc/Tuya_ble_lock_sdk_bk3431q/blob/master/tuya_ble_lock_sdk/doc/tuya_ble_%E9%80%9A%E7%94%A8_sdk/Tuya_ble_%E9%80%9A%E7%94%A8_sdk_%E5%8D%8F%E8%AE%AE_v3.1_190923.pdf)》，如下图所示（app_common.h文件）：

<img src="https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/fe-static/tuya-docs/ad995ca7-7149-4804-a8f8-3772fedaea2f.png" width = "620"> 

注意：有些平台在ota升级前还有其他一些平台相关的关键参数需要修改，需留意。  
2. 重新编译工程，找到路径
“\nRF5_SDK_15.3.0_59ac345\examples\ble_peripheral\ble_app_uart\pca10040\s132\arm5_no_packs\ota”内的“tuya_ble_lock_common_nRF52832_xx.xx.bin”文件；  
3. 登录涂鸦iot平台 → 找到需要操作的产品 → 进入产品页面 → 拓展功能 → 固件升级 → 创建新固件 → 填写新固件信息，并确定（若无权限联系涂鸦产品经理）即可；  
4. 打开涂鸦智能app，连接蓝牙设备 → 点击右上角的标志 → 检查固件升级，查看当前固件版本 → 设备信息 → 复制“虚拟ID”并发送到电脑，填入步骤3中的测试设备中；  
5. 再次点击步骤4中的“检查固件升级” → 更新 → 开始更新 → 等待更新完成即可，若失败请重试或检查固件。  




## 5 串口模拟硬件
&emsp;&emsp;为方便客户调试和快速验证方案的可行性，使用简单的串口指令模拟多种硬件开门方式和其他指令。


#### 5.1 串口指令格式
[串口指令格式](https://docs.qq.com/sheet/DWlJKTmFMZ0hyZ0dC?c=C24A0A0)


#### 5.2 示例（nRf52832）
1.	准备一块Nordic官方开发板PCA10040（推荐）或其他载有nRF52832芯片的开发板（串口保持跟PCA10040开发板一致，RX=8，TX=6），将开发板通过j-link连接电脑，打开RTT-viewer调试窗口；  
2.	打开路径
“\nRF5_SDK_15.3.0_59ac345\examples\ble_peripheral\ble_app_uart\pca10040\s132\arm5_no_packs\hex”，双击“load_softdevice_bootloader_app.bat”下载固件（若脚本运行失败，请打开readme文档，下载安装“JLink_Windows_V652e.exe”和“nrfconnectsetup260.exe”后尝试），等待指令执行完成，调试窗口会出现log；
3.	打开路径“\tuya_ble_lock_sdk\tools\uart_simulate_hard\SSCOM”内的“sscom5.13.1.exe”串口软件；
4.	点击“多字符串”按钮，会发现上节所述的部分串口指令已经在软件右侧文本框输入完毕；
5.	调整串口波特率为“9600”，即可开始串口模拟硬件，如下图所示：

<img src="https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/fe-static/tuya-docs/5837c260-09b5-4a60-9ff2-c184dc6fcc32.png" width = "620"> 

6.	此时，打开涂鸦智能app → 右上角+号 → 自动发现 → 开始搜索 → 找到“涂鸦蓝牙门锁公版” → 下一步 → 添加（可选） → 等待绑定完成即可；
7.	进入“涂鸦蓝牙门锁公版”界面 → 长按“长按解锁”，即可看到蓝牙解锁的log → 点击串口调试助手上的“开门记录- 指纹”，即可看到涂鸦智能app上提示“指纹解锁”字样 → 其他功能可自行探索。


#### 5.3 示例（bk3431q）
1.	准备一块bk官方Demo板，阅读《涂鸦_bk3431q开发指南》；
2.	使用任一种方式烧录固件，烧录成功后需焊接上Demo板上的P16、P17引脚排针，使之接入usb转串口工具对应的RX、TX引脚，打开串口调试助手（921600），复位Demo板，即可观察到log；
3.	打开路径“\tuya_ble_lock_sdk\tools\uart_simulate_hard\SSCOM”内的“sscom5.13.1.exe”串口软件（第2步也可使用该软件）；
4.	点击“多字符串”按钮，会发现上节所述的部分串口指令已经在软件右侧文本框输入完毕；
5.	调整串口波特率为“921600”，即可开始串口模拟硬件，如下图所示：

<img src="https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/fe-static/tuya-docs/817096e6-5373-4a05-a272-ca3fa0fb30d9.png" width = "620"> 

6.	此时，打开涂鸦智能app → 右上角+号 → 自动发现 → 开始搜索 → 找到“涂鸦蓝牙门锁公版” → 下一步 → 添加（可选） → 等待绑定完成即可；
7.	进入“涂鸦蓝牙门锁公版”界面 → 长按“长按解锁”，即可看到蓝牙解锁的log → 点击串口调试助手上的“开门记录- 指纹”，即可看到涂鸦智能app上提示“指纹解锁”字样 → 其他功能可自行探索。

