/*

 * Raspberry Pi Pico 2W <-> ESP32-S3 WireClaw

 *

 * Sensors:

 *   AHT20  : Temperature + Humidity

 *   HW-611 : Pressure

 *

 * I2C:

 *   SDA = GP20

 *   SCL = GP21

 *

 * UART0:

 *   GP0 = TX

 *   GP1 = RX

 *

 * ESP32-S3:

 *   GPIO19 = RX

 *   GPIO20 = TX

 *

 * UART:

 *   9600 baud

 *

 * Sensor data:

 *   10 samples averaged

 *

 * Outputs:

 *   LED       : LED_BUILTIN

 *   GPIO22    : Digital output controlled by ESP32

 *

 * Commands from ESP32:

 *   PING

 *   STATUS

 *   READ

 *   LED ON

 *   LED OFF

 *   GPIO22 ON

 *   GPIO22 OFF

 */


#include <Arduino.h>

#include <Wire.h>

#include <Adafruit_AHTX0.h>

#include <Adafruit_BMP280.h>


// ============================================================

// Pin configuration

// ============================================================


#define I2C_SDA 20

#define I2C_SCL 21


// UART0

#define UART_TX 0

#define UART_RX 1


// Onboard LED

#define LED_PIN LED_BUILTIN


// External digital output

#define GPIO22_PIN 22


// ============================================================

// Sampling configuration

// ============================================================


#define AVERAGE_SAMPLES 10

#define SAMPLE_INTERVAL_MS 200


// ============================================================

// Sensors

// ============================================================


Adafruit_AHTX0 aht;

Adafruit_BMP280 bmp;


// ============================================================

// Sensor status

// ============================================================


bool ahtOK = false;

bool bmpOK = false;

bool sensorError = false;


// ============================================================

// Output status

// ============================================================


bool ledState = false;

bool gpio22State = false;


// ============================================================

// Last averaged sensor values

// ============================================================


float lastAHTTemp = NAN;

float lastHumidity = NAN;

float lastPressure = NAN;


// ============================================================

// Send current sensor JSON

// ============================================================


void sendSensorJSON()

{

    String json = "{";


    // Device information

    json += "\"device\":\"Pico2W\",";


    // Overall status

    json += "\"status\":\"";

    json += sensorError ? "ERROR" : "OK";

    json += "\",";


    // AHT20 temperature

    json += "\"aht_temp\":";


    if (!isnan(lastAHTTemp))

        json += String(lastAHTTemp, 2);

    else

        json += "null";


    // Humidity

    json += ",\"humidity\":";


    if (!isnan(lastHumidity))

        json += String(lastHumidity, 2);

    else

        json += "null";


    // Pressure

    json += ",\"pressure\":";


    if (!isnan(lastPressure))

        json += String(lastPressure, 2);

    else

        json += "null";


    // Sensor status

    json += ",\"aht_status\":\"";

    json += ahtOK ? "OK" : "ERROR";

    json += "\"";


    json += ",\"bmp_status\":\"";

    json += bmpOK ? "OK" : "ERROR";

    json += "\"";


    // Sampling information

    json += ",\"sample_count\":";

    json += String(AVERAGE_SAMPLES);


    // Pico uptime

    json += ",\"uptime_ms\":";

    json += String(millis());


    // Output states

    json += ",\"led\":";

    json += ledState ? "true" : "false";


    json += ",\"gpio22\":";

    json += gpio22State ? "true" : "false";


    json += "}";


    // --------------------------------------------------------

    // Send exactly ONE JSON packet to ESP32

    // --------------------------------------------------------


    Serial1.println(json);


    // --------------------------------------------------------

    // USB serial debug

    // --------------------------------------------------------


    Serial.print("UART -> ESP32: ");

    Serial.println(json);


    Serial.print("JSON length: ");

    Serial.print(json.length());

    Serial.println(" bytes");

}


// ============================================================

// Process commands from ESP32

// ============================================================


void processCommand(String command)

{

    command.trim();


    if (command.length() == 0)

        return;


    Serial.print("Command received: ");

    Serial.println(command);


    // ----------------------------------------------------------

    // PING

    // ----------------------------------------------------------


    if (command.equalsIgnoreCase("PING"))

    {

        Serial1.println("PONG");


        Serial.println("Response: PONG");

    }


    // ----------------------------------------------------------

    // STATUS

    // ----------------------------------------------------------


    else if (command.equalsIgnoreCase("STATUS"))

    {

        String status = "AHT=";


        status += (ahtOK ? "OK" : "ERROR");


        status += ",BMP=";


        status += (bmpOK ? "OK" : "ERROR");


        status += ",LED=";


        status += (ledState ? "ON" : "OFF");


        status += ",GPIO22=";


        status += (gpio22State ? "ON" : "OFF");


        Serial1.println(status);


        Serial.print("Response: ");

        Serial.println(status);

    }


    // ----------------------------------------------------------

    // READ

    // ----------------------------------------------------------


    else if (command.equalsIgnoreCase("READ"))

    {

        /*

         * Send exactly one JSON packet.

         *

         * JSON is NOT sent automatically anymore.

         */

        sendSensorJSON();

    }


    // ----------------------------------------------------------

    // LED ON

    // ----------------------------------------------------------


    else if (command.equalsIgnoreCase("LED ON"))

    {

        digitalWrite(LED_PIN, HIGH);


        ledState = true;


        Serial1.println("LED=ON");


        Serial.println("Response: LED=ON");

    }


    // ----------------------------------------------------------

    // LED OFF

    // ----------------------------------------------------------


    else if (command.equalsIgnoreCase("LED OFF"))

    {

        digitalWrite(LED_PIN, LOW);


        ledState = false;


        Serial1.println("LED=OFF");


        Serial.println("Response: LED=OFF");

    }


    // ----------------------------------------------------------

    // GPIO22 ON

    // ----------------------------------------------------------


    else if (command.equalsIgnoreCase("GPIO22 ON"))

    {

        digitalWrite(GPIO22_PIN, HIGH);


        gpio22State = true;


        Serial1.println("GPIO22=ON");


        Serial.println("Response: GPIO22=ON");

    }


    // ----------------------------------------------------------

    // GPIO22 OFF

    // ----------------------------------------------------------


    else if (command.equalsIgnoreCase("GPIO22 OFF"))

    {

        digitalWrite(GPIO22_PIN, LOW);


        gpio22State = false;


        Serial1.println("GPIO22=OFF");


        Serial.println("Response: GPIO22=OFF");

    }


    // ----------------------------------------------------------

    // Unknown command

    // ----------------------------------------------------------


    else

    {

        Serial1.println("ERROR=UNKNOWN_COMMAND");


        Serial.print("Response: ERROR=UNKNOWN_COMMAND");

    }

}


// ============================================================

// Check UART for incoming commands

// ============================================================


void checkUARTCommand()

{

    static String command = "";


    while (Serial1.available())

    {

        char c = Serial1.read();


        if (c == '\n' || c == '\r')

        {

            if (command.length() > 0)

            {

                processCommand(command);


                command = "";

            }

        }

        else

        {

            command += c;


            // Safety limit for commands

            if (command.length() > 100)

            {

                command = "";


                Serial1.println("ERROR=COMMAND_TOO_LONG");


                Serial.println("Response: ERROR=COMMAND_TOO_LONG");

            }

        }

    }

}


// ============================================================

// Setup

// ============================================================


void setup()

{

    // ----------------------------------------------------------

    // Outputs

    // ----------------------------------------------------------


    pinMode(LED_PIN, OUTPUT);


    // Start with LED OFF

    digitalWrite(LED_PIN, LOW);


    ledState = false;


    pinMode(GPIO22_PIN, OUTPUT);


    // Start GPIO22 LOW

    digitalWrite(GPIO22_PIN, LOW);


    gpio22State = false;


    // ----------------------------------------------------------

    // USB Serial

    // ----------------------------------------------------------


    Serial.begin(115200);


    delay(1000);


    Serial.println();

    Serial.println("======================================");

    Serial.println("Pico 2W WireClaw Sensor Node");

    Serial.println("Two-Way UART Mode");

    Serial.println("REQUEST/RESPONSE JSON MODE");

    Serial.println("======================================");


    // ----------------------------------------------------------

    // I2C

    // ----------------------------------------------------------


    Wire.setSDA(I2C_SDA);

    Wire.setSCL(I2C_SCL);

    Wire.begin();


    Serial.println("I2C started");

    Serial.println("SDA = GP20");

    Serial.println("SCL = GP21");


    // ----------------------------------------------------------

    // AHT20

    // ----------------------------------------------------------


    if (aht.begin(&Wire))

    {

        ahtOK = true;


        Serial.println("AHT20 detected");

    }

    else

    {

        ahtOK = false;


        Serial.println("ERROR: AHT20 not detected");

    }


    // ----------------------------------------------------------

    // BMP280 / HW-611

    // ----------------------------------------------------------


    if (bmp.begin(0x76))

    {

        bmpOK = true;


        Serial.println("HW-611/BMP280 detected at 0x76");

    }

    else if (bmp.begin(0x77))

    {

        bmpOK = true;


        Serial.println("HW-611/BMP280 detected at 0x77");

    }

    else

    {

        bmpOK = false;


        Serial.println("ERROR: HW-611/BMP280 not detected");

    }


    // ----------------------------------------------------------

    // UART0

    // ----------------------------------------------------------


    Serial1.setTX(UART_TX);

    Serial1.setRX(UART_RX);

    Serial1.begin(9600);


    Serial.println("UART0 started");

    Serial.println("TX = GP0");

    Serial.println("RX = GP1");

    Serial.println("Baud = 9600");


    // ----------------------------------------------------------

    // GPIO status

    // ----------------------------------------------------------


    Serial.println("--------------------------------------");


    Serial.print("LED initial state   : ");

    Serial.println(ledState ? "ON" : "OFF");


    Serial.print("GPIO22 initial state: ");

    Serial.println(gpio22State ? "ON" : "OFF");


    Serial.println("--------------------------------------");


    // ----------------------------------------------------------

    // Sensor status

    // ----------------------------------------------------------


    if (ahtOK && bmpOK)

    {

        sensorError = false;


        Serial.println("All sensors ready.");

    }

    else

    {

        sensorError = true;


        Serial.println("WARNING: Sensor error detected.");

    }


    Serial.println("--------------------------------------");

}


// ============================================================

// Main loop

// ============================================================


void loop()

{

    // Always listen for ESP32 commands

    checkUARTCommand();


    // ----------------------------------------------------------

    // Accumulators

    // ----------------------------------------------------------


    float sumAHTTemp = 0.0;

    float sumHumidity = 0.0;

    float sumPressure = 0.0;


    int validAHTSamples = 0;

    int validPressureSamples = 0;


    // ----------------------------------------------------------

    // Take 10 samples

    // ----------------------------------------------------------


    for (int i = 0; i < AVERAGE_SAMPLES; i++)

    {

        // Check for commands during sampling

        checkUARTCommand();


        // --------------------------------------------------------

        // AHT20

        // --------------------------------------------------------


        if (ahtOK)

        {

            sensors_event_t humidity;

            sensors_event_t temp;


            aht.getEvent(&humidity, &temp);


            if (!isnan(temp.temperature) &&

                !isnan(humidity.relative_humidity))

            {

                sumAHTTemp += temp.temperature;

                sumHumidity += humidity.relative_humidity;


                validAHTSamples++;

            }

        }


        // --------------------------------------------------------

        // Pressure

        // --------------------------------------------------------


        if (bmpOK)

        {

            float pressure =

                bmp.readPressure() / 100.0F;


            if (!isnan(pressure))

            {

                sumPressure += pressure;


                validPressureSamples++;

            }

        }


        // --------------------------------------------------------

        // Check UART periodically

        // --------------------------------------------------------


        for (int j = 0; j < 20; j++)

        {

            checkUARTCommand();


            delay(SAMPLE_INTERVAL_MS / 20);

        }

    }


    // ----------------------------------------------------------

    // Calculate averages

    // ----------------------------------------------------------


    lastAHTTemp = NAN;

    lastHumidity = NAN;

    lastPressure = NAN;


    if (validAHTSamples > 0)

    {

        lastAHTTemp =

            sumAHTTemp / validAHTSamples;


        lastHumidity =

            sumHumidity / validAHTSamples;

    }


    if (validPressureSamples > 0)

    {

        lastPressure =

            sumPressure / validPressureSamples;

    }


    // ----------------------------------------------------------

    // Error detection

    // ----------------------------------------------------------


    sensorError = false;


    if (isnan(lastAHTTemp) ||

        isnan(lastHumidity) ||

        isnan(lastPressure))

    {

        sensorError = true;

    }


    // ----------------------------------------------------------

    // Display on USB Serial

    // ----------------------------------------------------------


    Serial.println();

    Serial.println("----- 10 Sample Average -----");


    if (!isnan(lastAHTTemp))

    {

        Serial.print("AHT20 Temperature : ");

        Serial.print(lastAHTTemp, 2);

        Serial.println(" °C");

    }

    else

    {

        Serial.println("AHT20 Temperature : ERROR");

    }


    if (!isnan(lastHumidity))

    {

        Serial.print("AHT20 Humidity    : ");

        Serial.print(lastHumidity, 2);

        Serial.println(" %");

    }

    else

    {

        Serial.println("AHT20 Humidity    : ERROR");

    }


    if (!isnan(lastPressure))

    {

        Serial.print("HW-611 Pressure   : ");

        Serial.print(lastPressure, 2);

        Serial.println(" hPa");

    }

    else

    {

        Serial.println("HW-611 Pressure   : ERROR");

    }


    Serial.print("LED               : ");

    Serial.println(ledState ? "ON" : "OFF");


    Serial.print("GPIO22            : ");

    Serial.println(gpio22State ? "ON" : "OFF");


    // IMPORTANT:

    // There is intentionally NO sendSensorJSON() here.

    //

    // JSON is sent ONLY when ESP32 sends:

    //

    //     READ

    //

    // This prevents continuous UART traffic.

}
