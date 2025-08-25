# ⏱️ Dynamic Chrono Display

**Dynamic Chrono Display** es un sistema de visualización digital basado en una **matriz LED MAX7219** controlada por un **ESP32**, diseñado para mostrar en tiempo real información como hora, temperatura, humedad y mensajes personalizados.  

El proyecto integra sensores de luz y ambiente que permiten ajustar automáticamente el brillo de la matriz y desplegar datos ambientales. La comunicación se realiza desde un **móvil vía conexión inalámbrica**, asegurando flexibilidad y facilidad de uso.  

---

## 🎯 Objetivo del Proyecto
Desarrollar un sistema interactivo y llamativo que combine **electrónica digital, programación y comunicación inalámbrica** para mostrar datos relevantes en distintos contextos.  

---

## 🚀 Características
- 📡 **Comunicación inalámbrica** entre ESP32 y dispositivo móvil.  
- 🔆 **Ajuste automático de brillo** mediante fotoresistor KY-018.  
- 🌡️ **Lectura de temperatura y humedad** con sensor DHT11.  
- ⌚ **Visualización de hora y zona horaria**.  
- 📝 **Mensajes personalizados** enviados desde el dispositivo móvil.  

---

## 🛠️ Tecnologías y Hardware
- **ESP32** (microcontrolador con Wi-Fi y Bluetooth).  
- **Matriz LED MAX7219** para visualización de caracteres alfanuméricos.  
- **Sensor KY-018** (fotoresistor).  
- **Sensor DHT11** (temperatura y humedad).  
- **Arduino IDE** como entorno de desarrollo.  

---

## ⚙️ Instalación y Uso
1. Clona este repositorio:
   ```bash
   git clone https://github.com/tu-usuario/Dynamic-Chrono-Display.git
   ```
2. Abre el archivo `Proyecto_DCD.ino` en Arduino IDE.
3. Instala las librerías necesarias desde el gestor de librerías:
   - `Adafruit GFX`
   - `DHT sensor library`
   - `LedControl` (para matriz LED)
   - `Universal Telegram Bot`
   - `NTPClient`

4. Configura las credenciales en el código:
   - 🔑 **Wi-Fi**: SSID y contraseña de tu red.  
   - 🤖 **Bot de Telegram**: token y chat ID.  
   - 📌 **Sensores y pines**: define los pines GPIO para la matriz LED, DHT11 y KY-018 según tu conexión.  
5. Selecciona la placa ESP32 y el puerto correspondiente.
6. Carga el código en el microcontrolador.
7. Usa telegram para enviar mensajes o visualizar datos en la matriz.

