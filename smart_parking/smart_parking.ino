         
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <ESP8266WiFi.h>//
#include "CTBot.h"

#define BUZZER D8 //buzzer pin
#define ACCESS_DELAY 500 // lamanya buzzer bunyi
#define DENIED_DELAY 2000 // lamanya buzzer bunyi
#define SERVO_PIN D0
#define SS_PIN D4
#define RST_PIN D3


MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
Servo myservo;
CTBot myBot;

//Konfigurasi Wifi
String ssid = "Kost Putri";
String pass = "TAHUN2022";
//Konfigurasi Telegram
String token = "5919985306:AAExVVxT77UfnJ7TQIcUpD7Hpgahqj5zDC4";
const int id = 1939145647;//id akun telegram
//sediakan untuk status sensor
int aktif = 1; //nilai awal (1=aktif, 0=tidak aktif)

void setup() {
  
lcd.begin(); //m tampilan lcd
lcd.backlight(); //menyalakan lampu latar lcd
Serial.begin(9600);
Serial.println("Starting TelegramBot. Koneksi ke Wifi");
//conncect nodemcu ke telegram melalui wifi
myBot.wifiConnect(ssid, pass);
myBot.setTelegramToken(token);
//cek koneksi wifi
if(myBot.testConnection()){
Serial.println("Koneksi Berhasil");
} else {
Serial.println("Koneksi Gagal");
}

lcd.noCursor(); //biar gak ada cursor di lcd


lcd.clear(); //clear lcd
Serial.begin(9600); // Initiate a serial communication
SPI.begin(); // Initiate SPI bus
mfrc522.PCD_Init(); // Initiate MFRC522
Serial.println("Dekatkan kartu...");
Serial.println();
lcd.setCursor(0,0);
lcd.print("Dekatkan Kartu");
delay(1000);
//pinMode(LED_Hijau, OUTPUT);
//pinMode(LED_Merah, OUTPUT);
pinMode(BUZZER, OUTPUT);
noTone(BUZZER);
myservo.attach(SERVO_PIN);
myservo.write(0);
}
void loop()
{
  //baca pesan tele yg dikirim dari hp kita
TBMessage msg;
//noTone(BUZZER);
//uji apakah ada pesan baru yang masuk ke nodemcu?
if(myBot.getNewMessage(msg))
{
//baca pesan masuk
String pesan = msg.text;
//uji pesan apakah ON atau OFF?
if(pesan == "ON")
{
myBot.sendMessage(msg.sender.id, "Sistem Parkir Pintar Ready");
aktif = 1;
}
else if(pesan == "OFF")
{
myBot.sendMessage(msg.sender.id, "Sistem Parkir Pintar Dimatikan");
aktif = 0;
}
}
// Melihat kartu baru
if ( ! mfrc522.PICC_IsNewCardPresent())
{
return;
}
// Pilih salah satu kartu
if ( ! mfrc522.PICC_ReadCardSerial())
{
return;
}
// Menampilkan UID di serial monitor
lcd.setCursor(0,0);
lcd.print("UID :");
Serial.print("UID tag :");
String content= "";
byte letter;
for (byte i = 0; i < mfrc522.uid.size; i++)
{
lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
lcd.print(mfrc522.uid.uidByte[i], HEX);
Serial.print(mfrc522.uid.uidByte[i], HEX);
content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
content.concat(String(mfrc522.uid.uidByte[i], HEX));
}
Serial.println();


Serial.print("Pesan : ");
content.toUpperCase();
if (content.substring(1) == "C3 03 65 A7") // ganti kode kartu jika ingin mendapat akses
{
myservo.write(180);
//tone(BUZZER, 3000);
//delay(ACCESS_DELAY);
digitalWrite(BUZZER, HIGH);
delay(1000);
lcd.setCursor(0,1);
lcd.print("ID Diterima ");
Serial.println("ID Diterima ");
myBot.sendMessage(id, "Ada Kendaraan Keluar...");
noTone(BUZZER);
myservo.write(0);
}
else {
lcd.setCursor(0,1);
lcd.print("ID Ditolak ");
Serial.println(" ID Ditolak ");
delay(1000);

digitalWrite(BUZZER, LOW);
delay(1000);

myBot.sendMessage(id, "ID Penyusup...");
}
}
