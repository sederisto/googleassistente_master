/*
******************************  Projeto de Automação Residencial Interface WEB AJAX *******************************************************
******************************  Autor : GiovaniDuarte               ***********************************************************************
******************************  Criado em : 06/03/2015              ***********************************************************************
******************************  Última revisão v2 :17/06/20206     ***********************************************************************
 pulsadores avançado para ligar lampadas eletronicamente e valvula hidraulica
 sensor de temperatura WEB
 
  Faça o download da última biblioteca Blynk aqui:
  https://github.com/blynkkk/blynk-library/releases/latest

  Blynk é uma plataforma com aplicativos iOS e Android para controlar
  Arduino, Raspberry Pi e afins pela Internet.
  Você pode criar facilmente interfaces gráficas para todos os seus
  projetos simplesmente arrastando e soltando widgets.

    Downloads, documentos, tutoriais: http://www.blynk.cc
    Gerador de esboço: http://examples.blynk.cc
    Comunidade Blynk: http://community.blynk.cc
    Redes sociais: http://www.fb.com/blynkapp
    http://twitter.com/blynk_app

  A biblioteca Blynk é licenciada sob licença do MIT
  Este código de exemplo está em domínio público.

 **************************************************** ***********
  Este exemplo mostra como configurar o IP estático com Ethernet.
    NOTA: Os pinos 10, 11, 12 e 13 são reservados para o módulo Ethernet.
        NÃO os use diretamente no seu esboço!

  AVISO: Se você possui um cartão SD, pode ser necessário desativá-lo
        colocando o pino 4 em ALTO. Leia mais aqui:
        https://www.arduino.cc/en/Main/ArduinoEthernetShield
 */ 

#define BLYNK_PRINT Serial
#include <EEPROM.h>
#include <SPI.h> 
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
                                                                                                                                                                          
#include "DHT.h"
#define DHTPIN 2           //onde sensor esta conectado
#define DHTTYPE DHT11     

//chave do blynk enviado no email o app
char auth[] = "ppdxsLMYhSRd3U_sTPnwI53j6L9lQU8t";

DHT dht(DHTPIN, DHTTYPE); //define sensor
BlynkTimer timer;
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Falha ao ler o sensor DHT!");
    return;
  }
  // Você pode enviar qualquer valor a qualquer momento.
  // Por favor, não envie mais que 10 valores por segundo.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
}

IPAddress server_ip (45, 55, 96, 146);
byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress arduino_ip ( 192,   168,   15,  53);
IPAddress dns_ip     (  8,   8,   8,   8);
IPAddress gateway_ip ( 192,  168,   15,   1);
IPAddress subnet_mask(255, 255, 255,   0);
EthernetServer server(8081);

#define W5100_CS  10
#define SDCARD_CS 4

// Atribuindo nomes aos pinos de entrada, conectados aos pulsadores
const int pulsa1 = 31;  //Tecla jardim
const int pulsa2 = 33;  //Tecla cenario
const int pulsa3 = 35;  //Tecla garagem
const int pulsa4 = 37;  //Tecla cozinha
const int pulsa5 = 39;  //Tecla wc suite
const int pulsa6 = 41;  //Tecla sala
const int pulsa7 = 43;  //Tecla quarto3
const int pulsa8 = 45;  //Tecla wc superior lavabo
const int pulsa9 = 47;  //Tecla escadaria quartos
const int pulsa10 = 49; //Tecla lavabo inferior
const int pulsa11 = 29; //Tecla quarto2
const int pulsa12 = 27; //Tecla led escadaria em baixo
const int pulsa13 = 25; //Tecla geral valvula hidraulica
const int pulsa14 = 23; //Tecla quarto1 PRINCIPAL
const int pulsa15 = 21; //Tecla atico
const int pulsa16 = 19; //Tecla valvula de gas D18>V18 VIRTUALIZAR

// Atribuindo nomes aos pinos de saida, conectados aos relés correspondentes as lampadas
const int spot1 = 30;   //relé jardim 
const int spot2 = 32;   //relé cenario                   
const int spot3 = 34;   //relé garagem                   
const int spot4 = 36;   //relé cozinha
const int spot5 = 38;   //relé wc suite
const int spot6 = 40;   //relé sala
const int spot7 = 42;   //relé quarto3
const int spot8 = 44;   //relé wc superior lavabo 
const int spot9 = 46;   //relé escadaria quartos
const int spot10 = 48;  //relé lavabo inferior
const int spot11 = 28;  //relé quarto2
const int spot12 = 26;  //relé led escadaria em baixo
const int spot13 = 24;  //relé geral valvula hidraulica 
const int spot14 = 22;  //relé quarto1 PRINCIPAL
const int spot15 = 20;  //relé atico
const int spot16 = 18;  //relé valvula de gas D18>V18 VIRTUALIZAR

// Variaveis para armazenar o estado das lampadas
boolean spot1_estado = false;
boolean spot2_estado = false;
boolean spot3_estado = false;
boolean spot4_estado = false;
boolean spot5_estado = false;
boolean spot6_estado = false;
boolean spot7_estado = false;
boolean spot8_estado = false;
boolean spot9_estado = false;
boolean spot10_estado = false;
boolean spot11_estado = false;
boolean spot12_estado = false;
boolean spot13_estado = false;
boolean spot14_estado = false;
boolean spot15_estado = false;
boolean spot16_estado = false;

// Variaveis para armazenar o estado dos pulsadores
boolean pulsa1_estado = false;
boolean pulsa2_estado = false;
boolean pulsa3_estado = false;
boolean pulsa4_estado = false;
boolean pulsa5_estado = false;
boolean pulsa6_estado = false;
boolean pulsa7_estado = false;
boolean pulsa8_estado = false;
boolean pulsa9_estado = false;
boolean pulsa10_estado = false;
boolean pulsa11_estado = false;
boolean pulsa12_estado = false;
boolean pulsa13_estado = false;
boolean pulsa14_estado = false;
boolean pulsa15_estado = false;
boolean pulsa16_estado = false;

// Variavel que controla o tempo até uma detecção confiavel
const int debounceTime = 20;

// Declarando função que detecta por quanto tempo o pulsador foi pressionado;
// pino é o pino sendo verificado;
// &state é o estado do pino (HIGH ou LOW);
// &inicio é quando ele foi pressionado
unsigned long pulsadorTempo(int pino, boolean &state, unsigned long &inicio);

// Variavel para acionar todas as lampadas
boolean todas = false;

//Teste toques multiplos
int contagem=0;
long tempoA=0;

char incomingByte[6];
int inchar = 0;

void setup(){
  dht.begin();        
  Serial.begin(9600);
  Serial.println("Iniciando programa..");
  Serial.println();
  server.begin();

  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Desabilita sd card do shild

  Blynk.begin(auth, server_ip, 8080, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
  // Ou este:
  //Blynk.begin(auth, "blynk-cloud.com", 80, arduino_ip, dns_ip, gateway_ip, subnet_mask, arduino_mac);
  timer.setInterval(1000L, sendSensor);
  
  // Definindo os pinos conectados aos pulsadores como entrada
  pinMode(pulsa1,INPUT);
  pinMode(pulsa2,INPUT);
  pinMode(pulsa3,INPUT);
  pinMode(pulsa4,INPUT);
  pinMode(pulsa5,INPUT);
  pinMode(pulsa6,INPUT);
  pinMode(pulsa7,INPUT);
  pinMode(pulsa8,INPUT);
  pinMode(pulsa9,INPUT);
  pinMode(pulsa10,INPUT);
  pinMode(pulsa11,INPUT);
  pinMode(pulsa12,INPUT);
  pinMode(pulsa13,INPUT);
  pinMode(pulsa14,INPUT);
  pinMode(pulsa15,INPUT);
  pinMode(pulsa16,INPUT); 
    
  // Ativando o resistor PULL-UP
  digitalWrite(pulsa1,HIGH);
  digitalWrite(pulsa2,HIGH);
  digitalWrite(pulsa3,HIGH);
  digitalWrite(pulsa4,HIGH);
  digitalWrite(pulsa5,HIGH);
  digitalWrite(pulsa6,HIGH);
  digitalWrite(pulsa7,HIGH);
  digitalWrite(pulsa8,HIGH);
  digitalWrite(pulsa9,HIGH);
  digitalWrite(pulsa10,HIGH);
  digitalWrite(pulsa11,HIGH);
  digitalWrite(pulsa12,HIGH);
  digitalWrite(pulsa13,HIGH);
  digitalWrite(pulsa14,HIGH);
  digitalWrite(pulsa15,HIGH);
  digitalWrite(pulsa16,HIGH);
  
  // Definindo os pinos conectados aos relés como saida
  pinMode(spot1,OUTPUT);
  pinMode(spot2,OUTPUT);
  pinMode(spot3,OUTPUT);
  pinMode(spot4,OUTPUT);
  pinMode(spot5,OUTPUT);
  pinMode(spot6,OUTPUT);
  pinMode(spot7,OUTPUT);
  pinMode(spot8,OUTPUT);
  pinMode(spot9,OUTPUT);
  pinMode(spot10,OUTPUT);
  pinMode(spot11,OUTPUT);
  pinMode(spot12,OUTPUT);
  pinMode(spot13,OUTPUT);
  pinMode(spot14,OUTPUT);
  pinMode(spot15,OUTPUT);
  pinMode(spot16,OUTPUT);
 //===============================Escreve na eeprom as saidas, no caso falta de energia executa uma vez..
  digitalWrite(spot1,EEPROM.read(0));        
  digitalWrite(spot2,EEPROM.read(1));        
  digitalWrite(spot3,EEPROM.read(2));        
  digitalWrite(spot4,EEPROM.read(3));  
  digitalWrite(spot5,EEPROM.read(4)); 
  digitalWrite(spot6,EEPROM.read(5));        
  digitalWrite(spot7,EEPROM.read(6));        
  digitalWrite(spot8,EEPROM.read(7));        
  digitalWrite(spot9,EEPROM.read(8));  
  digitalWrite(spot10,EEPROM.read(9));
  digitalWrite(spot11,EEPROM.read(10)); 
  digitalWrite(spot12,EEPROM.read(11));
  digitalWrite(spot13,EEPROM.read(12));
  digitalWrite(spot14,EEPROM.read(13));
  digitalWrite(spot15,EEPROM.read(14));   
  digitalWrite(spot16,EEPROM.read(15));
  
}
void loop(){
 Blynk.run();
 timer.run();   
//=========================INICIO PULSADORES AVANÇADO=========================================================  
//============================pulso spot1=====================================================================
    int tempoPulsa1 = pulsador1Tempo();
    
    if (tempoPulsa1 > 1800){
      if(!todas){
        int i = 0;
        for (i=0;i<4;i++){
          tone(13, 800, 50); // play the tone
          delay(100);
        }
      }
      todas=true;
    }
    else if (tempoPulsa1 > debounceTime){
      pulsa1_estado = true;
    }
    else {
      if (todas){
        aciona_todas();
        pulsa1_estado = false;
        todas = false;
      }
      else if (pulsa1_estado == true ) {
        //comutaRele(spot1, spot1_estado);
        pulsa1_estado = false;
        contagem++;
        tempoA=millis();
      }
      if (contagem > 0){
        if(millis() - tempoA > 250){
          if(contagem==1)
            comutaRele(spot1,spot1_estado);
          else if (contagem == 2){
            comutaRele(spot1,spot1_estado);
            tone(13, 800, 50);
          }
          contagem=0;
        }
      }
   //        if (contagem ==1){
   //    if(millis() - tempoA > 2000){
   //       comutaRele(spot1,spot1_estado);
   //        contagem=0;
   //         }
   //      }
  
    }
   //============================pulso spot2===================================
    int tempoPulsa2 = pulsador2Tempo();
    if (tempoPulsa2 > debounceTime){
      pulsa2_estado = true;
    }
    
    else {
      if (pulsa2_estado == true ) {
        comutaRele(spot2, spot2_estado);
        pulsa2_estado = false;
      }
    }
  //============================pulso spot3=========================================== 
    int tempoPulsa3 = pulsador3Tempo();
    if (tempoPulsa3 > debounceTime){
      pulsa3_estado = true;
    }
    
    else {
      if (pulsa3_estado == true ) {
        comutaRele(spot3, spot3_estado);
        pulsa3_estado = false;
      }
   }
   //============================pulso spot4=========================================== 
    int tempoPulsa4 = pulsador4Tempo();
    if (tempoPulsa4 > debounceTime){
      pulsa4_estado = true;
    }
    
    else {
      if (pulsa4_estado == true ) {
        comutaRele(spot4, spot4_estado);
        pulsa4_estado = false;
      }
   }
   //============================pulso spot5=========================================== 
    int tempoPulsa5 = pulsador5Tempo();
    if (tempoPulsa5 > debounceTime){
      pulsa5_estado = true;
    }
    
    else {
      if (pulsa5_estado == true ) {
        comutaRele(spot5, spot5_estado);
        pulsa5_estado = false;
      }
   }
   //============================pulso spot6=========================================== 
    int tempoPulsa6 = pulsador6Tempo();
    if (tempoPulsa6 > debounceTime){
      pulsa6_estado = true;
    }
    
    else {
      if (pulsa6_estado == true ) {
        comutaRele(spot6, spot6_estado);
        pulsa6_estado = false;
      }
   }
  //============================pulso spot7=========================================== 
    int tempoPulsa7 = pulsador7Tempo();
    if (tempoPulsa7 > debounceTime){
      pulsa7_estado = true;
    }
    
    else {
      if (pulsa7_estado == true ) {
        comutaRele(spot7, spot7_estado);
        pulsa7_estado = false;
      }
   }
   //============================pulso spot8=========================================== 
    int tempoPulsa8 = pulsador8Tempo();
    if (tempoPulsa8 > debounceTime){
      pulsa8_estado = true;
    }
    
    else {
      if (pulsa8_estado == true ) {
        comutaRele(spot8, spot8_estado);
        pulsa8_estado = false;
      }
   }
   //============================pulso spot9=========================================== 
    int tempoPulsa9 = pulsador9Tempo();
    if (tempoPulsa9 > debounceTime){
      pulsa9_estado = true;
    }
    
    else {
      if (pulsa9_estado == true ) {
        comutaRele(spot9, spot9_estado);
        pulsa9_estado = false;
      }
   }
   //============================pulso spot10=========================================== 
    int tempoPulsa10 = pulsador10Tempo();
    if (tempoPulsa10 > debounceTime){
      pulsa10_estado = true;
    }
    
    else {
      if (pulsa10_estado == true ) {
        comutaRele(spot10, spot10_estado);
        pulsa10_estado = false;
      }
   }
   //============================pulso spot11=========================================== 
    int tempoPulsa11 = pulsador11Tempo();
    if (tempoPulsa11 > debounceTime){
      pulsa11_estado = true;
    }
    
    else {
      if (pulsa11_estado == true ) {
        comutaRele(spot11, spot11_estado);
        pulsa11_estado = false;
      }
   }
  //============================pulso spot12=========================================== 
    int tempoPulsa12 = pulsador12Tempo();
    if (tempoPulsa12 > debounceTime){
      pulsa12_estado = true;
    }
    
    else {
      if (pulsa12_estado == true ) {
        comutaRele(spot12, spot12_estado);
        pulsa12_estado = false;
      }
   }
 //============================pulso spot13=========================================== 
    int tempoPulsa13 = pulsador13Tempo();
    if (tempoPulsa13 > debounceTime){
      pulsa13_estado = true;
    }
    
    else {
      if (pulsa13_estado == true ) {
        comutaRele(spot13, spot13_estado);
        pulsa13_estado = false;
      }
   }
  //============================pulso spot14=========================================== 
    int tempoPulsa14 = pulsador14Tempo();
    if (tempoPulsa14 > debounceTime){
      pulsa14_estado = true;
    }
    
    else {
      if (pulsa14_estado == true ) {
        comutaRele(spot14, spot14_estado);
        pulsa14_estado = false;
      }
   }
//============================pulso spot15=========================================== 
    int tempoPulsa15 = pulsador15Tempo();
    if (tempoPulsa15 > debounceTime){
      pulsa15_estado = true;
    }
    
    else {
      if (pulsa15_estado == true ) {
        comutaRele(spot15, spot15_estado);
        pulsa15_estado = false;
      }
   }
//============================pulso spot16=========================================== 
    int tempoPulsa16 = pulsador16Tempo();
    if (tempoPulsa16 > debounceTime){
      pulsa16_estado = true;
    }
    
    else {
      if (pulsa16_estado == true ) {
        comutaRele(spot16, spot16_estado);
        pulsa16_estado = false;
      }
   }   
/*
    ***************************************************************************************************************************************
    *******************************  Grava informações na EEprom para Backup em caso de queda de energia.. ********************************
    ***************************************************************************************************************************************
*     
*     
*/
     EEPROM.write(0,digitalRead(spot1));              
     EEPROM.write(1,digitalRead(spot2));              
     EEPROM.write(2,digitalRead(spot3));              
     EEPROM.write(3,digitalRead(spot4));              
     EEPROM.write(4,digitalRead(spot5));
     EEPROM.write(5,digitalRead(spot6));              
     EEPROM.write(6,digitalRead(spot7));              
     EEPROM.write(7,digitalRead(spot8));              
     EEPROM.write(8,digitalRead(spot9));              
     EEPROM.write(9,digitalRead(spot10)); 
     EEPROM.write(10,digitalRead(spot11));
     EEPROM.write(11,digitalRead(spot12));
     EEPROM.write(12,digitalRead(spot13));
     EEPROM.write(13,digitalRead(spot14));
     EEPROM.write(14,digitalRead(spot15));
     EEPROM.write(15,digitalRead(spot16));
     
 delay(10);
 }//FIM DO LOOP AQUI!!!!!!!


// Função para detectar por quanto tempo o pulsador foi pressionado
unsigned long pulsadorTempo(int pino, boolean &state, unsigned long &inicio){
  if(digitalRead(pino) != state) {
    state = !state;
    inicio = millis();
  }
  if (state == LOW)
    return millis() - inicio;
  else
    return 0;
}
//Funçoes abaixo sao especificas de cada pulsador  
long pulsador1Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa1, state, inicio);
}

long pulsador2Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa2, state, inicio);
}

long pulsador3Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa3, state, inicio);
}

long pulsador4Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa4, state, inicio);
}

long pulsador5Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa5, state, inicio);
}

long pulsador6Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa6, state, inicio);
}

long pulsador7Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa7, state, inicio);
}

long pulsador8Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa8, state, inicio);
}

long pulsador9Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa9, state, inicio);
}

long pulsador10Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa10, state, inicio);
}

long pulsador11Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa11, state, inicio);
}

long pulsador12Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa12, state, inicio);
}

long pulsador13Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa13, state, inicio);
}
long pulsador14Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa14, state, inicio);
}

long pulsador15Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa15, state, inicio);
}

long pulsador16Tempo(){
  static unsigned long inicio = 0;
  static boolean state;
  return pulsadorTempo(pulsa16, state, inicio);
}
//Funçao que comuta o rele - debauce acionado ligar spot '1 2 e 3' para ligar mais lampadas adicionar nesta funçao;
void comutaRele(int pino, boolean &estado){
  estado = !estado;
  digitalWrite(pino, estado);
  Serial.println("Mudou");
}

void aciona_todas(){                    
  if (!spot2_estado){                  //spot cenario sala
    spot2_estado = !spot2_estado;
    digitalWrite(spot2, spot2_estado);
  }
  if (!spot4_estado){                 //spot Cozinha 
    spot4_estado = !spot4_estado;
    digitalWrite(spot4, spot4_estado);
  }
  if (!spot6_estado){                 //spot Sala
    spot6_estado = !spot6_estado;
    digitalWrite(spot6, spot6_estado);
  }

}
