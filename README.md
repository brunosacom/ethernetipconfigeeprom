# Arduino Ethernet IP Config EEPROM
### (ethernetipconfigeeprom.ino)
#### Configurações de IP para o Arduino com interface WEB e gravação de dados na EEPROM

Este sketch exibe uma página WEB para configuração dos dados de IP, subnet mask, gateway e dns para o Arduino.

Esses dados são gravados na EEPROM do Arduino e assim mesmo ao desligar, as configurações irão permanecer.

O código HTML já tem inseridos:
 - BootStrap 5.3.3
 - Popper 2.9.1
 - JQuery 3.7.1
 - JqueryUI 1.14.0
 - JQueryUI Touch Punch 0.2.3
 - GoogleFonts

Basta eliminar as linhas que não interessarem.

A navegação entre páginas também está prevista.