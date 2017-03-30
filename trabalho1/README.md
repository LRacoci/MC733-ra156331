# Trabalho 1

## Parte 3

### Objetivo

Dados os resultados obtidos nas partes anteriores, isto é, a pontuação de cada benchmark para cada máquina que o usou, o objetivo desta parte do trabalho é encontrar uma forma de rankear as máquinas apresentadas [nesta](https://docs.google.com/spreadsheets/d/16KxX4SDPZrpQ8DpamVpU_aiWXRCeWYZW2HpoMa2TbNw/edit#gid=2127421095) planilha de forma a incluir todos os computadores.

O objetivo, da forma como apresentado, não cria nenhuma limitação na forma de fazer o rankeamento, exceto a de se ter que usar todas as informações que foram disponibilizadas. Daí pode se interpretar que a pontuação também deve ser considerada no método de rankeamento final e não apenas a ordem relativa entre as máquinas dada por cada benchmark.

### Organização Inicial dos Dados

Como temos pontuações sendo dadas por computadores e por benchmarks, uma forma interessante de se visualizar esses dados é na forma de uma tabela em cujas linhas se encontram as máquinas e nas colunas os benchmarks. 

Dessa forma a primeira percepção é a de que nem todos os benchmarks foram usados, o que era esperado já que nem todos foram aprovados para a segunda fase, o inesperado é que nem todos os computadores listados foram usados, já que não haveria motivo para listá-lo se não para usá-lo.

Dessa forma essas linhas e colunas vazias ou inválidas (por não terem sido aprovadas) podem ser eliminadas sem perda de informação.

O Resultado disso pode ser visto [aqui](https://docs.google.com/spreadsheets/d/1P6BALBoYm3LY91y-oUxEYFwfaaxNGRry66Kw0r58huU/edit#gid=1859423493) ou na tabela a seguir.

|     | B1    | B2     | B3     | B4     | B7       | B9        | B10  | B15      | B14   | B16    | B17       | B18  | B19   |
|-----|-------|--------|--------|--------|----------|-----------|------|----------|-------|--------|-----------|------|-------|
| C02 |       | 50.07  | 393.00 |        |          |           |      |          |       |        |           | 0.01 |       |
| C03 |       |        |        |        |          |           |      |          |       |        | 18,549.91 |      |       |
| C04 |       | 44.13  |        | 146.00 |          |           | 3.69 | 1,441.00 | 14.30 |        | 7,304.36  |      |       |
| C06 |       |        |        |        | 314.00   |           |      | 438.00   | 12.98 | 74.00  |           |      |       |
| C07 | 39.00 | 96.35  |        | 88.00  | 1,183.00 | 4,582.01  | 3.08 | 890.00   |       |        | 16,712.08 |      |       |
| C08 |       |        |        |        | 862.00   |           |      |          | 11.68 | 73.00  |           |      |       |
| C09 |       |        |        |        |          |           |      |          |       |        |           | 0.03 |       |
| C10 |       |        |        |        |          |           |      |          | 13.69 |        |           |      |       |
| C11 | 50.00 |        |        |        |          |           |      |          |       |        |           |      |       |
| C12 | 44.00 | 96.74  |        | 158.00 |          |           |      |          |       |        |           | 0.03 |       |
| C13 |       |        |        | 73.00  |          |           |      |          |       |        | 9,006.34  |      |       |
| C14 |       | 37.73  | 198.00 |        | 1,057.00 |           |      |          | 11.24 |        |           |      |       |
| C15 |       |        | 359.00 |        |          | 7,551.54  |      | 965.00   | 12.17 |        | 15,470.86 |      |       |
| C16 | 40.00 | 96.96  | 389.00 | 175.00 |          |           |      |          |       |        |           |      |       |
| C17 |       |        |        | 124.00 |          |           |      |          | 12.12 |        |           |      |       |
| C18 |       | 70.11  |        |        |          |           |      |          |       |        |           | 0.00 |       |
| C19 | 39.00 |        |        | 282.00 |          |           |      |          |       |        | 19,446.94 |      |       |
| C20 |       | 141.91 |        | 172.00 |          | 4,699.34  |      |          |       |        |           | 0.05 |       |
| C21 | 17.00 | 56.40  |        | 103.00 |          |           |      |          |       |        |           |      |       |
| C22 | 35.00 | 95.84  |        | 104.00 |          |           |      |          |       |        |           |      |       |
| C23 |       |        |        |        | 793.00   |           |      | 621.00   | 12.08 | 78.00  |           |      |       |
| C24 |       | 101.75 |        |        |          |           | 2.85 | 685.00   |       | 11.00  |           |      |       |
| C25 | 35.00 |        |        |        |          |           |      |          | 11.11 | 72.00  |           |      |       |
| C26 | 35.00 |        |        |        |          |           |      |          | 11.92 | -27.00 |           |      |       |
| C27 |       |        |        |        |          | 5,355.60  |      | 585.00   |       |        | 7,098.44  |      |       |
| C28 |       | 90.18  | 398.00 |        |          |           |      |          | 14.42 |        |           |      |       |
| C30 |       | 48.41  | 401.00 |        | 1,028.00 | 2,800.89  |      |          | 13.99 |        |           |      |       |
| C31 | 24.00 | 48.56  |        | 211.00 |          |           |      |          |       |        |           |      |       |
| C32 | 32.00 |        |        |        |          |           |      |          | 14.85 |        |           |      |       |
| C33 | 38.00 |        |        |        | 1,194.00 |           | 3.05 |          |       |        |           |      |       |
| C34 | 40.00 |        |        | 83.00  |          |           |      | 617.00   | 15.35 |        |           |      |       |
| C35 | 36.00 |        |        | 279.00 | 1,194.00 |           |      |          | 12.18 | 78.00  | 16,745.93 |      |       |
| C36 | 36.00 |        | 387.00 |        | 1,174.00 | 11,888.25 | 2.85 |          | 19.37 |        |           |      |       |
| C37 |       |        |        |        | 960.00   |           |      |          | 18.43 |        |           |      |       |
| C38 |       |        |        |        |          |           |      |          | 14.60 |        |           | 0.03 |       |
| C39 |       |        |        | 97.00  |          | 1,744.30  |      | 1,191.00 |       |        | 5,397.06  |      |       |
| C40 | 15.00 |        |        | 96.00  |          |           |      | 1,255.00 | 7.00  |        |           |      |       |
| C41 |       | 93.75  | 382.00 |        |          |           |      |          |       |        | 15,121.94 | 0.00 |       |
| C42 |       |        |        |        |          |           |      | 984.00   |       |        | 15,144.41 |      |       |
| C43 | 11.00 | 16.73  |        | 146.00 |          |           |      |          |       |        | 2,753.03  |      |       |
| C44 |       | 53.44  | 392.00 |        |          |           |      |          |       |        | 8,683.52  | 0.03 |       |
| C45 |       | 82.77  |        |        |          | 7,532.50  | 4.30 | 724.00   |       |        |           |      |       |
| C46 |       | 136.00 |        |        |          | 4,269.72  | 2.82 | 697.00   |       |        |           |      |       |
| C47 |       | 17.04  | 213.00 |        |          |           |      |          |       |        | 17,371.45 | 0.13 |       |
| C48 |       |        |        |        |          |           |      |          |       | 83.00  |           |      |       |
| C49 |       |        |        |        |          |           |      |          |       |        |           |      | 50.00 |
| C50 | 45.00 |        |        |        |          |           |      |          |       |        |           |      | 58.23 |
| C51 | 38.00 |        |        |        |          |           |      |          |       |        |           |      | 26.02 |
| C54 |       |        |        |        |          | 2,941.87  |      |          |       |        |           |      |       |
| C55 | 14.00 | 35.94  |        | 76.00  |          |           |      |          |       |        |           |      |       |

Como se pode ver, cada benchmark dá notas muito discrepantes, algumas até negativas como o C26 no benchmark B16, não sendo possível portanto considerar por exemplo uma média entre as notas dos benchmarks de que cada computador participou.


### Abordagens Para Análise

#### Primeira Abordagem

A primeira abordagem foi uma tentativa simples de normalização. Ou seja, considerava-se a máxima pontuação b_max e a mínima b_min de cada benchmark e para cada pontuação x de cada computador ci, obtinha-se uma nova pontuação normalizada x' = (x - b_min)/(b_max-b_min). Isso garantiria que cada banchmark tivesse sua melhor máquina com 1 ponto e sua pior com zero, o que por exemplo resolveria o problema de notas negativas.

Com esses dados é possível plotar gráficos para ajudar a compreender o que está acontecendo. Em todas as análises serão considerados 4 gráficos, em que só mudarão os valores decorrentes de cada fórmula/metodologia de rankeamento utilizada.

	1. Uma distribuição por máquina com a pontuação no eixo y e benchmarks no eixo x; 
	2. Uma distribuição por benchmark com a pontuação no eixo y e máquinas no eixo x;
	3. Um gráfico 3D mostrando as três grandesas, uma por eixo;
	4. Um Mapa de Matriz, colocando computadores no eixo x, benchmarks no y e colorindo a região dada pelas coordenadas (x,y) com a cor da pontuação correspondente

Esses gráficos podem ser vistos a seguir:

![alt text](https://gitlab.com/LRacoci/MC733-ra156331/blob/master/trabalho1/simple_normSBC.png "Gráfico 1.1")


![alt text](https://gitlab.com/LRacoci/MC733-ra156331/blob/master/trabalho1/simple_normSCB.png "Gráfico 1.2")


![alt text](https://gitlab.com/LRacoci/MC733-ra156331/blob/master/trabalho1/simple_norm3D.png "Gráfico 1.3")


![alt text](https://gitlab.com/LRacoci/MC733-ra156331/blob/master/trabalho1/simple_normMatrxMap.png "Gráfico 1.4")

#### Interpretação dos Resultados da Primeira Abordagem

É possível perceber pelos gráficos, que a normalização simples não vai funcionar para esses objetivos, porque muitas vezes a mesma máquina é avaliada de maneira muito discrepante por benchmarks diferentes.

Isso ocorre porque, em um caso extremo, a melhor máquina de um dado benchmark, que portanto receberá nota 1 pode ser a pior de outro e consequentemente receber nota 0. Embora este caso extremo não ocorra nenhuma vez nestes dados isso não diminui o problema, pois versões intermediárias destas discrepências podem ser claramente visualizadas pelos gráficos.

Para resolver este problema e possivelmente outros que venham a ser consequências deste, resolveu-se adotar uma estratégia parecida com as adotadas pela SPEC, como pode ser visto [aqui](https://mrob.com/pub/comp/benchmarks/spec.html), o que nos leva a segunda abordagem:

#### Segunda Abordagem

A segunda abordagem consiste de tentar encontrar uma máquina base e dela conseguir razões para todas as outras máquinas, de forma que todas as máquinas teriam como pontuação uma fração da pontuação desta máquina base. Idealmente seria desejável que a máquina base fosse a mais lenta, porque aí todas as outras máquinas seriam uma razão maior que 1 da performance dada pela pontuação, mas uma rquisição ainda mais fundamental não é satisfeita, porque não há nenhuma máquina que foi testada em todos os benchmarks. A máquina que chega o mais próximo disso,  é a máquina que foi ultilizada com maior frequência (clash C07), então estabeleceu-se que esta seria máquina base.

Para padronizar da mesma forma o desempenho dado pela pontuação das demais máquinas a estratégia adotada foi a seguinte:

	1. Ordena-se a matriz da máquina medida por mais benchmarks (clash C07) para a menos medida
	2. Para cada benchmark ainda não padronizado, encontra-se a máquina C mais medida desse benchmark (que aparece primeiro na matriz)
	3. Padroniza-se esse benchmark da mesma forma que foi feito antes considerando C como base.
	4. Multiplica-se todos os valores deste benchmark pela média geométrica da pontuação de C nos benchmarks já padronizados 
	5. Repete 2,3,4 até acabarem os benchmarks não padronizados

Essa abordagem não é perfeita, mas deveria garantir




### Metodologia Aplicada

Foram feitas 3 de 

### Ranking das Máquinas

### Fórmula de Cálculo

### Organização dos Dados

### Primeira Visualização

### Ajustes Gerais

### Ajustes Finos

### Consistência das Medições

### Considerações de Outros Critérios

Foi cojitada a idéia de usar 

### Critérios para Uso
