//include All the required library
#include <EEPROM.h> // This enables a tiny hard drive even when it becomes power off 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "lwip/lwip_napt.h"
#include "lwip/app/dhcpserver.h"



//EPSW(E_MOBILE_330,"MOBILE");

//
//void EPSW(int p_start_posn, char p_string[] ) { //EEPROMStringWrite
////Write a string to EEPROM and terminate it with a NULL
////We read first and do not rewrite bytes/chars that are unchanged
//
//  for (int l_posn = 0; l_posn < (int) strlen(p_string); l_posn ++) {
//  byte l_byte = (byte) p_string[l_posn];
//  byte l_read = EEPROM.read(p_start_posn + l_posn);
//  if (l_read != l_byte) {
//    EEPROM.write(p_start_posn + l_posn, l_byte);
//  }
//  }
//  //write the NULL termination
//  if (EEPROM.read(p_start_posn + strlen(p_string)) != 0)
//  EEPROM.write(p_start_posn + strlen(p_string), 0);
//  //
//} //EPSW
//
//
//
//
//String l_mobile = EPSR(E_MOBILE_330);
//
//String EPSR(int p_start_posn) { //EEPROMStringRead
////Read a NULL terminated string from EEPROM
////Only strings up to 128 bytes are supported
//  byte l_byte;
//
//  //Count first, reserve exact string length and then extract
//  int l_posn = 0;
//  while (true) {
//  l_byte = EEPROM.read(p_start_posn + l_posn);
//  if (l_byte == 0) {
//    break;
//  }
//  l_posn ++;
//  }
//
//  //Now extract the string
//  String l_string = "";
//  l_string.reserve(l_posn + 1);
//  char l_char;
//  l_posn = 0;
//  while (true) {
//  l_byte = EEPROM.read(p_start_posn + l_posn);
//  if (l_byte == 0) {
//    break;
//  }
//  l_char = (char) l_byte;
//  l_string += l_char;
//  l_posn ++;
//  if (l_posn == 128)
//    break;
//  //
//  }
//  return l_string;
//} //EPSR



String  sta_ssid = "";

// we are creating a test AP so that an user can connect to the server by entering into the Test AP.
// the name will be "test" 
const char* ap_ssid = "test";
const char* ap_pass = "";


//Variable for current status of ST and AP
String station_st = "Not connected to any ST";
String ap_st = "Current AP is : " + String(ap_ssid) ;

// Setting the server IP at 192.168.4.1 and the port at 80 
IPAddress server_ip(192, 168, 4, 1);
ESP8266WebServer server(server_ip, 80);    // Create a webserver object that listens for HTTP request on port 80

//Prototyping the function that we define below the setup and loop function
void handleRoot();        // function prototypes for HTTP handlers that will handle the "/" page
void handlePutAP();       // the function will handle when AP set button will be clicked
void handlePutSt();     // the function will handle the Station connect button 
void handleNotFound();  // if the url not found what to do, the function will handle this

void setup()
{
  ap_st = EEPROM.read(ap_ssid);
  if
  
  Serial.begin(115200); //beginning serial communicaiton
  Serial.println();
  WiFi.mode(WIFI_AP_STA); //Setting WiFi mode to AP and STA mode
  Serial.println("Configuring access point...");
  WiFi.softAP(ap_ssid); //createing initial test AP


  // declaring the page handler
  server.on("/", HTTP_GET, handleRoot); //when GET request will be send to http://192.168.4.1/
  server.on("/putst", HTTP_POST, handlePutSt); //when POST request will be send to http://192.168.4.1/putst actually for handling station
  server.on("/putap", HTTP_POST, handlePutAP); //when POST request will be send to http://192.168.4.1/putap actually for handling Access Point
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();                     // Listen for HTTP requests from clients
}

// declaring a function to know the Encryption type
String printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      return "WEP";
      break;
    case ENC_TYPE_TKIP:
      return "WPA";
      break;
    case ENC_TYPE_CCMP:
      return "WPA2";
      break;
    case ENC_TYPE_NONE:
      return "OPEN";
      break;
    default:
      return "PASSWORD PROTECTED";
      break;
  }
}
void handleRoot() {     // When URI / is requested, send a web page
  String disableAP = "";
  String messAP = "";
  if (station_st =="Not connected to any ST")
  {
    disableAP = "disabled";
    messAP = "<p>You must be connected to st if you want to create an AP </p>";
  }

  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid;
  for (int i=0; i<3; i++)
  {
    numSsid = WiFi.scanNetworks();
    if (numSsid != -1) break;
    delay(500);
  }
  
  String ssids = "";
  if (numSsid != -1) { // if numSsid is not empty
    for (int thisNet = 0; thisNet < numSsid; thisNet++) {
      Serial.print(WiFi.SSID(thisNet));
      //adding the option in the SSID select option in the Station form
      ssids += "<option value='" + WiFi.SSID(thisNet) + "'>" + WiFi.SSID(thisNet) + "---> encType : " +  printEncryptionType(WiFi.encryptionType(thisNet)) + "</option>";
    }
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);
  //added the minified html code for the index page. in the single line. Consider editing if need to change any text(message)
  server.send(200, "text/html", "<!DOCTYPE html><html><head> <meta charset='utf-8'> <meta http-equiv='X-UA-Compatible' content='IE=edge'> <title>ESP WIFI NAT</title> <meta name='viewport' content='width=device-width, initial-scale=1'> <script type='text/javascript'> function togglevisiblity(){console.log('i am here'); var t=document.getElementById('enctype'); var selectedText=t.options[t.selectedIndex].value; var d=document.getElementById('hidethis'); if (selectedText=='open'){d.style.display='none';}else{d.style.display='block';}}</script></head><body> <h1>ESP WIFI NAT Router Config</h1> <h3>STA Setting</h3> <p>" + station_st + " </p><form action='/putst' method='post'> SSID:<br><select name='ssid'>" + ssids + " </select><br>consider a reload if you can't see your ssid.<br><br>Password:<br><input type='password' name='password' placeholder='password'><br><br><input type='submit' value='connect'> </form> <h3>AP Settings</h3> <p> " + ap_st + "</p> " + messAP + "<form action='/putap' method='post'> SSID: <br><input type='text' placeholder='myap' name='MyAP'><br>Security Type<br><select name='enctype' id='enctype' onchange='togglevisiblity()'> <option value='encrypted'>WPA / WPA2</option> <option value='open'>open</option> </select><br><div id='hidethis' style='display:block;'> Password: <br><input type='password' name='mypass' placeholder='none'> </div><br><input type='submit' value='Set' "+ disableAP+"> <p> Password: min <strong>8</strong> chars </form> <p> If you set new AP The server will stop responding as AP will be changed.<br> Make sure to be in the new AP Network you created to again access the server.</p></body></html>");  
}

void handlePutSt() {                         // If a POST request is made to URI /putst
  if (server.hasArg("ssid") || server.hasArg("password")) { //checking if the ssid and password is posted in the url. It will be added as args in the server method
    WiFi.mode(WIFI_AP_STA); 
    if (server.arg("password") == "") { // if no password is entered. Means connecting to Open Station Network
      WiFi.begin(server.arg("ssid"));
    }
    else { // if password is entered. Connecting to Protected Network
      WiFi.begin(server.arg("ssid"),server.arg("password"));
    }
  // if not connected yet wait for 1 second 
    if (WiFi.status() != WL_CONNECTED) { 
    Serial.print("Connecting");
      delay(1000);
    }
    // If now it is not connected means there is some problem connecting to it. May be Not accepting connection or the password is not correctly entered.
    if (WiFi.status() != WL_CONNECTED) {
      server.send(200, "text/html", "<!DOCTYPE html><html><head> <meta charset='utf-8'> <meta http-equiv='X-UA-Compatible' content='IE=edge'> <title>ESP WIFI NAT</title> <meta name='viewport' content='width=device-width, initial-scale=1'> <script ></script></head><body> <h3>We have a problem connecting with the Network. Make Sure you have entered the right password </h3><p> Go back to <a href='/'>Home</a></body> </html>");
    }
    else { //if connected
      sta_ssid = WiFi.SSID();
      station_st = "connected to '" + sta_ssid + "' as station ";
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(sta_ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.print("dnsIP address: ");
      Serial.println(WiFi.dnsIP());
      Serial.print("gatewayIP address: ");
      Serial.println(WiFi.gatewayIP());
      Serial.print("subnetMask address: ");
      Serial.println(WiFi.subnetMask());
      server.send(200, "text/html", "<!DOCTYPE html><html><head> <meta charset='utf-8'> <meta http-equiv='X-UA-Compatible' content='IE=edge'> <title>ESP WIFI NAT</title> <meta name='viewport' content='width=device-width, initial-scale=1'> <script ></script></head><body> <h3>Your request have been processed</h3><p> Go back to <a href='/'>Home</a></body> </html>");
    }
  }
}

void handlePutAP() {
  if (server.hasArg("MyAP")) {
    Serial.println("");
    Serial.println("Configuring access point...");
    // We will only allow if password is null or password is more than or equal to 8 chars. Else We will not do anything
    if ((server.arg("mypass") == "") || (server.arg("mypass").length() >= 8 ) ) {
      server.send(200, "text/html", "<!DOCTYPE html><html><head> <meta charset='utf-8'> <meta http-equiv='X-UA-Compatible' content='IE=edge'> <title>ESP WIFI NAT</title> <meta name='viewport' content='width=device-width, initial-scale=1'> <script ></script></head><body> <p>Your AP Will be changed within short time.<br> The server will stop Responding when the AP will be created </p> <strong>To access The server again please connect this divice into the New  AP and click here <a href='http://192.168.4.1/'>http://192.168.4.1/</a></h3></body> </html>");
      delay(100); // delay to reset the server so that the user can see the message
      if (server.arg("mypass") == "") { // if open AP is given
        WiFi.softAP(server.arg("MyAP")); 
      }
      else { // If protected AP is given
        WiFi.softAP(server.arg("MyAP"), server.arg("mypass"));
      }
      ap_st = "Current access point name is : " + server.arg("MyAP") ; //Updating Access Point status
      //setting the server again..
      IPAddress server_ip(192, 168, 4, 1);
      ESP8266WebServer server(server_ip, 80);
      server.begin();
      IPAddress myIP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(myIP);

      // Initialize the NAT feature
      ip_napt_init(IP_NAPT_MAX, IP_PORTMAP_MAX);

      // Enable NAT on the AP interface
      ip_napt_enable_no(1, 1);

      // Set the DNS server for clients of the AP to the one we also use for the STA interface
      dhcps_set_DNS(WiFi.dnsIP());
    }
    else {
      server.send(200, "text/html", "<!DOCTYPE html><html><head> <meta charset='utf-8'> <meta http-equiv='X-UA-Compatible' content='IE=edge'> <title>ESP WIFI NAT</title> <meta name='viewport' content='width=device-width, initial-scale=1'> <script ></script></head><body> <h3>Make sure you insert password at least 8 characters long or leave it empty to make an open AP</h3><p> Go back to <a href='/'>Home</a></body> </html>");
    }
  }
}


void handleNotFound() {
  server.send(200, "text/html", "<!DOCTYPE html><html><head> <meta charset='utf-8'> <meta http-equiv='X-UA-Compatible' content='IE=edge'> <title>ESP WIFI NAT</title> <meta name='viewport' content='width=device-width, initial-scale=1'> <script ></script></head><body> <h3>Page not found</h3><p> Go back to <a href='/'>Home</a></body> </html>");
// Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
