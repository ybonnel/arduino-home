

#include <VirtualWire.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

#define SENSOR_NUM 1
#define DELAY_BETWEEN_MESURE 30000


void setup()
{
  Serial.begin(9600); //Start the serial connection with the computer
  //to view the result open the serial monitor

  // Initialise the IO and ISR
  vw_setup(2000);   // Bits per sec

  dht.begin();
}


void sendMessageOverRF(String type, int numero, int value) {
  digitalWrite(13, HIGH);
  char msg[30];
  sprintf(msg, "temp %2d %d", numero, value);
  vw_send((uint8_t *)msg, strlen(msg));
  vw_wait_tx();
  digitalWrite(13, LOW);
}

void sendMessage(float humidity, float temp) {
  sendMessageOverRF("temp", SENSOR_NUM, (int)(temp * 100));
  sendMessageOverRF("humi", SENSOR_NUM, (int)(humidity * 100));
}

void loop() // run over and over again
{
  delay(DELAY_BETWEEN_MESURE);
  //getting the voltage reading from the temperature sensor
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity : ");
  Serial.print(h);
  Serial.print(" %\tTemperature : ");
  Serial.print(hic);
  Serial.println(" *C ");
  sendMessage(h, hic);

}
