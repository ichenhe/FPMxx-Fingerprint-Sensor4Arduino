# FPMxx-Fingerprint-Sensor4Arduino
A FPMxx Optical Fingerprint-Sensor library for Arduino. 
一个FPMxx光学指纹传感器的Arduino库。

本库编写时参考了[**Adafruit-Fingerprint-Sensor-Library**](https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library) 的思想。

# 特点

- 对象化操作，易理解。
- 封装常用方法，调用方便。
- 返回值规范丰富，便于容错判断。

# 快速开始

首先请包含``FPMXX.h``头文件。

```c++
#include "FPMXX.h"
```

接着实例化FPMXX类。

```c++

uint8_t FPM_Address[] = { 0xFF,0xFF,0xFF,0xFF };
uint8_t FPM_Password[] = { 0x00,0x00,0x00,0x00 };

SoftwareSerial mySerial(3, 2); //请注意修改此处端口号与硬件匹配
FPMXX fs = FPMXX(&mySerial, FPM_Address, 57600);
//当地址、密码、波特率均为出厂默认时，也可以使用下面简化语句来实例化：
//FPMXX fs = FPMXX(&mySerial);
```

现在你可以开心地使用各种功能了 :)

# 使用

本库已经贴心地封装好了绝大部分方法，只需调用并判断返回值即可。
标准返回值均使用``FPMXX_``开头的宏进行了定义。

## 验证密码

若模块密码已被修改，则必须先执行此函数，否则模块将拒绝响应一切请求。也可用于判断模块是否正常连接。

```c++
Serial.print("Verify Password: ");
	uint8_t r = fs.verifyPassword(FPM_Password);
	switch (r)
	{
	case FPMXX_CODE_OK:
		Serial.println("ok");
		break;
	case FPMXX_REPLY_TIME_OUT: //获取响应数据超时
		Serial.println("Sensor not connected");
		break;
	case FPMXX_REPLY_BAD_PACKAGE: //响应数据异常
		Serial.println("Response abnormal");
		break;
	case FPMXX_CODE_DATA_ERROR: //模块收到的请求异常
		Serial.println("Sensor receive err");
		break;
	}
```

## 添加指纹

完整的添加指纹流程为：

1. 扫描指纹生成图像。
2. 图像生成特征。
3. 再次扫描并生成特征。
4. 合并两个特征生成模板。
5. 保存模板。

其中3、4步可以省略。

### 扫描指纹

模块将扫描指纹并将图像保存在图像缓存区。

```c++
Serial.print("Please scan finger...");
//这里循环判断直到扫描成功为止，可以根据实际需要对返回值进行更加详细的处理。
while (fs.getFingerImage() != FPMXX_CODE_OK) {}
Serial.println("ok");
```

### 生成特征

模块将根据图像缓存区的图像提取指纹特征并保存在指定的特征缓存区。

```c++
Serial.print("Image2TZ...");
//模块有2个特征缓存区，编号分别为1、2，这里先存入1.
fs.image2tz(1);
Serial.println("ok");
```

### 合并特征生成模板

模块将合并2个特征缓存区的特征，要求为同一手指，否则会返回对应错误码。合并后的结果依然在特征缓存区（两个相同）

```c++
Serial.print("Build template...");
fs.buildTemplate();
Serial.println("ok");
```

### 保存

```c++
Serial.print("Save on id: 2...");
//这里将特征缓存区1的指纹保存在2号位置。（指纹储存位置从0开始）
fs.storeFinger(1, 2);
Serial.println("ok");
```

## 搜索指纹（1:N匹配）

```c++
//执行前需要先录入指纹并生成特征。
//这里将特征缓存区1的指纹与0至100的指纹进行逐一匹配。
//本函数返回的是执行结果，具体匹配到的id将存入参数变量。
uint16_t id;
fs.searchFinger(1, 0, 100, &id)
```

## 其他

除了以上，本库还封装了其他函数，他们包括：

| 函数名                | 功能         |
| ------------------ | ---------- |
| getFingerStoreSize | 获取指纹库容量    |
| getFingerNum       | 获取已有指纹个数   |
| setSecurityLevel   | 设置模块安全等级   |
| setPassword        | 设置模块密码     |
| loadFinger         | 从指纹库读取指纹特征 |
| deleteFinger       | 删除指纹       |
| emptyFinger        | 清空指纹库      |
| contrastFinger     | 精确比对2个指纹   |
|                    |            |
| sendCmd            | 发送自定义数据    |
| getReply           | 获取响应数据     |

# 例程

本库自带的例程实现了：

1. 清空指纹库。
2. 录入指纹（两次合并）。
3. 保存指纹。
4. 循环搜索匹配指纹。

的整个流程。

其余函数拥有详细的中文注释。

# 关于

本库采用``Apache``开源协议。 FPMXX is relased under the ``Apache license``

有问题欢迎Issue.

© [晨鹤 Chenhe](http://chenhe.cc/) 2017