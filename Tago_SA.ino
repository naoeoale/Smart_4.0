//exemplo base
//referência: https://raw.githubusercontent.com/vinicius0082/esp32-tagoIO/main/exemplos/mpu_esp32_mqtt_tagoIO


//Envio de Dados para tagoIO via MQTT

#include <ArduinoJson.h>
#include "EspMQTTClient.h"

//pinos de entrada e saída
const int pino_botao = 25; //input
const int pino_lado1 = 33; //input
const int pino_lado2 = 32; //input
const int pino_lado3 = 35; //input
const int pino_led = 26; //output

//variáveis para Json
char json_btn[100];
char json_lado1[100];
char json_lado2[100];
char json_lado3[100];

//variáveis internas
int valor_btn;
int valor_lado1;
int valor_lado2;
int valor_lado3;


//configurações da conexão MQTT
EspMQTTClient client
(
  "FIESC_IOT", //nome da sua rede Wi-Fi
  "C6qnM4ag81", //senha da sua rede Wi-Fi
  "mqtt.tago.io",  // MQTT Broker server ip padrão da tago
  "Token",   // username
  "19a56446-0f07-46fa-ad11-91c6e97c6f60",   // Código do Token
  "TestClient",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

//configuração dos pinos
void setup()
{
  Serial.begin(115200);
  //configurações dos pinos
  pinMode(pino_botao, INPUT); //configura o pino como entrada
  pinMode(pino_lado1, INPUT); //configura o pino como entrada
  pinMode(pino_lado2, INPUT); //configura o pino como entrada
  pinMode(pino_lado3, INPUT); //configura o pino como entrada
  pinMode(pino_led, OUTPUT); //configura o pino como entrada
  digitalWrite(pino_botao, HIGH); //ativa pull-up no pino de entrada
  digitalWrite(pino_lado1, HIGH); //ativa pull-up no pino de entrada
  digitalWrite(pino_lado2, HIGH); //ativa pull-up no pino de entrada
  digitalWrite(pino_lado3, HIGH); //ativa pull-up no pino de entrada
}



void leitura_sinais()
{
  valor_btn = digitalRead(pino_botao);
  valor_lado1 = digitalRead(pino_lado1);
  valor_lado2 = digitalRead(pino_lado2);
  valor_lado3 = digitalRead(pino_lado3);
}

void converte_json()
{
  StaticJsonDocument<300> sjson_btn;

  sjson_btn["variable"] = "botao";
  sjson_btn["value"] = valor_btn;
  serializeJson(sjson_btn, json_btn);

  StaticJsonDocument<300> sjson_lado1;

  sjson_lado1["variable"] = "lado1";
  sjson_lado1["value"] = valor_lado1;
  serializeJson(sjson_lado1, json_lado1);

  StaticJsonDocument<300> sjson_lado2;

  sjson_lado2["variable"] = "lado2";
  sjson_lado2["value"] = valor_lado2;
  serializeJson(sjson_lado2, json_lado2);

  StaticJsonDocument<300> sjson_lado3;

  sjson_lado3["variable"] = "lado3";
  sjson_lado3["value"] = valor_lado3;
  serializeJson(sjson_lado3, json_lado3);

 
}

void envia_msg()
{
  client.publish("node/btn", json_btn); // You can activate the retain flag by setting the third parameter to true
  client.publish("node/btn", json_lado1); // You can activate the retain flag by setting the third parameter to true
  client.publish("node/lado2e3", json_lado2); // You can activate the retain flag by setting the third parameter to true
  client.publish("node/lado2e3", json_lado3); // You can activate the retain flag by setting the third parameter to true
}

//loop do programa
void loop()
{
  leitura_sinais();
  converte_json();
  envia_msg();

  delay(1000);

  client.loop();
}

void processa_msg(const String payload)
{
  StaticJsonDocument<300> msg;

  DeserializationError err = deserializeJson(msg, payload);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.f_str());
  }
  Serial.print("var:");  
  String var = msg["variable"];
  Serial.println(var);
  if(var == "status")
  {
    Serial.print("value:");
    String val = msg["value"];
    Serial.println(val);
    if(val == "on")
      digitalWrite(pino_led, LOW);
    else
      digitalWrite(pino_led, HIGH);
  }
 
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
   client.subscribe("node/status", [] (const String &payload)  {
   Serial.println(payload);
   processa_msg(payload);
  });
}
