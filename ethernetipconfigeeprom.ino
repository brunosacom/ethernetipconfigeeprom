#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

//no vscode

#define MaxHeaderLength 255 //maximum length of http header required

#define espacoEEPROM 4096 //1024 bytes (1Kb) on the ATmega328P,
                          //512 bytes on the ATmega168 and ATmega8,
                          //4096 bytes (4 Kb) on the ATmega1280 and ATmega2560

//reset function
void (*resetFunc)(void) = 0; //declare reset function at address 0

// function write string byte after byte in EEPROM (based on Flavio Guimarães' code)
void EEPROMWriteStr(int address, String value)
{
  int i;

  for (i = 0; i < value.length(); i++)
  {
    EEPROM.write(address + i, value.charAt(i));

    if (i >= 1000)
    {
      break;
    }
  }

  if (i < 1000)
  {
    EEPROM.write(address + i, (char)0);
  }
}

// function read string byte after byte from EEPROM  (based on Flavio Guimarães' code)
String EEPROMReadStr(int address)
{
  String readStr = "";
  char readByte;
  int readAddress = address;

  do
  {
    readByte = EEPROM.read(readAddress);
    readStr += readByte;
    readAddress++;
  } while ((readByte != (char)0) && (readAddress < (address + 1000)));

  return readStr;
}

//BMB IPconfig presence variables
int variavelCFGpos = 1;
String variavelCFG = "";

//BMB IP Address
byte variavelIP1 = EEPROM.read(51); //Posicao 51 da EEPROM
byte variavelIP2 = EEPROM.read(52); //Posicao 52 da EEPROM
byte variavelIP3 = EEPROM.read(53); //Posicao 53 da EEPROM
byte variavelIP4 = EEPROM.read(54); //Posicao 54 da EEPROM

//BMB Subnet Mask
byte variavelSB1 = EEPROM.read(56); //Posicao 56 da EEPROM
byte variavelSB2 = EEPROM.read(57); //Posicao 57 da EEPROM
byte variavelSB3 = EEPROM.read(58); //Posicao 58 da EEPROM
byte variavelSB4 = EEPROM.read(59); //Posicao 59 da EEPROM

//BMB Gateway
byte variavelGW1 = EEPROM.read(61); //Posicao 61 da EEPROM
byte variavelGW2 = EEPROM.read(62); //Posicao 62 da EEPROM
byte variavelGW3 = EEPROM.read(63); //Posicao 63 da EEPROM
byte variavelGW4 = EEPROM.read(64); //Posicao 64 da EEPROM

//BMB DNS
byte variavelDN1 = EEPROM.read(66); //Posicao 66 da EEPROM
byte variavelDN2 = EEPROM.read(67); //Posicao 67 da EEPROM
byte variavelDN3 = EEPROM.read(68); //Posicao 68 da EEPROM
byte variavelDN4 = EEPROM.read(69); //Posicao 69 da EEPROM

// IP config defaults
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; //physical mac address
byte ip[] = {
    192, 168, 0, 95}; // static ip of Arduino
byte subnet[] = {
    255, 255, 255, 0}; //subnet mask
byte gateway[] = {
    192, 168, 0, 1}; // gateway address
byte dns[] = {
    192, 168, 0, 1}; // dns address
EthernetServer server(80); //web server port

String HttpHeader = String(MaxHeaderLength);

String HttpHeaderCFG;

String HttpHeaderIP1;
String HttpHeaderIP2;
String HttpHeaderIP3;
String HttpHeaderIP4;

String HttpHeaderSB1;
String HttpHeaderSB2;
String HttpHeaderSB3;
String HttpHeaderSB4;

String HttpHeaderGW1;
String HttpHeaderGW2;
String HttpHeaderGW3;
String HttpHeaderGW4;

String HttpHeaderDN1;
String HttpHeaderDN2;
String HttpHeaderDN3;
String HttpHeaderDN4;

byte HttpHeaderCFGbyte;

byte HttpHeaderIP1byte;
byte HttpHeaderIP2byte;
byte HttpHeaderIP3byte;
byte HttpHeaderIP4byte;

byte HttpHeaderSB1byte;
byte HttpHeaderSB2byte;
byte HttpHeaderSB3byte;
byte HttpHeaderSB4byte;

byte HttpHeaderGW1byte;
byte HttpHeaderGW2byte;
byte HttpHeaderGW3byte;
byte HttpHeaderGW4byte;

byte HttpHeaderDN1byte;
byte HttpHeaderDN2byte;
byte HttpHeaderDN3byte;
byte HttpHeaderDN4byte;

void setup()
{
  variavelCFG = EEPROMReadStr(variavelCFGpos);

  pinMode(LED_BUILTIN, OUTPUT);

  //BMB check previous BMBIPCONFIG
  if (variavelCFG == "BMB_ipconf")
  {
    for (int i = 0; i < 4; i++)
    {
      ip[i] = EEPROM.read(i + 51);
    }
    for (int i = 0; i < 4; i++)
    {
      subnet[i] = EEPROM.read(i + 56);
    }
    for (int i = 0; i < 4; i++)
    {
      gateway[i] = EEPROM.read(i + 61);
    }
    for (int i = 0; i < 4; i++)
    {
      dns[i] = EEPROM.read(i + 66);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    EEPROMWriteStr(variavelCFGpos, "BMB_ipconf");
  }
  
  ;
  //start Ethernet
  Ethernet.begin(mac, ip, dns, gateway, subnet);

  //enable serial monitor
  Serial.begin(9600);

  //initialize variable
  HttpHeader = "";
}

void loop()
{
  // Create a client connection
  EthernetClient client = server.available();
  if (client)
  {
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        //read MaxHeaderLength number of characters in the HTTP header
        //discard the rest until \n
        if (HttpHeader.length() < MaxHeaderLength)
        {
          //store characters to string
          HttpHeader = HttpHeader + c;
        }

        //if HTTP request has ended
        if (c == '\n')
        {

          HttpHeaderCFG = HttpHeader;
          HttpHeaderCFG.remove(HttpHeader.indexOf("I1=") - 1, HttpHeader.length() - HttpHeader.indexOf("I1="));
          HttpHeaderCFG.remove(0, HttpHeader.indexOf("CF=") + 3);
          HttpHeaderCFGbyte = HttpHeaderCFG[0];

          HttpHeaderIP1 = HttpHeader;
          HttpHeaderIP1.remove(HttpHeader.indexOf("I2=") - 1, HttpHeader.length() - HttpHeader.indexOf("I2="));
          HttpHeaderIP1.remove(0, HttpHeader.indexOf("I1=") + 3);
          HttpHeaderIP1byte = HttpHeaderIP1.toInt();

          HttpHeaderIP2 = HttpHeader;
          HttpHeaderIP2.remove(HttpHeader.indexOf("I3=") - 1, HttpHeader.length() - HttpHeader.indexOf("I3="));
          HttpHeaderIP2.remove(0, HttpHeader.indexOf("I2=") + 3);
          HttpHeaderIP2byte = HttpHeaderIP2.toInt();

          HttpHeaderIP3 = HttpHeader;
          HttpHeaderIP3.remove(HttpHeader.indexOf("I4=") - 1, HttpHeader.length() - HttpHeader.indexOf("I4="));
          HttpHeaderIP3.remove(0, HttpHeader.indexOf("I3=") + 3);
          HttpHeaderIP3byte = HttpHeaderIP3.toInt();

          HttpHeaderIP4 = HttpHeader;
          HttpHeaderIP4.remove(HttpHeader.indexOf("S1=") - 1, HttpHeader.length() - HttpHeader.indexOf("S1="));
          HttpHeaderIP4.remove(0, HttpHeader.indexOf("I4=") + 3);
          HttpHeaderIP4byte = HttpHeaderIP4.toInt();

          HttpHeaderSB1 = HttpHeader;
          HttpHeaderSB1.remove(HttpHeader.indexOf("S2=") - 1, HttpHeader.length() - HttpHeader.indexOf("S2="));
          HttpHeaderSB1.remove(0, HttpHeader.indexOf("S1=") + 3);
          HttpHeaderSB1byte = HttpHeaderSB1.toInt();

          HttpHeaderSB2 = HttpHeader;
          HttpHeaderSB2.remove(HttpHeader.indexOf("S3=") - 1, HttpHeader.length() - HttpHeader.indexOf("S3="));
          HttpHeaderSB2.remove(0, HttpHeader.indexOf("S2=") + 3);
          HttpHeaderSB2byte = HttpHeaderSB2.toInt();

          HttpHeaderSB3 = HttpHeader;
          HttpHeaderSB3.remove(HttpHeader.indexOf("S4=") - 1, HttpHeader.length() - HttpHeader.indexOf("S4="));
          HttpHeaderSB3.remove(0, HttpHeader.indexOf("S3=") + 3);
          HttpHeaderSB3byte = HttpHeaderSB3.toInt();

          HttpHeaderSB4 = HttpHeader;
          HttpHeaderSB4.remove(HttpHeader.indexOf("G1=") - 1, HttpHeader.length() - HttpHeader.indexOf("G1="));
          HttpHeaderSB4.remove(0, HttpHeader.indexOf("S4=") + 3);
          HttpHeaderSB4byte = HttpHeaderSB4.toInt();

          HttpHeaderGW1 = HttpHeader;
          HttpHeaderGW1.remove(HttpHeader.indexOf("G2=") - 1, HttpHeader.length() - HttpHeader.indexOf("G2="));
          HttpHeaderGW1.remove(0, HttpHeader.indexOf("G1=") + 3);
          HttpHeaderGW1byte = HttpHeaderGW1.toInt();

          HttpHeaderGW2 = HttpHeader;
          HttpHeaderGW2.remove(HttpHeader.indexOf("G3=") - 1, HttpHeader.length() - HttpHeader.indexOf("G3="));
          HttpHeaderGW2.remove(0, HttpHeader.indexOf("G2=") + 3);
          HttpHeaderGW2byte = HttpHeaderGW2.toInt();

          HttpHeaderGW3 = HttpHeader;
          HttpHeaderGW3.remove(HttpHeader.indexOf("G4=") - 1, HttpHeader.length() - HttpHeader.indexOf("G4="));
          HttpHeaderGW3.remove(0, HttpHeader.indexOf("G3=") + 3);
          HttpHeaderGW3byte = HttpHeaderGW3.toInt();

          HttpHeaderGW4 = HttpHeader;
          HttpHeaderGW4.remove(HttpHeader.indexOf("D1=") - 1, HttpHeader.length() - HttpHeader.indexOf("D1="));
          HttpHeaderGW4.remove(0, HttpHeader.indexOf("G4=") + 3);
          HttpHeaderGW4byte = HttpHeaderGW4.toInt();

          HttpHeaderDN1 = HttpHeader;
          HttpHeaderDN1.remove(HttpHeader.indexOf("D2=") - 1, HttpHeader.length() - HttpHeader.indexOf("D2="));
          HttpHeaderDN1.remove(0, HttpHeader.indexOf("D1=") + 3);
          HttpHeaderDN1byte = HttpHeaderDN1.toInt();

          HttpHeaderDN2 = HttpHeader;
          HttpHeaderDN2.remove(HttpHeader.indexOf("D3=") - 1, HttpHeader.length() - HttpHeader.indexOf("D3="));
          HttpHeaderDN2.remove(0, HttpHeader.indexOf("D2=") + 3);
          HttpHeaderDN2byte = HttpHeaderDN2.toInt();

          HttpHeaderDN3 = HttpHeader;
          HttpHeaderDN3.remove(HttpHeader.indexOf("D4=") - 1, HttpHeader.length() - HttpHeader.indexOf("D4="));
          HttpHeaderDN3.remove(0, HttpHeader.indexOf("D3=") + 3);
          HttpHeaderDN3byte = HttpHeaderDN3.toInt();

          HttpHeaderDN4 = HttpHeader;
          HttpHeaderDN4.remove(HttpHeader.indexOf("HTT") - 1, HttpHeader.length() - HttpHeader.indexOf("HTT"));
          HttpHeaderDN4.remove(0, HttpHeader.indexOf("D4=") + 3);
          HttpHeaderDN4byte = HttpHeaderDN4.toInt();

          Serial.print("Length: ");
          Serial.println(HttpHeader.length());
          Serial.print("HttpHeader: ");
          Serial.println(HttpHeader);
          Serial.print("IndexOf.IP1: ");
          Serial.println(HttpHeader.indexOf("IP1="));
          Serial.print("IndexOf.IP2: ");
          Serial.println(HttpHeader.indexOf("IP2="));
          Serial.print("IndexOf.IP3: ");
          Serial.println(HttpHeader.indexOf("IP3="));
          Serial.print("IndexOf.IP4: ");
          Serial.println(HttpHeader.indexOf("IP4="));
          Serial.print("IP1: " + HttpHeaderIP1);
          Serial.print("IP2: " + HttpHeaderIP2);
          Serial.print("IP3: " + HttpHeaderIP3);
          Serial.print("IP4: " + HttpHeaderIP4);
          Serial.print("HttpHeader[0]: ");
          Serial.println(HttpHeader[0]);
          Serial.print("EEProm[51]: ");
          Serial.println(EEPROM.read(51));
          Serial.print("EEProm[52]: ");
          Serial.println(EEPROM.read(52));
          Serial.print("EEProm[53]: ");
          Serial.println(EEPROM.read(53));
          Serial.print("EEProm[54]: ");
          Serial.println(EEPROM.read(54));
          Serial.print("D1: " + HttpHeaderDN1);
          Serial.print("D2: " + HttpHeaderDN2);
          Serial.print("D3: " + HttpHeaderDN3);
          Serial.print("D4: " + HttpHeaderDN4);
          Serial.print("D3byte: ");
          Serial.println(HttpHeaderDN3byte);
          Serial.print("D4byte: ");
          Serial.println(HttpHeaderDN4byte);
          Serial.print("IndexOf.D4: ");
          Serial.println(HttpHeader.indexOf("D4="));
          Serial.print("CFGbyte: ");
          Serial.println(HttpHeaderCFGbyte);
          Serial.print("EEProm[69]: ");
          Serial.println(EEPROM.read(69));
          Serial.print("ip[3]: ");
          Serial.println(ip[3]);
          Serial.print("variavelCFG: ");
          Serial.println(variavelCFG);
          Serial.println("0123456789");
          Serial.print("HttpHeaderCFG: ");
          Serial.println(HttpHeaderCFG);

          if (HttpHeader[0] == 'A')
          {
            digitalWrite(LED_BUILTIN, HIGH);
          }
          if (HttpHeader[0] == 'a')
          {
            digitalWrite(LED_BUILTIN, LOW);
          }

          // start of web page
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html lang='en'>");
          client.println("<head>");
          client.println("<title>IP Config</title>");
          client.println("<meta charset='utf-8'>");
          client.println("<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>");
          client.println("<link href='https://fonts.googleapis.com/css?family=Didact Gothic' rel='stylesheet'>");
          client.println("<!-- Bootstrap CSS -->");
          client.println("<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css' integrity='sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z' crossorigin='anonymous'>");
          client.println("<!-- jQuery UI CSS -->");
          client.println("<link rel='stylesheet' href='https://code.jquery.com/ui/1.12.1/themes/base/jquery-ui.css'>");
          client.println("<!-- jQuery first, then Popper.js, then Bootstrap JS -->");
          client.println("<script src='https://code.jquery.com/jquery-3.5.1.js' integrity='sha256-QWo7LDvxbWT2tbbQ97B53yJnYU3WhH/C8ycbRAkjPDc=' crossorigin='anonymous'></script>");
          client.println("<!-- Optional JavaScript -->");
          client.println("<script src='https://code.jquery.com/ui/1.12.1/jquery-ui.js' integrity='sha256-T0Vest3yCU7pafRw9r+settMBX6JkKN06dqBnpQ8d30=' crossorigin='anonymous'></script>");
          client.println("<script src='https://cdn.jsdelivr.net/npm/jquery-ui-touch-punch@0.2.3/jquery.ui.touch-punch.js' integrity='sha256-S9605h/+fTHx8kE89v4NQWuTGCEQJF0B9UGvMFYAiO8=' crossorigin='anonymous'></script>");
          client.println("<script src='https://unpkg.com/@popperjs/core@2'></script>");
          client.println("<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js' integrity='sha384-B4gt1jrGC7Jh4AgTPSdUtOBvfO8shuf57BaghqFfPlYxofvL8/KUEfYiJOMMV+rV' crossorigin='anonymous'></script>");
          client.println("</head>");
          client.println("<body style='font-family:Didact Gothic;'><div class='container'><h2><strong>IP Config Page</strong></h2>");
          client.println("<form><input type='hidden' name='CF' value='BMB_ipconf'>");
          client.print("<div class='form-row my-2'><div class='col-md-3 col-xs-12 text-nowrap'>IP address: </div><div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='I1' value='");
          client.print(ip[0]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='I2' value='");
          client.print(ip[1]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='I3' value='");
          client.print(ip[2]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='I4' value='");
          client.print(ip[3]);
          client.println("'></div></div>");

          client.print("<div class='form-row my-2'><div class='col-md-3 col-xs-12 text-nowrap'>Subnet Mask: </div><div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='S1' value='");
          client.print(subnet[0]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='S2' value='");
          client.print(subnet[1]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='S3' value='");
          client.print(subnet[2]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='S4' value='");
          client.print(subnet[3]);
          client.println("'></div></div>");

          client.print("<div class='form-row my-2'><div class='col-md-3 col-xs-12 text-nowrap'>Gateway: </div><div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='G1' value='");
          client.print(gateway[0]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='G2' value='");
          client.print(gateway[1]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='G3' value='");
          client.print(gateway[2]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='G4' value='");
          client.print(gateway[3]);
          client.println("'></div></div>");

          client.print("<div class='form-row my-2'><div class='col-md-3 col-xs-12 text-nowrap'>DNS: </div><div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='D1' value='");
          client.print(dns[0]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='D2' value='");
          client.print(dns[1]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='D3' value='");
          client.print(dns[2]);
          client.print("'></div>.");
          client.print("<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='D4' value='");
          client.print(dns[3]);
          client.println("'></div></div>");
          client.println("<div class='form-row my-2'><input class='btn btn-warning btn-sm' type='submit' value='submit'></div></form>");
          client.println("</div></body>");
          client.println("</html>");

          if (HttpHeaderCFG[0] == 66 && HttpHeaderCFG[1] == 77 && HttpHeaderCFG[2] == 66 && HttpHeaderCFG[3] == 95 && HttpHeaderCFG[4] == 105 && HttpHeaderCFG[5] == 112 && HttpHeaderCFG[6] == 99 && HttpHeaderCFG[7] == 111 && HttpHeaderCFG[8] == 110 && HttpHeaderCFG[9] == 102)
          {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(500);
            digitalWrite(LED_BUILTIN, LOW);
            delay(500);
            digitalWrite(LED_BUILTIN, HIGH);
            delay(500);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000);
            digitalWrite(LED_BUILTIN, HIGH);
            delay(1000);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000);
            digitalWrite(LED_BUILTIN, HIGH);
            EEPROM.write(51, HttpHeaderIP1byte);
            EEPROM.write(52, HttpHeaderIP2byte);
            EEPROM.write(53, HttpHeaderIP3byte);
            EEPROM.write(54, HttpHeaderIP4byte);

            EEPROM.write(56, HttpHeaderSB1byte);
            EEPROM.write(57, HttpHeaderSB2byte);
            EEPROM.write(58, HttpHeaderSB3byte);
            EEPROM.write(59, HttpHeaderSB4byte);

            EEPROM.write(61, HttpHeaderGW1byte);
            EEPROM.write(62, HttpHeaderGW2byte);
            EEPROM.write(63, HttpHeaderGW3byte);
            EEPROM.write(64, HttpHeaderGW4byte);

            EEPROM.write(66, HttpHeaderDN1byte);
            EEPROM.write(67, HttpHeaderDN2byte);
            EEPROM.write(68, HttpHeaderDN3byte);
            EEPROM.write(69, HttpHeaderDN4byte);
            HttpHeader = "";
            HttpHeaderCFG = "";
            resetFunc();
          };

          //clearing string for next read
          HttpHeader = "";

          //stopping client
          client.stop();
        }
      }
    }
  }
}
