
**Nome**: Lucas da Silva de Lima
**RA**: 11201722101

**Link de acesso para o jogo na web**: https://lucasslimam.github.io/abcg/

# Proposta da aplicação
A intenção do projeto foi aplicar os conceitos de geração de Objetos em tempo de processamento, praticar a utilização de iluminação e textura através da programação de um **Sistema Solar** simplificado.

# Funcionamento
No projeto implementado, todos os astros são **esferas geradas por um algoritmo** ao invés de utilizar um .obj. A câmera utilizada foi na verdade, em parte, reutilizada do projeto anterior, apenas foram adicionadas as funções de movimentação da câmera para o usuário. Todos os astros são **instâncias parametrizadas** da mesma classe e os shaders, para iluminação, se utilizam de Blinn-Phong com sombreamento de Phong.

Dado que temos um Sol em nosso sistema e ele se trata de uma fonte de luz, fazemos a **direção da luz partir da origem do Sol** para todo o espaço.

Além disso, para ter a sensação de infinito, foi utilizado **mapeamento cúbico** com texturas de espaço, para simular realmente um espaço infinito, pois a câmera será sempre a origem, ou seja, ela sempre estará inserida no espaço criado.


# Resumo dos Desafios
0. Entender como criar uma esfera por um algoritmo;
1. Utilizar o mapeamento cúbico com a lookat; 
2. Fazer o Sol ditar a direção da luz no sistema;
3. Implementar dependencias de orbitas.


# Manual
|Tecla   | Ação             |
|--------|------------------|
| W | Ir para frente    |
| A | Ir para esquerda |
| S | Ir para fras  |
| D | Ir para direita |
