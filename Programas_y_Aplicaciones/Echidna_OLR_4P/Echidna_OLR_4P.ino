

/*  
 * ____                     _      ______ _____    _____
  / __ \                   | |    |  ____|  __ \  |  __ \               
 | |  | |_ __   ___ _ __   | |    | |__  | |  | | | |__) |__ _  ___ ___ 
 | |  | | '_ \ / _ \ '_ \  | |    |  __| | |  | | |  _  // _` |/ __/ _ \
 | |__| | |_) |  __/ | | | | |____| |____| |__| | | | \ \ (_| | (_|  __/
  \____/| .__/ \___|_| |_| |______|______|_____/  |_|  \_\__,_|\___\___|
        | |                                                             
        |_|          
 Open LED Race
 An minimalist cars race for LED strip  
  
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 by gbarbarov@singulardevices.com  for Arduino day Seville 2019 
 Code made dirty and fast, next improvements in: 
 https://github.com/gbarbarov/led-race
 https://www.hackster.io/gbarbarov/open-led-race-a0331a
 https://twitter.com/openledrace

- Adaptación para EchidnaShield (Arduino Uno / Leonardo) por XDeSIG

- Adaptación para Echidnashield  (Arduino Uno / Leonardo)en modo MKMK para 4 Jugadores por Lobotic 22/12/2019
*/

                                                            
#include <Adafruit_NeoPixel.h>
#define MAXLED         300 // MAX LEDs actives on strip

#define PIN_LED        8    // R 500 ohms to DI pin for WS2812 and WS2813, for WS2813 BI pin of first LED to GND 
#define PIN_P1         A0    // switch player 1 to PIN A0 and GND
#define PIN_P2         A1    // switch player 2 to PIN A1 and GND 
#define PIN_P3         A2    // switch player 3 to PIN A2 and GND
#define PIN_P4         A3    // switch player 4 to PIN A3 and GND 
#define PIN_AUDIO     10    // BUZZER
#define PIN_LED_R     13    // Led Red
#define PIN_LED_O     12    // Led Orange
#define PIN_LED_G     11    // Led Green
#define PIN_LED_B      6    // Led Blue


int NPIXELS=MAXLED; // leds on track

#define COLOR1    track.Color(255,0,0)
#define COLOR2    track.Color(0,255,0)
#define COLOR3    track.Color(255,0,255)
#define COLOR4    track.Color(0,0,255)
int win_music[] = {
  2637, 2637, 0, 2637, 
  0, 2093, 2637, 0,
  3136    
};
      
byte  gravity_map[MAXLED];     

int TBEEP=3; 

float speed1=0;
float speed2=0;
float speed3=0;
float speed4=0;
float dist1=0;
float dist2=0;
float dist3=0;
float dist4=0;

byte loop1=0;
byte loop2=0;
byte loop3=0;
byte loop4=0;

byte leader=0;
byte loop_max=3; //total laps race


float ACEL=0.2;
float kf=0.015; //friction constant
float kg=0.003; //gravity constant

byte flag_sw1=0;
byte flag_sw2=0;
byte flag_sw3=0;
byte flag_sw4=0;
byte draworder=0;
 
unsigned long timestamp=0;

Adafruit_NeoPixel track = Adafruit_NeoPixel(MAXLED, PIN_LED, NEO_GRB + NEO_KHZ800);

int tdelay = 5; 

void set_ramp(byte H,byte a,byte b,byte c)
{for(int i=0;i<(b-a);i++){gravity_map[a+i]=127-i*((float)H/(b-a));};
 gravity_map[b]=127; 
 for(int i=0;i<(c-b);i++){gravity_map[b+i+1]=127+H-i*((float)H/(c-b));};
}

void set_loop(byte H,byte a,byte b,byte c)
{for(int i=0;i<(b-a);i++){gravity_map[a+i]=127-i*((float)H/(b-a));};
 gravity_map[b]=255; 
 for(int i=0;i<(c-b);i++){gravity_map[b+i+1]=127+H-i*((float)H/(c-b));};
}


void setup() {
  for(int i=0;i<NPIXELS;i++){gravity_map[i]=127;};
  track.begin(); 
  pinMode(PIN_P1,INPUT); //Choose INPUT_PULLUP or INPUT. Using EchidnaShield: INPUT
  pinMode(PIN_P2,INPUT);
  pinMode(PIN_P3,INPUT);
  pinMode(PIN_P4,INPUT);
  pinMode(PIN_LED_R,OUTPUT);
  pinMode(PIN_LED_O,OUTPUT);      
  pinMode(PIN_LED_G,OUTPUT);
  pinMode(PIN_LED_B,OUTPUT);

  if ((digitalRead(PIN_P1)==0)) //push switch 1 on reset for activate physic
  {
    set_ramp(12,90,100,110);    // ramp centred in LED 100 with 10 led fordward and 10 backguard 
    for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,0,(127-gravity_map[i])/8) );};
    digitalWrite (PIN_LED_B, HIGH);
    track.show();
  };
  start_race();    
}

void start_race(){for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,0,0));};
                  digitalWrite(PIN_LED_O,LOW);
                  track.show();
                  delay(2000);
                  track.setPixelColor(12, track.Color(255,0,0));
                  track.setPixelColor(11, track.Color(255,0,0));
                  track.show();
                  digitalWrite(PIN_LED_R,HIGH);
                  tone(PIN_AUDIO,400);
                  delay(1000);
                  digitalWrite(PIN_LED_R,LOW);
                  delay(1000);
                  noTone(PIN_AUDIO);                  
                  track.setPixelColor(12, track.Color(0,0,0));
                  track.setPixelColor(11, track.Color(0,0,0));
                  track.setPixelColor(10, track.Color(255,200,0));
                  track.setPixelColor(9, track.Color(255,200,0));
                  track.show();
                  digitalWrite(PIN_LED_O,HIGH);
                  tone(PIN_AUDIO,600);
                  delay(1000);
                  digitalWrite(PIN_LED_O,LOW);
                  delay(1200);
                  digitalWrite(PIN_LED_G,HIGH);
                  noTone(PIN_AUDIO);                  
                  track.setPixelColor(9, track.Color(0,0,0));
                  track.setPixelColor(10, track.Color(0,0,0));
                  track.setPixelColor(8, track.Color(0,255,0));
                  track.setPixelColor(7, track.Color(0,255,0));
                  track.show();
                  tone(PIN_AUDIO,1200);
                  delay(2000);
                  digitalWrite(PIN_LED_G,LOW);
                  digitalWrite(PIN_LED_B,LOW);
                  analogWrite(PIN_LED_G,16);
                  noTone(PIN_AUDIO);                               
                  timestamp=0;              
                 };

void winner_fx() {
               int msize = sizeof(win_music) / sizeof(int);
               for (int note = 0; note < msize; note++) {
               tone(PIN_AUDIO, win_music[note],200);
               delay(230);
               noTone(PIN_AUDIO);

}

                                               
              };

void burning1(){
//to do
 }

void burning2(){
//to do
 }

void track_rain_fx(){
//to do
 }

void track_oil_fx(){
//to do
 }

void track_snow_fx(){
//to do
 }


void fuel_empty(){
//to do
 }

void fill_fuel_fx(){
//to do
 }

void in_track_boxs_fx(){
//to do
 }

void pause_track_boxs_fx(){
//to do
 }
 
void flag_boxs_stop(){
//to do
 }

void flag_boxs_ready(){
//to do
 }

void draw_safety_car(){
//to do
 }

void telemetry_rx(){
  //to do
 }
 
void telemetry_tx(){
  //to do
 }

void telemetry_lap_time_car1(){
//to do
 }

void telemetry_lap_time_car2(){
//to do
 }

void telemetry_record_lap(){
//to do
 }

void telemetry_total_time(){
//to do
 }

int read_sensor(byte player){
//to do
}

int calibration_sensor(byte player){
  //to do  
}

int display_lcd_laps(){
  //to do  
}

int display_lcd_time(){
  //to do  
}



void draw_car1(void){for(int i=0;i<=loop1;i++){track.setPixelColor(((word)dist1 % NPIXELS)+i, track.Color(0,255-i*20,0));};                   
  }

void draw_car2(void){for(int i=0;i<=loop2;i++){track.setPixelColor(((word)dist2 % NPIXELS)+i, track.Color(255-i*20,0,0));};            
 }

void draw_car3(void){for(int i=0;i<=loop3;i++){track.setPixelColor(((word)dist3 % NPIXELS)+i, track.Color(255-i*20,0,255-i*20));};                   
  }

void draw_car4(void){for(int i=0;i<=loop4;i++){track.setPixelColor(((word)dist4 % NPIXELS)+i, track.Color(0,0,255-i*20));};            
 }
  
void loop() {
    //for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,0,0));};
    for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,0,(127-gravity_map[i])/8) );};
    
    if ( (flag_sw1==1) && (digitalRead(PIN_P1)==1) ) {flag_sw1=0;speed1+=ACEL;}; //0
    if ( (flag_sw1==0) && (digitalRead(PIN_P1)==0) ) {flag_sw1=1;}; //1

    if ((gravity_map[(word)dist1 % NPIXELS])<127) speed1-=kg*(127-(gravity_map[(word)dist1 % NPIXELS]));
    if ((gravity_map[(word)dist1 % NPIXELS])>127) speed1+=kg*((gravity_map[(word)dist1 % NPIXELS])-127);
    
    
    speed1-=speed1*kf; 
    
    if ( (flag_sw2==1) && (digitalRead(PIN_P2)==1) ) {flag_sw2=0;speed2+=ACEL;};
    if ( (flag_sw2==0) && (digitalRead(PIN_P2)==0) ) {flag_sw2=1;};

    if ((gravity_map[(word)dist2 % NPIXELS])<127) speed2-=kg*(127-(gravity_map[(word)dist2 % NPIXELS]));
    if ((gravity_map[(word)dist2 % NPIXELS])>127) speed2+=kg*((gravity_map[(word)dist2 % NPIXELS])-127);
        
    speed2-=speed2*kf; 

    if ( (flag_sw3==1) && (digitalRead(PIN_P3)==1) ) {flag_sw3=0;speed3+=ACEL;};
    if ( (flag_sw3==0) && (digitalRead(PIN_P3)==0) ) {flag_sw3=1;};

    if ((gravity_map[(word)dist3 % NPIXELS])<127) speed3-=kg*(127-(gravity_map[(word)dist3 % NPIXELS]));
    if ((gravity_map[(word)dist3 % NPIXELS])>127) speed3+=kg*((gravity_map[(word)dist3 % NPIXELS])-127);
        
    speed3-=speed3*kf; 

    if ( (flag_sw4==1) && (digitalRead(PIN_P4)==1) ) {flag_sw4=0;speed4+=ACEL;};
    if ( (flag_sw4==0) && (digitalRead(PIN_P4)==0) ) {flag_sw4=1;};

    if ((gravity_map[(word)dist4 % NPIXELS])<127) speed4-=kg*(127-(gravity_map[(word)dist4 % NPIXELS]));
    if ((gravity_map[(word)dist4 % NPIXELS])>127) speed4+=kg*((gravity_map[(word)dist4 % NPIXELS])-127);
        
    speed4-=speed4*kf; 
        
    dist1+=speed1;
    dist2+=speed2;
    dist3+=speed3;
    dist4+=speed4;

    if (dist1>dist2 and dist1>dist3 and dist1>dist4) {leader=1;} 
    if (dist2>dist1 and dist2>dist3 and dist2>dist4) {leader=2;}
    if (dist3>dist1 and dist3>dist2 and dist3>dist4) {leader=3;}
    if (dist4>dist1 and dist4>dist2 and dist4>dist3) {leader=4;} ;
      
    if (dist1>NPIXELS*loop1) {loop1++;tone(PIN_AUDIO,600);TBEEP=2;};
    if (dist2>NPIXELS*loop2) {loop2++;tone(PIN_AUDIO,700);TBEEP=2;};
    if (dist3>NPIXELS*loop3) {loop3++;tone(PIN_AUDIO,600);TBEEP=2;};
    if (dist4>NPIXELS*loop4) {loop4++;tone(PIN_AUDIO,700);TBEEP=2;};

    if (loop1>loop_max) {for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,16,0));}; track.show();
                                                    winner_fx();loop1=0;loop2=0;loop3=0;loop4=0;dist1=0;dist2=0;dist3=0;dist4=0;speed1=0;speed2=0;speed3=0;speed4=0;timestamp=0;
                                                     digitalWrite(PIN_LED_G,LOW);
                                                    start_race();
                                                   }
    if (loop2>loop_max) {for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(16,0,0));}; track.show(); //255 -> 128
                                                    winner_fx();loop1=0;loop2=0;loop3=0;loop4=0;dist1=0;dist2=0;dist3=0;dist4=0;speed1=0;speed2=0;speed3=0;speed4=0;timestamp=0;
                                                     digitalWrite(PIN_LED_G,LOW);
                                                    start_race();
                                                   }

    if (loop3>loop_max) {for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(16,0,16));}; track.show();
                                                    winner_fx();loop1=0;loop2=0;loop3=0;loop4=0;dist1=0;dist2=0;dist3=0;dist4=0;speed1=0;speed2=0;speed3=0;speed4=0;timestamp=0;
                                                     digitalWrite(PIN_LED_G,LOW);
                                                    start_race();
                                                   }
    if (loop4>loop_max) {for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,0,16));}; track.show(); //255 -> 128
                                                   winner_fx();loop1=0;loop2=0;loop3=0;loop4=0;dist1=0;dist2=0;dist3=0;dist4=0;speed1=0;speed2=0;speed3=0;speed4=0;timestamp=0;
                                                     digitalWrite(PIN_LED_G,LOW);
                                                    start_race();
                                                   }
                                                  
    if ((millis() & 512)==(512*draworder)) {if (draworder==0) {draworder=1;}
                          else {draworder=0;}   
                         }; 

    if (draworder==0) {draw_car1();draw_car2();draw_car3();draw_car4();}
        else {draw_car4();draw_car3();draw_car2();draw_car1();}   
                 
    track.show(); 
    delay(tdelay);
    
    if (TBEEP>0) {TBEEP-=1; 
                  if (TBEEP==0) {noTone(PIN_AUDIO);}; // lib conflict !!!! interruption off by neopixel
                 };   
}
