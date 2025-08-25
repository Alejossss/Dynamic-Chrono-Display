#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>

// Configuración de la matriz LED
int pinCS = 15; // CS
int pinDIN = 23; // DIN a MOSI
int pinCLK = 18; // CLK a SCK
Max72xxPanel matrix = Max72xxPanel(pinCS, 1, 4);

// Configuración de sensores
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LDR_PIN 32  // Pin para el sensor KY-018 (sensor de luz)
#define BRIGHTNESS_LOW 1  // Brillo bajo para poca luz
#define BRIGHTNESS_HIGH 15 // Brillo alto para mucha luz

// Configuración WiFi y Telegram
const char* ssid = ""; // Nombre de la red WIFI
const char* password = ""; //Contraseña de la red WIFI
#define BOTtoken "" // Tu Bot Token (Obtener de Botfather)
#define CHAT_ID "" // Tu Chat ID
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Variables de estado
bool mostrarHora = true;
String mensajePersonalizado = "";
String zonaHoraria = "Bogota"; // Valor predeterminado

// Función para configurar la zona horaria usando TZ
void configurarZonaHoraria(String zona) {
  if (zona == "Bogota") setenv("TZ", "COT5", 1);           // America/Bogota
  else if (zona == "Los Angeles") setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);   // America/Los_Angeles
  else if (zona == "Lima") setenv("TZ", "PET5", 1);      // America/Lima
  else if (zona == "Buenos Aires") setenv("TZ", "ART3ARST,M10.1.0/0,M3.3.0/0", 1);  // Argentina/Buenos_Aires
  else if (zona == "Hong Kong") setenv("TZ", "HKT-8", 1);      // Asia/Hong_Kong
  else if (zona == "Melbourne") setenv("TZ", "EST-10EST,M10.1.0,M4.1.0/3", 1); // Australia/Melbourne
  else if (zona == "London") setenv("TZ", "GMT0BST,M3.5.0/1,M10.5.0", 1);    // Europe/London
  else if (zona == "Mexico") setenv("TZ", "CST6CDT,M4.1.0,M10.5.0", 1);   // Mexico
  tzset();
}

// Función para manejar mensajes nuevos de Telegram
void handleNewMessages() {
  String chat_id = String(bot.messages[0].chat_id);
  if (chat_id != CHAT_ID) {
    bot.sendMessage(chat_id, "Usuario no autorizado", "");
    return;
  }
  String text = bot.messages[0].text;
  
  if (text == "/hora") {
    bot.sendMessage(CHAT_ID, "\n¿Cuál zona horaria deseas?\nLista:\n- Bogota\n- Los Angeles\n- Lima\n- Buenos Aires\n- Hong Kong\n- Melbourne\n- London\n- Mexico\n", "");
  } else if (text.startsWith("zona ")) {
    zonaHoraria = text.substring(5);
    configurarZonaHoraria(zonaHoraria);
    bot.sendMessage(CHAT_ID, "Zona horaria configurada a " + zonaHoraria, "");
    mostrarHora = true;
  } else if (text.startsWith("/mensaje ")) {
    mostrarHora = false;
    mensajePersonalizado = text.substring(9);
    bot.sendMessage(CHAT_ID, "Mensaje personalizado configurado", "");
  } else if (text == "/clima") {
    mostrarClima();
    delay(3000); // Muestra el clima por 3 segundos
    mostrarHora = true; // Vuelve a mostrar la hora
  } else {
    bot.sendMessage(CHAT_ID, "Comandos válidos:\n/hora - Muestra la hora\n/mensaje <texto> - Muestra mensaje personalizado\n/clima - Muestra temperatura y humedad", "");
  }
}


// Función para mostrar la hora en la matriz LED
void mostrarHoraEnMatrix() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    matrix.setCursor(0, 0);
    matrix.print("Error");
    return;
  }
  
  char buffer[10];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo); // Formato de 24 horas
  matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  matrix.print(buffer);
  matrix.write();
}

// Función para mostrar mensaje personalizado en la matriz LED
void mostrarMensaje() {
  int spacer = 1;
  int width = 6 + spacer; // Ancho de cada carácter más un espacio adicional
  for (int i = 0; i < width * mensajePersonalizado.length() + matrix.width(); i++) {
    matrix.fillScreen(0);

    int letter = i / width;
    int x = matrix.width() - (i % width);  // Desplazamiento horizontal en el lado largo
    int y = 0; // Centrado en el lado corto: comenzamos desde la primera fila (y=0)

    while (x + width - spacer >= 0 && letter >= 0) {
      if (letter < mensajePersonalizado.length()) {
        matrix.drawChar(x, y, mensajePersonalizado[letter], 1, 0, 1);
      }
      letter--;
      x -= width;
    }
    matrix.write();
    delay(100); // Control de velocidad de desplazamiento
  }
}


// Función para mostrar la temperatura y humedad del sensor DHT11
void mostrarClima() {
  int temp = dht.readTemperature();
  int hum = dht.readHumidity();

  String climaMensaje = "T: " + String(temp) + "C H: " + String(hum) + "%";
  
  // Desplazar el mensaje para que sea legible en la matriz
  int spacer = 1;
  int width = 6 + spacer;
  for (int i = 0; i < width * climaMensaje.length() + matrix.width(); i++) {
    matrix.fillScreen(0);
    int letter = i / width;
    int x = matrix.width() - (i % width);
    int y = 0; // Centrado en el lado corto

    while (x + width - spacer >= 0 && letter >= 0) {
      if (letter < climaMensaje.length()) {
        matrix.drawChar(x, y, climaMensaje[letter], 1, 0, 1);
      }
      letter--;
      x -= width;
    }
    matrix.write();
    delay(100); // Control de velocidad de desplazamiento
  }
}


// Función para ajustar el nivel de luz con el sensor KY-018
void ajustarBrillo() {
  int sensorValue = analogRead(LDR_PIN);  // Lee el valor del sensor KY-018

  // Mapear el valor del sensor (0 a 4095) al rango de brillo (1 a 15)
  int brillo = map(sensorValue, 0, 4095, BRIGHTNESS_LOW, BRIGHTNESS_HIGH);  // 15 es el brillo máximo, 1 el mínimo
  brillo = constrain(brillo, 1, 15);  // Asegura que el valor esté dentro de los límites permitidos

  matrix.setIntensity(brillo);  // Aplica el brillo mapeado a la matriz
}

void setup() {
  matrix.fillScreen(0);

  // Configurar la rotación para que el texto se muestre horizontalmente
  matrix.setRotation(1);  // Rota 90 grados clockwise para que el texto use los 32 píxeles de ancho
  
  // Configurar la posición de cada módulo en una columna de 4 módulos
  matrix.setPosition(0, 0, 0);  // Primer módulo en la parte superior
  matrix.setPosition(1, 0, 1);  // Segundo módulo justo debajo del primero
  matrix.setPosition(2, 0, 2);  // Tercer módulo debajo del segundo
  matrix.setPosition(3, 0, 3);  // Cuarto módulo en la parte inferior
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  bot.sendMessage(CHAT_ID, "Bot iniciado", "");

  dht.begin();

  // Configurar NTP y zona horaria
  configTime(0, 0, "pool.ntp.org"); // Sin offset inicial, usará el de zonaHoraria
  configurarZonaHoraria(zonaHoraria);
}


void loop() {
  ajustarBrillo();  // Ajuste constante del brillo de la matriz basado en la luz ambiente
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  if (numNewMessages) handleNewMessages();
  
  if (mostrarHora) {
    mostrarHoraEnMatrix();
  } else {
    mostrarMensaje();
  }
  
  delay(1000);
}