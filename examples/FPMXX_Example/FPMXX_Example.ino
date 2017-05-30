#include <SoftwareSerial.h>
#include "FPMXX.h"

//若不能连接模块请先修改以下默认参数以及波特率。
uint8_t FPM_Address[] = { 0xFF,0xFF,0xFF,0xFF };
uint8_t FPM_Password[] = { 0x00,0x00,0x00,0x00 };

//请注意修改此处端口号与硬件连接匹配
SoftwareSerial mySerial(3, 2);
FPMXX fs = FPMXX(&mySerial, FPM_Address, 57600);

uint16_t size = 0;

/*
警告：本例程会清空模块的指纹库并在id2位置上保存一个新指纹。
*/
void setup()
{
  Serial.begin(9600);

  Serial.println("---Test FPMXX Senor---");

  //验证模块连接及密码
  Serial.print("Verify Password: ");
  uint8_t r = fs.verifyPassword(FPM_Password);
  switch (r)
  {
  case FPMXX_CODE_OK:
    Serial.println("ok");
    break;
  case FPMXX_REPLY_TIME_OUT:
    Serial.println("Sensor not connected");
    break;
  case FPMXX_REPLY_BAD_PACKAGE:
    Serial.println("Response abnormal");
    break;
  case FPMXX_CODE_DATA_ERROR:
    Serial.println("Sensor receive err");
    break;
  }

  //获取指纹容量大小
  fs.getFingerStoreSize(&size);
  Serial.print("Finger store size: ");
  Serial.println(size);

  //获取已有指纹个数
  uint16_t num = 0;
  fs.getFingerNum(&num);
  Serial.print("Finger num: ");
  Serial.println(num);

  //清空指纹库
  Serial.print("Clear fingerprints...");
  fs.emptyFinger();
  Serial.println("ok");

  //扫描指纹
  Serial.print("Please scan finger...");
  while (fs.getFingerImage() != FPMXX_CODE_OK) {}
  Serial.println("ok");
  //指纹图像生成特征
  Serial.print("Image2TZ...");
  fs.image2tz(1);
  Serial.println("ok");
  delay(1000);
  //再扫描一遍
  Serial.print("scan same finger again...");
  while (fs.getFingerImage() != FPMXX_CODE_OK) {}
  Serial.println("ok");
  Serial.print("Image2TZ...");
  fs.image2tz(2);
  Serial.println("ok");
  //两遍特征合成模板
  Serial.print("Build template...");
  fs.buildTemplate();
  Serial.println("ok");
  //保存模板
  Serial.print("Save on id: 2...");
  fs.storeFinger(1, 2);
  Serial.println("ok");
}

void loop()
{
  uint8_t r;
  r = fs.getFingerImage(); //扫描指纹
  if (r == FPMXX_GET_IMAGE_NO_FINGER)
    Serial.println("Not find finger"); //手指没有放上去

  else if (r == FPMXX_GET_IMAGE_FAILED)
    Serial.println("Scan failed"); //扫描失败
  else {
    fs.image2tz(1); //生成指纹特征
    uint16_t id = 0;
    // 与指纹库进行匹配
    if (fs.searchFinger(1, 0, size - 1, &id) == FPMXX_CODE_OK) {
      Serial.print("find finger: ");
      Serial.println(id);
    }
    else Serial.println("not find");
  }

  delay(1000);
}

