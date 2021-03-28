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

//BMBS IPconfig presence variables
String variavelCFG = "";

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
byte HttpHeaderValue(String fieldA)
{
  String HttpHeaderVAL;
  byte HttpHeaderVALbyte;
  HttpHeaderVAL = HttpHeader;
  HttpHeaderVAL.remove(0, HttpHeaderVAL.indexOf(fieldA) + fieldA.length());
  HttpHeaderVAL.remove(HttpHeaderVAL.indexOf("&"), HttpHeaderVAL.length() - HttpHeaderVAL.indexOf("&") + 1);
  HttpHeaderVALbyte = HttpHeaderVAL.toInt();
  return HttpHeaderVALbyte;
};

//BMBS repeating HTML codes
String divRow = "<div class='col-sm-6'><div class='input-group mb-2 input-group-sm col-2'><span class='col-3 input-group-text text-nowrap'>";
String divClassInput0 = "<input class='form-control form-control-sm' type='number' max='255' name='";
String divClassInput1 = "' value='";
String divClassInput2 = "'><span class='input-group-text input-group-sm'>.</span>";

void setup()
{
  variavelCFG = ((String)char(EEPROM.read(1)) + char(EEPROM.read(2)) + char(EEPROM.read(3)) + char(EEPROM.read(4)) + char(EEPROM.read(5)) + char(EEPROM.read(6)) + char(EEPROM.read(7)) + char(EEPROM.read(8)) + char(EEPROM.read(9)) + char(EEPROM.read(10))); 


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
  };

  //start Ethernet
  Ethernet.begin(mac, ip, dns, gateway, subnet);

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

          HttpHeaderIP1byte = HttpHeaderValue("I1=");
          HttpHeaderIP2byte = HttpHeaderValue("I2=");
          HttpHeaderIP3byte = HttpHeaderValue("I3=");
          HttpHeaderIP4byte = HttpHeaderValue("I4=");

          HttpHeaderSB1byte = HttpHeaderValue("S1=");
          HttpHeaderSB2byte = HttpHeaderValue("S2=");
          HttpHeaderSB3byte = HttpHeaderValue("S3=");
          HttpHeaderSB4byte = HttpHeaderValue("S4=");

          HttpHeaderGW1byte = HttpHeaderValue("G1=");
          HttpHeaderGW2byte = HttpHeaderValue("G2=");
          HttpHeaderGW3byte = HttpHeaderValue("G3=");
          HttpHeaderGW4byte = HttpHeaderValue("G4=");

          HttpHeaderDN1byte = HttpHeaderValue("D1=");
          HttpHeaderDN2byte = HttpHeaderValue("D2=");
          HttpHeaderDN3byte = HttpHeaderValue("D3=");
          HttpHeaderDN4byte = HttpHeaderValue("D4=");

          //BMBS web page's header
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F(""));
          client.println(F("<!DOCTYPE HTML>"));
          client.println(F("<html lang='en'>"));
          client.println(F("<head>"));
          client.println(F("<meta charset='utf-8'>"));
          client.println(F("<meta name='author' content='Bruno Sá - www.bruno-sa.com'>"));
          client.println(F("<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>"));
          client.println(F("<link href='https://fonts.googleapis.com/css?family=Didact Gothic' rel='stylesheet'>"));
          client.println(F("<!-- Bootstrap CSS -->"));
          client.println(F("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-eOJMYsd53ii+scO/bJGFsiCZc+5NDVN2yr8+0RDqr0Ql0h+rP48ckxlpbzKgwra6' crossorigin='anonymous'>"));
          client.println(F("<!-- jQuery UI CSS -->"));
          client.println(F("<link rel='stylesheet' href='https://code.jquery.com/ui/1.12.1/themes/base/jquery-ui.css'>"));

          //BMBS internal page: /fsend
          //character conversion in https://www.arduino.cc/en/Reference/ASCIIchart
          if (HttpHeader[5] == 102 && HttpHeader[6] == 115 && HttpHeader[7] == 101 && HttpHeader[8] == 110 && HttpHeader[9] == 100)
          {
            client.println(F("<title>Form Sended</title>"));
            client.println(F("</head><body style='font-family:Didact Gothic; color:#FFF; background-color:#333;'><div class='container'><h2><strong>Form Sended</strong></h2>"));
            client.println(F("<h3><a href='/' class='row justify-content-center'><button class='btn btn-warning'>reload</button></a></h3><br><br>"));
          }

          //BMBS internal page: /ipcfg
          //character conversion in https://www.arduino.cc/en/Reference/ASCIIchart
          else if (HttpHeader[5] == 105 && HttpHeader[6] == 112 && HttpHeader[7] == 99 && HttpHeader[8] == 102 && HttpHeader[9] == 103)
          {
            client.println(F("<title>IP Config</title>"));
            client.println(F("</head>"));
            client.println(F("<body style='font-family:Didact Gothic; color:#FFF; background-color:#333;'><div class='container'><h2><strong>IP Config Page</strong></h2>"));
            client.println(F("<form action='/fsend'><input type='hidden' name='CF' value='BMB_ipconf'>"));
            client.print(divRow);
            client.print(F("IP Address: </span>"));
            client.print(divClassInput0);
            client.print(F("I1"));
            client.print(divClassInput1);
            client.print(ip[0]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("I2"));
            client.print(divClassInput1);
            client.print(ip[1]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("I3"));
            client.print(divClassInput1);
            client.print(ip[2]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("I4"));
            client.print(divClassInput1);
            client.print(ip[3]);
            client.println(F("'></div></div>"));
            client.print(divRow);
            client.print(F("Subnet Mask: </span>"));
            client.print(divClassInput0);
            client.print(F("S1"));
            client.print(divClassInput1);
            client.print(subnet[0]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("S2"));
            client.print(divClassInput1);
            client.print(subnet[1]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("S3"));
            client.print(divClassInput1);
            client.print(subnet[2]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("S4"));
            client.print(divClassInput1);
            client.print(subnet[3]);
            client.println(F("'></div></div>"));
            client.print(divRow);
            client.print(F("Gateway: </span>"));
            client.print(divClassInput0);
            client.print(F("G1"));
            client.print(divClassInput1);
            client.print(gateway[0]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("G2"));
            client.print(divClassInput1);
            client.print(gateway[1]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("G3"));
            client.print(divClassInput1);
            client.print(gateway[2]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("G4"));
            client.print(divClassInput1);
            client.print(gateway[3]);
            client.println(F("'></div></div>"));
            client.print(divRow);
            client.print(F("DNS: </span>"));
            client.print(divClassInput0);
            client.print(F("D1"));
            client.print(divClassInput1);
            client.print(dns[0]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("D2"));
            client.print(divClassInput1);
            client.print(dns[1]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("D3"));
            client.print(divClassInput1);
            client.print(dns[2]);
            client.print(divClassInput2);
            client.print(divClassInput0);
            client.print(F("D4"));
            client.print(divClassInput1);
            client.print(dns[3]);
            client.println(F("'></div></div>"));
            client.println(F("<div class='form-row my-2'><input class='btn btn-warning btn-sm' type='submit' value='submit'></div></form>"));
          }

          //BMBS default internal page: /
          else
          {
            client.println(F("<title>Home</title>"));
            client.println(F("</head>"));
            client.println(F("<body style='font-family:Didact Gothic; color:#FFF; background-color:#333;'><div class='container'><h2><strong>Home Page</strong></h2>"));
            client.println(F("<div class='row my-2'>"));
            client.println(F("<div class='col-3 bg-secondary'>Network</div>"));
            client.println(F("<div class='col'></div>"));
            client.println(F("</div>"));
            client.println(F("<div class='row my-2'>"));
            client.println((String)"<div class='col-3'>IP: " + ip[0] + "." + ip[1] + "." +  ip[2] + "." +  ip[3] + "</div>");
            client.println(F("<div class='col'></div>"));
            client.println(F("</div>"));
            client.println(F("<div class='row my-2'>"));
            client.println((String)"<div class='col-3'>Subnet Mask: " + subnet[0] + "." +  subnet[1] + "." +  subnet[2] + "." +  subnet[3] + "</div>");
            client.println(F("<div class='col'></div>"));
            client.println(F("</div>"));
            client.println(F("<div class='row my-2'>"));
            client.println((String)"<div class='col-3'>Gateway: " + gateway[0] + "." +  gateway[1] + "." +  gateway[2] + "." +  gateway[3] + "</div>");
            client.println(F("</div>"));
            client.println(F("<div class='row my-2'>"));
            client.println((String)"<div class='col-3'>DNS: " + dns[0] + "." +  dns[1] + "." +  dns[2] + "." +  dns[3] + "</div>");
            client.println(F("</div>"));
          }
          //BMBS web page's footer
          client.println(F("<div class='d-flex justify-content-center'><div><a href='/'>home</a> | <a href='/ipcfg'>IP config</a></div></div>"));
          client.println(F("<hr>"));
          client.println(F("<div class='d-flex justify-content-center'><div><small><small>Created by Bruno Sá - <a href='https://www.bruno-sa.com' target='_blank'>www.bruno-sa.com</a></small></small></div></div>"));
          client.println(F("</div>"));
          client.println(F("<!-- Optional JavaScript -->"));
          client.println(F("<!-- jQuery first, then Popper.js, then Bootstrap JS -->"));
          client.println(F("<script src='https://code.jquery.com/jquery-3.6.0.js' integrity='sha256-H+K7U5CnXl1h5ywQfKtSj8PCmoN9aaq30gDh27Xc0jk=' crossorigin='anonymous'></script>"));
          client.println(F("<script src='https://code.jquery.com/ui/1.12.1/jquery-ui.js' integrity='sha256-T0Vest3yCU7pafRw9r+settMBX6JkKN06dqBnpQ8d30=' crossorigin='anonymous'></script>"));
          client.println(F("<script src='https://cdn.jsdelivr.net/npm/jquery-ui-touch-punch@0.2.3/jquery.ui.touch-punch.js' integrity='sha256-S9605h/+fTHx8kE89v4NQWuTGCEQJF0B9UGvMFYAiO8=' crossorigin='anonymous'></script>"));
          client.println(F("<script src='https://unpkg.com/@popperjs/core@2'></script>"));
          client.println(F("<script src='https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/js/bootstrap.min.js' integrity='sha384-j0CNLUeiqtyaRmlzUHCPZ+Gy5fQu0dQ6eZ/xAww941Ai1SxSY+0EQqNXNE6DZiVc' crossorigin='anonymous'></script>"));
          client.println(F("</body></html>"));

          //BMBS checking previous IP config to overwrite
          if (HttpHeaderCFG[0] == 66 && HttpHeaderCFG[1] == 77 && HttpHeaderCFG[2] == 66 && HttpHeaderCFG[3] == 95 && HttpHeaderCFG[4] == 105 && HttpHeaderCFG[5] == 112 && HttpHeaderCFG[6] == 99 && HttpHeaderCFG[7] == 111 && HttpHeaderCFG[8] == 110 && HttpHeaderCFG[9] == 102)
          {
            EEPROM.write(1, 66);
            EEPROM.write(2, 77);
            EEPROM.write(3, 66);
            EEPROM.write(4, 95);
            EEPROM.write(5, 105);
            EEPROM.write(6, 112);
            EEPROM.write(7, 99);
            EEPROM.write(8, 111);
            EEPROM.write(9, 110);
            EEPROM.write(10, 102);
            
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
