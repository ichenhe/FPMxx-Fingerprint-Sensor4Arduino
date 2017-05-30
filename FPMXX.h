/*********************************
* FPMXX Fingerprint Sensor for Arduino
* Version: 1.0
*
* Author: Chenhe 晨鹤
* Created: 2017/5/28
* Blog: http://chenhe.cc
*
*********************************/

#ifndef _FPMXX_h
#define _FPMXX_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <SoftwareSerial.h>

#define FPMXX_DEFAULT_REPLY_TIME_OUT 3000

/*公共返回值，涉及通讯的均可能返回*/
#define FPMXX_REPLY_TIME_OUT 0xFF
#define FPMXX_REPLY_BAD_PACKAGE 0xFE
/*应答包中的公共确认码*/
#define FPMXX_CODE_OK 0x00
#define FPMXX_CODE_DATA_ERROR 0x01 //模块收包有错
/*其他确认码*/
#define FPMXX_VERIFY_PASSWORD_INCORRECT 0x13 //密码错误
#define FPMXX_GET_IMAGE_NO_FINGER 0x02 //传感器上没有手指
#define FPMXX_GET_IMAGE_FAILED 0x03 //录入不成功
#define FPMXX_IMAGE2TZ_CHAOS 0x06 //指纹图像太乱而生不成特征
#define FPMXX_IMAGE2TZ_LESS 0x07 //特征点太少而生不成特征
#define FPMXX_IMAGE2TZ_NO_IMAGE 0x15 //图像缓冲区内没有有效原始图
#define FPMXX_BUILD_TEMPLATE_FAILED 0x0A //合并失败（两枚指纹不属于同一手指）
#define FPMXX_STROE_FINGER_OVERFLOW 0x0B //PageId超出范围
#define FPMXX_STROE_FINGER_FLASH_ERR 0x18 //写 FLASH 出错
#define FPMXX_LOAD_FINGER_FAILED 0x0C //读出有错或模板无效
#define FPMXX_LOAD_FINGER_OVERFLOW 0x0B // PageID 超出指纹库范围
#define FPMXX_DELETE_FINGER_FAILED 0x10 //删除指纹失败
#define FPMXX_EMPTY_FINGER_FAILED 0x11 //清空指纹失败
#define FPMXX_CONTRAST_FINGER_MISMATCHING 0x08 //指纹不匹配
#define FPMXX_SEARCH_FINGER_MISMATCHING 0x09 //没有搜索到匹配指纹


class FPMXX
{
public:
	FPMXX(SoftwareSerial *softSerial);
	FPMXX(SoftwareSerial *softSerial, uint8_t adder[]);
	FPMXX(SoftwareSerial *softSerial, uint16_t baudRate);
	FPMXX(SoftwareSerial *softSerial, uint8_t adder[], uint16_t baudRate);
	/*设置模块地址，用于通讯。不设置默认为0xFFFFFFFF*/
	void setCurrentAddress(uint8_t address[]);
	/*获取指纹库容量*/
	uint16_t getFingerStoreSize(uint16_t *num);
	/*获取已有指纹个数*/
	uint16_t getFingerNum(uint16_t *num);
	/*设置模块安全等级*/
	uint8_t setSecurityLevel(uint8_t level);
	/*设置模块密码*/
	uint8_t setPassword(uint8_t password[]);
	/*验证模块密码（非初始密码必须先验证）*/
	uint8_t verifyPassword();
	uint8_t verifyPassword(uint8_t password[]);
	/*录入指纹图像*/
	uint8_t getFingerImage();
	/*图像生成指纹特征*/
	uint8_t image2tz(uint8_t bufferId);
	/*2个特征合成指纹模板*/
	uint8_t buildTemplate();
	/*保存指纹*/
	uint8_t storeFinger(uint8_t bufferId, uint16_t pageId);
	/*从指纹库读取指纹特征*/
	uint8_t loadFinger(uint8_t bufferId, uint16_t pageId);
	/*删除指纹*/
	uint8_t deleteFinger(uint16_t pageId, uint16_t count);
	/*清空指纹库*/
	uint8_t emptyFinger();
	/*精确比对2个指纹*/
	uint8_t contrastFinger(uint16_t * score);
	/*搜索指纹库*/
	uint8_t searchFinger(uint8_t bufferId, uint16_t startPage, uint16_t endPage, uint16_t *pageId, uint16_t *score);
	uint8_t searchFinger(uint8_t bufferId, uint16_t startPage, uint16_t endPage, uint16_t *pageId);

	/*以下方法为发送自定义数据及获得响应数据。不推荐使用，尽量用上面封装好的方法。*/
	void sendCmd(uint16_t length, uint8_t *content);
	uint8_t getReply(uint8_t pkg[], uint16_t timeout);
	uint8_t getReply(uint8_t pkg[]);

private:
	SoftwareSerial *mySerial;

	uint8_t Pack_Head[2] = { 0xEF, 0x01 };
	uint8_t Adder[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
};

#endif
