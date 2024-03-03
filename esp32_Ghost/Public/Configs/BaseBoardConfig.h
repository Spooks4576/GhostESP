#ifndef BASE_BOARD_CONFIG_H
#define BASE_BOARD_CONFIG_H

#include <Arduino.h>
#include <HTTPUpdate.h>
#include <Adafruit_NeoPixel.h>
#include <wifi.h>
#include "../../Public/Controllers/YoutubeController.h"
#include "../../Public/Features/Dial.h"

void retainLineExcludingKeywords(String &content, const String &keyword1, const String &keyword2) {
    int index1 = content.indexOf(keyword1);
    int index2 = content.indexOf(keyword2);
    
    // If neither keyword is found, return the content as-is
    if (index1 == -1 && index2 == -1) {
        return;
    }

    // Split the content into lines
    int startPos = 0;
    int endPos = content.indexOf('\n');
    while (endPos != -1) {
        String line = content.substring(startPos, endPos);
        if (line.indexOf(keyword1) == -1 && line.indexOf(keyword2) == -1 && line.length() > 0) { // Check for non-empty line
            content = line;
            return;
        }
        startPos = endPos + 1;
        endPos = content.indexOf('\n', startPos);
    }

    // Check the last line (if there's no newline at the end)
    if (startPos < content.length()) {
        String line = content.substring(startPos);
        if (line.indexOf(keyword1) == -1 && line.indexOf(keyword2) == -1 && line.length() > 0) { // Check for non-empty line
            content = line;
            return;
        }
    }

    content = "";  // If no suitable line is found, clear the content
}

struct BaseBoardConfig {
    int ledPin_B = -1;
    int ledPin_G = -1;
    int ledPin_R = -1;
    int bluetoothTxPin = -1;
    int bluetoothRxPin = -1;
    int neopixelPin = -1;
    int pixel_count = -1;
    int sdpin = -1;
    bool SupportsBluetooth = false;
    bool SupportsNeoPixel = false;
    Adafruit_NeoPixel strip;

    BaseBoardConfig() {

        if (SupportsNeoPixel && neopixelPin != -1 && pixel_count != -1)
        {
            Adafruit_NeoPixel instrip(pixel_count, neopixelPin, NEO_GRB + NEO_KHZ800);
            strip = instrip;
        }
    }

    virtual void init() {

        if (ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            pinMode(ledPin_B, OUTPUT);
            pinMode(ledPin_G, OUTPUT);
            pinMode(ledPin_R, OUTPUT);
        }

        if (sdpin != -1)
        {
            pinMode(sdpin, OUTPUT);
        }

        if (SupportsBluetooth && bluetoothTxPin != -1 && bluetoothRxPin != -1) {
            // TODO Add Bluetooth Initilization
        }

        
        Serial.println("ESP-IDF version is: " + String(esp_get_idf_version()));
        Serial.println(F("Welcome to Ghost ESP Made by Spooky"));
    }

    virtual void blinkLed() {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, LOW);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, LOW);
            digitalWrite(ledPin_R, HIGH);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, LOW);
            delay(700);
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, HIGH);
        }
        else 
        {
            if (SupportsNeoPixel && neopixelPin != -1) {
                // Run the same color setup pattern for neopixel
                strip.setPixelColor(0, strip.Color(255, 0, 0));
                strip.show();
                delay(700);
                strip.setPixelColor(0, strip.Color(0, 255, 0));
                strip.show();
                delay(700);
                strip.setPixelColor(0, strip.Color(0, 0, 255));
                strip.show();
                delay(700);
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
            }
        }
    }

    virtual void TurnoffLed()
    {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            digitalWrite(ledPin_B, HIGH);
            digitalWrite(ledPin_G, HIGH);
            digitalWrite(ledPin_R, HIGH);
        }
        else 
        {
            if (SupportsNeoPixel && neopixelPin != -1) {
                // Turn off the neopixel LED
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
            }
        }
    }

    virtual void setLedColor(int r, int g, int b) {
        if (!SupportsNeoPixel && ledPin_B != -1 && ledPin_G != -1 && ledPin_R != -1) {
            analogWrite(ledPin_B, r);
            analogWrite(ledPin_G, g);
            analogWrite(ledPin_R, b);
        }
        else 
        {
            if (SupportsNeoPixel && neopixelPin != -1) {
                strip.setPixelColor(0, strip.Color(r, g, b));
                strip.show();
            }
        }
    }

    String readSerialBuffer(bool &isHtml, bool &isAp) {
        String buffer = "";
        buffer = Serial.readStringUntil('\r\n');

        isHtml = buffer.startsWith("sethtml=");
        isAp = buffer.startsWith("setap=");

        return buffer;
    }

    virtual void Loop()
    {
        if (Serial.available() > 0) {
            String flipperMessage;
            bool StartsWithHTML;
            bool StartsWithAP;
            flipperMessage = readSerialBuffer(StartsWithHTML, StartsWithAP);

            Serial.println(flipperMessage.c_str());

            if (flipperMessage.startsWith("settings"))
            {
                retainLineExcludingKeywords(flipperMessage, "settings -s EnableLED enable", "settings -s SavePCAP enable");
            } // Hack for Marauder app. until i can produce my own flipper app

            Serial.println(flipperMessage.c_str());

            if (flipperMessage.startsWith("YTDialConnect")) {
                flipperMessage.remove(0, 14); // Remove "YTDialConnect" from the message
    
                Serial.println("Debug: " + flipperMessage); // Debugging line to check the message

                int firstSpace = flipperMessage.indexOf(' ');
                int secondSpace = flipperMessage.indexOf(' ', firstSpace + 1);

                // Debugging lines to check the positions of spaces
                Serial.println("First space at: " + String(firstSpace));
                Serial.println("Second space at: " + String(secondSpace));

                if (firstSpace != -1 && secondSpace != -1) {
                    String YTURL = flipperMessage.substring(0, firstSpace);
                    String SSID = flipperMessage.substring(firstSpace + 1, secondSpace);
                    String Password = flipperMessage.substring(secondSpace + 1);

                    // Debugging lines to check the parsed parts
                    Serial.println("YTURL: " + YTURL);
                    Serial.println("SSID: " + SSID);
                    Serial.println("Password: " + Password);

                    YTURL.trim();
                    SSID.trim();
                    Password.trim();


                    YoutubeController* YtController = new YoutubeController();
                    DIALClient* dial = new DIALClient(YTURL.c_str(), SSID.c_str(), Password.c_str(), YtController);
                    setLedColor(0, 1, 0);


                    dial->Execute();
                    delete dial; // Clean up after execution
                    delete YtController;

                    TurnoffLed();
                } else {
                    // Handle error: message format incorrect
                    Serial.println("Error: Incorrect message format for YT Dial connection.");
                }
            }
            
            if (flipperMessage.startsWith("YTChromeConnect"))
            {
                flipperMessage.remove(0, 16); // Remove "YTChromeConnect" from the message + Space
    
                Serial.println("Debug: " + flipperMessage); // Debugging line to check the message

                int firstSpace = flipperMessage.indexOf(' ');
                int secondSpace = flipperMessage.indexOf(' ', firstSpace + 1);

                // Debugging lines to check the positions of spaces
                Serial.println("First space at: " + String(firstSpace));
                Serial.println("Second space at: " + String(secondSpace));

                if (firstSpace != -1 && secondSpace != -1) {
                    String YTURL = flipperMessage.substring(0, firstSpace);
                    String SSID = flipperMessage.substring(firstSpace + 1, secondSpace);
                    String Password = flipperMessage.substring(secondSpace + 1);

                    // Debugging lines to check the parsed parts
                    Serial.println("YTURL: " + YTURL);
                    Serial.println("SSID: " + SSID);
                    Serial.println("Password: " + Password);

                    YTURL.trim();
                    SSID.trim();
                    Password.trim();
                }
            }

            if (flipperMessage.startsWith("Update"))
            {
                flipperMessage.remove(0, 7);
    
                Serial.println("Debug: " + flipperMessage); // Debugging line to check the message

                int firstSpace = flipperMessage.indexOf(' ');
                int secondSpace = flipperMessage.indexOf(' ', firstSpace + 1);

                // Debugging lines to check the positions of spaces
                Serial.println("First space at: " + String(firstSpace));
                Serial.println("Second space at: " + String(secondSpace));

                if (firstSpace != -1 && secondSpace != -1) {
                    String SSID = flipperMessage.substring(0, firstSpace);
                    String Password = flipperMessage.substring(firstSpace + 1, secondSpace);

                    Serial.println("SSID: " + SSID);
                    Serial.println("Password: " + Password);

                    SSID.trim();
                    Password.trim();

                    WiFi.begin(SSID, Password);
                    while (WiFi.status() != WL_CONNECTED) {
                        delay(100);
                        Serial.print(".");
                    }
                    Serial.println("\nConnected to Wi-Fi");
                    Serial.println("\nSet Callback");

                    WiFiClient client;

                    t_httpUpdate_return ret = httpUpdate.update(client, "http://cdn.spookytools.com/assets/ghostesp.bin", "1.0.0", 0);

                    switch(ret) {
                        case HTTP_UPDATE_FAILED:
                            Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
                            break;

                        case HTTP_UPDATE_NO_UPDATES:
                            Serial.println("HTTP_UPDATE_NO_UPDATES");
                            break;

                        case HTTP_UPDATE_OK:
                            Serial.println("HTTP_UPDATE_OK");
                            break;
                    }
                }
            }
        }
    }
};

#endif