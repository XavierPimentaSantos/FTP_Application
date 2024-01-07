![](https://sigarra.up.pt/feup/pt/imagens/LogotipoSI)

## Redes de Computadores
# 2º Trabalho Laboratorial
## Desenvolvimento de uma aplicação de *download* & Configuração e Estudo de uma Rede

Xavier Maria Pimenta Santos (up202108894)
Artur Telo Luís (up202104487)

## Índice
- Sumário
- Introdução
- Parte 1 - Aplicação de *Download*

## Sumário

Este relatório foi elaborado no âmbito da UC de Redes de Computadores, e apresenta compreensivamente o código desenvolvido para uma aplicação de *download* de ficheiros com protocolo FTP, bem como um guia pelos passos seguidos para a configuração de uma rede, segundo [este guião](https://moodle2324.up.pt/pluginfile.php/72936/mod_resource/content/4/Lab%202%20-%20Gui%C3%A3o%20v5.1.pdf).

## Introdução

Neste projeto desenvolvemos uma aplicação de *download* de ficheiros com o FTP, e configuramos uma rede (no laboratório I320) para conseguir acesso à *internet*, a qual usamos em conjunto com a aplicação desenvolvida para testar a transferência de vários ficheiros.

## Parte 1 - Aplicação de *Download*

A aplicação é separada em três ficheiros com código (.c), e os seus respetivos ficheiros *header* (.h).

getIP.c contém apenas a função `getIP(const char* hostname, char* ip)`, que nos é fornecida, responsável por guardar em `ip` o endereço IP do *host* `hostname`.

clientTCP.c contém a função `clientTCP(const char* ip, uint16_t port)`. Este ficheiro, tal como getIP.c, foi-nos fornecido com o código; a função `clientTCP` recebe como argumentos um endereço IP e uma *port*, abre uma nova conexão (*file descriptor*) para a *port* no *ip* fornecido, e retorna o valor associado a esse *fd*.

main.c contém as principais funções:
- `checkResponse(const int socket, char* buf)`: verifica a resposta do servidor, e devolve o valor da mesma.
- `connectToServer(const int socket, const char* user, const char* password)`: com as credenciais fornecidas pelo utilizador (ou as *default*) tenta autenticar-se no servidor.
- `enterPassiveMode(const int socket, char* ip_address, int* port)`: coloca o servidor em modo passivo, e recebe em *ip_address* e *port* o endereço IP e *port*, respetivamente, para abrir nova conexão com o servidor, de onde irá transferir o ficheiro.
- `main(int argc, char* argv[])`: função principal, responsável por receber e filtrar o *input* do utilizador, e controlar o fluxo de execução do programa, chamando as funções auxiliares quando necessário.

Um dos testes à aplicação foi realizado na máquina tux43, na sala I320 da FEUP. Após compilar o código-fonte, corremos no terminal o comando `./download ftp://rcom:rcom@netlab1.fe.up.pt/pipe.txt`, para transferir o ficheiro pipe.txt, no *host* netlab1.fe.up.pt, com as credenciais `USER rcom` e `PASS rcom`. A comunicação entre tux43 e o servidor foi capturada no computador com o Wireshark; podemos ver essa comunicação nas capturas de ecrã seguintes.

![](https://i.imgur.com/n0l7E9R.png)
![](https://i.imgur.com/rHDMG1o.png)

Os pacotes 4 e 5 são pacotes DNS, usados para o cliente obter o endereço IP do *host* com o qual está a tentar comunicar.
Os pacotes 6 e 7 são do protocolo TCP, em que o cliente e o servidor estabelecem a ligação.
Pacotes 8 a 19, 22 a 27, e 29 a 35 alternam entre TCP e FTP, e contêm a troca de mensagens entre as duas máquinas.
Os pacotes 20 e 21 são, à semelhança dos 6 e 7, do protocolo TCP, e representam o segundo terminal do tux43 a ligar-se à porta do servidor obtida após o pedido 'pasv' no primeiro terminal.
Pacotes 28, 36, 37 e 38 são TCP, e representam o fecho das conexões.

## Parte 2 - Configuração e Estudo de uma Rede

### Experiência 1 - Configurar uma *IP network*

Nesta experiência 

### Experiência 5 - DNS

Nesta experiência, foi-nos pedido que configurássemos o DNS (Domain Name System) nos três computadores, com o servidor DNS da FEUP (193.136.28.10), para poder aceder a *hosts* com base no seu nome (ex: [google.com](www.google.com)). para tal, corremos `nano /etc/resolv.conf` no terminal em cada computador, e no ficheiro resolv.conf adicionámos a linha `nameserver 193.136.28.10`.
Para testar este passo, fizemos `ping google.com` nos três computadores, com sucesso.

#### Perguntas
Como configurar o serviço DNS num *host*?
: O DNS é configurado adicionando a linha "nameserver <endereço IP do servidor>" no ficheiro `/etc/resolv.conf`.

Que pacotes são trocados por DNS e que informação é transportada?
: O cliente envia pacotes do tipo *DNS Query Packet*, com informação como o tipo de endereço que procura, o nome do *host*, etc.; o servidor responde com *DNS Response Packets*, que contêm, principalmente, o endereço IP pedido pelo cliente.

### Experiência 6 - Conexões TCP

Aqui foi-nos pedido que testássemos a nossa aplicação de *download* na máquina tux43, com captura de *packets* usando o Wireshark.
Para tal, usamos o comando `./download ftp://rcom:rcom@netlab1.fe.up.pt/pipe.txt`

![](https://i.imgur.com/V6niIh3.png)
tux43 faz um pedido DNS para encontrar o endereço IP do *host* netlab1.fe.up.pt (linhas azuis), e estabelece a ligação ao servidor através do protocolo TCP (linhas cinza).
![](https://i.imgur.com/Lp5GJO1.png)
O terminal A do tux43 comunica com o servidor, enviando as informações de início de sessão, e fazendo o pedido 'pasv' (linhas rosa); o terminal B do tux43 estabelece a ligação com a porta obtida após o pedido 'pasv', através do protocolo TCP (linhas cinza).
![](https://i.imgur.com/dPqABbW.png)
O terminal A faz o pedido 'retr pipe.txt' ao servidor, que descarrega o ficheiro para a porta do terminal B (linha 26) (linhas rosa); a porta de *download* do servidor envia um pacote TCP FIN+ACK para o terminal B (linha cinza).
![](https://i.imgur.com/nZIW4ZA.png)
O terminal A continua a comunicação com o servidor, enviando um pedido 'quit' (linhas rosa); o servidor e os terminais A e B trocam pacotes TCP FIN+ACK, fechando a conexão (linhas cinza e duas últimas linhas rosa).

Depois, foi-nos pedido que repetíssemos a transferência no tux43, mas iniciássemos uma transferência no tux42 durante a primeira.


#### Perguntas
Quantas conexões TCP são abertas pela aplicação?
: Duas, uma para ligar tux43 à porta 21 do servidor, e outra para ligar tux43 à porta de download (obtida pela resposta ao pedido 'pasv') do servidor.

Em que conexão é transportada a informação de controlo do FTP?
: Na conexão entre tux43 e a porta 21 do servidor, através da qual são enviados os comandos relativos ao FTP.

Quais são as fases de uma conexão TCP?
: Estabelecimento da conexão (SYN, SYN+ACK, ACK), transferência de dados, fecho da conexão (FIN, ACK(+FIN), ACK).

Como funciona o mecanismo ARQ TCP? Quais são os campos TCP relevantes? Que informação relevante pode ser observada nos *logs*?
: Este mecanismo serve para reenviar pacotes TCP perdidos/corrompidos, caso a comunicação cliente-servidor ocorra numa rede com muito tráfego, na qual a perda de informação é mais provável. Para detetar um pacote perdido/corrompido, o protocolo TCP baseia-se em *timeouts* e nos valores associados às mensagens ACK.

Como funcionam os mecanismos de gestão de congestionamento do protocolo TCP? Quais são os campos relevantes? Como é que a taxa de transferência da conexão evolui ao longo do tempo? É de acordo com o mecanismo de controlo de congestão do TCP?
: O emissor começa a enviar pacotes com uma taxa baixa ("slow start"), que vai aumentando exponencialmente até atingir a *congestion window*, um parâmetro do protocolo. A partir daqui, o emissor vai aumentar a taxa linearmente, para evitar congestionar a rede. Caso ocorra um *timeout*, o emissor vai voltar à fase inicial, e recomeçar o aumento da taxa de transmissão.

A taxa de transferência de uma conexão TCP é perturbada pela aparição de uma segunda conexão TCP? Como?
: Sim, uma vez que a *bandwidth* terá de ser dividida por mais uma conexão, limitando assim a taxa de transferência da primeira.