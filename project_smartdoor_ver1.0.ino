/*Project Smartdoor - version 1.0
Du an phat trien thiet bi phan cung va lap trinh phan mem danh cho cua thong minh.
Nhan dang the RFID, Nhap mat khau de mo cua
Phien ban hien tai : 1.0
Phat trien ung dung : Nguyen Van Nhan.*/

/* Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * 
 */

#include <MFRC522.h>
#include <SPI.h>
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
#include <Keypad.h> // call library keypad

/*-----( Declare Constants )-----*/
/*-----( Declare objects )-----*/
// set the LCD address to 0x27 for a 16 chars 2 line display
// A FEW use address 0x3F
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {9,8,7,6}; //Rows 0 to 3
byte colPins[numCols]= {5,4,3,2}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

// Khai bao chan dong co buoc
int motorPin1 = 14;
int motorPin2 = 15;
int motorPin3 = 16;
int motorPin4 = 17;
int motorStep = 0;
int _step = 0; 

#define SS_PIN 10
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nhanKey[4] = {112,59,55,213};
byte trangKey[4] = {246,63,97,73};
boolean chkStart = true;
int motorSpeed =1;

void setup() {
  //Thiet lap LCD su dung 16x2
  lcd.begin(16,2); 

  //Thiet lap ket noi cong SPI
  Serial.begin(9600);    // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();   // Init MFRC522
  rfid.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  //Thiet lap chan motor
  //pinMode(motorPin1,OUTPUT);
  //pinMode(motorPin2,OUTPUT);
  //pinMode(motorPin3,OUTPUT);
  //pinMode(motorPin4,OUTPUT);
 
}

int motorRotateTime=0;

void loop() {
  
  if (chkStart) {
    mainScreenLCD();
    chkStart = false;
  } else { 
      startScreen();
  }
  
}

void testKeypad () {
  //Test KeyPad
  char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY)
  {
    String inputTxt(keypressed);
    displayLCD(inputTxt);
  }
}

void readCard_procedure() {
  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! rfid.PICC_ReadCardSerial()) {
    return;
  }

  //Check key Nhan
   if (rfid.uid.uidByte[0] == nhanKey[0] && 
    rfid.uid.uidByte[1] == nhanKey[1] && 
    rfid.uid.uidByte[2] == nhanKey[2] && 
    rfid.uid.uidByte[3] == nhanKey[3] ) {
    Serial.println("Welcome home Van Nhan.");
    loginSuccessScreen("VNhan"); 
   }
   else if (rfid.uid.uidByte[0] == trangKey[0] && 
    rfid.uid.uidByte[1] == trangKey[1] && 
    rfid.uid.uidByte[2] == trangKey[2] && 
    rfid.uid.uidByte[3] == trangKey[3] ) {
    Serial.println("Welcome home Kieu Trang."); 
    loginSuccessScreen("KTrang"); 
    }
    else {
      Serial.println("Sorry wrong card."); 
      loginFailScreen();
    }
    // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


//Ham hien thi man hinh chinh 
void startScreen() {
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("SmartDoor System");
  lcd.setCursor(0,1);
  lcd.print("SCANNING KEY...");
}

//Ham hien thi dang nhap thanh cong
void loginSuccessScreen(String txtWelcome) {
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  txtWelcome = "Welcome " + txtWelcome;
  lcd.print(txtWelcome);
  lcd.setCursor(0,1);
  lcd.print("UNLOCKING DOOR...");
  delay(3000);
}

//Ham hien thi LCD noi dung bat ky
void displayLCD(String txtInput) {
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(txtInput);
  //delay(3000);
}

//Ham hien thi dang nhap khong thanh cong
void loginFailScreen () {
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("WRONG KEY ...");
  lcd.setCursor(0,1);
  lcd.print("Input another");
  delay(3000);
}

//Ham khoi dong motor mo khoa
void motorRotate_unlockDoor_procedure() {
  for (motorRotateTime=1;motorRotateTime<200;motorRotateTime++){
      Serial.println("Start motor couterclockwise");
      Serial.println(motorRotateTime);
      counterclockwise();  
    }
}

void motorRotate_lockDoor_procedure() {
  for (motorRotateTime=1;motorRotateTime<200;motorRotateTime++){
      clockwise();  
      Serial.println("Start motor clockwise");
      Serial.println(motorRotateTime);
    }
}

//Ham khoi dong man hinh LCD
void mainScreenLCD() {
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Starting");
  delay(1000);
  lcd.setCursor(8,0);
  lcd.print(".");
  delay(1000);
  lcd.setCursor(9,0);
  lcd.print(".");
  delay(1000);
  lcd.setCursor(10,0);
  lcd.print(".");
  delay(1000);
  lcd.setCursor(11,0);
  lcd.print("OK");
  delay(1000);
  lcd.setCursor(0,1);
  lcd.print("VNhanHome-Ver1.0");
  delay(3000);
  //lcd.clear();
  //lcd.noDisplay();
}

// Ham hien thi yeu cau nhap password
void printLCD_txt_InputPassword() {

}

//Ham quay Motor quay theo chieu kiem dong ho
void counterclockwise (){
 // 1
 digitalWrite(motorPin1, HIGH);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, LOW);
 delay(motorSpeed);
 // 2
 digitalWrite(motorPin1, HIGH);
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, LOW);
 delay (motorSpeed);
 // 3
 digitalWrite(motorPin1, LOW);
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, LOW);
 delay(motorSpeed);
 // 4
 digitalWrite(motorPin1, LOW);
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW);
 delay(motorSpeed);
 // 5
 digitalWrite(motorPin1, LOW);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, LOW);
 delay(motorSpeed);
 // 6
 digitalWrite(motorPin1, LOW);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin4, HIGH);
 delay (motorSpeed);
 // 7
 digitalWrite(motorPin1, LOW);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH);
 delay(motorSpeed);
 // 8
 digitalWrite(motorPin1, HIGH);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin4, HIGH);
 delay(motorSpeed);
}

//////////////////////////////////////////////////////////////////////////////
//set pins to ULN2003 high in sequence from 4 to 1
//delay "motorSpeed" between each pin setting (to determine speed)

void clockwise(){
 // 1
 digitalWrite(motorPin4, HIGH);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin1, LOW);
 delay(motorSpeed);
 // 2
 digitalWrite(motorPin4, HIGH);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin1, LOW);
 delay (motorSpeed);
 // 3
 digitalWrite(motorPin4, LOW);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin1, LOW);
 delay(motorSpeed);
 // 4
 digitalWrite(motorPin4, LOW);
 digitalWrite(motorPin3, HIGH);
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin1, LOW);
 delay(motorSpeed);
 // 5
 digitalWrite(motorPin4, LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin1, LOW);
 delay(motorSpeed);
 // 6
 digitalWrite(motorPin4, LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin2, HIGH);
 digitalWrite(motorPin1, HIGH);
 delay (motorSpeed);
 // 7
 digitalWrite(motorPin4, LOW);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin1, HIGH);
 delay(motorSpeed);
 // 8
 digitalWrite(motorPin4, HIGH);
 digitalWrite(motorPin3, LOW);
 digitalWrite(motorPin2, LOW);
 digitalWrite(motorPin1, HIGH);
 delay(motorSpeed);
}

