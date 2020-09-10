/* BMBS - Bruno Sá - www.bruno-sa.com */

#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>

#define MaxHeaderLength 255 //maximum length of http header required

// EEPROM size
#define espacoEEPROM 4096
/*
1024 bytes (1Kb) on the ATmega328P,
512 bytes on the ATmega168 and ATmega8,
4096 bytes (4 Kb) on the ATmega1280 and ATmega2560
*/

//reset function
void (*resetFunc)(void) = 0; //declare reset function at address 0

// function write string byte after byte in EEPROM
void eepromWriteStr(int eepromBlock, String text)
{
  int i;

  for (i = 0; i < text.length(); i++)
  {
    EEPROM.write(eepromBlock + i, text.charAt(i));

    if (i >= 1000)
    {
      break;
    }
  }

  if (i < 1000)
  {
    EEPROM.write(eepromBlock + i, (char)0);
  }
}

// function read string byte after byte from EEPROM
String eepromReadStr(int eepromBlock)
{
  String readStr = "";
  char readByte;
  int readEepromBlock = eepromBlock;

  do
  {
    readByte = EEPROM.read(readEepromBlock);
    readStr += readByte;
    readEepromBlock++;
  } while ((readByte != (char)0) && (readEepromBlock < (eepromBlock + 1000)));

  return readStr;
}

//BMBS IPconfig presence variables
int variavelCFGpos = 1;
String variavelCFG = "";

//BMBS IP Address from EEPROM
byte variavelIP1 = EEPROM.read(51);
byte variavelIP2 = EEPROM.read(52);
byte variavelIP3 = EEPROM.read(53);
byte variavelIP4 = EEPROM.read(54);

//BMBS Subnet Mask from EEPROM
byte variavelSB1 = EEPROM.read(56);
byte variavelSB2 = EEPROM.read(57);
byte variavelSB3 = EEPROM.read(58);
byte variavelSB4 = EEPROM.read(59);

//BMBS Gateway from EEPROM
byte variavelGW1 = EEPROM.read(61);
byte variavelGW2 = EEPROM.read(62);
byte variavelGW3 = EEPROM.read(63);
byte variavelGW4 = EEPROM.read(64);

//BMBS DNS from EEPROM
byte variavelDN1 = EEPROM.read(66);
byte variavelDN2 = EEPROM.read(67);
byte variavelDN3 = EEPROM.read(68);
byte variavelDN4 = EEPROM.read(69);

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

//BMBS declare HTTP string variables from form
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

//BMBS declare HTTP byte variables to write in EEPROM
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

//BMBS function HTTP GET byte from form
byte HttpHeaderValue(String fieldA, String fieldB)
{
  String HttpHeaderVAL;
  byte HttpHeaderVALbyte;
  HttpHeaderVAL = HttpHeader;
  HttpHeaderVAL.remove(HttpHeader.indexOf(fieldA) - 1, HttpHeader.length() - HttpHeader.indexOf(fieldA));
  HttpHeaderVAL.remove(0, HttpHeader.indexOf(fieldB) + 3);
  HttpHeaderVALbyte = HttpHeaderVAL.toInt();
  return HttpHeaderVALbyte;
};

//BMBS repeating HTML codes
String divRow = "<div class='form-row my-2'><div class='col-md-3 col-xs-12 text-nowrap'>";
String divClassInput0 = "<div class='col-2'><input class='form-control form-control-sm' type='number' size='3' max='255' name='";
String divClassInput1 = "' value='";
String divClassInput2 = "'></div>.";

void setup()
{
  variavelCFG = eepromReadStr(variavelCFGpos);

  //BMBS check previous BMBIPCONFIG
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
  }
  else
  {
    eepromWriteStr(variavelCFGpos, "BMB_ipconf");
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

          HttpHeaderIP1byte = HttpHeaderValue("I2=", "I1=");
          HttpHeaderIP2byte = HttpHeaderValue("I3=", "I2=");
          HttpHeaderIP3byte = HttpHeaderValue("I4=", "I3=");
          HttpHeaderIP4byte = HttpHeaderValue("S1=", "I4=");

          HttpHeaderSB1byte = HttpHeaderValue("S2=", "S1=");
          HttpHeaderSB2byte = HttpHeaderValue("S3=", "S2=");
          HttpHeaderSB3byte = HttpHeaderValue("S4=", "S3=");
          HttpHeaderSB4byte = HttpHeaderValue("G1=", "S4=");

          HttpHeaderGW1byte = HttpHeaderValue("G2=", "G1=");
          HttpHeaderGW2byte = HttpHeaderValue("G3=", "G2=");
          HttpHeaderGW3byte = HttpHeaderValue("G4=", "G3=");
          HttpHeaderGW4byte = HttpHeaderValue("D1=", "G4=");

          HttpHeaderDN1byte = HttpHeaderValue("D2=", "D1=");
          HttpHeaderDN2byte = HttpHeaderValue("D3=", "D2=");
          HttpHeaderDN3byte = HttpHeaderValue("D4=", "D3=");
          HttpHeaderDN4byte = HttpHeaderValue("HTT", "D4=");

          Serial.print("Length: ");
          Serial.println(HttpHeader.length());
          Serial.print("HttpHeader: ");
          Serial.println(HttpHeader);
          Serial.print("HttpHeader[0]: ");
          Serial.println(HttpHeader[0]);
          Serial.print("HttpHeader[1]: ");
          Serial.println(HttpHeader[1]);
          Serial.print("HttpHeader[2]: ");
          Serial.println(HttpHeader[2]);
          Serial.print("HttpHeader[3]: ");
          Serial.println(HttpHeader[3]);
          Serial.print("HttpHeader[4]: ");
          Serial.println(HttpHeader[4]);
          Serial.print("HttpHeader[5]: ");
          Serial.println(HttpHeader[5]);

          //BMBS web page's header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html lang='en'>");
          client.println("<head>");
          client.println("<meta charset='utf-8'>");
          client.println("<meta name='author' content='Bruno Sá - www.bruno-sa.com'>");
          client.println("<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>");
          client.println("<link href='https://fonts.googleapis.com/css?family=Didact Gothic' rel='stylesheet'>");
          client.println("<!-- Bootstrap CSS -->");
          client.println("<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css' integrity='sha384-JcKb8q3iqJ61gNV9KGb8thSsNjpSL0n8PARn9HuZOnIxN0hoP+VmmDGMN5t9UJ0Z' crossorigin='anonymous'>");
          client.println("<!-- jQuery UI CSS -->");
          client.println("<link rel='stylesheet' href='https://code.jquery.com/ui/1.12.1/themes/base/jquery-ui.css'>");

          //BMBS internal page: /ipcfg
          if (HttpHeader[5] == 105 && HttpHeader[6] == 112 && HttpHeader[7] == 99 && HttpHeader[8] == 102 && HttpHeader[9] == 103)
          {
            client.println("<title>IP Config</title>");
            client.println("</head>");
            client.println("<body style='font-family:Didact Gothic; color:#FFF; background-color:#333;'><div class='container'><h2><strong>IP Config Page</strong></h2>");
            client.println("<form><input type='hidden' name='CF' value='BMB_ipconf'>");
            client.print(divRow);
            client.print("IP Address: </div>");
            client.print(divClassInput0);
            client.print("I1");
            client.print(divClassInput1);
            client.print(ip[0]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("I2");
            client.print(divClassInput1);
            client.print(ip[1]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("I3");
            client.print(divClassInput1);
            client.print(ip[2]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("I4");
            client.print(divClassInput1);
            client.print(ip[3]);
            client.println("'></div></div>");
            client.print(divRow);
            client.print("Subnet Mask: </div>");
            client.print(divClassInput0);
            client.print("S1");
            client.print(divClassInput1);
            client.print(subnet[0]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("S2");
            client.print(divClassInput1);
            client.print(subnet[1]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("S3");
            client.print(divClassInput1);
            client.print(subnet[2]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("S4");
            client.print(divClassInput1);
            client.print(subnet[3]);
            client.println("'></div></div>");
            client.print(divRow);
            client.print("Gateway: </div>");
            client.print(divClassInput0);
            client.print("G1");
            client.print(divClassInput1);
            client.print(gateway[0]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("G2");
            client.print(divClassInput1);
            client.print(gateway[1]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("G3");
            client.print(divClassInput1);
            client.print(gateway[2]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("G4");
            client.print(divClassInput1);
            client.print(gateway[3]);
            client.println("'></div></div>");
            client.print(divRow);
            client.print("DNS: </div>");
            client.print(divClassInput0);
            client.print("D1");
            client.print(divClassInput1);
            client.print(dns[0]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("D2");
            client.print(divClassInput1);
            client.print(dns[1]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("D3");
            client.print(divClassInput1);
            client.print(dns[2]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print("D4");
            client.print(divClassInput1);
            client.print(dns[3]);
            client.println("'></div></div>");
            client.println("<div class='form-row my-2'><input class='btn btn-warning btn-sm' type='submit' value='submit'></div></form>");
          }

          //BMBS default internal page: /
          else
          {
            client.println("<title>Home</title>");
            client.println("</head>");
            client.println("<body style='font-family:Didact Gothic; color:#FFF; background-color:#333;'><div class='container'><h2><strong>Home Page</strong></h2>");
          }
          //BMBS web page's footer
          client.println("<div class='row justify-content-center'><div><a href='/'>home</a> | <a href='/ipcfg'>IP config</a></div></div>");
          client.println("</div>");
          client.println("<!-- Optional JavaScript -->");
          client.println("<!-- jQuery first, then Popper.js, then Bootstrap JS -->");
          client.println("<script src='https://code.jquery.com/jquery-3.5.1.js' integrity='sha256-QWo7LDvxbWT2tbbQ97B53yJnYU3WhH/C8ycbRAkjPDc=' crossorigin='anonymous'></script>");
          client.println("<script src='https://code.jquery.com/ui/1.12.1/jquery-ui.js' integrity='sha256-T0Vest3yCU7pafRw9r+settMBX6JkKN06dqBnpQ8d30=' crossorigin='anonymous'></script>");
          client.println("<script src='https://cdn.jsdelivr.net/npm/jquery-ui-touch-punch@0.2.3/jquery.ui.touch-punch.js' integrity='sha256-S9605h/+fTHx8kE89v4NQWuTGCEQJF0B9UGvMFYAiO8=' crossorigin='anonymous'></script>");
          client.println("<script src='https://unpkg.com/@popperjs/core@2'></script>");
          client.println("<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js' integrity='sha384-B4gt1jrGC7Jh4AgTPSdUtOBvfO8shuf57BaghqFfPlYxofvL8/KUEfYiJOMMV+rV' crossorigin='anonymous'></script>");
          client.println("</body></html>");

          //BMBS checking previous IP config to overwrite
          if (HttpHeaderCFG[0] == 66 && HttpHeaderCFG[1] == 77 && HttpHeaderCFG[2] == 66 && HttpHeaderCFG[3] == 95 && HttpHeaderCFG[4] == 105 && HttpHeaderCFG[5] == 112 && HttpHeaderCFG[6] == 99 && HttpHeaderCFG[7] == 111 && HttpHeaderCFG[8] == 110 && HttpHeaderCFG[9] == 102)
          {
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
