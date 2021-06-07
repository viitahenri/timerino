
/*
 * 
 * DRF - Added rounding to f/stop stepping
 * DRF - Fixed missing cases for LCD operation and LED dimming
 * DRF - Change relay output to active low
 * DRF - Eliminated startp scroll brag line
 * DRF - Fixed LCD clearing issues, extraneous letters left on screen
 * DRF - Initialized button and mode pins with internal pullups
 * DRF - Changed button sense to active low
 * 
 Darkroom timer with programmable functions
 Supported models: DL002A CMG001A

 This sketch implements a darkroom timer with various modes.
 It's developed after Daniele Lucarelli's version on analogica.it
 (http://www.analogica.it/upgrade-timer-con-keypad-t6797.html)
 
 To build this sketch you need, besides core libraries, the Keypad.h
 library: http://playground.arduino.cc/code/Keypad

 The circuit:
 * Arduino UNO/Duemilanove
 * 4x4 keypad
 * buzzer
 * pushbutton
 * female 1/4" jack and pedal (optional)
 * 220V relay
 * 2 toggle switch

 Model DL002A specific components:
 * 7-Segment 4 number serial LCD display
 * Grove LED Bar
 
 Model CMG001A specific components
 * 16x2 matrix LCD display
 * 1 or 2 potentiometers
 
 The wiring schemes should come with this sketch, if not drop me a line.
 
 ### Pin Map Recap - DL002A ###
 D0:
 D1:
 D(2, 3, 4, 5, 6, 7, 8, 9): Keypad
 D10: Buzzer
 D11: Relay
 D12: progression switch (linear or f/stop) - maybe superfluous? 
 D13: main button (pedal/pushbutton)
 A(0, 1): Ledbar 
 A2: 7 segment display


 created  11 Nov 2013
 by Daniele Lucarelli
 adapted with LCD 16x2 display 5 Dec 2013
 by Ciro Mattia Gonano <ciromattia@gmail.com>

 */

#include <Keypad.h>
#include <avr/eeprom.h>

#include "Timerino.h"  // include personal conf

#include <LiquidCrystal.h>
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// define wired pins
const byte buzzer = 10; // buzzer pin
const byte mainbtn = 13; // main button (pushbutton/pedal) pin
const byte relay = 11; // relay pin
const byte selector = 12; // progression switch pin
// define tone and delays
const int tone_up = 600;
const int tone_down = 300;
const int scrollTime = 180;
const int waitTime = 800;


/************* YOU SHOULD NOT TOUCH ANYTHING BELOW THIS LINE *************/
// keypad
const byte ROWS = 4; // keypad: 4 rows
const byte COLS = 4; // keypad: 4 cols
// modes
const byte MODLINFREE = 0b00000;
const byte MODLINUP = 0b00010;
const byte MODLINDOWN = 0b00100;
const byte MODLINDDS = 0b00110;
const byte MODFSTPREC = 0b00001;
const byte MODFSTTEST = 0b00011;
const byte MODFSTDOWN = 0b00101;
const byte MODFSTBURN = 0b00111;

// Multiplier formula for f/stop progression (thanks to Gergio)
// mult = 2^(1/precision)

// Variables
int i = 0;
int ch = 0;
int brightness = 0;
int ncifra = 0;
int ncifra_countdown[9];
int ncifra_dds[9];
int ncifra_fsttest[9];
int ncifra_fstdown[9];
int ncifra_brn[9];
int time = 0;
int time_succ;
int appo_time;
int time_burn;
int time_countdown[9];
int time_dds[9];
int time_fsttest[9];
int time_fstdown[9];
int time_brn[9];
byte timer_mode = MODLINFREE;
long last_time = 0;
long errlet = 0;
int btnstatus = 1;         // main button status
int lastbtnstatus = HIGH;     // last main button status
int selstatus = HIGH;
int lastselstatus = HIGH;
long lum;
int lastlum;
boolean running = false;
boolean btnhigh = false;
boolean selbtnhigh = false;
boolean firstpress = true;
byte mute = false;
boolean scherma = false;
float mult = 1.0;
int precis = 1;
char _buffer[17];

/* init matrix keypad */
char keys[ROWS][COLS] = {
  {'1','2','3','A' },
  {'4','5','6','B' },
  {'7','8','9','C' },
  {'*','0','#','D' }
};
byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4, 5}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


/***************** DISPLAY functions *****************/
void beep(int this_tone, int duration) {
  if (!mute)
    tone(buzzer, this_tone, duration);
}
void metronome() {
  if (running && time%10 == 0) {
    beep(tone_down, 80);
  }
}
void metronome_b() {
  if (running && time_burn%10 == 0) {
    beep(tone_down, 80);
  }
}

void setup_display() {
    lcd.begin(16, 2);
    lcd.clear();
    /*
    sprintf(_buffer,"%s","ANALOGICA.IT    ");
    lcd.setCursor(16,1);
    lcd.autoscroll();
    for (int thisChar=0; thisChar < 16; thisChar++) {
      lcd.print(_buffer[thisChar]);
      delay(scrollTime);
    }
    lcd.noAutoscroll();
    */
}

/*
float myround(float f)   //Improved rounding function for predictable fstop inc/dec
{
  if (f >= 0x1.0p23) return f;
  return (float) (unsigned int) (f + 0.49999997f);
}


float myround(float f)
{
  if (f >= 0x1.0p23) return f;
  if (f <= 0.5) return 0;
  return (float) (unsigned int) (f + 0.5f);
}

*/

void say_ch() {
  switch (ch) {
    case 0:
      lcd.setCursor(0,1);
      lcd.print("Ch-1");
      break;
    case 1:
      lcd.setCursor(0,1);
      lcd.print("Ch-2");
      break;
    case 2:
      lcd.setCursor(0,1);
      lcd.print("Ch-3");
      break;
    case 3:
      lcd.setCursor(0,1);
      lcd.print("Ch-4");
      break;
    case 4:
      lcd.setCursor(0,1);
      lcd.print("Ch-5");
      break;
    case 5:
      lcd.setCursor(0,1);
      lcd.print("Ch-6");
      break;
    case 6:
      lcd.setCursor(0,1);
      lcd.print("Ch-7");
      break;
    case 7:
      lcd.setCursor(0,1);
      lcd.print("Ch-8");
      break;
    case 8:
      lcd.setCursor(0,1);
      lcd.print("Ch-9");
      break;
  }
}
    
void say_reset() {
  lcd.setCursor(0,0);
  lcd.clear();
}

void say_clearprecis() {
  lcd.setCursor(12,1);
  lcd.print("     ");
}
void say_cleartime() {
  lcd.setCursor(0,1);
  lcd.print("        ");
}
void say_time() {
  //say_cleartime();
  float ftime = time > 0 ? (float)time / 10 : 0.0;
  dtostrf(ftime, 3, 1, _buffer);
  lcd.setCursor(0,1);
  lcd.print(_buffer);
}

void say_time_b() {
  float ftime = time_burn > 0 ? (float)time_burn / 10 : 0.0;
  dtostrf(ftime, 3, 1, _buffer);
  lcd.setCursor(0,1);
  lcd.print(_buffer);
}

void say_prec() {
  say_clearprecis();
  switch(precis) {
    case 1: lcd.setCursor(15,1); lcd.print("1");   break;
    case 2: lcd.setCursor(13,1); lcd.print("1/2"); break;
    case 3: lcd.setCursor(13,1); lcd.print("1/3"); break;
    case 4: lcd.setCursor(13,1); lcd.print("1/4"); break;  
    case 6: lcd.setCursor(13,1); lcd.print("1/6"); break;
    case 8: lcd.setCursor(13,1); lcd.print("1/8"); break;
    case 12: lcd.setCursor(12,1); lcd.print("1/12"); break;
    case 24: lcd.setCursor(12,1); lcd.print("1/24"); break;
    case 32: lcd.setCursor(12,1); lcd.print("1/32"); break;  
    case 48: lcd.setCursor(12,1); lcd.print("1/48"); break;
  }
}

void say_free() {
  lcd.setCursor(0,0);
  lcd.print("Setup Mode       ");
  beep(tone_up, 200);

  say_clearprecis();
  say_cleartime();

  #if 1 == EPR
    eeprom_write_byte(0, MODLINFREE);
  #endif

}

void say_up(){
  lcd.setCursor(0,0);
  lcd.print("Stopwatch       ");
  beep(tone_up, 200);

  say_clearprecis();
  say_cleartime();
  say_time();
  
  #if 1 == EPR
    eeprom_write_byte(0, MODLINUP);
  #endif
  
}
void say_down(){
  lcd.setCursor(0,0);
  lcd.print("Countdown       ");
  beep(tone_down, 200);
  
  say_clearprecis();
  say_cleartime();
  say_time();
  
  #if 1 == EPR
    eeprom_write_byte(0, MODLINDOWN);
  #endif

}
void say_dds() {
  lcd.setCursor(0,0);
  lcd.print("DDS Mode        ");
  beep(tone_down, 200);
  
  say_clearprecis();
  say_cleartime();
  say_time();
  
  #if 1 == EPR
    eeprom_write_byte(0, MODLINDDS);
  #endif  
  
}

void say_precis() {
  lcd.setCursor(0,0);
  lcd.print("F/Stop Precision");
  beep(tone_down, 200);
  say_cleartime();
  
  say_prec();
  
  #if 1 == EPR
    eeprom_write_byte(0, MODFSTPREC);
  #endif  

}
void say_test_strip() {
  lcd.setCursor(0,0);
  lcd.print("F/stop TestStrip");
  beep(tone_down, 200);
  
  say_prec();
  say_cleartime();
  say_time();
  
  #if 1 == EPR
    eeprom_write_byte(0, MODFSTTEST);
  #endif    

}
void say_fstopdown() {
  lcd.setCursor(0,0);
  lcd.print("F/stop Countdown");
  beep(tone_down, 200);
  
  say_prec();
  say_cleartime();
  say_time();
  
  #if 1 == EPR
    eeprom_write_byte(0, MODFSTDOWN);
  #endif      
  
}

void say_burn() {
  lcd.setCursor(0,0);
  lcd.print("F/stopBurn&Dodge");
  beep(tone_down, 200);
  
  say_prec();
  say_cleartime();
  say_time();

  #if 1 == EPR
    eeprom_write_byte(0, MODFSTBURN);
  #endif      
  
}

void say_sound() {
  lcd.setCursor(0,0);
  lcd.print("Sound On        ");
  beep(tone_down, 200);
  eeprom_write_byte(5, false);
}

void say_mute() {
  lcd.setCursor(0,0);
  lcd.print("Sound Off       ");
  beep(tone_down, 200);
  eeprom_write_byte(5, true);
}

void say_timermode() {
  switch (timer_mode) {
    case MODLINFREE: say_free();       break;
    case MODLINUP:   say_up();         break;
    case MODLINDOWN: say_down();       break;
    case MODLINDDS:  say_dds();        break;
    case MODFSTPREC: say_precis();     break;
    case MODFSTTEST: say_test_strip(); break;
    case MODFSTDOWN: say_fstopdown();  break;
    case MODFSTBURN: say_burn();      break;    
    default: break;
  }
}


/************ MODE FUNCTIONS ********/
void off() {
  // no action taken
}
void countdown() {
  if (time > 0 && (millis() - last_time >= 100)) {
    last_time = millis();
    time--;
    metronome();
    say_time();
    if (time == 0) {
      // if time reached 0, shut down the relay and reset the function
      digitalWrite(relay, HIGH);
      running = false;
      time = appo_time;
      beep(tone_down, 75);
      delay(150);
      beep(tone_down, 75);
      delay(150);
      beep(tone_down, 150);
      if (timer_mode == MODLINDDS || timer_mode == MODFSTBURN) {
        firstpress = true;
        scherma = false;
      }
      say_time();
    }
  }
}
void countdown_b() {
  if (time_burn > 0 && (millis() - last_time >= 100)) {
    last_time = millis();
    time_burn--;
    metronome_b();
    say_time_b();
    if (time_burn == 0) {
      // if time reached 0, shut down the relay and reset the function
      digitalWrite(relay, HIGH);
      running = false;
      //time = appo_time;
      beep(tone_down, 75);
      delay(150);
      beep(tone_down, 75);
      delay(150);
      beep(tone_down, 150);
      if (timer_mode == MODFSTBURN) {
        firstpress = false;
        scherma = false;
      }
      say_time();
    }
  }
}
void stopwatch() {
  if (millis() - last_time >= 100) {
    last_time = millis();
    time++;
    metronome();
    say_time();
  }
}
void test_strip() {
  if (millis() - last_time >= 100) {
    last_time = millis();
    time++;
    say_time();
    if (time == time_succ) {
      beep(tone_up, 150);    
      mult = pow(2.0,(1.0/precis));  
      time_succ = int(float(time) * mult);
    }
    if (time_succ - time <= 3) {
      beep(tone_up, 50);
    }
  }
}
void reset() {
  say_reset();
  time = 0;
  ncifra = 0;
  appo_time = 0;
  time_burn = 0;
  firstpress = true;
  scherma = false;
}


// Manage brightness and audio on / off in EEPROM 
#if 1 == EPR
  void load_eeprom() {
    brightness = eeprom_read_word((uint16_t *)1);      // 2 byte
    precis = eeprom_read_word((uint16_t *)3);          // 2 byte

    timer_mode = eeprom_read_byte(0);
    // if mode is not valid set the first one
    if ((timer_mode & 0b11000) > 0) {
      timer_mode = MODLINFREE;
      //return;
    }

    mute = eeprom_read_byte(5);
    // if mute is not valid set to unmute
    if (mute > 1) {mute = false;}
    
/*
    time_countdown = eeprom_read_word((uint16_t *)1);  // 2 byte
    time_dds = eeprom_read_word((uint16_t *)3);        // 2 byte
    time_fsttest = eeprom_read_word((uint16_t *)5);    // 2 byte
    time_fstdown = eeprom_read_word((uint16_t *)7);    // 2 byte
    time_burn = eeprom_read_word((uint16_t *)11);      // 2 byte
*/    
  }
#endif      

void init_timermode() {
  switch (timer_mode) {
    case MODLINDOWN:
      time = time_countdown[ch];
      ncifra = ncifra_countdown[ch];
      appo_time = time_countdown[ch];
      break;
    case MODLINDDS:
      time = time_dds[ch];
      ncifra = ncifra_dds[ch];
      appo_time = time_dds[ch];
      firstpress = true;
      break;
    case MODFSTTEST:
      time = time_fsttest[ch];
      ncifra = ncifra_fsttest[ch];
      appo_time = time_fsttest[ch];
      break;
    case MODFSTDOWN:
      time = time_fstdown[ch];
      ncifra = ncifra_fstdown[ch];
      //appo_time = ncifra_fstdown[ch]; //DRF - Bug in original code. Wrong variable.
      appo_time = time_fstdown[ch]; //DRF
      break;
    case MODFSTBURN:
      time = time_brn[ch];
      ncifra = ncifra_brn[ch];
      appo_time = time_brn[ch];
      break;
    case MODLINFREE:
    case MODLINUP:
    case MODFSTPREC:
      time = 0;
      ncifra = 0;
      appo_time = 0;
      break;
    default:
      break;
  }
  say_timermode();
}


/*** Keypad management functions ***/
//void keypadEvent(KeypadEvent key) {}
void read_key() {
  char key = ' ';
  boolean ast = false;
  boolean canc = false;
  if (keypad.getKeys()) { //Fill an array with all the keys pressed
    for (int ki=0; ki<LIST_MAX; ki++) { // Browse the array
      if (keypad.key[ki].kchar != NO_KEY) {
        key = keypad.key[ki].kchar;
        if (key == '*') {
          ast = true;
        }
        if (key == '#') {
          canc = true;
        }
      }
    }
    if (ast == true && canc == true) {
      key = '%';
    }
    
    switch (key) {
      case 'A':  // Free mode
        reset();
        timer_mode = MODLINFREE | (timer_mode & 0b00001);
        init_timermode();
        break;
      case 'B':
        reset();
        timer_mode = MODLINUP | (timer_mode & 0b00001);
        init_timermode();
        break;
      case 'C':
        reset();
        timer_mode = MODLINDOWN | (timer_mode & 0b00001);
        init_timermode();
        break;
      case 'D':
        reset();
        timer_mode = MODLINDDS | (timer_mode & 0b00001);
        init_timermode();
        break;
        
      case '%': // reset//Needed to clear only second line of LCD display with variable reset
        time = 0;
        ncifra = 0;
        appo_time = 0;
        time_burn = 0;
        firstpress = true;
        scherma = false;
        lcd.setCursor(0,1);
        lcd.print("     ");
        say_time();
        lcd.setCursor(0,0);
        break;    
  
      case '*': 
        if (timer_mode == MODFSTDOWN) {
          // Step down
          mult = pow(2.0, (1.0/precis));
          time = int(round(float(time) / mult));
          //time = int(float(time) / mult);
          if (time <= 1)
            time = 1;
          appo_time = time;
          say_time();
        } else if (timer_mode == MODFSTBURN) {      /////////////////////////////////////////////  
          //Scherma
          scherma = true;
          mult = 1.0/precis;
          time_burn = int(round(float(time) * mult));
          //time_burn = int(float(time) * mult);
          if (time >= 9999)
            time = 9999;
          say_time_b();
          firstpress = true;
        } else if (timer_mode == MODLINFREE) {
        }
        break;
  
      case '#':
        if (timer_mode == MODFSTDOWN) {                
          // Step up
          mult = pow(2.0, (1.0/precis));
          time = int(round(float(time) * mult));
          //time = int(float(time) * mult);
          if (time >= 9999)
            time = 9999;
          appo_time = time;
          say_time();
        } else if (timer_mode == MODFSTBURN) {          
          //Brucia
          scherma = false;
          mult = 1.0/precis;
          time_burn = int(round(float(time) * mult));
          //time_burn = int(float(time) * mult);
          if (time_burn >= 9999)
            time_burn = 9999;
          say_time_b();
          firstpress = true;        
        } else if (timer_mode == MODLINFREE) {
        }
        break;
  
      default: 
        if (timer_mode == MODLINUP) { // stopwatch does not accept numbers input
          say_up();
          appo_time = time;
          say_time();
        } else if (timer_mode == MODLINFREE) { // setup audio on/off
          switch (key) {
            case '0':
              if (mute == true) {
                mute = false;
                beep(tone_up, 200);
                say_sound();
              } else {
                mute = true;
                say_mute();              
              }
              break;
            case '1':  ch = 0; say_ch();
              break;
            case '2':  ch = 1; say_ch();
              break;
            case '3':  ch = 2; say_ch();
              break;
            case '4':  ch = 3; say_ch();
              break;
            case '5':  ch = 4; say_ch();
              break;
            case '6':  ch = 5; say_ch();
              break;
            case '7':  ch = 6; say_ch();
              break;
            case '8':  ch = 7; say_ch();
              break;
            case '9':  ch = 8; say_ch();
              break;
            default: break;
          }
        } else if (timer_mode == MODFSTPREC) { // set precision
          switch (key) {
            case '1':  precis = 1; say_prec();
                      #if 1 == EPR
                        eeprom_write_word((uint16_t *)3, precis);
                      #endif
                      break;
            case '2':  precis = 2; say_prec();
                     #if 1 == EPR
                        eeprom_write_word((uint16_t *)3, precis);
                     #endif
                     break;
            case '3':  precis = 3; say_prec();
                     #if 1 == EPR
                        eeprom_write_word((uint16_t *)3, precis);
                     #endif
                     break;
            case '4':  precis = 4; say_prec();
                      #if 1 == EPR
                        eeprom_write_word((uint16_t *)3, precis);
                      #endif  
                      break;
            case '5':  precis = 6; say_prec();
                      #if 1 == EPR          
                        eeprom_write_word((uint16_t *)3, precis);
                      #endif 
                      break;
            case '6':  precis = 8; say_prec();
                      #if 1 == EPR          
                        eeprom_write_word((uint16_t *)3, precis);
                      #endif  
                      break;
            case '7':  precis = 12; say_prec();
                      #if 1 == EPR          
                        eeprom_write_word((uint16_t *)3, precis);
                      #endif  
                      break;
            case '8':  precis = 24; say_prec();
                      #if 1 == EPR          
                        eeprom_write_word((uint16_t *)3, precis);
                      #endif  
                      break;
            case '9':  precis = 32; say_prec();
                      #if 1 == EPR          
                        eeprom_write_word((uint16_t *)3, precis);
                      #endif
                      break;
            case '0':  precis = 48; say_prec();
                      #if 1 == EPR          
                        eeprom_write_word((uint16_t *)3, precis);
                      #endif
                      break;
            default: break;
          } 
        } else {
            switch (ncifra) {
              case 0:
                time = time + (key-48);
                ncifra++;
                break;
              case 1:
                time = time * 10 + (key-48);
                ncifra++;
                break;
              case 2:
                time = time * 10 + (key-48);
                ncifra++;
                break;
              case 3:
                time = time * 10 + (key-48);
                ncifra++;
                break;
              default:
                // Non faccio nulla
                break;
            }
          appo_time = time;
          say_time();
          
          
  /*
          if (time < 1) {
            time = key == '0' ? 0 : key;
          } else {
            time *= 10;
            if (time > 9999)
              time %= 10000;
            if (key != '0')
              time += key;
          }
  */
  
  
      }
    }
  }
}

void setup() {
//  Serial.begin(9600);
//  Serial.println(">>> Debug <<<");

  /* ensure analog pins are set to OUTPUT */
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
     digitalWrite(relay, HIGH); // shut down the relay
  pinMode(mainbtn, INPUT_PULLUP);
  pinMode(selector, INPUT_PULLUP);

  // read from EEPROM stored values
  #if 1 == EPR
    load_eeprom();
  #endif
  
  // Setup display
  setup_display();

  // init fstop/linear mode
  // bitClear(timer_mode,0); // linear mode

  /*
  selstatus = digitalRead(selector);
  lastselstatus = selstatus;
  if (selstatus == HIGH) {
    bitSet(timer_mode,0); // f/stop mode
  } else {
    bitClear(timer_mode,0); // linear mode
  }
*/

  // TODO: add event listener for keypad
  //keypad.addEventListener(keypadEvent);

  // cleanup and get ready to start
  delay(waitTime);
  say_reset();
  init_timermode();
  errlet = millis();
}

void loop() {
  if (millis() - errlet <= 20) {
  } else {
    btnstatus = digitalRead(mainbtn);
    if (btnstatus != lastbtnstatus) {
      // main button has been toggled
      if (ACTIONSIGNAL == btnstatus) {
        // main button has been pressed
        btnhigh = true; 
      }
      lastbtnstatus = btnstatus;
    }

    selstatus = digitalRead(selector);
    if (selstatus != lastselstatus) {
      // mode select button has been pressed
      if (ACTIONSIGNAL == selstatus) {
        // mode select button has been pressed
        selbtnhigh = true;
      }
      lastselstatus = selstatus;
    }
    errlet = millis();
  }

/*  selstatus = digitalRead(selector);
  if (selstatus != lastselstatus) {
    // mode change
    lastselstatus = selstatus;
    timer_mode = (selstatus == HIGH) ? MODFSTPREC : MODLINFREE;
    say_timermode();
  } */

  if (!running) { //Check for mode key depressed if timer not running
      if (selbtnhigh == true) {
        // mode select button has been pressed
        //if (timer_mode == MODFSTPREC) {timer_mode = MODLINFREE;} else {timer_mode = MODFSTPREC;} //Toggle modes
        if(bitRead(timer_mode, 0)) {bitClear(timer_mode,0);} else {bitSet(timer_mode,0);}  //Toggle linear/fstop modes
        init_timermode();
      }
  }
  selbtnhigh = false;
  
  if (running && btnhigh) {
    // shut down the relay and pause the timer
    digitalWrite(relay, HIGH);
    btnhigh = false;
    running = false;
    if (timer_mode == MODLINDDS && firstpress) {
      firstpress = false;
    }
  } else if (running && !btnhigh) {
    switch (timer_mode) {
      case MODLINFREE:
        off();
        break;
      case MODLINUP:
        stopwatch();
        break;
      case MODLINDOWN:
        countdown();
        break;
      case MODLINDDS:
        firstpress ? off() : countdown();
        break;
      case MODFSTPREC:
        off();
        break;
      case MODFSTTEST:
        test_strip();
        break;
      case MODFSTDOWN:
        countdown();
        break;
      case MODFSTBURN:
        firstpress ? countdown_b() : countdown();
        break;
    }
  } else if (!running && btnhigh) { // not running
    btnhigh = false;
    switch (timer_mode) {
      case MODLINFREE:
      case MODFSTPREC:
        digitalWrite(relay, LOW); // powerup the relay
        running = true;
        off();
        break;
      case MODLINUP:
        digitalWrite(relay, LOW); // powerup the relay
        running = true;
        last_time = millis(); // reset timer counter
        stopwatch();
        break;
      case MODLINDOWN:
        time_countdown[ch] = appo_time;
        ncifra_countdown[ch] = ncifra;
        /*
        #if 1 == EPR
          eeprom_write_word((uint16_t *)1, time_countdown);
        #endif
        */
        if (time > 0) {
          digitalWrite(relay, LOW); // powerup the relay
          running = true;
          last_time = millis(); // reset timer counter
          countdown();
        }
        break;
      case MODLINDDS:
        time_dds[ch] = appo_time;
        ncifra_dds[ch] = ncifra;
        /*
        #if 1 == EPR
             eeprom_write_word((uint16_t *)3, time_dds);
        #endif
        */
        if (time > 0) {
          digitalWrite(relay, LOW); // powerup the relay
          running = true;
          if (firstpress == true) {
            off();
          } else {
            last_time = millis(); // reset timer counter
            countdown();
          }
        }
        break;
      case MODFSTBURN:      /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        time_brn[ch] = appo_time;
        ncifra_brn[ch] = ncifra;
        /*
        #if 1 == EPR
             eeprom_write_word((uint16_t *)3, time_dds);
        #endif
        */
        if (scherma == true) {
          time = time - time_burn;
        }
        
        if (time > 0) {
          if (time_burn > 0) {
            digitalWrite(relay, LOW); // powerup the relay
            running = true;
            if (firstpress == true) {
              last_time = millis(); // reset timer counter
              countdown_b();
            } else {
              last_time = millis(); // reset timer counter
              countdown();
            }
          } else {
              firstpress = false;
              digitalWrite(relay, LOW); // powerup the relay
              running = true;
              last_time = millis(); // reset timer counter
              countdown();
          }
        }
        break;        
      
      case MODFSTTEST:
        time_fsttest[ch] = appo_time;
        ncifra_fsttest[ch] = ncifra;  
        /*      
        #if 1 == EPR
             eeprom_write_word((uint16_t *)5, time_fsttest);
        #endif
        */
        digitalWrite(relay, LOW); // powerup the relay
        running = true;
        last_time = millis(); // reset timer counter
        if (time < 10)
          time = 10;
        time_succ = time;
        time = 0;
        test_strip();
        break;
      case MODFSTDOWN:
        time_fstdown[ch] = appo_time;
        ncifra_fstdown[ch] = ncifra;                
        /*
        #if 1 == EPR
             eeprom_write_word((uint16_t *)7, appo_time);
        #endif
        */
        if (time > 0) {
          digitalWrite(relay, LOW); // powerup the relay
          running = true;
          last_time = millis(); // reset timer counter
          countdown();        
        }
        break;
    }
  } else { // not running nor button released, so read keypad input
    read_key();
  }
}

