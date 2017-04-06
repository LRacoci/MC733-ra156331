Relatório Exercício 2
=======================

## Introdução



## Objetivos
Este exercício tem como objetivo avaliar o desempenho de 1 nível de cache de dados e instrução em traces de execução disponibilizados de 4 benchmarks considerando os seguintes parâmetros:
- Tamanho total da Cache
- Tamanho do Bloco (Linha da Tabela)
- Associatividade (Conjuntos)
- Política de Substituição

## Procedimento

Para evitar explosão combinatória seguiu-se o seguinte procedimento:

1. Fixou-se os valores de tamnhos de blocos, associatividade e Política de Substituição em todos os banchmarks e variou-se o tamanho das caches L1 de dados (L1d) e de instruções (L1i).
2. Encontra-se o melhor valor para o tamanho das caches
3. Com os melhores tamanhos fixados, varia-se os outros parâmetros de forma a obter a melhor combinação.


## Tamanhos de Cache Avaliados

Primeiramente, avaliou-se com apenas 10 traces todas as possíveis combinações de tamanhos para caches L1i e L1d.

Obtendo assim os gráficos mostrados a seguir:

![alt text](size_all/figs/size_art.png "Gráfico 1.1")
![alt text](size_all/figs/size_gcc.png "Gráfico 1.2")
![alt text](size_all/figs/size_galgel.png "Gráfico 1.3")
![alt text](size_all/figs/size_mesa.png "Gráfico 1.4")

Como pode ser observado os tamanhos de L1i não influenciam na tacha de Misses de L1d e vice-versa.

Assim pode-se considerar apenas os casos em que size(L1i) == size(L1d).

Assim, executa-se uma análise mais detalhada, dessa vez com 100 traces somente para os casos em que os tamanhos de ambas as caches são iguais.

![alt text](size/figs/gcc.png "Gráfico 2.2")
![alt text](size/figs/mesa.png "Gráfico 2.4")
![alt text](size/figs/galgel.png "Gráfico 2.3")
![alt text](size/figs/art.png "Gráfico 2.1")


Para facilitar a visualização plotou-se a derivada numérica aproximada desses gráficos, como podes ser visto a seguir:

![alt text](size/figs/gccChange.png "Gráfico 3.2")
![alt text](size/figs/mesaChange.png "Gráfico 3.4")
![alt text](size/figs/galgelChange.png "Gráfico 3.3")
![alt text](size/figs/artChange.png "Gráfico 3.1")




Definiu-se (64K 32K 8K 4K) para (gcc, mesa, galgel, art) usando como critério3 variações na taxa e procurou-se obter o valor que maximiza a variação com relação ao anterior e minimiza a variação com relação ao próximo. E toma-se o primeiro tamanho que atende a este critério entre as caches de dados e instruções.


## Análise dos Outros Parâmetros

Fixado o tamanho da cache, agora varia-se os outros parâmentros:
- Tamanho do Bloco (Linha)
- Associatividade (Número de Conjuntos)
- Política de Substituição

Sabendo o tempo que demorou as primeiras execuções e considerando a quantidade de parâmetros sendo avaliados dessa vez, limitou-se o número máximo de traces para 20. Obtêm-se assim, dados suficientes para os seguintes gráficos:

### L1d
No caso de L1d, notou-se que em todos os casos onde há diferença significativa de desempenho considerando apenas as políticas de substituição, a random geralmente é a pior alternativa, como esperado. Entretanto esta política é mais fácil de implementar e portanto mais barata, já que não é necessário guardar o estado de acessos recentes.

Outra observação relevante é que a diferença entre as políticas de substituição se torna maior com o aumento da associatividade, o que é bem razoável já que se não houver associatividade, não há opções para a substituição e portanto as políticas adotadas são irrelevantes e quanto mais associatividade mais opções há e portanto maior espaço para divergência.

![alt text](others/figs/surface3d_gcc_d.png "Gráfico 4.2")
Neste gráfico pode-se observar que a Política de Substiuição LRU (Least Recent Used) é a que se sai melhor comparada as outras já que apresenta uma taxa de hit maior. Também é possível perceber que o tamanho do bloco parece influenciar mais que a associatividade.
![alt text](others/figs/surface3d_mesa_d.png "Gráfico 4.4")
Neste gráfico pode-se observar que as políticas de substituição não parecem ser tão determinantes. Mas o mais interessante é a queda,mesmo que pequena em números absolutos da taxa de hit quando o tamanho do bloco é grande mas a associatividade é baixa. O que mostra a importância de a associatividade exisitir
![alt text](others/figs/surface3d_galgel_d.png "Gráfico 4.3")
Neste gráfico é interessante notar que com blocos pequenos, a política de substituição influencia bastante no desempenho. Outra observação é que o desempenho parece depender mais do tamanho do bloco do que da associatividade.
![alt text](others/figs/surface3d_art_d.png "Gráfico 4.1")
Neste gráfico é interessante notar como o desempenho parece depender quase que exclusivamente do tamanho do bloco, aumentando o hit conforme o aumento do tamanho, como esperado.

###L1i

Já para L1i, a primeira observação interessante é que a quantidade de hits depender menos da associatividade que do tamanho do bloco, consequentemente a divergência quanto a política de substituição é quase nula em todos os casos, o que é razoável considerando que a maior parte dos códigos executa sequencialmente.
![alt text](others/figs/surface3d_gcc_i.png "Gráfico 4.2")
Este é o gráfico que apresenta um desempenho mais simétrico em relação ao tamanho do bloco e a associatividade, consequentemente também é o que apresenta a maior divergência encontrada quanto a política de substituição
![alt text](others/figs/surface3d_mesa_i.png "Gráfico 4.4")
Neste gráfico verifica-se claramente que uma associatividade maior que 2^1 = 2 não traz nenhum desempenho a este benchamrk, já que para valores maiores a taxa de hits continua 1 invariavelmente.

![alt text](others/figs/surface3d_galgel_i.png "Gráfico 4.3")
Similar ao anterior.

![alt text](others/figs/surface3d_art_i.png "Gráfico 4.1")
Este gráfico também mostra que a taxa de hits da cache chega ao limite para valores de bloco maiores que 2^3 = 8 bytes e de associatividade maiores que 2^1 = 2 caches paralelas.

## Os Melhores Parâmetros
Dos gráficos acima é possível inferir que a melhor política de substituição é a LRU, pois sempre que há diferença siginificativa entre as políticas a LRU está por cima quase sempre.

## Tamanhos de bloco avaliados

## Associatividade

## Outras configurações avaliadas

## Interpretação dos Resultados
