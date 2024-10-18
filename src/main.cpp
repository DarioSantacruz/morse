#include <Arduino.h>
#include <LiquidCrystal.h>
#include <avr/wdt.h>

#define LCDWIDTH 16
#define LCDROWS 2
#define RS 12
#define EN 11
#define D4 5
#define D5 4
#define D6 3
#define D7 2

#define BUZZER 8

const int buzzer_speed = 800;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

const char *morseAlfabeto[] = {
  ".-",    // A
  "-...",  // B
  "-.-.",  // C
  "-..",   // D
  ".",     // E
  "..-.",  // F
  "--.",   // G
  "....",  // H
  "..",    // I
  ".---",  // J
  "-.-",   // K
  ".-..",  // L
  "--",    // M
  "-.",    // N
  "---",   // O
  ".--.",  // P
  "--.-",  // Q
  ".-.",   // R
  "...",   // S
  "-",     // T
  "..-",   // U
  "...-",  // V
  ".--",   // W
  "-..-",  // X
  "-.--",  // Y
  "--..",  // Z
  ".----", // 1
  "..---", // 2
  "...--", // 3
  "....-", // 4
  ".....", // 5
  "-....", // 6
  "--...", // 7
  "---..", // 8
  "----.", // 9
  "-----"  // 0
};

String convertirAMorse(String entrada);
String convertirAMorseLCD(String entrada);
String obtenerCharMorse(char c);
String LCDCharsMorse(char c);
String eliminarEspacios(String cadenaEntrada);
void buzz(String entrada);
void buzzChar(char charActual);
void desplazarMensajeEnLCD(String mensaje, int repeticiones);
void reinicioSoftware();
String obtenerMapeoMorse(char c, bool incluirChar);

void setup() {
  pinMode(BUZZER, OUTPUT);
  lcd.begin(LCDWIDTH, LCDROWS);
  lcd.clear();

  Serial.begin(9600);

  for (int i = 0; i < 5; i++) {
    Serial.println();
  }
  Serial.println("Iniciando...");
  Serial.println("Mensaje a codificar en Morse:");
}

void loop() {
  if (Serial.available() > 0) {
  String codigoMorseLCD;
  String mensajeActual;
  String codigoMorse;

  mensajeActual = Serial.readStringUntil('\n');
  mensajeActual.trim();
  mensajeActual.toUpperCase();
  Serial.println(mensajeActual);

  codigoMorse = convertirAMorse(mensajeActual);

  Serial.println("Código Morse: ");
  Serial.println(codigoMorse);
  Serial.println();

  Serial.println("Código morse para pantalla: ");
  codigoMorseLCD = convertirAMorseLCD(mensajeActual);
  Serial.println(codigoMorseLCD);

  Serial.println("Desplegando mensaje en LCD y convirtiéndolo en tonos: ");
  desplazarMensajeEnLCD(eliminarEspacios(codigoMorseLCD), 2);

  Serial.println("\n");
  delay(5000);

  reinicioSoftware();
  }
}

String eliminarEspacios(String cadenaEntrada) {
  String resultado = "";
  for (unsigned int i = 0; i < cadenaEntrada.length(); i++) {
  if (cadenaEntrada[i] != ' ') {
    resultado += cadenaEntrada[i];
  }
  }
  return resultado;
}

void buzzChar(char charActual) {
  if (charActual == '.') {
  digitalWrite(BUZZER, HIGH);
  delay(buzzer_speed / 10);
  digitalWrite(BUZZER, LOW);
  delay(buzzer_speed / 10);
  } else if (charActual == '-') {
  digitalWrite(BUZZER, HIGH);
  delay(buzzer_speed / 3);
  digitalWrite(BUZZER, LOW);
  delay(buzzer_speed / 10);
  } else if (charActual == '/') {
  digitalWrite(BUZZER, LOW);
  delay(buzzer_speed);
  delay(buzzer_speed / 3);
  } else {
  digitalWrite(BUZZER, LOW);
  delay(buzzer_speed / 3);
  }
}

void buzz(String entrada) {
  for (unsigned int i = 0; i < entrada.length(); i++) {
  buzzChar(entrada[i]);
  }
}

String obtenerMapeoMorse(char c, bool incluirChar) {
  if (c >= 'A' && c <= 'Z') {
  return (incluirChar ? String(c) : "") + String(morseAlfabeto[c - 'A']) + " ";
  } else if (c >= '0' && c <= '9') {
  return (incluirChar ? String(c) : "") + String(morseAlfabeto[c - '0' + 26]) + " ";
  } else if (c == ' ') {
  return " / ";
  }
  return "";
}

String obtenerCharMorse(char c) {
  return obtenerMapeoMorse(c, false);
}

String LCDCharsMorse(char c) {
  return obtenerMapeoMorse(c, true);
}

String convertirAMorse(String entrada) {
  String codigoMorse = "";
  entrada.toUpperCase();
  for (unsigned int i = 0; i < entrada.length(); i++) {
  char c = entrada.charAt(i);
  codigoMorse += obtenerCharMorse(c);
  }

  return codigoMorse;
}

String convertirAMorseLCD(String entrada) {
  String codigoMorse = "";
  entrada.toUpperCase();

  for (int i = 0; i < entrada.length(); i++) {
  char c = entrada.charAt(i);
  codigoMorse += LCDCharsMorse(c);
  }

  return codigoMorse;
}

void desplazarMensajeEnLCD(String mensaje, int repeticiones) {
  int longitudMensaje = mensaje.length();
  int indiceChar = 0;
  int linea = 0;

  for (int repeticion = 0; repeticion < repeticiones; repeticion++) {
  lcd.clear();
  indiceChar = 0;
  linea = 0;

  while (indiceChar < longitudMensaje) {
    lcd.setCursor(indiceChar % LCDWIDTH, linea);
    lcd.print(mensaje[indiceChar]);
    buzzChar(mensaje[indiceChar]);
    indiceChar++;

    if (indiceChar % LCDWIDTH == 0) {
    linea = (linea + 1) % LCDROWS;
    if (linea == 0) {
      lcd.clear();
    }
    }
  }

  delay(5000);
  lcd.clear();
  }
}

void reinicioSoftware() {
  wdt_enable(WDTO_15MS);

  while (true) {
  }
}

