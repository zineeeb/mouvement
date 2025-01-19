#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Configuration de l'écran OLED
#define SCREEN_WIDTH 40  // Largeur de l'écran OLED
#define SCREEN_HEIGHT 20  // Hauteur de l'écran OLED
#define OLED_ADDR 0x3C

// Configuration de la connexion WiFi et Firebase
#define WIFI_SSID "Diane"
#define WIFI_PASSWORD "22130512bandaage"
#define API_KEY "AIzaSyDsXF4AHN03nNDHGzGpHqhXWQRhGYOYkRg"
#define DATABASE_URL "https://mouvement1854-default-rtdb.europe-west1.firebasedatabase.app/"

// Initialisation de l'écran OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Initialisation de Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Déclaration des broches
int pirPin = 4;    // Broche pour le capteur PIR
int ledPin = 2;    // Broche pour la LED/lampe
int pirState = LOW;

void setup() {
  // Initialisation de la communication série
  Serial.begin(115200);
  
  // Initialisation des broches
  pinMode(pirPin, INPUT);    // Définir la broche PIR comme entrée
  pinMode(ledPin, OUTPUT);   // Définir la broche LED comme sortie

  // Initialisation de l'écran OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("Échec de l'initialisation de l'écran OLED"));
    for (;;); // Boucle infinie en cas d'échec
  }
  display.clearDisplay();
  display.setTextSize(1);  // Taille du texte
  display.setTextColor(SSD1306_WHITE); // Couleur du texte
  display.setCursor(0, 0);
  display.println("System Ready...");
  display.display();
  delay(1000);

  // Connexion WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Configuration Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  int val = digitalRead(pirPin); // Lire l'état du PIR

  if (val == HIGH && pirState == LOW) {
    // Mouvement détecté
    Serial.println("Mouvement détecté !");
    digitalWrite(ledPin, HIGH);  // Allumer la lampe

    // Envoi de l'événement à Firebase
    String timestamp = String(millis());
    String path = "mouvement/" + timestamp;
    if (Firebase.RTDB.setString(&fbdo, path + "/event", "Mouvement détecté") &&
        Firebase.RTDB.setString(&fbdo, path + "/timestamp", timestamp)) {
      Serial.println("Données envoyées à Firebase !");
    } else {
      Serial.println("Erreur d'envoi vers Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Afficher sur l'écran OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Mouvement detecte !");
    display.display();

    pirState = HIGH;
  } 
  else if (val == LOW && pirState == HIGH) {
    // Aucun mouvement
    Serial.println("Aucun mouvement.");
    digitalWrite(ledPin, LOW);  // Éteindre la lampe

    // Afficher sur l'écran OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Aucun mouvement.");
    display.display();

    pirState = LOW;
  }

  delay(10);
}
