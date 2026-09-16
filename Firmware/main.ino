#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

// --- DEFINICIJA PINOVA ---
#define PUMPA_PIN    PB5
#define BUZZER_PIN   PB0 

// Ultrazvucni senzor
#define TRIG_PIN     PA0 
#define ECHO_PIN     PA1  

// OLED Displej
#define OLED_SDA     PB7
#define OLED_SCL     PB6
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// TASTATURA 4x4
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {PB12, PB13, PB14, PB15}; 
byte colPins[COLS] = {PB8,  PB9,  PB3,  PB4}; 

Keypad tastatura = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// KALIBRACIJA PUMPE
const float MS_PO_ML = 35.0; 

String unosMilli = "";
unsigned long zadnjeMerenje = 0;
unsigned long zadnjiBip = 0;
int maticnaDistanca = 0;

void bip(int trajanjeMs = 30) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(trajanjeMs);
  digitalWrite(BUZZER_PIN, LOW);
}

int izmeriDistancu() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long trajanje = pulseIn(ECHO_PIN, HIGH, 15000);
  if (trajanje == 0) return 99; 
  
  return trajanje * 0.034 / 2;
}

void osveziDisplej(int distanca, String poruka = "") {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  if (poruka.length() > 0) {
    display.setTextSize(2);
    display.setCursor(0, 8);
    display.print(poruka);
    display.display();
    return;
  }

  display.setTextSize(1);
  display.setCursor(0, 0);
  
  if (distanca > 0 && distanca <= 12) {
    display.print("CASA: PRISUTNA");
  } else {
    display.print("CASA: NEMA!");
  }

  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  display.setCursor(0, 16);
  display.print("Unos: ");
  display.setTextSize(2);
  display.setCursor(45, 14);
  display.print(unosMilli);
  
  if (unosMilli.length() > 0) {
    display.setTextSize(1);
    display.print(" ml");
  }

  display.display();
}

void pokreniSipanje(int ml) {
  unsigned long vremeRada = ml * MS_PO_ML;

  // Slanje poruke na CH340 preko Serial1 (PA9/PA10)
  Serial1.println("dozator_marko/status/pumpa:UKLJUCENA");

  bip(120); 
  osveziDisplej(maticnaDistanca, "SIPAM...");

  digitalWrite(PUMPA_PIN, HIGH);
  delay(vremeRada);
  digitalWrite(PUMPA_PIN, LOW);

  Serial1.println("dozator_marko/status/pumpa:ISKLJUCENA");
  Serial1.print("dozator_marko/status/sipano:");
  Serial1.println(ml);

  bip(80); delay(80); bip(80);

  osveziDisplej(maticnaDistanca, "ZAVRSENO!");
  delay(1500);
}

void setup() {
  pinMode(PUMPA_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(PUMPA_PIN, LOW); 
  digitalWrite(BUZZER_PIN, LOW);

  Serial1.begin(9600);

  tastatura.setDebounceTime(60);

  Wire.setSDA(OLED_SDA);
  Wire.setSCL(OLED_SCL);
  Wire.begin();

  if(display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.println("Sistem spreman!");
    display.display();
    delay(1000);
  }
}

void loop() {
  char taster = tastatura.getKey();
  
  if (taster && tastatura.getState() == PRESSED) {
    bip(40); 

    if (taster >= '0' && taster <= '9') {
      if (unosMilli.length() < 4) unosMilli += taster;
    } 
    else if (taster == '*') {
      if (unosMilli.length() > 0) unosMilli.remove(unosMilli.length() - 1);
    }
    else if (taster == '#') {
      int mililitara = unosMilli.toInt();

      if (mililitara <= 0) {
        osveziDisplej(maticnaDistanca, "UNESI ML!");
        bip(200);
        delay(1000);
      } 
      else if (maticnaDistanca <= 0 || maticnaDistanca > 12) {
        osveziDisplej(maticnaDistanca, "STAVI CASU!");
        bip(300);
        delay(1200);
      } 
      else {
        pokreniSipanje(mililitara);
        unosMilli = ""; 
      }
    }
  }

  if (millis() - zadnjeMerenje > 150) { 
    zadnjeMerenje = millis();
    maticnaDistanca = izmeriDistancu();
    osveziDisplej(maticnaDistanca);

    if (maticnaDistanca > 0 && maticnaDistanca <= 15) {
      int pauzaIzmedjuBipova = maticnaDistanca * 35; 

      if (millis() - zadnjiBip > pauzaIzmedjuBipova) {
        zadnjiBip = millis();
        bip(15); 
      }
    }
  }
}
