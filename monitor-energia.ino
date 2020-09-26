/*Programa : Medidor de tensão(P8) e corrente(SCT-013 100A) com ESP32
  Autor : Elizabeth Carvalho
  Baseado no programa exemplo da biblioteca EmonLib e Blynk
*/

//Carrega as bibliotecas
#include "EmonLib.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//Constantes
#define BLYNK_PRINT Serial
#define pinSCT 34
#define pinP8 35

//Variaveis
EnergyMonitor SCT013;
BlynkTimer timer;
char auth[] = ""; //Token autenticação app Blynk
char ssid[] = ""; //SSID rede Wi-Fi 2.4Ghz
char pass[] = ""; //Senha da rede Wi-Fi
float kWh = 0;
unsigned long lastmillis = millis();
int inputP8 = 0;
double tensao = 0;
double custokwhLight = 0.68334;
double custo = 0; 
unsigned long start, finished, elapsed;
String tempo = "";

void timerEvent(){
  Serial.println("---------------------------------"); 
  
  //Tensao
  inputP8 = analogRead(pinP8);
  tensao = (inputP8*6.5)/102.4;
  Serial.print("Tensão: "); Serial.print(tensao); Serial.println("V"); 
  Blynk.virtualWrite(V0, tensao);
  
  //Corrente
  double corrente = SCT013.calcIrms(1480)/10; // Calcula o valor da Corrente
  Serial.print("Corrente: "); Serial.print(corrente); Serial.println("A"); 
  Blynk.virtualWrite(V1, corrente);

  //Potencia
  double potencia = corrente * tensao; // Calcula o valor da Potencia Ativa (P = V x A x Fp | Fp=1)
  Serial.print("Potencia: "); Serial.print(potencia); Serial.println("W"); 
  Blynk.virtualWrite(V2, potencia);

  //Tempo de Monitoramento em segundos / minutos
  finished=millis();
  elapsed=(finished-start)/1000;
  if(elapsed < 60){
    tempo = String(elapsed,DEC) + " seg";
  }
  else{
    elapsed = elapsed/60;
    tempo = String(elapsed,DEC) + " min";
  }
  Serial.println("Monitorando a " + tempo);
  Blynk.virtualWrite(V5, "Monitorando a:");
  Blynk.virtualWrite(V6, tempo);
  
  //Consumo
  kWh = kWh + potencia*(millis()-lastmillis)/3600000000.0;
  Serial.print("kWh: "); Serial.print(kWh, 4); Serial.println("kWh"); 
  lastmillis = millis();
  Blynk.virtualWrite(V3, kWh);

  //Custo
  custo = custokwhLight * kWh;
  Serial.print("Custo: R$"); Serial.println(custo, 4); 
  Blynk.virtualWrite(V4, custo);

  
  
  Serial.println("---------------------------------"); 
}


void setup()
{
  pinMode(pinP8,INPUT);
  SCT013.current(pinSCT, 60.606);
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, timerEvent);
  start=millis();
}

void loop()
{
  Blynk.run();
  timer.run();  
}
