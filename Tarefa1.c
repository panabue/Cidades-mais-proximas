/*Dado um codigo_ibge, retorna as informações desta cidade*/

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

//Função para procurar uma cidade na tabela hash
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

int main() {
    // Abrindo o arquivo JSON para leitura
    FILE *file = fopen("municipios.json", "r");
    if (file == NULL) {
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

        //Inserindo a cidade na tabela hash
        Insere_cidade(hash_table, &cidades[i]);
    }

    //Código IBGE da cidade a ser buscada
    int codigo_ibge;
    printf("Digite o codigo IBGE da cidade a ser buscada: ");
    scanf("%d", &codigo_ibge);

    //Buscando a cidade na tabela hash
    City *cidadeBuscada = Procura_cidade(hash_table, codigo_ibge);

    //Exibindo as informações da cidade buscada, se encontrada
    if (cidadeBuscada != NULL) {
        printf("\nCidade encontrada:\n");
        printf("Codigo IBGE: %d\n", cidadeBuscada->codigo_ibge);
        printf("Nome: %s\n", cidadeBuscada->nome);
        printf("Latitude: %.4f\n", cidadeBuscada->latitude);
        printf("Longitude: %.4f\n", cidadeBuscada->longitude);
        printf("Capital: %s\n", cidadeBuscada->capital ? "Sim" : "Não");
        printf("Codigo UF: %d\n", cidadeBuscada->codigo_uf);
        printf("SIAFI ID: %d\n", cidadeBuscada->siafi_id);
        printf("DDD: %d\n", cidadeBuscada->ddd);
        printf("Fuso Horario: %s\n", cidadeBuscada->fuso_horario);
    } else {
        printf("Cidade não encontrada.\n");
    }

    //Liberando memória alocada
    cJSON_Delete(raiz);
    free(jsonBuffer);
    free(hash_table->table);
    free(hash_table);

    return 0;
}
