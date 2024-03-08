# FECHADURA ELETRÔNICA INTELIGENTE: UMA ABORDAGEM INTEGRADA COM BIOMETRIA E RFID

## :dart: Objetivo deste projeto
Desenvolver um sistema de controle de acesso que integre
biometria e RFID, utilizando uma abordagem modular,
extensível e escalável, empregando a linguagem C++ em
conjunto com bibliotecas open-source.

## :gear: Funcionamento do Sistema
A operação do sistema ocorre em dois modos principais: Figura 3. Montagem do Protótipo
acesso e cadastro. O funcionamento do sistema é detalhado
no fluxograma a seguir:

![Fluxograma](https://github.com/JonasFortes12/Security-Access-Control-System/assets/43821439/8eb8e35e-f118-4489-820d-f169946b1a42)

O sistema foi desenvolvido em C++ para simplificar a
extensibilidade e manutenção. Cada funcionalidade foi
encapsulada em funções individuais e organizadas em
bibliotecas separadas, permitindo a inclusão no arquivo
principal do projeto.

## :file_folder: Diagrama de Dependências:

![diagrama de dependência](https://github.com/JonasFortes12/Security-Access-Control-System/assets/43821439/5232683a-497b-4a66-b079-4d8dc3cfe515)

## :electric_plug: Hardware:

![WhatsApp Image 2023-10-30 at 12 19 14_c6cab313](https://github.com/JonasFortes12/Security-Access-Control-System/assets/43821439/3a1229fe-d541-4b5e-9410-160e7eb8d8cd)
Para a construção do protótipo do sistema, utilizou-se o
microcontrolador ESP-WROOM-32, com CPU Xtensa®
Dual-Core de 32 bits, 448KB de memória ROM, 540KB de
memória RAM e 4MB de memória Flash, suportando
conexões Wi-Fi 2.4GHz. Além disso, foi incluído o módulo
biométrico DY50 e o módulo RFID-RC522 para validação
de acesso, a tranca elétrica HDL FEC-91 para o
acionamento do sistema e um display LCD 16x2 para exibir
informações de acesso.

## :fast_forward: Próximos passos
Atualmente, o protótipo do sistema está funcional. O
próximo passo é desenvolver uma PCB (Printed Circuit
Board) para criar uma interface física mais amigável. A
modularização permitirá uma integração simples com a
internet, possibilitando o monitoramento via aplicação
web, um avanço futuro do projeto.
