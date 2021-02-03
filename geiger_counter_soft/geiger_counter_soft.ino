#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define CONV_FACTOR 0.0117
#define HWver 0.2
#define SWver 0.1

const int ledPin =  LED_BUILTIN;
const int pulse = 2;
const int ledPulse = 3;
const int buzzer = 4;
         
int ledState = LOW;  

unsigned long previousMillis = 0;        
const long interval = 60000; //60000          

volatile int count=0;
volatile int bip = 0;

float moy=0;
int cpt=0;


extern const unsigned char nuclearBitmap [] PROGMEM;
extern const unsigned char hazardeousBitmap [] PROGMEM;
extern const unsigned char radresBitmap [] PROGMEM;
extern const unsigned char invertedNuclearBitmap [] PROGMEM;

void setup() {
  Serial.begin(9600);
  Serial.println("Geiger Counter");
  Serial.println("HW ver " +String(HWver));
  Serial.println("SW ver " +String(SWver));
  
  pinMode(ledPulse, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  pinMode(ledPin, OUTPUT);
  pinMode(pulse, INPUT);
  
  digitalWrite(ledPulse, LOW );
  digitalWrite(buzzer, LOW);

  attachInterrupt(digitalPinToInterrupt(pulse), countparticle, FALLING);

  //ssd1306 command

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  display.drawPixel(0, 0, SSD1306_WHITE);
  display.display();
  display.clearDisplay();

  //nuclear icon
  /*display.drawBitmap(112,0,nuclearBitmap,16,8,WHITE);
  display.display();*/

  //display.drawBitmap(32,0,radresBitmap,48,24,WHITE);
  //display.display();

  
  //affichage nom + version + logo
  //hazardeous splash
  display.fillRect(96, 0, 32, 32, WHITE);
  display.fillRect(0, 0, 32, 32, WHITE);
  display.drawBitmap(32,0,hazardeousBitmap,64,32,WHITE);
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  //nom + version
  display.setTextSize(2); 
  display.setTextColor(WHITE);
  display.setCursor(0,0);                  // On va écrire en x=0, y=0
  display.println("RAD meter");
  display.setTextSize(1); 
  display.println("HW v0.2");
  display.println("SW v0.1");
  
  //logo nuclear inverted
  display.drawBitmap(100,20,invertedNuclearBitmap,16,8,WHITE);
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  //default interface
  display.drawBitmap(112,0,nuclearBitmap,16,8,WHITE);
  display.drawBitmap(80,8,radresBitmap,48,24,WHITE);
  display.fillRect(0, 0, 112, 8, WHITE);
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0,0); 
  display.println(" RAD meter   <1 CPM");
  display.setTextColor(WHITE);
  display.println("CPMm: " + String(0));
  display.println("uSv/h: " + String(0));    
  display.println("Min: " + String(0));
  display.display();
}

void loop() {

 if(bip == 1){
    digitalWrite(ledPulse, HIGH );
    digitalWrite(buzzer, HIGH);
    delay(1);
    digitalWrite(ledPulse, LOW );
    digitalWrite(buzzer, LOW);
    bip = 0;
 }
 
 
 unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {//1min ecoulée

    int cpm = count;
    count=0;//raz du compteur pour la prochaine minute de comptage
    Serial.print("cpm instantané: ");
    Serial.println(cpm);

    Serial.print("cpm moyen: ");
    cpt++;
    moy=(cpm+moy)/cpt;
    Serial.println(moy);
    
    Serial.print("Sievert: ");
    float siever = cpmtosiever(cpm); //uSv/h
    Serial.println(siever);//converion cpm en siever

    //cli led
    Serial.print("temps: ");
    Serial.print(currentMillis/60000);
    Serial.println(" min");
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
    updateDisplay(cpm, moy,siever, currentMillis/60000);
  }
}

void inipwm(){
  pinMode (3, OUTPUT) ;
  TCCR2A = 0x23 ;
  TCCR2B = 0x09 ; // mode 7, clock prescale by 1
  OCR2A = 160-1 ;  // 160 clock periods = 10us per cycle
  OCR2B =0 ;
  TCNT2 =0 ;
  OCR2B = 60;//75//60
}

float cpmtosiever(int cpm){
  return cpm*CONV_FACTOR;
}

void updateDisplay(int cpm,float cpmmoy,float siever,int tps){

  display.clearDisplay();
  display.drawBitmap(112,0,nuclearBitmap,16,8,WHITE);
  display.drawBitmap(80,8,radresBitmap,48,24,WHITE);
  display.fillRect(0, 0, 112, 8, WHITE);
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(0,0); 
  display.println(" RAD meter "+String(cpm)+" CPM");
  display.setTextColor(WHITE);
  display.println("CPMm: " + String(cpmmoy));
  display.println("uSv/h: " + String(siever));
  display.println("Min: " + String(tps));    
  display.display();
  
}

void countparticle(){//compte le nombre de particule
  count++;
  Serial.println("+1 rad");
  bip = 1;
}

