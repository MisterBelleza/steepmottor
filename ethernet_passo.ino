#include <Ethernet.h>
#include <Stepper.h>

// Configurações do motor de passo
const int stepsPerRevolution = 150; // Número de passos por volta (ajuste conforme seu motor)
Stepper myStepper(stepsPerRevolution, 3,4,5,6);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 124, 200);
EthernetServer server(80);

void setup() {
  Serial.begin(9600);

  myStepper.setSpeed(50); // Velocidade em RPM

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    while(true);
  }
  server.begin();

  Serial.print("Servidor iniciado em: ");
  Serial.println(Ethernet.localIP());
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
      client.println("<h1>"+thisString+"</h1>");
      myStepper.step(stepsPerRevolution);
      Serial.println("Motor girou para frente");
      client.println("<p><a href=\"/\"><button>Voltar</button></a></p>");
    } else if (request.indexOf("GET /backward") >= 0) {
      thisString = String("Girando Tras");
      client.println("<h1>"+thisString+"</h1>");
      myStepper.step(-stepsPerRevolution);
      Serial.println("Motor girou para trás");
      client.println("<p><a href=\"/\"><button>Voltar</button></a></p>");
    }else if(request.indexOf("GET /") >= 0){
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
    client.println("<p><a href=\"/backward\"><button>Girar para Tras</button></a></p>");
    client.println("</body>");
    client.println("</html>");
    }

    delay(1);
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}