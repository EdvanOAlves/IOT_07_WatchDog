#include <WiFi.h>
#include <HTTPClient.h>
#include <WifiClientSecure.h>

int ledVerde = 18;
int ledVermelho = 19;
boolean errorProtocol;

// Rede de teste fracasso
// const char* ssid = "Esp32WifiBravo";
// const char* password = "Senhateste";

// Rede de teste sucesso
const char* ssid = "Galaxy A017688";
const char* password =                                                                                                                 "pdtn6853";

// URL de teste (JSONPlaceholder é um serviço comum para testes de API)
const char* serverName = "https://jsonplaceholder.typicode.com/todos/1";

void setup() {
  Serial.begin(115200);

  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
}

void loop() {
  //Reiniciando a detecção de erros
  errorProtocol = false;

  //Inicia conexão como Station
  WiFi.begin(ssid, password);

  Serial.print("Iniciando tentativa de conexão, rede: ");
  Serial.print(ssid);
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 10) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }

  if (tentativas == 10) {
    errorProtocol = true;
    tentativas = 0;
  }


  if (WiFi.status() == WL_CONNECTED && !errorProtocol) {
    // Criamos o objeto cliente seguro
    WiFiClientSecure* client = new WiFiClientSecure;

    if (client) {
      // Em um cenário real, você usaria client ->setCACert(rotCACertificate);
      //Para este exemplo simples, vamos ignorar a validação do certificado :
      client->setInsecure();

      HTTPClient https;

      Serial.print("[HTTPS] Iniciando requisição... \n");
      digitalWrite(ledVerde, HIGH);
      digitalWrite(ledVermelho, LOW);
      if (https.begin(*client, serverName)) {

        // Realiza o GET
        int httpCode = https.GET();

        // httpCode será positivo se o servidor responder
        if (httpCode > 0) {
          Serial.printf("[HTTPS] Código de resposta: %d\n", httpCode);

          if (httpCode == HTTP_CODE_OK) {
            String payload = https.getString();
            Serial.println("Conteúdo recebido:");
            Serial.println(payload);

            // Atualizando estado
            errorProtocol = false;
          }
        } else {
          // Atualizando estado
          errorProtocol = true;
          Serial.printf("[HTTPS] Falha na requisição, erro: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
      } else {
        // Atualizando estado
        errorProtocol = true;
        Serial.printf("[HTTPS] Não foi possível conectar ao servidor\n");
      }

      delete client;  // Libera memória
    }
  }

  //enviando sinal para leds
  if (!errorProtocol) {  //Sucesso
    Serial.print(errorProtocol);
      digitalWrite(ledVerde, HIGH);
    digitalWrite(ledVermelho, LOW);
  } else {  //Falha de rede, de servidor
      digitalWrite(ledVermelho, HIGH);
    digitalWrite(ledVerde, LOW);
  }

  // Aguarda 15 segundos para a próxima leitura (evita sobrecarga no servidor)
  delay(5000);
}