/*dado o nome de uma cidade, retorna todas as informações das N cidades vizinhas mais próximas.*/

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

//Função usada para converter uma string em um índice na tabela hash
unsigned int Hash_function_nomes(const char *str, unsigned int tamanho) {
    //valor escolhido arbitrariamente
    unsigned int hash = 7;
    int c;

    while ((c = *str++))//atribui o valor ASCII do caractere atual à variável c e move o ponteiro para o próximo caractere na string
        /*Multiplica o valor hash por 33 (equivale a um deslocamento para a esquerda de 5 bits e depois adiciona o valor original de hash)
        Em seguida, adiciona o valor ASCII do caractere atual, representado pela variável c*/
        hash = ((hash << 5) + hash) + c; 

    return hash % tamanho;
}

//Estrutura da tabela hash
typedef struct {
    int tamanho;
    City **table;
} HashTable_Nomes;

// Função para inicializar a tabela hash (nomes)
HashTable_Nomes *Cria_hash_table_nomes(int tamanho) {
    //Aloca memoria para a hashtable
    HashTable_Nomes *hash_table = (HashTable_Nomes *)malloc(sizeof(HashTable_Nomes));
    hash_table->tamanho = tamanho;
    //Aloca dinamicamente memória para o array de ponteiros para City dentro da tabela hash
    hash_table->table = (City **)calloc(tamanho, sizeof(City *));
    return hash_table;
}

//Função para inserir uma cidade na tabela hash (nomes)
void Insere_nome(HashTable_Nomes *hash_table, City *cidade) {
    //Calcula o indice usando a funcao hash para transfomar strings em valores hash
    unsigned int index = Hash_function_nomes(cidade->nome, hash_table->tamanho);
    //Enquanto a hastable na posisao do index estiver ocupada (colisao):
    while (hash_table->table[index] != NULL) {
        //Atualiza o índice para a próxima posição na tabela hash
        index = (index + 1) % hash_table->tamanho;
    }
    //Insere a cidade na posicao da hashtable disponivel 
    hash_table->table[index] = cidade;
}

//Função para buscar uma cidade pelo nome na tabela hash
City **Procura_nome(HashTable_Nomes *hash_table, const char *nome, int *numCidadesEncontradas) {
    //Calcula o indice usando a funcao hash para transfomar strings em valores hash
    unsigned int index = Hash_function_nomes(nome, hash_table->tamanho);
    //Inicializa o contador
    int count = 0;
    //Aloca memoria para o vetor das cidades encontradas
    City **cidadesEncontradas = (City **)malloc(hash_table->tamanho * sizeof(City *));

    //Enquanto a hastable na posisao do index estiver ocupada (colisao):
    while (hash_table->table[index] != NULL) {
        //Caso o nome dado na funcao seja igual ao nome armazenado na posicao index da hashtable:
        if (strcmp(hash_table->table[index]->nome, nome) == 0) {
            //A cidade é adicionada ao vetor cidadesEncontradas na posição indicada pelo contador count, e o contador é incrementado
            cidadesEncontradas[count++] = hash_table->table[index];
        }
        //Atualiza o índice para a próxima posição na tabela hash
        index = (index + 1) % hash_table->tamanho;
    }

    /*Atualiza o valor apontado por numCidadesEncontradas com o valor final do contador count, 
    representando o número total de cidades encontradas com o nome especificado*/
    *numCidadesEncontradas = count;
    return cidadesEncontradas;
}

//Função para exibir a lista de cidades buscadas que possuem o mesmo nome
void Exibir_cidades_encontradas(City **cidades, int numCidades) {
    printf("Foram encontradas multiplas cidades com esse nome. Escolha uma:\n");
    for (int i = 0; i < numCidades; i++) {
        printf("%d - %s\n", i + 1, cidades[i]->nome);
    }
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
    printf("Espere um momento...\n");

    //Abrindo o arquivo JSON para leitura
    FILE *file = fopen("municipios.json", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    char buffer[1024];
    char *jsonBuffer = (char *)malloc(1);
    jsonBuffer[0] = '\0'; //Inicializa a string vazia

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
    cJSON *root = cJSON_Parse(jsonBuffer);
    if (root == NULL) {
        printf("Erro ao fazer o parsing do JSON.\n");
        free(jsonBuffer);
        return 1;
    }

    //Criando a a tabela hash para buscar pelo nome
    HashTable_Nomes *hash_table = Cria_hash_table_nomes(10000);

    //Criando a tabela hash para busca dos vizinhos
    HashTable *hash_table_vizinho = Cria_hash_table(10000);

    //Array de cidades
    int numCidades = cJSON_GetArraySize(root);
    City cidades[numCidades];

    for (int i = 0; i < numCidades; ++i) {
        cJSON *cidadeJson = cJSON_GetArrayItem(root, i);

        //Salvar a cidade atual no array de cidades
        cidades[i].codigo_ibge = cJSON_GetObjectItem(cidadeJson, "codigo_ibge")->valueint;
        strcpy(cidades[i].nome, cJSON_GetObjectItem(cidadeJson, "nome")->valuestring);
        cidades[i].latitude = cJSON_GetObjectItem(cidadeJson, "latitude")->valuedouble;
        cidades[i].longitude = cJSON_GetObjectItem(cidadeJson, "longitude")->valuedouble;
        cidades[i].capital = cJSON_GetObjectItem(cidadeJson, "capital")->valueint;
        cidades[i].codigo_uf = cJSON_GetObjectItem(cidadeJson, "codigo_uf")->valueint;
        cidades[i].siafi_id = cJSON_GetObjectItem(cidadeJson, "siafi_id")->valueint;
        cidades[i].ddd = cJSON_GetObjectItem(cidadeJson, "ddd")->valueint;
        strcpy(cidades[i].fuso_horario, cJSON_GetObjectItem(cidadeJson, "fuso_horario")->valuestring);

        //Inserindo a cidade na tabela hash de nomes
        Insere_nome(hash_table, &cidades[i]);
        //Inserindo a cidade na tabela hash de vizinhos
        Insere_cidade(hash_table_vizinho, &cidades[i]);
    }

    //Recebe o nome de uma cidade dada pelo usuário
    char cidade_nome[100];
    printf("\nDigite o nome da cidade a ser buscada: ");
    fgets(cidade_nome, 100, stdin);
    cidade_nome[strcspn(cidade_nome, "\n")] = '\0'; //Remove o caractere de nova linha

    int numCidadesEncontradas, codigo_IBGE;
    //Procura uma ou mais cidades pelo nome inserido pelo usuário
    City **cidadesEncontradas = Procura_nome(hash_table, cidade_nome, &numCidadesEncontradas);

    //Encontrou apenas 1 cidade com o nome dado
    if (numCidadesEncontradas == 1) {
        printf("\nCidade encontrada:\n");
        printf("Codigo IBGE: %d\n", cidadesEncontradas[0]->codigo_ibge);
        //Salva o codigo IBGE da cidade
        codigo_IBGE = cidadesEncontradas[0]->codigo_ibge;
    }

    //Encontrou mais de uma cidade com o nome dado
    else if (numCidadesEncontradas > 1) {
        Exibir_cidades_encontradas(cidadesEncontradas, numCidadesEncontradas);
        int escolha;
        //Usuário deve escolher qual cidade ele quer pegar o código IBGE
        printf("\nEscolha uma cidade pelo numero correspondente: ");
        scanf("%d", &escolha);
        //Verifica se o número digitado é valido
        if (escolha > 0 && escolha <= numCidadesEncontradas) {
            printf("\nCidade escolhida:\n");
            printf("Codigo IBGE: %d\n", cidadesEncontradas[escolha - 1]->codigo_ibge);
            //Salva o codigo IBGE da cidade
            codigo_IBGE = cidadesEncontradas[escolha - 1]->codigo_ibge;
        } else {
            printf("Escolha invalida.\n");

            // Liberando a memória alocada para as cidades
            cJSON_Delete(root);
            free(jsonBuffer);
            free(cidadesEncontradas);
            for (int i = 0; i < hash_table->tamanho; ++i) {
                if (hash_table->table[i] != NULL) {
                    free(hash_table->table[i]);
                }
            }
            free(hash_table->table);
            free(hash_table);
            free(hash_table_vizinho->table);
            free(hash_table_vizinho);
            return 1;
        }
    } else {
        printf("Cidade nao encontrada.\n");

        // Liberando a memória alocada para as cidades
        cJSON_Delete(root);
        free(jsonBuffer);
        free(cidadesEncontradas);
        for (int i = 0; i < hash_table->tamanho; ++i) {
            if (hash_table->table[i] != NULL) {
                free(hash_table->table[i]);
            }
        }
        free(hash_table->table);
        free(hash_table);
        free(hash_table_vizinho->table);
        free(hash_table_vizinho);
        return 1;
    }

     /*TAREFA 2:*/

    //Construir a KD-tree
    KDNode *node_raiz = NULL;
    for (int i = 0; i < numCidades; i++) {
        node_raiz = Insere_cidade_kd(node_raiz, &cidades[i], 0);
    }

    int cidade_busca = codigo_IBGE;
    int N; //Número de vizinhos desejados
    //Usuário deve escolher a quantidade de vizinhos a serem buscados
    printf("\nDigite a quantidade de vizinhos a serem buscados: ");
    scanf("%d", &N);
    City cidadeBusca;
    //Procura na array de cidades a cidade com o codigo IBGE encontrado
    for (int i = 0; i < numCidades; i++) {
        if (cidades[i].codigo_ibge == cidade_busca) {
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

    //Procura os vizinhos da cidade com o codigo IBGE encontrado
    Procura_vizinhos(node_raiz, &cidadeBusca, N+1, distancias, vizinhos, 0);

    /*TAREFA 1:*/

    printf("\nAs informacoes da(s) %d cidade(s) vizinha(s) mais proxima(s) a %s sao:\n", N, cidade_nome);
    for (int i = 0; i < N+1; i++) {
        if (vizinhos[i] != cidade_busca) { //Verifica se não é a própria cidade de busca
            //Código IBGE da cidade a ser buscada
            int codigo_ibge = vizinhos[i];
            //Buscando a cidade na tabela hash
            City *cidadeBuscada = Procura_cidade(hash_table_vizinho, codigo_ibge);

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

    // Liberando a memória alocada para as cidades
    cJSON_Delete(root);
    free(jsonBuffer);
    free(cidadesEncontradas);
    for (int i = 0; i < hash_table->tamanho; ++i) {
        if (hash_table->table[i] != NULL) {
            free(hash_table->table[i]);
        }
    }
    free(hash_table->table);
    free(hash_table);
    free(node_raiz);
    free(hash_table_vizinho->table);
    free(hash_table_vizinho);

    return 0;
}
