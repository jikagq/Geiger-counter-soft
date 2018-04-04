
const int ledPin =  LED_BUILTIN;
const int pulse = 2;
int ledState = LOW;           

unsigned long previousMillis = 0;        
const long interval = 60000;           

int count=0;

void setup() {
  Serial.begin(9600);
  Serial.println("echo");
  pinMode(ledPin, OUTPUT);

  pinMode(pulse, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pulse), countparticle, RISING);

  inipwm();
}

void loop() {

  //rien on compte pendant 1min les particules
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {//1min ecoulée

    int cpm = count;
    count=0;//raz du compteur pour la prochaine minute de comptage
    Serial.print("cpm: ");
    Serial.println(cpm);
    
    cpmtosiever(cpm);//converion cpm en siever

    //cli led
    Serial.print("temp: ");
    Serial.println(currentMillis);
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }
}

void inipwm(){
  pinMode (3, OUTPUT) ;
  TCCR2A = 0x23 ;
  TCCR2B = 0x09 ; // mode 7, clock prescale by 1
  OCR2A = 160-1 ;  // 160 clock periods = 10us per cycle
  OCR2B =0 ;
  TCNT2 =0 ;
  OCR2B = 80;//75//60
}

void cpmtosiever(int cpm){

  
}

void countparticle(){//compte le nombre de particule
  count++;
}

