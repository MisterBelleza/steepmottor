# Controle de Motor de Passo via Ethernet

Este projeto permite controlar um motor de passo através de uma interface web, utilizando um Arduino com um módulo Ethernet. O motor pode ser girado para frente ou para trás a partir de uma página web acessível na rede local.

## Componentes Necessários

- Arduino (Uno, Mega, etc.)
- Módulo Ethernet (como o W5100)
- Motor de Passo
- Fios e jumpers
- Fonte de alimentação para o motor de passo

## Configuração

1. **Conexões do Motor de Passo:**
   - Conecte os pinos do motor de passo aos pinos 3, 4, 5 e 6 do Arduino.
   - Certifique-se de que o motor esteja corretamente alimentado.

2. **Conexão Ethernet:**
   - Conecte o módulo Ethernet ao Arduino.
   - Conecte o módulo Ethernet à rede local via cabo RJ45.

3. **Configuração de Rede:**
   - O Arduino obtém um endereço IP automaticamente usando DHCP. Certifique-se de que sua rede tenha um servidor DHCP ativo.
   - Após conectar o Arduino à rede, o endereço IP atribuído será exibido no monitor serial da IDE do Arduino.

4. **Upload do Código:**
   - Carregue o código `ethernet_passo.ino` para o Arduino usando a IDE do Arduino.

## Uso

1. Após conectar o Arduino à rede e carregar o código, o servidor web será iniciado.
2. Abra o monitor serial da IDE do Arduino para ver o endereço IP atribuído ao Arduino.
3. Acesse o IP exibido no monitor serial e acesse-o pelo navegador web na mesma rede.
4. Você verá uma página web com dois botões:
   - **Girar para Frente**: Gira o motor de passo no sentido horário.
   - **Girar para Trás**: Gira o motor de passo no sentido anti-horário.

## Código

O código principal está no arquivo `ethernet_passo.ino`. Ele utiliza as bibliotecas `Ethernet` e `Stepper` para controlar o motor de passo e criar o servidor web.

```cpp
#include <Ethernet.h>
#include <Stepper.h>

// Configurações do motor de passo
const int stepsPerRevolution = 150; // Número de passos por volta (ajuste conforme seu motor)
Stepper myStepper(stepsPerRevolution, 3, 4, 5, 6);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Endereço MAC único
EthernetServer server(80); // Servidor na porta 80

void setup() {
  Serial.begin(9600);

  myStepper.setSpeed(50); // Velocidade em RPM

  // Inicializa a conexão Ethernet usando DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Falha ao configurar Ethernet usando DHCP");
    while (true); // Trava o programa se não conseguir conectar
  }

  server.begin(); // Inicia o servidor

  Serial.print("Servidor iniciado em: ");
  Serial.println(Ethernet.localIP()); // Exibe o IP atribuído pelo DHCP
}

void loop() {
  EthernetClient client = server.available();
  String thisString;
  if (client) {
    Serial.println("Novo cliente conectado!");
    String request = "";

    // Lê a requisição HTTP
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n') { 
          break;
        }
      }
    }

    if (request.indexOf("GET /forward") >= 0) {
      thisString = String("Girando Frente");
      client.println("<h1>" + thisString + "</h1>");
      myStepper.step(stepsPerRevolution);
      Serial.println("Motor girou para frente");
      client.println("<p><a href=\"/\"><button>Voltar</button></a></p>");
    } else if (request.indexOf("GET /backward") >= 0) {
      thisString = String("Girando Trás");
      client.println("<h1>" + thisString + "</h1>");
      myStepper.step(-stepsPerRevolution);
      Serial.println("Motor girou para trás");
      client.println("<p><a href=\"/\"><button>Voltar</button></a></p>");
    } else if (request.indexOf("GET /") >= 0) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.println("<!DOCTYPE HTML>");
      client.println("<html>");
      client.println("<head><title>Controle de Motor de Passo</title></head>");
      client.println("<body>");
      client.println("<h1>Controle de Motor de Passo</h1>");
      client.println("<p><a href=\"/forward\"><button>Girar para Frente</button></a></p>");
      client.println("<p><a href=\"/backward\"><button>Girar para Trás</button></a></p>");
      client.println("</body>");
      client.println("</html>");
    }

    delay(1);
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}
