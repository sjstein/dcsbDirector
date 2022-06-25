#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

//define some colour values
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0

#define A10     0x6EE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
extern uint8_t A10CFont[];



LCDWIKI_KBV mylcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset

char* line9str = "mt";
char* line8str = "mt";
char* line7str = "mt";
char* line6str = "mt";
char* line5str = "mt";

char debugStr[40];
int debugLine = 0;
int debug = 0;

void trial(unsigned char c){
  char x = c;
  x++;
  delay(1);
}

void drawChar(int row, int col, unsigned char c) {
  int16_t x = 20 + col * 19;
  int16_t y = row * 32 + 6;
  mylcd.Draw_Char( x, y, c, A10, BLACK, 3, 0);
}

void printLine(int row, int col, char* newValue) {
  int16_t y = row * 32 + 6;
  mylcd.Print_String("                        ", CENTER, y);
  mylcd.Print_String( newValue, CENTER, y );
}

char readByte() {
  char ret = Serial.read();
  delayMicroseconds(200);
  return(ret);
}

void flushSerialBuffer() {
  while(Serial.available()){
    char trash = Serial.read();
  }
  return;
}

// Functions for debugging
void blinky() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(10);                       // wait 
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(LED_BUILTIN, OUTPUT);
  mylcd.Init_LCD();
  mylcd.Fill_Screen(BLACK);
  mylcd.Set_Rotation(1);
  mylcd.Set_Text_colour(A10);
  mylcd.Set_Text_Back_colour(BLACK);
  mylcd.Set_Text_Size(3);
  for (int row = 250; row <= 280; row += 30) {
    blinky();
    mylcd.Print_String("..CDU initializing..", CENTER, row);
    delay(10);
    mylcd.Fill_Screen(BLACK);
  }
}

 

void loop() {
  if (Serial.available()){
    static char msg[26];
    static unsigned int message_pos = 0;
    char inByte = readByte();
    
    switch(inByte) {
      case 'C':
        // Request to update a specific character
        for (int i = 0; i < 4; i++){
          msg[i]=readByte();
        }
        drawChar(char(msg[0]),char(msg[1]),char(msg[2]));
        // msg[3] contains the 0xFF flag
        break;
   
      case 'L':
      // Request to update a whole line
        for (int i = 0; i < 25; i++){
          msg[i]=readByte();
        }
        msg[25] = '\0';
        printLine(char(msg[0]), 0, &msg[1]);
        flushSerialBuffer();
        break;

      default:
        printLine(0,0,"Hit default block");
    }
  }
  delay(0.5);  
 }
