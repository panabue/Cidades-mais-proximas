/*dado o nome de uma cidade, retorna todas as informações das N cidades vizinhas mais próximas. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "cJSON.c"

//Definição da estrutura para representar uma cidade
typedef struct {
    int codigo_ibge;
    char nome[100];
    float latitude;
    float longitude;
    int capital;
    int codigo_uf;
    int siafi_id;
    int ddd;
    char fuso_horario[50];
} City;

//Estrutura da tabela hash(nomes)
typedef struct {
    int tamanho;
    char **nomes;
    int *valores_nomes;
} HashTable_Nomes;

//Função hash para strings (nome das cidades)
int hash_function_nomes(char *nomes, int tamanho) {
    int hash = 0;
    int len = strlen(nomes);
    for (int i = 0; i < len; i++) {
        hash += nomes[i];
    }
    return hash % tamanho;
}

//Função para inicializar a tabela hash(nomes)
HashTable_Nomes *cria_hash_table_nomes(int tamanho) {
    // Aloca memória para a tabela hash
    HashTable_Nomes *hash_table = (HashTable_Nomes *)malloc(sizeof(HashTable_Nomes));
    hash_table->tamanho = tamanho;
    // Aloca memória para os arrays de chaves e valores
    hash_table->nomes = (char **)malloc(tamanho * sizeof(char *));
    hash_table->valores_nomes = (int *)malloc(tamanho * sizeof(int));
    // Inicializa os arrays com NULL e -1
    for (int i = 0; i < tamanho; i++) {
        hash_table->nomes[i] = NULL;
        hash_table->valores_nomes[i] = -1;
    }
    return hash_table;
}

//Função para inserir um par nome-valor na tabela hash
void insere_nome(HashTable_Nomes *hash_table_nomes, char *nomes, int valores_nomes) {
    int index = hash_function_nomes(nomes, hash_table_nomes->tamanho);
    // Verifica se a posição está vazia
    while (hash_table_nomes->nomes[index] != NULL) {
        //Em caso de colisão, procura pela próxima posição vazia
        index = (index + 1) % hash_table_nomes->tamanho;
    }
    //Insere a chave e o valor na posição encontrada
    hash_table_nomes->nomes[index] = strdup(nomes);
    hash_table_nomes->valores_nomes[index] = valores_nomes;
}

// Função para buscar o valor associado a uma chave na tabela hash
int procura_nome(HashTable_Nomes *hash_table_nomes, char *nome) {
    int index = hash_function_nomes(nome, hash_table_nomes->tamanho);
    // Procura pela chave na tabela
    while (hash_table_nomes->nomes[index] != NULL) {
        // Se a chave for encontrada, retorna o valor associado
        if (strcmp(hash_table_nomes->nomes[index], nome) == 0) {
            return hash_table_nomes->valores_nomes[index];
        }
        // Avança para a próxima posição
        index = (index + 1) % hash_table_nomes->tamanho;
    }
    // Se a chave não for encontrada, retorna -1
    return -1;
}

/*TAREFA 1*/

//Primeira função hash 
int Hash_function1(int codigo_ibge, int tamanho) {
    return codigo_ibge % tamanho;
}

//Segunda função hash 
int Hash_function2(int codigo_ibge) {
    return 7 - (codigo_ibge % 7);
}

//Estrutura da tabela hash
typedef struct {
    int tamanho;
    City **table;
} HashTable;

//Função para inicializar a tabela hash
HashTable *Cria_hash_table(int tamanho) {
    //Aloca memoria para hash table
    HashTable *hash_table = (HashTable *)malloc(sizeof(HashTable));
    hash_table->tamanho = tamanho;
    //Aloca memoria para o vetor de ponteiros para a struct City
    hash_table->table = (City **)malloc(tamanho * sizeof(City *));
    for (int i = 0; i < tamanho; i++) {
        hash_table->table[i] = NULL;
    }
    return hash_table;
}

//Função para inserir uma cidade na tabela hash
void Insere_cidade(HashTable *hash_table, City *cidade) {
    int index = Hash_function1(cidade->codigo_ibge, hash_table->tamanho);
    //Caso a posição esteja vazia insere a cidade nela
    if (hash_table->table[index] == NULL) {
        hash_table->table[index] = cidade;
    //Caso a posição esteja ocupada calcularemos o proximo index
    //e procuraremos a proxima posicao vazia
    } else {
        int index2 = Hash_function2(cidade->codigo_ibge);
        int i = 1;
        while (1) {
            int novo_index = (index + i * index2) % hash_table->tamanho;
            if (hash_table->table[novo_index] == NULL) {
                hash_table->table[novo_index] = cidade;
                break;
            }
            i++;
        }
    }
}

//Função para buscar uma cidade na tabela hash
City *Procura_cidade(HashTable *hash_table, int codigo_ibge) {
    int index = Hash_function1(codigo_ibge, hash_table->tamanho);
    //retorna a cidade a ser buscada se ela estiver no index calculado pela funcao hash_function1
    if (hash_table->table[index] != NULL && hash_table->table[index]->codigo_ibge == codigo_ibge) {
        return hash_table->table[index];
    } 
    //Caso a cidade nao esteja no primeiro index calculado procuramos ela calculando 
    //um novo index usando a função hash_function2 e iterando-o até achar uma posição vazia na hash table
    else {
        int index2 = Hash_function2(codigo_ibge);
        int i = 1;
        while (1) {
            int novo_index = (index + i * index2) % hash_table->tamanho;
            if (hash_table->table[novo_index] != NULL && hash_table->table[novo_index]->codigo_ibge == codigo_ibge) {
                return hash_table->table[novo_index];
            }
            i++;
        }
    }
}

/*TAREFA 2:*/

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
KDNode* Insere_cidade_kd(KDNode *raiz, City *cidade, int profundidade) {
    //Caso ache um nó vazio insere a cidade
    if (raiz == NULL) {
        return Novo_node(cidade);
    }

    //Alterna latitude e longitude
    int alternador = profundidade % 2;

    //Caso o alternador seja igual a 0, compara a latitude da cidade a ser inserida com 
    //a latitude do nó pai para decidir se ela sera filho esquerdo ou direito do pai
    if (alternador == 0) {
        if (cidade->latitude < raiz->node_pai->latitude) {
            raiz->node_esquerdo = Insere_cidade_kd(raiz->node_esquerdo, cidade, profundidade + 1);
        } else {
            raiz->node_direito = Insere_cidade_kd(raiz->node_direito, cidade, profundidade + 1);
        }
    } 
    //Caso o alternador seja igual a 1, compara a longitude da cidade a ser inserida com 
    //a longitude do nó pai para decidir se ela sera filho esquerdo ou direito do pai
    else {
        if (cidade->longitude < raiz->node_pai->longitude) {
            raiz->node_esquerdo = Insere_cidade_kd(raiz->node_esquerdo, cidade, profundidade + 1);
        } else {
            raiz->node_direito = Insere_cidade_kd(raiz->node_direito, cidade, profundidade + 1);
        }
    }

    //retorna o node atual
    return raiz;
}

// Função para buscar as N cidades vizinhas mais próximas
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
    // Abrindo o arquivo JSON para leitura
    FILE *file = fopen("municipios.json", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    char buffer[1024];
    char *jsonBuffer = (char *)malloc(1);
    jsonBuffer[0] = '\0'; // Inicializa a string vazia

    //Lendo o conteúdo do arquivo e concatenando no jsonBuffer
    while (fgets(buffer, 1024, file) != NULL) {
        char *temp = realloc(jsonBuffer, strlen(jsonBuffer) + strlen(buffer) + 1);
        if (temp == NULL) {
            printf("Erro ao alocar memória.\n");
            free(jsonBuffer);
            fclose(file);
            return 1;
        }
        jsonBuffer = temp;
        strcat(jsonBuffer, buffer);
    }

    fclose(file);

    //Parsing do JSON
    cJSON *raiz = cJSON_Parse(jsonBuffer);
    if (raiz == NULL) {
        printf("Erro ao fazer o parsing do JSON.\n");
        free(jsonBuffer);
        return 1;
    }

    //Criando a tabela hash(nomes)
    HashTable_Nomes *hash_table_nomes = cria_hash_table_nomes(10000);

    //Criando a tabela hash
    HashTable *hash_table = Cria_hash_table(10000);

    //Array de cidades
    int numCidades = cJSON_GetArraySize(raiz);
    City cidades[numCidades];

    //Lendo cada cidade do JSON e inserindo na tabela hash
    for (int i = 0; i < numCidades; ++i) {
        cJSON *cidadeJson = cJSON_GetArrayItem(raiz, i);

        cidades[i].codigo_ibge = cJSON_GetObjectItem(cidadeJson, "codigo_ibge")->valueint;
        strcpy(cidades[i].nome, cJSON_GetObjectItem(cidadeJson, "nome")->valuestring);
        cidades[i].latitude = cJSON_GetObjectItem(cidadeJson, "latitude")->valuedouble;
        cidades[i].longitude = cJSON_GetObjectItem(cidadeJson, "longitude")->valuedouble;
        cidades[i].capital = cJSON_GetObjectItem(cidadeJson, "capital")->valueint;
        cidades[i].codigo_uf = cJSON_GetObjectItem(cidadeJson, "codigo_uf")->valueint;
        cidades[i].siafi_id = cJSON_GetObjectItem(cidadeJson, "siafi_id")->valueint;
        cidades[i].ddd = cJSON_GetObjectItem(cidadeJson, "ddd")->valueint;
        strcpy(cidades[i].fuso_horario, cJSON_GetObjectItem(cidadeJson, "fuso_horario")->valuestring);

        //Inserindo a cidade na tabela hash para nomes
        insere_nome(hash_table_nomes, cidades[i].nome, cidades[i].codigo_ibge);
        //Inserindo a cidade na tabela hash de codigos_IBGE
        Insere_cidade(hash_table, &cidades[i]);
    }

    //Nome da cidade a ser buscada
    char cidade_busca[100];
    strcpy(cidade_busca, "Abadia dos Dourados");

    //Buscando o código IBGE da cidade na tabela hash
    int codigo_ibge = procura_nome(hash_table_nomes, cidade_busca);

    //Exibindo as informações da cidade buscada, se encontrada
    if (codigo_ibge != -1) {
        printf("Código IBGE: %d\n", codigo_ibge);
    } else {
        printf("Cidade não encontrada.\n");
        return 1;
    }

    /*TAREFA 2:*/

    //Construir a KD-tree
    KDNode *node_raiz = NULL;
    for (int i = 0; i < numCidades; i++) {
        node_raiz = Insere_cidade_kd(node_raiz, &cidades[i], 0);
    }

    int cidade_busca2 = codigo_ibge;
    int N = 10; // Número de vizinhos desejados
    City cidadeBusca;
    for (int i = 0; i < numCidades; i++) {
        if (cidades[i].codigo_ibge == cidade_busca2) {
            cidadeBusca = cidades[i];
            break;
        }
    }

    //Inicializa os vetores de distancias e de vizinhos
    double distancias[N+1];
    int vizinhos[N+1];
    for (int i = 0; i < N+1; i++) {
        distancias[i] = INFINITY;
        vizinhos[i] = -1;
    }

    Procura_vizinhos(node_raiz, &cidadeBusca, N+1, distancias, vizinhos, 0);

    /*TAREFA 1:*/
    printf("As informações das %d cidades vizinhas mais próximas à %s são:\n", N, cidade_busca);
    for (int i = 0; i < N+1; i++) {
        if (vizinhos[i] != cidade_busca2) { // Verifica se não é a própria cidade de busca
            //Código IBGE da cidade a ser buscada
            int codigo_ibge = vizinhos[i];
            //Buscando a cidade na tabela hash
            City *cidadeBuscada = Procura_cidade(hash_table, codigo_ibge);

            printf("Cidade %d :\n", i);
            //Exibindo as informações da cidade buscada, se encontrada
            if (cidadeBuscada != NULL) {
                printf("Codigo IBGE: %d\n", cidadeBuscada->codigo_ibge);
                printf("Nome: %s\n", cidadeBuscada->nome);
                printf("Latitude: %.4f\n", cidadeBuscada->latitude);
                printf("Longitude: %.4f\n", cidadeBuscada->longitude);
                printf("Capital: %s\n", cidadeBuscada->capital ? "Sim" : "Nao");
                printf("Codigo UF: %d\n", cidadeBuscada->codigo_uf);
                printf("SIAFI ID: %d\n", cidadeBuscada->siafi_id);
                printf("DDD: %d\n", cidadeBuscada->ddd);
                printf("Fuso Horario: %s\n\n", cidadeBuscada->fuso_horario);
            } else {
                printf("Cidade nao encontrada.\n");
            }
        }
    }

    //Liberando memória alocada
    cJSON_Delete(raiz);
    free(jsonBuffer);
    for (int i = 0; i < hash_table_nomes->tamanho; i++) {
        free(hash_table_nomes->nomes[i]);
    }
    free(hash_table_nomes->nomes);
    free(hash_table_nomes->valores_nomes);
    free(hash_table_nomes);
    free(node_raiz);
    free(hash_table->table);
    free(hash_table);

    return 0;
}
