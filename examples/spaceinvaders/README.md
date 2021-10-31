**Nome**: Lucas da Silva de Lima
**RA**: 11201722101

**Link de acesso para o jogo na web**: https://lucasslimam.github.io/abcg/

# Proposta do Jogo
A aplicação em questão se trata de uma tentativa de replicar o jogo Space Invaders utilizando algumas referências de Star Wars, as quais utilizei para me inspirar no desenho das naves. Neste projeto, me foquei nos itens que considerei mais importantes para ser possível jogar e que, de certa forma, utilizasse boa parte do visto em aula.
Dessa forma, acabei desenhando a Nave, os inimigos, os lasers utilizados e acabei reutilizando os *layers* de estrelas feitos em aula com o projeto Asteroids.

# Macro funcionamento
O controle da nave se dá apenas pelo movimento esquerda-direita, sendo um movimento fluído, enquanto que os inimigos, querendo reproduzir o funcionamento de Space Invaders, se movem de forma cadenciada, seguindo uma sequência definida = {Leste, Leste, Sul, Oeste, Oeste, Sul} que fica se repetindo até que os inimigos ou a nave do jogador sejam derrotados.

# Micro funcionamento
- A nave se move por um escalar multiplicado pela variação do tempo, que dita o deslocamento da nave, que se dá visivelmente de forma contínua diferentemente dos inimigos;
- Os inimigos possuem a ação de se mover de forma mais cadenciada, podendo dar o próximo passo a cada 0,5 segundos;
- Os inimigos atiram apenas quando a Nave aparece em sua visão;
- Os inimigos não podem atirar na nave caso haja outro inimigo logo abaixo, isso é feito verificando se a coordenada X da nave está em um certo intervalo X de visão em relação ao inimigo.

# Maiores desafios implementados
1. Após o inimigo ser derrotado pela nave, ele fica por alguns instantes piscando, para realizar isso, a solução encontrada foi variar o **Alpha** dos vértices da nave inimiga de acordo com uma função **Seno** evitando condicionais. Dessa forma, durante todas as renderizações faço o alpha variar entre um alphaMin e alphaMax, de tal forma que quando o inimigo é atingido, o alphaMIn que antes era igual ao alphaMax=1, acaba recebendo o valor zero e passa dar a sensação de **blink**;
2. A própria criação de cada um dos objetos e suas diferentes cores, no caso da nave e do inimigo, principalmente, dado que o mesmo objeto possui cores diferentes em seus vértices;
3. Tratar as interações e colisões entre cada um dos objetos, de tal forma que o inimigo não poderia matar outro inimigo, e um inimigo não poder atirar na nave caso. Além disso, tratar dos shaders e colisões envolvendo os tiros inimigos e da nave do jogador.

# Como Jogar
**Objetivo**: Derrotar todos os inimigos sem ser atingido.

|Tecla   | Ação             |
|--------|------------------|
| Espaço | Atirar           |
| D/-->  | Ir para direita  |
|  A/<-- | Ir para esquerda |


**Atenção**: O último inimigo vivo tem sua movimentação aumentada.
