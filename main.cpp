/*
  O Código pode ser utilizado como base para o carrinho OBR de apenas os funcionários da Assinco Cursos, qualquer compartilhamento indevido do código está sujeito a processos criminais.

  Desenvolvido pelos instrutores [Eduardo Rezende e Gabriel Faria]
  Feito em [(21/01/2025) Polo 01 - SANTANA DE PARNAÍBA]]

  Informações:
              O Carrinho utiliza dos seguintes componentes: 
                  4 - Servo Motores Contínuos 
                  2 - Sensores Segue Linha
                  1 - Sensor Ultrassônico 
                  2 - Sensores de Cor TCS3200

              O Funcionamento básico do carrinho é desviar de obstáculos, identificar cores e seguir linhas

              Qualquer dúvida que surgir podem chamar nos seguintes emails:
                  dudumoises2005@gmail.com (instrutor Eduardo)
                  gabrielfaria.maker@gmail.com (instrutor Gabriel)

  OBS:
            Provavelmente será necessário alterar os valores de delays e rotações dos servos para se adequar corretamente as necessidades de seu carrinho. (Não tenha medo de mexer no código)

  Boa sorte a todos! =)
*/

#include <Servo.h>  //Incluindo biblioteca para controlar Servos Motores

#define sensorSegueLinha2 24  //É o sensor da direita visto pela parte de trás do carrinho
#define sensorSegueLinha1 25  //É o sensor da esquerda visto pela parte de trás do carrinho

//Pinos Trigger e Echo do sensor Ultrassônico
#define trig 22
#define echo 23

//Distância Máxima e Mínima para sensor reconhecer obstáculo
#define max_dist 5
#define min_dist 1

//Pinos do sensor de cor (É o sensor da esquerda visto pela parte de trás do carrinho)
#define S0A 7
#define S1A 8
#define S2A 3
#define S3A 2
#define OUTA 6

//Pinos do sensor de cor (É o sensor da direita visto pela parte de trás do carrinho)
#define S0B 9
#define S1B 10
#define S2B 11
#define S3B 12
#define OUTB 13

//Definindo os limites das cores para efetuar as lógicas de identificar cor
#define limite_branco 15
#define limite_preto 65

#define limite_vermelho 100

//Declarando e inicializando variáveis que serão utilizadas no carrinho:

//Declarando as 4 variáveis que representaram respectivamente cada Servo Motor
Servo servo1, servo2, servo3, servo4;

//Declarando para lógica do sensor segue linha
int valorSensorSegueLinha1 = 0, valorSensorSegueLinha2 = 0;

//Declarando para calcular distância de objeto pelo Ultrassônico
int duracao = 0, distancia = 0;

//Declarando para calcular as cores vistas pelo sensor de Cor
int redA = 0, blueA = 0, greenA = 0, somaA = 0;
int redB = 0, blueB = 0, greenB = 0, somaB = 0;

//Função de Configuração
void setup() {

  //Inicia comunicação com Monitor Serial
  Serial.begin(9600);

  //Definindo o modo de cada pino dos sensores que serão utilizados: (INPUT ou OUTPUT):

  //Os dois sensores de segue linha são INPUT (Entrada de dados)
  pinMode(sensorSegueLinha1, INPUT);
  pinMode(sensorSegueLinha2, INPUT);

  //Pino trigger é Saída (OUTPUT) e Echo é INPUT
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  //Estamos vendo pela parte de trás do carrinho cada Servo Motor
  servo1.attach(26);  // motor1 parte da frente esquerda
  servo2.attach(27);  // motor2 parte da frente direita
  servo3.attach(28);  // motor3 parte de trás esquerda
  servo4.attach(29);  // motor4 parte de trás direita

  //Configuração dos pinos do sensor de cor
  pinMode(S0A, OUTPUT);
  pinMode(S1A, OUTPUT);
  pinMode(S2A, OUTPUT);
  pinMode(S3A, OUTPUT);
  pinMode(OUTA, INPUT);

  //Configuração dos pinos do sensor de cor
  pinMode(S0B, OUTPUT);
  pinMode(S1B, OUTPUT);
  pinMode(S2B, OUTPUT);
  pinMode(S3B, OUTPUT);
  pinMode(OUTB, INPUT);

  //Configurando sensores de cor para 100% de frequência
  digitalWrite(S0A, HIGH);
  digitalWrite(S1A, HIGH);

  digitalWrite(S0B, HIGH);
  digitalWrite(S1B, HIGH);
}

//Função de Repetição
void loop() {
  Distancia();
  cor();
  segueLinha();
}

//Obtem a distância do obstáculo e caso necessário desvia
void Distancia() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  //Obtem o tempo (em microssegundos) entre o envio da onda e o recebimento.
  duracao = pulseIn(echo, HIGH);

  //Converte o tempo de microssegundos para centímetros (velocidade do som: 0.034 cm/µs) 
  //O resultado é dividido por 2 porque o tempo registrado inclui o percurso de ida e volta
  distancia = duracao * 0.034 / 2;
  delay(5);

  Serial.print("Distância: ");
  Serial.println(distancia);

  //Condição: Está próximo ao Obstáculo ? Se sim desvia.
  if (distancia > min_dist && distancia < max_dist)
    desviar();
}

//Função que identifica as cores R G B dos sensores A,B e a partir disso verifica se é necessário mudar a direção
void cor() {

  //Obtendo informações das cores lidas pelos sensores de cor
  obterSensorCorA();
  obterSensorCorB();

  //Efetua a soma dos R G B dos sensores A e B para identificar a cor verde com precisão 
  somaA = greenA + blueA + redA;
  somaB = greenB + blueB + redB;

  Serial.print("Soma A: ");
  Serial.println(somaA);

  Serial.print("Soma B: ");
  Serial.println(somaB);

  //Condições sobre as cores lidas pelos sensores de cor:

  //Verifica se ambos os sensores lerão verde
  if ((somaA + somaB) > limite_preto && greenA < redA && greenA < blueA && 
                                        greenB < redB && greenB < blueB)
  {
    Serial.println("É verde !!! nos sensores A e B");
  
    girar360();
  } 
  else if (greenA < redA && greenA < blueA && greenA > greenB && 
           greenA > limite_branco && somaA > limite_preto)  //Verifica se leu verde apenas no sensor A
  {
    Serial.println("É verde!!! no sensor A");

    parar();
    delay(2000);
    avancarEsquerda();
    delay(3000);
    parar();
    delay(2000);
    avancar();
    delay(1000);
  } 
  else if (greenB < redB && greenB < blueB && greenB > greenA && 
           greenB > limite_branco && somaB > limite_preto)  //Verifica se leu verde apenas no sensor B
  {
    Serial.println("É verde!!! no sensor B");

    parar();
    delay(2000);
    avancarDireita();
    delay(3000);
    parar();
    delay(2000);
    avancar();
    delay(1000);
  } 
  else if (redA < greenA && redA < blueA && redA > redB && somaA > limite_vermelho)  //Verifica se leu vermelho no sensor A
  {
    Serial.println("É vermelho!!! no sensor A");

    //while (true) parar(); //Fica parado para sempre
  } 
  else if (redB < greenB && redB < blueB && redB > redA && somaB > limite_vermelho)  //Verifica se leu vermelho no sensor B
  {
    Serial.println("É vermelho!!! no sensor B");

    //while (true) parar();  //Fica parado para sempre
  }
}

//Função que lê os valores dos sensores de segue linha e decide como vai se direcionar na pista 
void segueLinha() {

  //Obtendo o valor dos sensores segue linha: Branco ou Preto (HIGH, LOW)
  valorSensorSegueLinha1 = digitalRead(sensorSegueLinha1);
  valorSensorSegueLinha2 = digitalRead(sensorSegueLinha2);

  Serial.print("SensorsegueLinha 1: ");
  Serial.println(valorSensorSegueLinha1);

  Serial.print("SensorsegueLinha 2: ");
  Serial.println(valorSensorSegueLinha2);

  //Condições para o carrinho se deslocar:
  if (valorSensorSegueLinha1 == 0 && valorSensorSegueLinha2 == 0) 
  {
    avancar();
  } 
  else if (valorSensorSegueLinha1 == 1 && valorSensorSegueLinha2 == 1) 
  {
    avancar();
  }                             
  else if (valorSensorSegueLinha1 == 0 && valorSensorSegueLinha2 == 1)
  {
    avancarDireita();
  } 
  else 
  {
    avancarEsquerda();
  }
}

//Ajustar + testes
void avancar() {
  servo1.write(112);
  servo2.write(73);
  servo3.write(112);
  servo4.write(73);
}

//Ajustar + testes
void avancarDireita() {
  servo1.write(85);
  servo2.write(85);
  servo3.write(85);
  servo4.write(85);
}

//Ajustar + testes
void avancarEsquerda() {
  servo1.write(105);
  servo2.write(105);
  servo3.write(105);
  servo4.write(105);
}

void parar() {
  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
}

//Ajustar + testes
void desviar() {
  parar();
  delay(2000);

  avancarEsquerda();
  delay(3000);

  parar();
  delay(2000);

  avancar();
  delay(3000);

  parar();
  delay(2000);

  avancarDireita();
  delay(3000);

  parar();
  delay(2000);

  avancar();
  delay(2000);

  parar();
  delay(2000);

  avancarDireita();
  delay(3000);

  parar();
  delay(2000);

  avancar();
}

//Ajustar + testes
void girar360() {
  parar();
  delay(2000);

  avancarEsquerda();
  delay(6000);

  parar();
  delay(2000);

  avancar();
  delay(2000);
}

//Função que obtem as cores lidas pelo sensor de cor A
void obterSensorCorA() {
  digitalWrite(S2A, LOW);
  digitalWrite(S3A, LOW);
  delay(5);
  redA = pulseIn(OUTA, LOW);

  digitalWrite(S2A, HIGH);
  digitalWrite(S3A, HIGH);
  delay(5);
  greenA = pulseIn(OUTA, LOW);

  digitalWrite(S2A, HIGH);
  digitalWrite(S3A, HIGH);
  delay(5);
  blueA = pulseIn(OUTA, LOW);

  Serial.print("Vermelho A: ");
  Serial.println(redA);

  Serial.print("Azul A: ");
  Serial.println(blueA);

  Serial.print("Verde A: ");
  Serial.println(greenA);
}

//Função que obtem as cores lidas pelo sensor de cor B
void obterSensorCorB() {
  digitalWrite(S2B, LOW);
  digitalWrite(S3B, LOW);
  delay(5);
  redB = pulseIn(OUTB, LOW);

  digitalWrite(S2B, HIGH);
  digitalWrite(S3B, HIGH);
  delay(5);
  greenB = pulseIn(OUTB, LOW);

  digitalWrite(S2B, HIGH);
  digitalWrite(S3B, HIGH);
  delay(5);
  blueB = pulseIn(OUTB, LOW);

  Serial.print("Vermelho B: ");
  Serial.println(redB);

  Serial.print("Azul B ");
  Serial.println(blueB);

  Serial.print("Verde B: ");
  Serial.println(greenB);
}
