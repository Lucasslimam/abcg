
**Nome**: Lucas da Silva de Lima
**RA**: 11201722101

**Link de acesso para o jogo na web**: https://lucasslimam.github.io/abcg/
**Diretório do código fonte**: abcg/examples/screensaver/

# Proposta da aplicação
A intenção do projeto foi fazer um screensaver baseado em transformações geradas por colisão e somadas a algum efeito visual nos objetos e também houve a intenção de lidar com a animação da própria câmera.

Dessa forma, o projeto em questão se trata de um screensaver composto por uma esfera e um cubo em um chão xadrez, de tal forma que a esfera inicia com uma cor e a cada colisão, a esfera transfere sua cor para o cubo em que está inserida, e também há a interação colisão-chão, pois o chão imitará a diferença de cores existentes entre a esfera e a caixa, de tal forma que o chão continua tendo o aspecto de xadrez mas com as cores alternando de acordo com a esfera e a caixa. Além disso, a próxima cor da esfera após cada colisão é completamente aleatória.

# Funcionamento
Para a transparência da caixa são renderizados apenas os triângulos internos, possibilitando que pudéssemos ver as colisões esfera-caixa. A mudança de cores é feita todas pela função que verifica colisão, sendo a colisão detectada por uma distância radial em relação ao centro da esfera. Após haver a colisão a caixa absorve a cor da esfera e a esfera muda de cor aleatoriamente, após essa mudança o chão absorve as cores da caixa e da esfera, mantendo sua alternância de cores.

Além disso, todas as colisões da bola são perfeitamente elásticas, não há alteração nenhuma na velocidade com cada uma das colisões e também a reflexão é feita apenas invertendo o vetor que dita a velocidade da bola em todos os eixos (x, y e z).

Outro ponto que foi um desafio implementar, foi a câmera circundar o objeto ao invés do objeto rotacionar (fazendo o **m_eye** rotacionar em torno do **m_at**). Com isso, precisamos tomar **m_at** como o ponto de referência e então transladamos o **m_eye** em **-m_at**, em seguida, aplicamos a rotação com o ângulo recebido em torno do eixo **m_up**, e por fim, voltamos para o espaço original transladando **m_eye** por **+m_at**.


# Resumo dos Desafios
0. Planejar/criar os objetos bola e cubo.
1. Conseguir tornar a caixa "transparente" para ver a bola.
2. Implementar mudança de cores pós colisão.
3. Implementar a física da bola.
4. Fazer a câmera girar em torno do cubo.

# Manual
**Controles:** Nenhum