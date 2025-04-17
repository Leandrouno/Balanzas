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

// Pines
#define DT 3
#define SCK 2
#define PIN_TARA 4
#define PIN_SET10 5

// Objetos
HX711 balanza;
LiquidCrystal_I2C lcd(0x27, 20, 4);

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
  lcd.setCursor(2, 1);
  lcd.print("Balanza Contadora");
  delay(2000);
  lcd.clear();

  // Balanza
  balanza.begin(DT, SCK);
  balanza.set_scale(); // Ajustar si tenés factor calibrado
  balanza.tare();      // Pone en cero

  // Botones
  pinMode(PIN_TARA, INPUT_PULLUP);
  pinMode(PIN_SET10, INPUT_PULLUP);
}

void loop() {
  // Leer peso promedio
  pesoActual = balanza.get_units(15); // promedio de 15 lecturas

  // Tara
  if (digitalRead(PIN_TARA) == LOW) {
    balanza.tare();
    lcd.clear();
    lcd.setCursor(2, 1);
    lcd.print("Tara realizada");
    delay(1000);
  }

  // Seteo de 10 unidades
  if (digitalRead(PIN_SET10) == LOW) {
    peso10 = pesoActual;
    if (peso10 > 0.1) {
      pesoPromedioUnidad = peso10 / 10.0;
    }
    lcd.clear();
    lcd.setCursor(2, 1);
    lcd.print("Set: 10 unidades");
    delay(1000);
  }

  // Cálculo de objetos
  if (pesoPromedioUnidad > 0.1) {
    cantidadObjetos = round(pesoActual / pesoPromedioUnidad);
  } else {
    cantidadObjetos = 0;
  }

  // Mostrar en LCD
  lcd.setCursor(0, 0);
  lcd.print("Peso: ");
  lcd.print(pesoActual, 2);
  lcd.print(" g     ");

  lcd.setCursor(0, 1);
  lcd.print("Cant: ");
  lcd.print(cantidadObjetos);
  lcd.print(" objetos     ");

  lcd.setCursor(0, 2);
  lcd.print("Set: ");
  lcd.print(peso10, 2);
  lcd.print(" g     ");

  lcd.setCursor(0, 3);
  lcd.print("Prom: ");
  lcd.print(pesoPromedioUnidad, 2);
  lcd.print(" g     ");

  delay(250);
}
