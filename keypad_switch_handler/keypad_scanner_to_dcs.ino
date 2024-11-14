/* 
A10 Keypad scanner
*/
#include <Keypad.h>
#define DCSBIOS_IRQ_SERIAL
#include "DcsBios.h"

const byte ROWS = 9; //nine rows
const byte COLS = 8; //eight columns
// NOTE: The designation on the PCB for ROWS and COLS does not agree with how the Keypad library does its scanning
// If we reverse the sense (rows become columns, columns become rows), things appear to work fine.

/* NOTE 2: There are keys on the CDU which do not correspond directly to ASCII characters. However, since there are no
  lower case characters present on that keyboard, I map them as follows:
  a : LSK1
  b : LSK2
  c : LSK3
  d : LSK4
  e : LSK5
  f : LSK6
  g : LSK7
  h : LSK8
  i : SYS
  j : NAV
  k : WP
  l : OSET
  m : FPM
  n : PREV
  o : DIM
  p : BRT
  q : PGUP
  r : PGDN
  s : BLNK1
  t : BLNK2
  u : BCK
  v : MK
  w : blrock1
  x : blrock2
  y : CLR
  z : FA
  _ : <space>
  ? : Undefined char
*/
//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'i', '4', '.', 'r', 'a', '1', '7', 'q'},
  {'k', '6', '/', '?', 'c', '3', '9', 't'},
  {'m', 'H', 'T', 'x', 'e', 'B', 'N', '_'},
  {'o', 'J', 'V', 'z', 'g', 'D', 'P', 'Z'},
  {'?', 'L', 'X', '?', '?', 'F', 'R', '?'},
  {'j', '5', '0', 'v', 'b', '2', '8', 's'},
  {'l', 'G', 'S', 'w', 'd', 'A', 'M', 'u'},
  {'n', 'I', 'U', 'y', 'f', 'C', 'O', 'Y'},
  {'p', 'K', 'W', '-', 'h', 'E', 'Q', '+'},
};
byte rowPins[ROWS] = {39, 38, 37, 36, 35, 34, 33, 32, 31}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {48, 47, 46, 45, 44, 43, 42, 41}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  DcsBios::setup();
  keypad.addEventListener(keypadEvent);  // Add an event listener.
  keypad.setHoldTime(100);               // Default is 1000mS
  keypad.setDebounceTime(50);           // Default is 50mS
}
  
void loop(){
  DcsBios::loop();
  char key = keypad.getKey();
}

void keypadEvent(KeypadEvent KEY){  
 switch (keypad.getState()) { // gives PRESSED, HOLD or RELEASED
 case PRESSED: 
   switch(KEY) { 
   case 'A': sendDcsBiosMessage("CDU_A", "1"); break; 
   case 'B': sendDcsBiosMessage("CDU_B", "1"); break; 
   case 'C': sendDcsBiosMessage("CDU_C", "1"); break;     
   case 'D': sendDcsBiosMessage("CDU_D", "1"); break; 
   case 'E': sendDcsBiosMessage("CDU_E", "1"); break; 
   case 'F': sendDcsBiosMessage("CDU_F", "1"); break; 
   case 'G': sendDcsBiosMessage("CDU_G", "1"); break; 
   case 'H': sendDcsBiosMessage("CDU_H", "1"); break; 
   case 'I': sendDcsBiosMessage("CDU_I", "1"); break; 
   case 'J': sendDcsBiosMessage("CDU_J", "1"); break; 
   case 'K': sendDcsBiosMessage("CDU_K", "1"); break; 
   case 'L': sendDcsBiosMessage("CDU_L", "1"); break; 
   case 'M': sendDcsBiosMessage("CDU_M", "1"); break; 
   case 'N': sendDcsBiosMessage("CDU_N", "1"); break; 
   case 'O': sendDcsBiosMessage("CDU_O", "1"); break;
   case 'P': sendDcsBiosMessage("CDU_P", "1"); break;
   case 'Q': sendDcsBiosMessage("CDU_Q", "1"); break; 
   case 'R': sendDcsBiosMessage("CDU_R", "1"); break;   
   case 'S': sendDcsBiosMessage("CDU_S", "1"); break;     
   case 'T': sendDcsBiosMessage("CDU_T", "1"); break; 
   case 'U': sendDcsBiosMessage("CDU_U", "1"); break;        
   case 'V': sendDcsBiosMessage("CDU_V", "1"); break; 
   case 'W': sendDcsBiosMessage("CDU_W", "1"); break; 
   case 'X': sendDcsBiosMessage("CDU_X", "1"); break; 
   case 'Y': sendDcsBiosMessage("CDU_Y", "1"); break; 
   case 'Z': sendDcsBiosMessage("CDU_Z", "1"); break;
   case '0': sendDcsBiosMessage("CDU_0", "1"); break; 
   case '1': sendDcsBiosMessage("CDU_1", "1"); break; 
   case '2': sendDcsBiosMessage("CDU_2", "1"); break; 
   case '3': sendDcsBiosMessage("CDU_3", "1"); break; 
   case '4': sendDcsBiosMessage("CDU_4", "1"); break; 
   case '5': sendDcsBiosMessage("CDU_5", "1"); break; 
   case '6': sendDcsBiosMessage("CDU_6", "1"); break; 
   case '7': sendDcsBiosMessage("CDU_7", "1"); break; 
   case '8': sendDcsBiosMessage("CDU_8", "1"); break; 
   case '9': sendDcsBiosMessage("CDU_9", "1"); break;
   case '.': sendDcsBiosMessage("CDU_POINT", "1"); break;
   case '/': sendDcsBiosMessage("CDU_SLASH", "1"); break;
  // Special keys       
   case 'a': sendDcsBiosMessage("CDU_LSK_3L", "1"); break;
   case 'b': sendDcsBiosMessage("CDU_LSK_5L", "1"); break;
   case 'c': sendDcsBiosMessage("CDU_LSK_7L", "1"); break;
   case 'd': sendDcsBiosMessage("CDU_LSK_9L", "1"); break;
   case 'e': sendDcsBiosMessage("CDU_LSK_3R", "1"); break;
   case 'f': sendDcsBiosMessage("CDU_LSK_5R", "1"); break;
   case 'g': sendDcsBiosMessage("CDU_LSK_7R", "1"); break;
   case 'h': sendDcsBiosMessage("CDU_LSK_9R", "1"); break;
   case 'i': sendDcsBiosMessage("CDU_SYS", "1"); break;     
   case 'j': sendDcsBiosMessage("CDU_NAV", "1"); break; 
   case 'k': sendDcsBiosMessage("CDU_WP", "1"); break; 
   case 'l': sendDcsBiosMessage("CDU_OSET", "1"); break;
   case 'm': sendDcsBiosMessage("CDU_FPM", "1"); break; 
   case 'n': sendDcsBiosMessage("CDU_PREV", "1"); break;
  // case 's': sendDcsBiosMessage("CDU_LSK_7L", "1"); break;  // Undefined key on hardware
  // case 't': sendDcsBiosMessage("CDU_LSK_9L", "1"); break;  // Undefined key on hardware
   case 'u': sendDcsBiosMessage("CDU_BCK", "1"); break;
   case 'v': sendDcsBiosMessage("CDU_MK", "1"); break;
  // case 'w': sendDcsBiosMessage("CDU_LSK_7R", "1"); break;  // Undefined key on hardware
  // case 'x': sendDcsBiosMessage("CDU_LSK_9R", "1"); break;  // Undefined key on hardware
   case 'y': sendDcsBiosMessage("CDU_CLR", "1"); break;
   case 'z': sendDcsBiosMessage("CDU_FA", "1"); break;
   case '_': sendDcsBiosMessage("CDU_SPC", "1"); break;
  // Rocker swiches
  // +/-
   case '+': sendDcsBiosMessage("CDU_DATA", "2"); break;
   case '-': sendDcsBiosMessage("CDU_DATA", "0"); break;  
  // DIM/BRT 
   case 'o': sendDcsBiosMessage("CDU_BRT", "2"); break; 
   case 'p': sendDcsBiosMessage("CDU_BRT", "0"); break;  
   // PGUP/PGDWN    
   case 'q': sendDcsBiosMessage("CDU_PG", "2"); break;
   case 'r': sendDcsBiosMessage("CDU_PG", "0"); break;

   }
 case RELEASED: 
   switch(KEY) { // Released KEYS or Neutral Rockers 
   case 'A': sendDcsBiosMessage("CDU_A", "0"); break; 
   case 'B': sendDcsBiosMessage("CDU_B", "0"); break; 
   case 'C': sendDcsBiosMessage("CDU_C", "0"); break;     
   case 'D': sendDcsBiosMessage("CDU_D", "0"); break; 
   case 'E': sendDcsBiosMessage("CDU_E", "0"); break; 
   case 'F': sendDcsBiosMessage("CDU_F", "0"); break; 
   case 'G': sendDcsBiosMessage("CDU_G", "0"); break; 
   case 'H': sendDcsBiosMessage("CDU_H", "0"); break; 
   case 'I': sendDcsBiosMessage("CDU_I", "0"); break; 
   case 'J': sendDcsBiosMessage("CDU_J", "0"); break; 
   case 'K': sendDcsBiosMessage("CDU_K", "0"); break; 
   case 'L': sendDcsBiosMessage("CDU_L", "0"); break; 
   case 'M': sendDcsBiosMessage("CDU_M", "0"); break; 
   case 'N': sendDcsBiosMessage("CDU_N", "0"); break; 
   case 'O': sendDcsBiosMessage("CDU_O", "0"); break;
   case 'P': sendDcsBiosMessage("CDU_P", "0"); break;
   case 'Q': sendDcsBiosMessage("CDU_Q", "0"); break; 
   case 'R': sendDcsBiosMessage("CDU_R", "0"); break;   
   case 'S': sendDcsBiosMessage("CDU_S", "0"); break;     
   case 'T': sendDcsBiosMessage("CDU_T", "0"); break; 
   case 'U': sendDcsBiosMessage("CDU_U", "0"); break;        
   case 'V': sendDcsBiosMessage("CDU_V", "0"); break; 
   case 'W': sendDcsBiosMessage("CDU_W", "0"); break; 
   case 'X': sendDcsBiosMessage("CDU_X", "0"); break; 
   case 'Y': sendDcsBiosMessage("CDU_Y", "0"); break; 
   case 'Z': sendDcsBiosMessage("CDU_Z", "0"); break;
   case '0': sendDcsBiosMessage("CDU_0", "0"); break;  
   case '1': sendDcsBiosMessage("CDU_1", "0"); break; 
   case '2': sendDcsBiosMessage("CDU_2", "0"); break; 
   case '3': sendDcsBiosMessage("CDU_3", "0"); break; 
   case '4': sendDcsBiosMessage("CDU_4", "0"); break; 
   case '5': sendDcsBiosMessage("CDU_5", "0"); break; 
   case '6': sendDcsBiosMessage("CDU_6", "0"); break; 
   case '7': sendDcsBiosMessage("CDU_7", "0"); break; 
   case '8': sendDcsBiosMessage("CDU_8", "0"); break; 
   case '9': sendDcsBiosMessage("CDU_9", "0"); break; 
   case '.': sendDcsBiosMessage("CDU_POINT", "0"); break;
   case '/': sendDcsBiosMessage("CDU_SLASH", "0"); break;
  // Special keys       
   case 'a': sendDcsBiosMessage("CDU_LSK_3L", "0"); break;
   case 'b': sendDcsBiosMessage("CDU_LSK_5L", "0"); break;
   case 'c': sendDcsBiosMessage("CDU_LSK_7L", "0"); break;
   case 'd': sendDcsBiosMessage("CDU_LSK_9L", "0"); break;
   case 'e': sendDcsBiosMessage("CDU_LSK_3R", "0"); break;
   case 'f': sendDcsBiosMessage("CDU_LSK_5R", "0"); break;
   case 'g': sendDcsBiosMessage("CDU_LSK_7R", "0"); break;
   case 'h': sendDcsBiosMessage("CDU_LSK_9R", "0"); break;
   case 'i': sendDcsBiosMessage("CDU_SYS", "0"); break;     
   case 'j': sendDcsBiosMessage("CDU_NAV", "0"); break; 
   case 'k': sendDcsBiosMessage("CDU_WP", "0"); break; 
   case 'l': sendDcsBiosMessage("CDU_OSET", "0"); break;
   case 'm': sendDcsBiosMessage("CDU_FPM", "0"); break; 
   case 'n': sendDcsBiosMessage("CDU_PREV", "0"); break;
  // case 's': sendDcsBiosMessage("CDU_LSK_7L", "1"); break;  // Undefined key on hardware
  // case 't': sendDcsBiosMessage("CDU_LSK_9L", "1"); break;  // Undefined key on hardware
   case 'u': sendDcsBiosMessage("CDU_BCK", "0"); break;
   case 'v': sendDcsBiosMessage("CDU_MK", "0"); break;
  // case 'w': sendDcsBiosMessage("CDU_LSK_7R", "1"); break;  // Undefined key on hardware
  // case 'x': sendDcsBiosMessage("CDU_LSK_9R", "1"); break;  // Undefined key on hardware
   case 'y': sendDcsBiosMessage("CDU_CLR", "0"); break;
   case 'z': sendDcsBiosMessage("CDU_FA", "0"); break;
   case '_': sendDcsBiosMessage("CDU_SPC", "0"); break;
  // Rocker swiches
  // +/-
   case '+': sendDcsBiosMessage("CDU_DATA", "1"); break;
   case '-': sendDcsBiosMessage("CDU_DATA", "1"); break;  
  // DIM/BRT 
   case 'o': sendDcsBiosMessage("CDU_BRT", "1"); break; 
   case 'p': sendDcsBiosMessage("CDU_BRT", "1"); break;  
   // PGUP/PGDWN    
   case 'q': sendDcsBiosMessage("CDU_PG", "1"); break;
   case 'r': sendDcsBiosMessage("CDU_PG", "1"); break;   
   }
 }
} 