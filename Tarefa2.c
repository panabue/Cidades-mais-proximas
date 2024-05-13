/*Dado o código_ibge, retorna o codigo_ibge das N cidades vizinhas mais próximas, onde N é um parâmetro do usuário.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "cJSON.h"
#include "cJSON.c"

//Definição da estrutura de dados para representar uma cidade
typedef struct {
    int codigo_ibge;
    double latitude;
    double longitude;
} City;

//Estrutura para representar um nó na KD-tree
typedef struct KDNode {
    City *node_pai;
    struct KDNode *node_esquerdo;
    struct KDNode *node_direito;
} KDNode;

//Função para calcular a distância entre duas cidades
double Distancia_cidades(City *cidade1, City *cidade2) {
    double lat1 = cidade1->latitude;
    double lon1 = cidade1->longitude;
    double lat2 = cidade2->latitude;
    double lon2 = cidade2->longitude;
    double dist_lat = lat2 - lat1;
    double dist_lon = lon2 - lon1;
    double dist_total = sqrt(dist_lat * dist_lat + dist_lon * dist_lon);
    return dist_total;
}

//Função para criar um novo nó na KD-tree
KDNode* Novo_node(City *cidade) {
    KDNode *node = (KDNode*)malloc(sizeof(KDNode));
    node->node_pai = cidade;
    node->node_esquerdo = NULL;
    node->node_direito = NULL;
    return node;
}

//Função para inserir uma cidade na KD-tree
KDNode* Insere_cidade(KDNode *raiz, City *cidade, int profundidade) {
    //Caso ache um nó vazio insere a cidade
    if (raiz == NULL) {
        return Novo_node(cidade);
    }

    //Alterna entre latitude e longitude
    int alternador = profundidade % 2;

    //Caso o alternador seja igual a 0, compara a latitude da cidade a ser inserida com 
    //a latitude do nó pai para decidir se ela sera filho esquerdo ou direito do pai
    if (alternador == 0) {
        if (cidade->latitude < raiz->node_pai->latitude) {
            raiz->node_esquerdo = Insere_cidade(raiz->node_esquerdo, cidade, profundidade + 1);
        } else {
            raiz->node_direito = Insere_cidade(raiz->node_direito, cidade, profundidade + 1);
        }
    } 
    //Caso o alternador seja igual a 1, compara a longitude da cidade a ser inserida com 
    //a longitude do nó pai para decidir se ela sera filho esquerdo ou direito do pai
    else {
        if (cidade->longitude < raiz->node_pai->longitude) {
            raiz->node_esquerdo = Insere_cidade(raiz->node_esquerdo, cidade, profundidade + 1);
        } else {
            raiz->node_direito = Insere_cidade(raiz->node_direito, cidade, profundidade + 1);
        }
    }

    //retorna o node atual
    return raiz;
}

//Função para buscar as N cidades vizinhas mais próximas
void Procura_vizinhos(KDNode *raiz, City *cidade, int N_vizinhos, double *vetor_distancias, int *vetor_vizinhos, int profundidade) {
    //Caso nao o node atual esteja vazio
    if (raiz == NULL) {
        return;
    }

    //Alterna entre latitude e longitude
    int alternador = profundidade % 2;

    //Calcula a distancia da cidade atual e a cidade alvo 
    double dist = Distancia_cidades(raiz->node_pai, cidade);

    //Atualizar a lista de vizinhos mais próximos
    for (int i = 0; i < N_vizinhos; i++) {
        if (dist < vetor_distancias[i]) {
            //Desloca os vizinhos para fazer espaço para o novo vizinho
            for (int j = N_vizinhos - 1; j > i; j--) {
                vetor_distancias[j] = vetor_distancias[j - 1];
                vetor_vizinhos[j] = vetor_vizinhos[j - 1];
            }
            vetor_distancias[i] = dist;
            vetor_vizinhos[i] = raiz->node_pai->codigo_ibge;
            break;
        }
    }

    if (alternador == 0) {
        //Se a latitude da cidade alvo for menor que a latitude do nó atual, 
        //chame recursivamente a função no filho esquerdo do nó atual.
        if (cidade->latitude < raiz->node_pai->latitude) {
            Procura_vizinhos(raiz->node_esquerdo, cidade, N_vizinhos, vetor_distancias, vetor_vizinhos, profundidade + 1);
            if (raiz->node_direito != NULL && (raiz->node_pai->latitude - cidade->latitude) * (raiz->node_pai->latitude - cidade->latitude) < vetor_distancias[N_vizinhos - 1]) {
                Procura_vizinhos(raiz->node_direito, cidade, N_vizinhos, vetor_distancias, vetor_vizinhos, profundidade + 1);
            }
        } 
        //Se o filho direito do nó atual existir e a diferença quadrática entre a latitude da cidade alvo e a latitude do nó atual for menor que a maior distância na matriz 
        //de distâncias, chame recursivamente a função no filho direito do nó atual.
        else {
            Procura_vizinhos(raiz->node_direito, cidade, N_vizinhos, vetor_distancias, vetor_vizinhos, profundidade + 1);
            if (raiz->node_esquerdo != NULL && (cidade->latitude - raiz->node_pai->latitude) * (cidade->latitude - raiz->node_pai->latitude) < vetor_distancias[N_vizinhos - 1]) {
                Procura_vizinhos(raiz->node_esquerdo, cidade, N_vizinhos, vetor_distancias, vetor_vizinhos, profundidade + 1);
            }
        }
    } else {
        //Se a longitude da cidade alvo for menor à longitude do nó atual, 
        //chame recursivamente a função no filho direito do nó atual.
        if (cidade->longitude < raiz->node_pai->longitude) {
            Procura_vizinhos(raiz->node_esquerdo, cidade, N_vizinhos, vetor_distancias, vetor_vizinhos, profundidade + 1);
            if (raiz->node_direito != NULL && (raiz->node_pai->longitude - cidade->longitude) * (raiz->node_pai->longitude - cidade->longitude) < vetor_distancias[N_vizinhos - 1]) {
                Procura_vizinhos(raiz->node_direito, cidade, N_vizinhos, vetor_distancias, vetor_vizinhos, profundidade + 1);
            }
        } 
        //Se o filho esquerdo do nó atual existir e a diferença quadrática entre a latitude ou longitude da cidade alvo e a latitude ou longitude do pai do nó atual for menor que a maior distância na matriz 
        //de distâncias, chame recursivamente Procura_vizinhos no filho esquerdo do nó atual.
        else {
            Procura_vizinhos(raiz->node_direito, cidade, N_vizinhos, vetor_distancias, vetor_vizinhos, profundidade + 1);
            if (raiz->node_esquerdo != NULL && (cidade->longitude - raiz->node_pai->longitude) * (cidade->longitude - raiz->node_pai->longitude) < vetor_distancias[N_vizinhos - 1]) {
                Procura_vizinhos(raiz->node_esquerdo, cidade, N_vizinhos, vetor_distancias, vetor_vizinhos, profundidade + 1);
            }
        }
    }
}

int main() {

    printf("Espere um momento...\n");

    //Ler o arquivo JSON e extrair os dados das cidades
    FILE *file = fopen("municipios.json", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    char buffer[1024];
    char *jsonBuffer = (char *)malloc(1);
    jsonBuffer[0] = '\0';// Inicializa a string vazia

    //Lendo o conteúdo do arquivo e concatenando no jsonBuffer
    while (fgets(buffer, 1024, file) != NULL) {
        char *temp = realloc(jsonBuffer, strlen(jsonBuffer) + strlen(buffer) + 1);
        if (temp == NULL) {
            printf("Erro ao alocar memoria.\n");
            free(jsonBuffer);
            fclose(file);
            return 1;
        }
        jsonBuffer = temp;
        strcat(jsonBuffer, buffer);
    }

    fclose(file);

    //Parse do JSON
    cJSON *raiz = cJSON_Parse(jsonBuffer);
    if (!raiz) {
        printf("Erro ao fazer parse do JSON.\n");
        return 1;
    }

    //Array de cidades
    int num_cidades = cJSON_GetArraySize(raiz);
    City cidades[num_cidades];

    //Lendo cada cidade do JSON 
    for (int i = 0; i < num_cidades; i++) {
        cJSON *item = cJSON_GetArrayItem(raiz, i);

        cidades[i].codigo_ibge = cJSON_GetObjectItem(item, "codigo_ibge")->valueint;
        cidades[i].latitude = cJSON_GetObjectItem(item, "latitude")->valuedouble;
        cidades[i].longitude = cJSON_GetObjectItem(item, "longitude")->valuedouble;
    }

    //Construir a KD-tree
    KDNode *node_raiz = NULL;
    for (int i = 0; i < num_cidades; i++) {
        node_raiz = Insere_cidade(node_raiz, &cidades[i], 0);
    }

    int cidade_busca;
    printf("Digite o codigo IBGE da cidade a ser buscada: ");
    scanf("%d", &cidade_busca);
    int N; //Número de vizinhos desejados
    printf("Digite a quantidade de cidades vizinhas a ser buscada: ");
    scanf("%d", &N);

    bool codigo_encontrado = false;

    //Procura a cidade relacionada ao codigo IBGE
    City cidadeBusca;
    for (int i = 0; i < num_cidades; i++) {
        if (cidades[i].codigo_ibge == cidade_busca) {
            cidadeBusca = cidades[i];
            codigo_encontrado = true;
            break;
        }
    }

    // Verifica se o código IBGE foi encontrado
    if (!codigo_encontrado) {
        printf("Codigo IBGE nao encontrado.\n");
        return 1;
    }

    //Inicializa os vetores de distancias e de vizinhos
    double distancias[N+1];
    int vizinhos[N+1];
    for (int i = 0; i < N+1; i++) {
        distancias[i] = INFINITY;
        vizinhos[i] = -1;
    }

    Procura_vizinhos(node_raiz, &cidadeBusca, N+1, distancias, vizinhos, 0);

    //Imprime os codigos_IBGE das cidades vizinhas
    printf("\nAs %d cidades vizinhas mais proximas a cidade %d sao:\n", N, cidade_busca);
    for (int i = 0; i < N+1; i++) {
        if (vizinhos[i] != cidade_busca) { //Verifica se não é a própria cidade de busca
        printf("%d \n", vizinhos[i]);
        }
    }

    //Liberar memória alocada
    cJSON_Delete(raiz);
    free(jsonBuffer);
    free(node_raiz);

    return 0;
}
