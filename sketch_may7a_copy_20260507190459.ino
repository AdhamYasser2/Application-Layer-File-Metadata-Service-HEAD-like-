#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Arduino static IP
IPAddress arduinoIP(192, 168, 1, 50);

// Mac Ethernet / Python server IP
IPAddress server(192, 168, 1, 10);


IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

EthernetClient client;

String oldHash = "";

void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac, arduinoIP, gateway, gateway, subnet);
  delay(1000);

  Serial.println("Arduino connected to network");
  Serial.print("Arduino IP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  String metadataResponse = requestMetadata();

  Serial.println("Metadata Response:");
  Serial.println(metadataResponse);

  String newHash = extractHash(metadataResponse);

  Serial.print("Old Hash: ");
  Serial.println(oldHash);

  Serial.print("New Hash: ");
  Serial.println(newHash);

  if (newHash != "" && newHash != oldHash) {
    Serial.println("File changed. Fetching full file...");
    oldHash = newHash;

    String fileContent = requestFile();
    Serial.println("File Content:");
    Serial.println(fileContent);
  } else {
    Serial.println("File not changed. No need to fetch full content.");
  }

  delay(10000); // check every 10 seconds
}

String requestMetadata() {
  String response = "";

  if (client.connect(server, 8000)) {
    client.println("GET /metadata HTTP/1.1");
    client.println("Host: 192.168.1.10");
    client.println("Connection: close");
    client.println();

    while (client.connected() || client.available()) {
      if (client.available()) {
        char c = client.read();
        response += c;
      }
    }

    client.stop();
  } else {
    Serial.println("Connection to server failed.");
  }

  return response;
}

String requestFile() {
  String response = "";

  if (client.connect(server, 8000)) {
    client.println("GET /file HTTP/1.1");
    client.println("Host: 192.168.1.10");
    client.println("Connection: close");
    client.println();

    while (client.connected() || client.available()) {
      if (client.available()) {
        char c = client.read();
        response += c;
      }
    }

    client.stop();
  } else {
    Serial.println("Connection to server failed.");
  }

  return response;
}

String extractHash(String response) {
  int hashIndex = response.indexOf("\"hash\"");
  if (hashIndex == -1) {
    return "";
  }

  int colonIndex = response.indexOf(":", hashIndex);
  int firstQuote = response.indexOf("\"", colonIndex);
  int secondQuote = response.indexOf("\"", firstQuote + 1);

  if (firstQuote == -1 || secondQuote == -1) {
    return "";
  }

  return response.substring(firstQuote + 1, secondQuote);
}