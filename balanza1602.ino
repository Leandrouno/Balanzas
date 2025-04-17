/*=====================================
  SISTEMAS LEANDROUNO
  ANTE CUALQUIER DUDA COMUNIQUESE :
  WHATSAPP 1155016504
  SISTEMAS@LEANDROUNO.COM.AR
  PROHIBIDA SU VENTA TOTAL O PARCIAL 
  SIN CONSENTIMIENTO DEL PROGRAMADOR
 ======================================*/
#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pines HX711
#define DT 3
#define SCK 2

// Pines de botones
#define PIN_TARA 4
#define PIN_SET10 5

// Objetos
HX711 balanza;
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 1602

// Variables
float pesoActual = 0.0;
float peso10 = 0.0;
float pesoPromedioUnidad = 0.0;
int cantidadObjetos = 0;

void setup() {
  Serial.begin(9600);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Iniciando...");
  delay(1500);
  lcd.clear();

  // Balanza
  balanza.begin(DT, SCK);
  balanza.set_scale(); // calibrar si es necesario
  balanza.tare();

  // Botones
  pinMode(PIN_TARA, INPUT_PULLUP);
  pinMode(PIN_SET10, INPUT_PULLUP);
}

void loop() {
  // Leer peso promedio
  pesoActual = balanza.get_units(15);

  // Tara
  if (digitalRead(PIN_TARA) == LOW) {
    balanza.tare();
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("Tara OK");
    delay(1000);
  }

  // Set 10 unidades
  if (digitalRead(PIN_SET10) == LOW) {
    peso10 = pesoActual;
    if (peso10 > 0.1) {
      pesoPromedioUnidad = peso10 / 10.0;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("10 unidades set");
    delay(1000);
  }

  // Calcular cantidad
  if (pesoPromedioUnidad > 0.1) {
    cantidadObjetos = round(pesoActual / pesoPromedioUnidad);
  } else {
    cantidadObjetos = 0;
  }

  // Mostrar en LCD
  lcd.setCursor(0, 0);
  lcd.print("Peso: ");
  lcd.print(pesoActual, 2);
  lcd.print("g  "); // extra espacios para limpiar residuos

  lcd.setCursor(0, 1);
  lcd.print("Cant: ");
  lcd.print(cantidadObjetos);
  lcd.print("       "); // limpia residuos

  delay(250);
}
