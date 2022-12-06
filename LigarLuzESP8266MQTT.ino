/* A project for activating a light switch using a servo motor, the MQTT protocol and the ESP8266 MCU. 
 * By: Juliano Moreira Aleixo
 * Github: https://github.com/JulianoAleixo
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient
#include <Servo.h>

#define LED D3 // Pino do LED

// Configurando rede
const char* ssid = "Nome";
const char* password = "Senha";
const char* mqtt_server = "broker.hivemq.com"; // Broker público gratuito

WiFiClient espClient;
PubSubClient client(espClient);

Servo myservo;

void setup_wifi() {
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char*topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem chegou no topico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  String data = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    data += (char)payload[i];
  }
  Serial.println();
  Serial.print("Tamanho da mensagem:");
  Serial.println(length);
  Serial.println();
  Serial.println("-----------------------");
  Serial.println(data);
  if (data == "ON") { // Se receber ON
    Serial.println("LED");
    digitalWrite(D3, 1);
    myservo.write(120);
  }
  else {
    digitalWrite(D3, 0);
    myservo.write(0);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar o MQTT...");
    String clientId = "CLIENT_MQTT"; // Nome de um cliente
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");
      client.subscribe("device/publisher");
    }
    else {
      Serial.print("falhou, rc =");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s.");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(D3, OUTPUT);
  myservo.attach(D1); // Pino do Servo
  myservo.write(0);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
