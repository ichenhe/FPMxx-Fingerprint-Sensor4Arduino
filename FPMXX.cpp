/*********************************
* FPMXX Fingerprint Sensor for Arduino
* Version: 1.0
*
* Author: Chenhe 晨鹤
* Created: 2017/5/28
* Blog: http://chenhe.cc
*
*********************************/

#if ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "FPMXX.h"

/*
使用默认地址：0xFFFFFFFF，默认波特率：576000.
*/
FPMXX::FPMXX(SoftwareSerial * softSerial)
{
	mySerial = softSerial;
	mySerial->begin(57600);
}

FPMXX::FPMXX(SoftwareSerial * softSerial, uint8_t adder[])
{
	mySerial = softSerial;
	mySerial->begin(57600);
	for (int i = 0; i < 4; i++)
	{
		Adder[i] = adder[i];
	}
}

FPMXX::FPMXX(SoftwareSerial * softSerial, uint16_t baudRate)
{
	mySerial = softSerial;
	mySerial->begin(baudRate);
}

FPMXX::FPMXX(SoftwareSerial * softSerial, uint8_t adder[], uint16_t baudRate)
{
	mySerial = softSerial;
	mySerial->begin(baudRate);
	for (int i = 0; i < 4; i++)
	{
		Adder[i] = adder[i];
	}
}

/*
设置模块地址，用于通讯。默认为0xFFFFFFFF。
本函数仅设置库内部的变量以便通讯，并不是修改模块的地址。
Params:
address: 模块地址，长度为4。
*/
void FPMXX::setCurrentAddress(uint8_t address[])
{
	for (int i = 0; i < 4; i++)
		Adder[i] = address[i];
}

/*
获取指纹容量。
Params:
num: 储存指纹容量。
*/
uint8_t FPMXX::getFingerStoreSize(uint16_t *num)
{
	uint8_t content[] = { 0x01,0x00,0x03,0x0F };
	sendCmd(3, content);
	uint8_t reply[17];
	uint8_t r = getReply(reply);
	if (r == 17)
	{
		/*Serial.println("--------");
		for (int i = 0;i < 17;i++)
		Serial.println(reply[i],HEX);
		Serial.println("--------");*/
		if (reply[0] != FPMXX_CODE_OK) return FPMXX_CODE_DATA_ERROR;
		*num = reply[5];
		*num <<= 8;
		*num |= reply[6];
		return reply[0];
	}
	return r;
}

/*
获取已储存的指纹个数。
Params:
num: 储存个数。
*/
uint8_t FPMXX::getFingerNum(uint16_t *num)
{
	uint8_t content[] = { 0x01,0x00,0x03,0x1D };
	sendCmd(3, content);
	uint8_t r = getReply(content);
	if (r == 3)
	{
		if (content[0] != FPMXX_CODE_OK) return FPMXX_CODE_DATA_ERROR;
		*num = content[1];
		*num <<= 8;
		*num |= content[2];
		return content[0];
	}
	return r;
}

/*
设置模块安全等级，等级越高拒认率越高。
重启后生效。
Params:
level: 0~5
*/
uint8_t FPMXX::setSecurityLevel(uint8_t level)
{
	uint8_t content[] = { 0x01,0x00,0x05,0x0E,0x05,0x00 };
	content[5] = level;
	sendCmd(5, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
设置模块口令。
Params:
password: 新口令，长度为4。
*/
uint8_t FPMXX::setPassword(uint8_t password[])
{
	uint8_t content[] = { 0x01,0x00,0x07,0x12,0x00,0x00,0x00,0x00 };
	for (int i = 0; i < 4; i++)
		content[4 + i] = password[i];
	sendCmd(7, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
验证模块口令，使用默认:0x00000000
*/
uint8_t FPMXX::verifyPassword()
{
	uint8_t content[] = { 0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00 };
	sendCmd(7, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
验证模块口令。
Params:
password: 口令，长度为4。
*/
uint8_t FPMXX::verifyPassword(uint8_t password[])
{
	uint8_t content[] = { 0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00 };
	for (int i = 0; i < 4; i++)
		content[4 + i] = password[i];
	sendCmd(7, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
录入指纹图像。
探测到后录入指纹图像存于模块ImageBuffer
*/
uint8_t FPMXX::getFingerImage()
{
	uint8_t content[] = { 0x01,0x00,0x03,0x01 };
	sendCmd(3, content);
	uint8_t r = getReply(content, 1000);
	if (r == 1)
		return content[0];
	return r;
}

/*
将ImageBuffer图像生成指纹特征。
存于 CharBuffer1 或 CharBuffer2
Params:
bufferId: 1~2，其他值按2处理。
*/
uint8_t FPMXX::image2tz(uint8_t bufferId)
{
	uint8_t content[] = { 0x01,0x00,0x04,0x02,0x00 };
	content[4] = bufferId;
	sendCmd(4, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
将 CharBuffer1 与 CharBuffer2 中的特征文件合并生成模板。
结果存于 CharBuffer1 与 CharBuffer2(两者内容相同)。
*/
uint8_t FPMXX::buildTemplate()
{
	uint8_t content[] = { 0x01,0x00,0x03,0x05 };
	sendCmd(3, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
储存指纹。
Params:
bufferId: 特征bufferId，1~2。
pageId: 指纹库位置，从0开始。
*/
uint8_t FPMXX::storeFinger(uint8_t bufferId, uint16_t pageId)
{
	uint8_t content[] = { 0x01,0x00,0x06,0x06,0x00,0x00,0x00 };
	content[4] = bufferId;
	content[5] = (uint8_t)(pageId >> 8);
	content[6] = (uint8_t)pageId;
	sendCmd(6, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
读取指纹。
将 flash 数据库中指定 ID 号的指纹模板读入到模板缓冲区。
Params:
bufferId: 特征bufferId，1~2。
pageId: 指纹库位置，从0开始。
*/
uint8_t FPMXX::loadFinger(uint8_t bufferId, uint16_t pageId)
{
	uint8_t content[] = { 0x01,0x00,0x06,0x07,0x00,0x00,0x00 };
	content[4] = bufferId;
	content[5] = (uint8_t)(pageId >> 8);
	content[6] = (uint8_t)pageId;
	sendCmd(6, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
删除指纹。
删除指定 ID 号开始的 N 个指纹模板。
Params:
pageId: 指纹库位置，从0开始。
count: 要删除的个数。
*/
uint8_t FPMXX::deleteFinger(uint16_t pageId, uint16_t count)
{
	uint8_t content[] = { 0x01,0x00,0x07,0x0C,0x00,0x00,0x00,0x00 };
	content[4] = (uint8_t)(pageId >> 8);
	content[5] = (uint8_t)pageId;
	content[6] = (uint8_t)(count >> 8);
	content[7] = (uint8_t)count;
	sendCmd(7, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
清空指纹库。
*/
uint8_t FPMXX::emptyFinger()
{
	uint8_t content[] = { 0x01,0x00,0x03,0x0D };
	sendCmd(3, content);
	uint8_t r = getReply(content);
	if (r == 1)
		return content[0];
	return r;
}

/*
精确比对 CharBuffer1 与 CharBuffer2 中的特征文件。
两特征缓冲区中的内容不变。
Params:
score: 记录比对得分。
*/
uint8_t FPMXX::contrastFinger(uint16_t * score)
{
	uint8_t content[] = { 0x01,0x00,0x03,0x03 };
	sendCmd(3, content);
	uint8_t r = getReply(content);
	if (r == 3) {
		if (content[0] != FPMXX_CODE_OK) *score = 0;
		else {
			*score = content[1];
			*score <<= 8;
			*score |= content[2];
		}
		return content[0];
	}
	return r;
}

/*
以buffer中的特征搜索指纹库。
Params:
bufferId: 1~2。
startPage: 开始页面。
endPage: 结束页码。
pageId: 记录搜索到的页码。
score: 记录比对得分。
*/
uint8_t FPMXX::searchFinger(uint8_t bufferId, uint16_t startPage, uint16_t endPage, uint16_t * pageId, uint16_t * score)
{
	uint8_t content[] = { 0x01,0x00,0x08,0x04,0x00,0xFF,0xFF,0x00,0x00 };
	content[4] = bufferId;
	content[5] = (uint8_t)(startPage >> 8);
	content[6] = (uint8_t)startPage;
	content[7] = (uint8_t)(endPage >> 8);
	content[8] = (uint8_t)endPage;
	sendCmd(8, content);
	uint8_t r = getReply(content);
	if (r == 5) {
		if (content[0] != FPMXX_CODE_OK) *score = 0;
		else {
			*pageId = content[1];
			*pageId <<= 8;
			*pageId |= content[2];
			*score = content[3];
			*score <<= 8;
			*score |= content[4];
		}
		return content[0];
	}
	return r;
}

uint8_t FPMXX::searchFinger(uint8_t bufferId, uint16_t startPage, uint16_t endPage, uint16_t * pageId)
{
	uint16_t s;
	return searchFinger(bufferId, startPage, endPage, pageId, &s);
}

/*
向传感器发送数据（命令）
Params:
len: 通信协议中的包长度值
content: 要发送的数据，从包标识开始。
*/
void FPMXX::sendCmd(uint16_t len, uint8_t *content)
{
	mySerial->listen();
	uint8_t i;

	//计算校验和
	uint16_t sum = 0;
	for (i = 0; i < len + 1; i++)
		sum += *(content + i);
	//等待缓冲区
	mySerial->flush();

	//发送包头
	for (i = 0; i < 2; i++)
		mySerial->write(Pack_Head[i]);

	//发送模块地址
	for (i = 0; i < 4; i++)
		mySerial->write(Adder[i]);

	//发送指令内容
	for (i = 0; i < len + 1; i++)
	{
		mySerial->write(*content);
		content++;
	}

	//发送校验和
	mySerial->write((uint8_t)(sum >> 8));
	mySerial->write((uint8_t)sum);
}

/*
接收传感器的返回数据并存入参数变量。
仅返回从确认码至校验和之前的数据与长度
param:
pkg: 用于存储返回数据的变量
timeout: 超时返回（毫秒）
return: 返回数据长度，或FPMXX_REPLY_开头错误码；
*/
uint8_t FPMXX::getReply(uint8_t pkg[], uint16_t timeout)
{
	uint16_t timer = 0;
	uint16_t len = 0; //通信协议中包长度的值
	uint8_t reply[40], i = 0;
	uint8_t pkgType; //通信协议中包标识的值
	while (1)
	{
		while (!mySerial->available())
		{
			delay(1);
			timer++;
			if (timer >= timeout) return FPMXX_REPLY_TIME_OUT;
		}

		reply[i] = mySerial->read();
		//Serial.println(reply[i],HEX);

		//检查数据
		if (i == 1)
		{
			if (reply[0] != Pack_Head[0] ||
				reply[1] != Pack_Head[1])
				//包头不符合
				return FPMXX_REPLY_BAD_PACKAGE;
		}
		else if (i == 5)
		{
			if (reply[2] != Adder[0] ||
				reply[3] != Adder[1] ||
				reply[4] != Adder[2] ||
				reply[5] != Adder[3])
				//地址不符合
				return FPMXX_REPLY_BAD_PACKAGE;
		}
		else if (i == 6) pkgType = reply[6]; //取包标识
		else if (i == 8) //取包长度
		{
			len = reply[7];
			len <<= 8;
			len |= reply[8];
		}
		if (i == 8 + len) break; //因为i从0开始所以不是9而是8
		i++;
	}

	//数据接收完毕
	if (i == 8 + len)
	{
		//校验
		uint16_t sum = 0;
		for (i = 6; i < 6 + len + 1; i++)
			sum += reply[i];

		if (((uint8_t)(sum >> 8)) != reply[7 + len] ||
			((uint8_t)sum) != reply[8 + len])
			//校验不通过
			return FPMXX_REPLY_BAD_PACKAGE;

		len -= 2; //去掉最后的校验和
		for (i = 0; i < len; i++) //从确认码开始返回
			pkg[i] = reply[9 + i];
		return len;
	}
}

uint8_t FPMXX::getReply(uint8_t pkg[])
{
	return getReply(pkg, FPMXX_DEFAULT_REPLY_TIME_OUT);
}