# Smart Bus Stop IoT 

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![NodeMCU](https://img.shields.io/badge/ESP8266-000000?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino IDE](https://img.shields.io/badge/Arduino_IDE-00979D?style=for-the-badge&logo=arduino&logoColor=white)

Un proyecto IoT para controlar y registrar el acceso a paraderos de transporte público utilizando un NodeMCU ESP8266. El sistema lee tarjetas RFID/NFC, verifica la autorización mediante un código duro (hardcoded), y acciona un servomotor para permitir el paso. Todos los accesos, tanto autorizados como denegados, se registran con su respectiva marca de tiempo obtenida vía NTP y se envían como un payload JSON al dashboard de Blynk y a la base de datos backend.

## Hardware
- NodeMCU ESP8266
- Lector RFID MFRC522 (NFC)
- Servomotor de 180 grados
- Fuente de alimentación externa (5.1V)

## Pinout y Conexiones

### ESP8266 a MFRC522 (Protocolo SPI)
| ESP8266 Pin | MFRC522 Pin | Función |
| :--- | :--- | :--- |
| **D8** | `SDA` (SS) | Selección de esclavo SPI |
| **D5** | `SCK` | Reloj SPI |
| **D7** | `MOSI` | Salida de datos del maestro, entrada al esclavo |
| **D6** | `MISO` | Entrada de datos al maestro, salida del esclavo |
| **D3** | `RST` | Reset |
| **3.3V** | `3.3V` | Alimentación del lector (estrictamente 3.3V) |
| **GND** | `GND` | Tierra común |

### ESP8266 a Servomotor
| ESP8266 Pin | Servomotor | Función |
| :--- | :--- | :--- |
| **D2** | Señal (Naranja/Blanco) | Señal PWM para controlar el ángulo |
| **N/A** | VCC (Rojo) | Alimentación del motor (directo desde el riel de 5.1V de la protoboard) |
| **GND** | GND (Café/Negro) | Tierra común compartida en la protoboard |

*Nota sobre alimentación:* El servomotor se alimenta directamente desde el riel de 5.1V de la protoboard. A su vez, el pin VIN del ESP8266 recibe 5.1V desde la protoboard, lo que permite que el regulador interno del NodeMCU alimente el lector NFC a través de su pin de 3.3V.

### Pines Virtuales de Blynk
| Pin Virtual | Widget de App | Función |
| :--- | :--- | :--- |
| **V2** | Terminal | Recibe el string de estado inicial y los payloads JSON completos de los accesos (Autorizados y Denegados) |

## ¿Cómo funciona la lógica?
El sistema evalúa el control de acceso de la siguiente manera:
1. **Detección:** El MFRC522 escanea una tarjeta, extrae su array de bytes y lo convierte en un string hexadecimal en mayúsculas (ej. " 04 4B C9 42 F6 22 90").
2. **Evaluación de Seguridad:** El script compara este string contra 3 identificadores preautorizados (`Id_1`, `Id_2`, `Id_3`).
3. **Registro (Timestamp):** El microcontrolador consulta un servidor NTP (`pool.ntp.org` en huso horario CST) para obtener el epoch actual y formatearlo como `AAAA-MM-DD HH:MM:SS`.
4. **Acción Autorizada:** Si el ID coincide, se genera un JSON con estado "Autorizado", se acciona el servomotor a 180° forzando un rango PWM de 500 a 2500 microsegundos para abrir la puerta, y tras un timer no bloqueante de 3 segundos, el motor regresa a 0°.
5. **Acción Denegada:** Si el ID no coincide, la puerta se mantiene bloqueada, pero se construye un JSON con estado "Denegado" para registrar el intento fallido de vulnerar el acceso.
6. **Transmisión:** En ambos casos, el payload JSON se sube a Blynk vía el Pin Virtual `V2`, el cual está diseñado para ser interceptado por la base de datos MySQL (tabla `registros_acceso` mapeada a `nodo_id = 1`).

## Notas para Configuración
1. Sustituye las credenciales de Wi-Fi (`ssid` y `password`) y los datos de Blynk (`BLYNK_TEMPLATE_ID`, `BLYNK_TEMPLATE_NAME`, `BLYNK_AUTH_TOKEN`) en el código.
2. Modifica los strings `Id_1`, `Id_2`, `Id_3` con los bytes hexadecimales de las tarjetas que deseas autorizar, conservando el espacio en blanco al inicio.
3. Si el servomotor vibra o zumba al estar en 0 o 180 grados, ajusta ligeramente los límites de microsegundos en la instrucción `servo.attach(Servo_PIN, 500, 2500);` (ej. 520, 2450).
