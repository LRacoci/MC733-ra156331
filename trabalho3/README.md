# Projeto 3

Este é o roteiro do projeto. O relatório está no outro arquivo markdown.

## Grupo 4:

|           Nome           |   RA   |
|:------------------------:|:------:|
|    Lucas Alves Racoci    | 156331 |
| Luiz Fernando da Fonseca | 156475 |
|        Rafael Zane       | 157079 |

## Roteiro

### Aplicação

Será utilizada uma aplicação que calcula o Espectro de Fourier de uma imagem. Para isso, será utilizada a forma mais simples da Transformada Discreta de Fourier bidimensional (DFT). Esta aplicação é altamente paralelizável, pois uma imagem é uma matriz bidimensional e o processamento pode ser separado entre as diferentes threads devido a transformada ser calculada para cada pixel (posição da matriz). A leitura da imagem original e a escrita do espectro serão realizadas por um único core. Outro processamento que precisa ser feito é uma multiplicação para deslocar a origem no Espectro de Fourier, mas que é apenas uma multiplicação de cada pixel por valores que variam com a posição do pixel na imagem, que também é facilmente paralelizável. A ideia então vai ser separar o processamento de cada core para que cada uma certa quantidade de linhas da matriz.

### Aceleração

Serão implementados periféricos que realizam operações de ponto flutuante e operações trigonométricas.

### Experimentos

Serão utilizadas as seguintes configurações para realização dos testes, rodando com imagens de tamanhos diferentes como entrada no código:

+ Single Core sem aceleração
+ Single Core com aceleração
+ 2 Cores sem aceleração
+ 2 Cores com aceleração
+ 4 Cores sem aceleração
+ 4 Cores com aceleração
+ 8 Cores sem aceleração
+ 8 Cores com aceleração