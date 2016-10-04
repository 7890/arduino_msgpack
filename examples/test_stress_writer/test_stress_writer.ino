#include "msgpck.h"
/* 
 * This is an example of message pack library use for Arduino uno.
 *
 * It writes some message pack serialized data to the serial port.
 * 
 * Based on test_uno_writer.ino example of arduino_msgpack library
 * https://github.com/HEADS-project/arduino_msgpack
 *
 * Expected output is a continuously repeated message (hexdump of one instance):
 *
 * 00000000  21 a4 2f 61 62 63 a3 69  66 73 00 ca 3d cc cc cd  |!./abc.ifs..=...|
 * 00000010  a8 6c 61 73 74 20 61 72  67                       |.last arg|
 *
 * testing speed and long runs
 *
 * Using a similar message structure as OSC, create "OSCPackMessage".
 * This library also works with gold old Arduino Duemilanove.
 * Compiled sketch needs around 5 kilobytes (15% of max 30k)
 * //tb/1610
*/
//==============SETUP
void setup() {
  Serial.begin(115200);
  delay(100);
/*
  // '!/'
  // 00000000  21 a1 2f a0                                       |!./.|
  msgpck_write_integer(&Serial,(uint8_t)'!');
  msgpck_write_string(&Serial,"/");
  msgpck_write_string(&Serial,""); // empty typetag
*/
}

//==============LOOP
void loop() {
  // '!/abc' ifs 0 .1 "last arg"
  write_new("/abc","ifs");
  write_i(0);
  write_f(.1);
  write_s("last arg");
  delay(1);
}

//==============WRAPPERS
void write_new(String addr, String types)
{
  write_start();
  write_address(addr);
  write_typetags(types);
}
void write_start()
{
  write_c((uint8_t)'!');
}
void write_address(String str)
{
  write_s(str);
}
void write_typetags(String str)
{
  write_s(str);
}
void write_c(uint8_t c)
{
  msgpck_write_integer(&Serial,c);
}
void write_s(String str)
{
  msgpck_write_string(&Serial,str);
}
void write_i(int i)
{
  msgpck_write_integer(&Serial,i);
}
void write_f(float f)
{
  msgpck_write_float(&Serial,f);
}
void write_boolean(boolean b)
{
  msgpck_write_bool(&Serial,b);
}
//EOF
