#include <LiquidCrystal_I2C.h>//lcd 
#include <Servo.h>//servo
#include <RFID.h>//rfid
#include <Wire.h>//I2C - lcd
#include <SPI.h>//rfid
#include "pitches.h" //buzzer

int BUZZER = 8;//buzzer-in e kemi lidhur me pinin 8
int LED_R = 3;
int LED_G = 2;

#define SDA 10 //definojme pinin Select Device RFID
#define RST 9//definojme pinin reset  i RFID
RFID rfid(SDA, RST); //krijojme rfid dhe ja caktojme pinat enable dhe reset

byte kartaUser1[5] = {146, 69, 9, 227, 61};
byte kartaUser2[5] = {88, 17, 123, 16, 34};
byte kartaUser3[5] = {15, 15, 221, 229, 58};

byte kartaProve[5];//ketu do ruhet perkohesisht karta e lexuar - do perdoret per krahasim
LiquidCrystal_I2C  lcd(0x3F, 2, 1, 0, 4, 5, 6, 7); // 0x3F address busi i I2C modulit
Servo servo;  // krijojme objekt Servo

int positive_melody[] = {NOTE_D7, NOTE_C7, NOTE_C7, NOTE_D7, NOTE_A7, NOTE_D7, NOTE_G7, NOTE_E7, NOTE_B7, NOTE_G7, NOTE_A7, NOTE_B7};//melodia kur user-i ka qasje
int positive_kohezgjatja_e_notave[] = {8, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 4}; //tempoja e melodise pozitive
int negative_melody[] = {NOTE_G2, NOTE_G2, NOTE_F2, NOTE_F2, NOTE_D2, NOTE_D2, NOTE_G2, NOTE_G2, NOTE_F2, NOTE_F2};//melodia kur user-i s'ka qasje
int negative_kohezgjatja_e_notave[] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 4}; //tempoja e melodise negative

void setup() {
  Serial.begin(9600);
  
  SPI.begin();
  rfid.init();//resetohet dhe fillon te punoje antena e RFID

  lcd.setBacklightPin(3, POSITIVE); //Positive - Enum i definuar ne LCD.h fajllin
  lcd.begin(16, 2); // specifikon dimensionet e LCD-se , kolonat dhe rreshtat
  lcd.clear();

  servo.attach(4);  // servo objektin e lidhim me pinin 4
  servo.write(90);
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);

  firstMessage(true); //ne LCD shkruajme 'RFID System'
}

void loop() {
  boolean qasjeUser1 = true;
  boolean qasjeUser2 = true;
  boolean qasjeUser3 = true;

  //ketu vijojne booleanet tjere

  if (rfid.isCard()) { // true nese gjendet kartele valide
    delay(1000);
    if (rfid.readCardSerial()) { // e lexon kartelen
      delay(1000);
      for (int i = 0; i < 5; i++) {
        kartaProve[i] = rfid.serNum[i];//numrin serik te RFID se lexuar e kopjojme qe ta krahasojme
      }
    }
    rfid.halt();
    for (int i = 0; i < 5; i++) {
      if (kartaUser2[i] != kartaProve[i])
      {
        qasjeUser2 = false;
      }
      if (kartaUser1[i] != kartaProve[i])
      {
        qasjeUser1 = false;
      }
      if (kartaUser3[i] != kartaProve[i])
      {
        qasjeUser3 = false;
      }
    }
    
    if (qasjeUser1) {
      //Serial.println("Welcome User1");
      digitalWrite(LED_G, HIGH);
      melody_YES();
      printoEmrin("User1", "155214");

      openGate();
      digitalWrite(LED_G, LOW);
      scanMessage();
    }
    else if (qasjeUser2)
    {
      digitalWrite(LED_G, HIGH);
      melody_YES();
      printoEmrin("User2", "155999");
      openGate();
      digitalWrite(LED_G, LOW);
      scanMessage();
    }
    else if (qasjeUser3)
    {
      digitalWrite(LED_G, HIGH);
      melody_YES();
      printoEmrin("User3", "155663");
      openGate();
      digitalWrite(LED_G, LOW);
      scanMessage();
    }

    //ketu do shtohen kartelat tjera
    else
    {
      digitalWrite(LED_R, HIGH);
      lcd.clear();
      lcd.print("Access Denied!");
      melody_NO();
      delay(3000);
      digitalWrite(LED_R, LOW);
      scanMessage();
    }
    //vijon paraqitja e numrit serik te RFID etiketes ne Serial Monitor
    Serial.print(rfid.serNum[0]);
    Serial.print("-");
    Serial.print(rfid.serNum[1]);
    Serial.print("-");
    Serial.print(rfid.serNum[2]);
    Serial.print("-");
    Serial.print(rfid.serNum[3]);
    Serial.print("-");
    Serial.print(rfid.serNum[4]);
    Serial.println("");
  }
  rfid.halt();
}

//vijojne funksionet
void printoEmrin(String user, String ID) {
  lcd.clear();
  delay(500);
  lcd.setCursor(0, 0);
  lcd.print("Welcome ");
  lcd.print(user);
  lcd.setCursor(0, 1);//kolona e pare rreshti i dyte
  lcd.print("ID: ");
  lcd.print(ID);
}

void firstMessage(bool buzzer) {
  if (buzzer) { //Do beje 'beep' vetem kur do te kycet sistemi
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
  }
  lcd.noBlink();
  lcd.clear();
  lcd.home();//kursori vendoset ne 0,0
  lcd.print(" RFID System");
  lcd.setCursor(0, 1);
  lcd.print("Made by: D.B");
}

void scanMessage() {
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
  lcd.noBlink();
  lcd.clear();
  lcd.setCursor(1, 0); //(kolona,rreshti)
  lcd.print("Scan your ID");
}

void openGate() {
  servo.write(180);
  delay(5000);
  servo.write(90);//motori leviz nga 90 ne 180 shkalle dhe anasjelltas
}

void melody_YES() {
  for (int i = 0; i < 12; i++)
  {
    int koha_e_notes = 1000 / positive_kohezgjatja_e_notave[i];
    tone(BUZZER, positive_melody[i], koha_e_notes);
    int pauza = koha_e_notes * 1.30;
    delay(pauza);
    noTone(BUZZER);
  }
}

void melody_NO() {
  for (int i = 0; i < 10; i++)
  {
    int koha_e_notes = 1000 / negative_kohezgjatja_e_notave[i];
    tone(BUZZER, negative_melody[i], koha_e_notes);
    int pauza = koha_e_notes * 1.30;
    delay(pauza);
    noTone(BUZZER);
  }
}