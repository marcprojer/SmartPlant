// Pin Definitionen
const int moistureSensor1 = 32; // Feuchtigkeitssensor 1 an D32
const int moistureSensor2 = 33; // Feuchtigkeitssensor 2 an D33

void setup() {
  // Serielle Kommunikation starten
  Serial.begin(115200);
}

void loop() {
  // Feuchtigkeitssensoren auslesen
  int moistureValue1 = analogRead(moistureSensor1);
  int moistureValue2 = analogRead(moistureSensor2);

  // Werte auf der seriellen Konsole ausgeben
  Serial.print("Feuchtigkeitssensor 1: ");
  Serial.println(moistureValue1);
  Serial.print("Feuchtigkeitssensor 2: ");
  Serial.println(moistureValue2);

  // Kurze Pause
  delay(1000);
}