/*=====================================
  SISTEMAS LEANDROUNO
  ANTE CUALQUIER DUDA COMUNIQUESE :
  WHATSAPP 1155016504
  SISTEMAS@LEANDROUNO.COM.AR
  WWW.LEANDROUNO.COM
  PROHIBIDA SU VENTA TOTAL O PARCIAL 
  SIN CONSENTIMIENTO DEL PROGRAMADOR
 ======================================*/
#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pines HX711
#define DT A1
#define SCK A0

// Pines de botones
#define PIN_TARA 3
#define PIN_SET10 2

// Objetos
HX711 balanza;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Cambiar 0x27 si es necesario

// Variables de peso
float pesoActual = 0.0;
float pesoFiltrado = 0.0;
float peso10 = 0.0;
float pesoUnidad = 0.0;
int cantidadObjetos = 0;

// Promedios
#define NUM_PROMEDIOS 3
float bufferPesos[NUM_PROMEDIOS];
int indicePeso = 0;

// Botones
bool estadoAnteriorTara = HIGH;
bool estadoAnteriorSet10 = HIGH;
unsigned long ultimaLecturaTara = 0;
unsigned long ultimaLecturaSet10 = 0;
const unsigned long debounceDelay = 150;

// Refresco
unsigned long ultimaActualizacion = 0;
const unsigned long intervaloActualizacion = 250; // 500 ms

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Iniciando...");
  lcd.setCursor(3, 1);
  lcd.print("Espere...");
  delay(1500);
  lcd.clear();

  balanza.begin(DT, SCK);
  balanza.set_scale(414.844);
  balanza.tare();

  pinMode(PIN_TARA, INPUT_PULLUP);
  pinMode(PIN_SET10, INPUT_PULLUP);

  for (int i = 0; i < NUM_PROMEDIOS; i++) {
    bufferPesos[i] = 0.0;
  }
}

void loop() {
  unsigned long ahora = millis();

  // Leer peso (en gramos)
  float lectura = balanza.get_units(1);
  bufferPesos[indicePeso] = lectura;
  indicePeso = (indicePeso + 1) % NUM_PROMEDIOS;

  float suma = 0.0;
  for (int i = 0; i < NUM_PROMEDIOS; i++) {
    suma += bufferPesos[i];
  }
  pesoFiltrado = suma / NUM_PROMEDIOS;
  pesoActual = pesoFiltrado / 1000.0; // En kilos

  // Calcular cantidad
  if (pesoUnidad > 0.0001) {
    cantidadObjetos = round((pesoActual * 1000.0) / pesoUnidad);
  } else {
    cantidadObjetos = 0;
  }

  // Refrescar LCD y Serial cada cierto tiempo
  if (ahora - ultimaActualizacion > intervaloActualizacion) {
    // LCD
    lcd.setCursor(0, 0);
    lcd.print("Peso: ");
    lcd.print(pesoActual, 2);
    lcd.print(" kg   ");

    lcd.setCursor(0, 1);
    lcd.print("Cant: ");
    lcd.print(cantidadObjetos);
    lcd.print("     ");

    // Serial
    Serial.print("Peso (kg): ");
    Serial.print(pesoActual, 2);
    Serial.print(" | Cantidad: ");
    Serial.println(cantidadObjetos);

    ultimaActualizacion = ahora;
  }

  // Botón TARA
  bool estadoTara = digitalRead(PIN_TARA);
  if (estadoTara == LOW && estadoAnteriorTara == HIGH && (ahora - ultimaLecturaTara > debounceDelay)) {
    balanza.tare();
    for (int i = 0; i < NUM_PROMEDIOS; i++) bufferPesos[i] = 0.0;
    pesoUnidad = 0.0;
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Reseteado.");
    lcd.setCursor(3, 1);
    lcd.print("Espere...");
    delay(1000);
    lcd.clear();
    ultimaLecturaTara = ahora;
  }
  estadoAnteriorTara = estadoTara;

  // Botón SET 10 unidades
  bool estadoSet = digitalRead(PIN_SET10);
  if (estadoSet == LOW && estadoAnteriorSet10 == HIGH && (ahora - ultimaLecturaSet10 > debounceDelay)) {
    peso10 = pesoFiltrado;
    if (peso10 > 1.0) {
      pesoUnidad = peso10 / 10.0;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("10 unidades");
    lcd.setCursor(3, 1);
    lcd.print("seteadas");
    delay(1000);
    lcd.clear();
    ultimaLecturaSet10 = ahora;
  }
  estadoAnteriorSet10 = estadoSet;
}

