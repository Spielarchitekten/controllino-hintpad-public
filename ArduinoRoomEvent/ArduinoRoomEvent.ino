/*

  Example sketch for sending Room Events to Hintpad
  from Arduino via ethernet

  https://www.arduino.cc/en/Reference/Ethernet

*/

#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // unique mac adress
IPAddress ip (192, 168, 0, 2);        // IP of Arduino (if no dhcp is available)
IPAddress node (192, 168, 0, 1);      // IP of HINTPAD node
EthernetClient gmClient;
int nodePort = 4010;
String nodePath = "/api/external-logs";

void setup() {
  Serial.begin(9600);
  //Ethernet.begin(mac, ip);  // using static IP
  Ethernet.begin(mac);        // using dhcp
  printIP();
}

void loop() {
  // you are allowed to use html tags inside the room event message
  sendRoomEvent ("TEST<br><font style = 'color:red' ><b>TEST</b></font><br>TEST<br><br><b>TEST</b>");
  delay(5000);
}

void sendRoomEvent (String theMessage) {
  String postData =  "{\"data\":{\"content\":\"";
  postData += theMessage;
  postData += "\" , \"origin\":\"Arduino\"}}";
  Serial.println (postData);
  gmClient.stop();
  if (gmClient.connect(node, nodePort)) {
      gmClient.print("POST "); gmClient.print(nodePath); gmClient.println(" HTTP/1.1");
      gmClient.print("Host:"); gmClient.println( node );
      gmClient.println("User-Agent: Arduino/1.0");
      gmClient.println("Connection: close");
      gmClient.println("Content-Type: application/json;charset=utf-8");
      gmClient.print ("Content-Length: "); gmClient.println (postData.length());
      gmClient.println();
      gmClient.println(postData);    
  }
}

void printIP() {
  Serial.print("My IP address: ");
  IPAddress myip = Ethernet.localIP();
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(myip[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println(); 
}

