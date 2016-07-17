
#include <VirtualWire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define NB_TEMP_SENSOR 3
String tempLabel[] = { "Chbre 1", "Chbre 2", "Salon" };

typedef struct {
  String type;
  int numero;
  int value;
} SensorData;

typedef struct {
  float temperature;
  float humidity;
} SensorValue;

SensorValue lastValues[] = { { -1, -1}, { -1, -1}, { -1, -1} };


byte deg[8] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
};

void setup()
{
  Serial.begin(9600);

  lcd.begin(16, 2);

  lcd.createChar(0, deg);
  lcd.print("Waiting data...");

  vw_set_rx_pin(8);
  vw_setup(2000);

  vw_rx_start();
}



SensorData decodeMessage(uint8_t* buf, uint8_t buflen) {
  SensorData sensorData = {"", 0, -1};
  sensorData.type = "";
  int index = 0;
  while (index < buflen && ((char)(buf[index])) != ' ') {
    sensorData.type += (char)(buf[index]);
    index++;
  }

  String numero = "";
  while (index < buflen && ((char)(buf[index])) != ' ') {
    numero += (char)(buf[index]);
    index++;
  }
  sensorData.numero = numero.toInt();

  String value = "";
  while (index < buflen && ((char)(buf[index])) != ' ') {
    value += (char)(buf[index]);
    index++;
  }
  sensorData.value = value.toInt();

  return sensorData;
}

void sensorManagement() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {

    digitalWrite(13, true); // Flash a light to show received good message
    // Message with a good checksum received, dump it.
    SensorData sensorData = decodeMessage(buf, buflen);
    if (sensorData.type == "temp") {
      Serial.print("Temp ");
      Serial.print(sensorData.numero);
      Serial.print(" : ");
      Serial.print(((float) sensorData.value) / 100);
      Serial.println(" *C");
      lastValues[sensorData.numero - 1].temperature = ((float) sensorData.value) / 100;
    } else if (sensorData.type == "humi") {
      Serial.print("Humi ");
      Serial.print(sensorData.numero);
      Serial.print(" : ");
      Serial.print(((float) sensorData.value) / 100);
      Serial.println(" %");
      lastValues[sensorData.numero - 1].humidity = ((float) sensorData.value) / 100;
    } else {
      Serial.print("Unknown sensor type : ");
      Serial.print(sensorData.type);
    }

    digitalWrite(13, false);
  }
}

int currentIndex = 0;
long lastLcdTime = -999999;

void lcdDisplay() {
  if (millis() - lastLcdTime > 5000) {
    lastLcdTime = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    String toWrite = tempLabel[currentIndex] + " : " + lastValues[currentIndex].temperature;
    lcd.print(toWrite);
    lcd.setCursor(toWrite.length(), 0);
    lcd.write(byte(0));

    currentIndex++;
    if (currentIndex >= NB_TEMP_SENSOR) {
      currentIndex = 0;
    }

    if (NB_TEMP_SENSOR > 1) {
      lcd.setCursor(0, 1);
      toWrite = tempLabel[currentIndex] + " : " + lastValues[currentIndex].temperature;
      lcd.print(toWrite);
      lcd.setCursor(toWrite.length(), 1);
      lcd.write(byte(0));

    }
  }
}

void loop()
{
  sensorManagement();
  lcdDisplay();
}
