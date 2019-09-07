
#define pino_sinal_analogico A0
 
float valor_analogico;
float percentual_umidade_solo;
 
void setup()
{
  Serial.begin(115200);
  pinMode(pino_sinal_analogico, INPUT);
}
 
void loop()
{
  //Le o valor do pino A0 do sensor
  valor_analogico = analogRead(pino_sinal_analogico);
  percentual_umidade_solo = (((1023-valor_analogico)/1023)*100);
  
  //Mostra o valor da porta analogica no serial monitor
  Serial.print("Porta analogica: ");
  Serial.println(valor_analogico);

  Serial.print("Percentual de umidade : ");
  Serial.print(percentual_umidade_solo);
  Serial.println(" %");
 /*
  //Solo umido, acende o led verde
  if (valor_analogico > 0 && valor_analogico < 400)
  {
    Serial.println(" Status: Solo umido");
  }
 
  //Solo com umidade moderada, acende led amarelo
  if (valor_analogico > 400 && valor_analogico < 800)
  {
    Serial.println(" Status: Umidade moderada");
  }
 
  //Solo seco, acende led vermelho
  if (valor_analogico > 800 && valor_analogico < 1024)
  {
    Serial.println(" Status: Solo seco");
  }*/
  delay(2000);
}
