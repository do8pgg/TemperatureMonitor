#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "credentials.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <UniversalTelegramBot.h>

#define ONE_WIRE_BUS D7

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
LiquidCrystal_I2C lcd(0x27, 20, 4);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

float tempe;
int counter = 0;              // Fuer zaehler
int pause = 1;                // Pause in Minuten nach Alarm
int schlafen = pause * 60000; // 'Umrechnung' Pause nach Millisekunden
int minmax = 35;              // Grenztemperatur ab der gewarnt wird
int Bot_mtbs = 1000;          // Bot-Voodoo
long Bot_lasttime;            // Bot-Voodoo
bool Start = false;           // Bot-Voodoo?

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/temp") {
      DS18B20.requestTemperatures();
      tempe = DS18B20.getTempCByIndex(0);
      String temptext = "Temperatur: ";
      temptext += String(tempe,0);
      temptext += "° C\n";
      temptext += "Grenzwert: ";
      temptext += String(minmax);
      temptext += "° C\n";
      bot.sendMessage(chat_id, temptext);
    }

    if (text.startsWith("/setwert")) {
      minmax = text.substring(9).toInt();
      Serial.println("minmax: " + String(minmax));
      bot.sendMessage(chat_id, "Wert wurde gesetzt: " + String(minmax));
    }

if (text == "/setwarn") {
      bot.sendMessage(chat_id, "Bitte gib einen neuen Warnwert ein:");
      while (text == "/setwarn") {
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        text = bot.messages[i].text;
        delay(500);
      }
      minmax = text.toInt();
      bot.sendMessage(chat_id, "Warntemperatur wurde geändert.");
    }
  }
}

void botlooper() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}

void AlarmDisplayAusgabe() {
            lcd.setCursor(3, 0);                        // Display-Ausgabe
            lcd.clear();
            lcd.print("!!! ALARM !!!");
            lcd.setCursor(0, 1);
            lcd.print("Temperatur: ");
            lcd.setCursor(13, 1);
            lcd.print(tempe,0);
            lcd.setCursor(0, 3);
            lcd.print("Warnungen:");
            lcd.setCursor(13, 3);
            lcd.print(counter);
}

void loopertemp() {
  DS18B20.requestTemperatures();
  tempe = DS18B20.getTempCByIndex(0);
  if (tempe < minmax) {
    counter++;                                          // counter +1
    String warntext = "!!!!! ALAAAAAARM !!!!!\n\n";     // Telegram-Nachricht zusammenstellen
    warntext += "Heizungstemperatur auf ";              // Telegram-Nachricht zusammenstellen
    warntext += String(tempe,0);                        // Telegram-Nachricht zusammenstellen
    warntext += "°C \n";                                // Telegram-Nachricht zusammenstellen
    warntext += "Ggf. wieder ausgefallen *Seufz* \n\n"; // Telegram-Nachricht zusammenstellen
    warntext += "Dies ist Warnung #";                   // Telegram-Nachricht zusammenstellen
    warntext += String(counter);                        // Telegram-Nachricht zusammenstellen
    warntext += "\n";                                   // Telegram-Nachricht zusammenstellen
          if(counter <= 3) {                            // Wenn counter < 3 dann Nachricht senden
            bot.sendMessage(chatid, warntext);          // Telegram-Nachricht absenden
            AlarmDisplayAusgabe();
          }
          else {                                        // Ab hier dann nur noch Display-Ausgabe
            AlarmDisplayAusgabe();          
               }
    delay(schlafen);
  }

  if (tempe > minmax) {
    if (counter >= 1) {                                 // Wenn Counter > oder = 1 ist, Telegram-Nachricht, das alles wieder gut ist
      String entwarntext = "Entwarnung,"; 
      entwarntext += "Temperatur wieder über ";
      entwarntext += minmax;
      entwarntext += "°C gestiegen!\n";
      entwarntext += "Zähler wird auf ";
      entwarntext += "wieder auf 0 gesetzt\n";
      counter = 0;
        bot.sendMessage(chatid, entwarntext);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.setCursor(0, 1);
          lcd.print("Temperatur: ");
          lcd.setCursor(18, 1);
          lcd.print(tempe,0);
    }
      botlooper();
          lcd.setCursor(0, 1);
          lcd.print("Temperatur:");
          lcd.setCursor(16, 1);
          lcd.print(tempe,0);
          lcd.setCursor(18, 1); // Gradzeichen
          lcd.print((char)223); // Gradzeichen
          lcd.setCursor(19, 1); // Gradzeichen
          lcd.print("C");       // Gradzeichen
          lcd.setCursor(0,3);
          lcd.print("Warntemperatur:");
          lcd.setCursor(16,3);
          lcd.print(minmax);
          lcd.setCursor(18, 3);
          lcd.print((char)223);
          lcd.setCursor(19, 3);
          lcd.print("C");
           ArduinoOTA.handle();
      delay(1000);
  }
}

void loopertemptest() {
  /* Nur hier spielen, nicht im anderen,
 * solange es halbwegs laeuft!
 */

  DS18B20.requestTemperatures();
  tempe = DS18B20.getTempCByIndex(0);
  if (tempe < minmax) {
    counter++;                                          // counter +1
    String warntext = "!!!!! ALAAAAAARM !!!!!\n\n";     // Telegram-Nachricht zusammenstellen
    warntext += "Heizungstemperatur auf ";              // Telegram-Nachricht zusammenstellen
    warntext += String(tempe,0);                        // Telegram-Nachricht zusammenstellen
    warntext += "°C \n";                                // Telegram-Nachricht zusammenstellen
    warntext += "Ggf. wieder ausgefallen *Seufz* \n\n"; // Telegram-Nachricht zusammenstellen
    warntext += "Dies ist Warnung #";                   // Telegram-Nachricht zusammenstellen
    warntext += String(counter);                        // Telegram-Nachricht zusammenstellen
    warntext += "\n";                                   // Telegram-Nachricht zusammenstellen
          if(counter <= 3) {                            // Wenn counter < 3 dann Nachricht senden
            bot.sendMessage(chatid, warntext);          // Telegram-Nachricht absenden
            AlarmDisplayAusgabe();
          }
          else {                                        // Ab hier dann nur noch Display-Ausgabe
            AlarmDisplayAusgabe();          
               }
    delay(schlafen);
  }

  if (tempe > minmax) {
    if (counter >= 1) {                                 // Wenn Counter > oder = 1 ist, Telegram-Nachricht, das alles wieder gut ist
      String entwarntext = "Entwarnung,"; 
      entwarntext += "Temperatur wieder über ";
      entwarntext += minmax;
      entwarntext += "°C gestiegen!\n";
      entwarntext += "Zähler wird auf ";
      entwarntext += "wieder auf 0 gesetzt\n";
      counter = 0;
        bot.sendMessage(chatid, entwarntext);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.setCursor(0, 1);
          lcd.print("Temperatur: ");
          lcd.setCursor(18, 1);
          lcd.print(tempe,0);
    }
      botlooper();
          lcd.setCursor(0, 1);
          lcd.print("Temperatur:");
          lcd.setCursor(16, 1);
          lcd.print(tempe,0);
          lcd.setCursor(18, 1); // Gradzeichen
          lcd.print((char)223); // Gradzeichen
          lcd.setCursor(19, 1); // Gradzeichen
          lcd.print("C");       // Gradzeichen
          lcd.setCursor(0,3);
          lcd.print("Warntemperatur:");
          lcd.setCursor(16,3);
          lcd.print(minmax);
          lcd.setCursor(18, 3);
          lcd.print((char)223);
          lcd.setCursor(19, 3);
          lcd.print("C");
           ArduinoOTA.handle();
      delay(1000);
  }
}

void loop() {
  loopertemp();
  //loopertemptest();
}

void OTA(){
  ArduinoOTA.setHostname("Heizungswaechter");
  //      10.0.0.244
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("OTA-Update!");
      lcd.setCursor(0, 2);
      lcd.print("Bitte warten!");


    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void lcdsetup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  }

void wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println(WiFi.localIP());
  WiFi.hostname("Heizungswaechter");
  client.setInsecure();
}

void setup() {
  delay(250);
  Serial.begin(115200);
  wifi();
  lcdsetup();
  OTA();
}
