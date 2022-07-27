#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

//define colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define A10     0x6EE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define ACK 0x55

// Declare which font we will be using
extern uint8_t A10CFont[];

LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

void drawChar(int row, int col, unsigned char c) {
  int16_t x = 24 + col * 18;
  int16_t y = row * 32 + 3;
  mylcd.Set_Text_Cousur(x, y);
  mylcd.write(c);
}

void printLine(int row, char* newValue) {
  int16_t y = row * 32 + 3;
  mylcd.Print_String( newValue, CENTER, y );
}

void printError(int row, char msg[]) {
  printLine(row, msg);
}

char readByte() {
  char ret = Serial.read();
  delayMicroseconds(200);
  return(ret);
}

void flushSerialBuffer() {
  char trash;
  while(Serial.available()){
    trash = Serial.read();
  }
  return;
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  mylcd.Init_LCD();
  mylcd.Fill_Screen(BLACK);
  mylcd.Set_Rotation(1);
  mylcd.Set_Text_colour(A10);
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Set_Text_Size(3);

  // Display random screen upon startup
  for (int i = 0; i < 500; i++) {
    drawChar(char(random(0,10)),char(random(0,24)),char(random(33,125)));
  }
  flushSerialBuffer();
  mylcd.Fill_Screen(BLACK);
}

void loop() {
  if (Serial.available()){
    static char msg[26];
    char inByte = readByte(); // First byte determine message type
    
    switch(inByte) {

      case 'C':
        // Request to update a specific character
        for (int i = 0; i < 4; i++){
          msg[i]=readByte();
        }
        drawChar(char(msg[0]),char(msg[1]),char(msg[2]));
        // msg[3] contains the 0xFF EOM flag
        Serial.write(ACK);
        break;
   
      case 'L': {  // Enclose this case within curly braces to allow inline declare/assignment of lnum 
        // Request to update a whole line
        char lnum = readByte();         // Second byte in message specifies line number
        for (int i = 0; i < 25; i++){   // Grab the rest of the string
          msg[i]=readByte();
          if (msg[i] == 0) {
            msg[i] = 32;                // Change empty memory location into space character
          }
        }
        msg[24] = '\0';                 // Terminate string
        printLine(lnum, msg);
        flushSerialBuffer();
        Serial.write(ACK);
        break;
      }
        
      case 'X':
        // Request to clear the screen
        mylcd.Fill_Screen(BLACK);
        flushSerialBuffer();
        Serial.write(ACK);
        break;
        
      default:
        mylcd.Fill_Screen(BLACK);
        printLine(3,"+----------------------+");
        printLine(4,"| ARDUINO EXCEPTION:   |");
        printLine(5,"| INVALID MSG CHAR     |");
        printLine(6,"| FROM BIOS DIR ->     |");
        printLine(7,"+----------------------+");    
        drawChar(char(6),char(19),char(inByte));
        flushSerialBuffer();
        Serial.write(ACK);
    }
  }
  delay(0.5);  // is this needed?
 }
