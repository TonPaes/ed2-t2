/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização e Recuperação da Informação
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 620190
 * Aluno: Washington Paes Marques da Silva
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 31
#define TAM_GENERO 2
#define TAM_NASCIMENTO 11
#define TAM_CELULAR 16
#define TAM_VEICULO 31
#define TAM_PLACA 9
#define TAM_DATA 9
#define TAM_HORA 6
#define TAM_TRAJETO 121
#define TAM_VALOR 7
#define TAM_VAGAS 2
#define TAM_STRING_INDICE (30 + 6 + 4 + 1)

#define TAM_REGISTRO 256
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS "Busca por %s.\nNos percorridos:\n"

/* Registro da Carona */
typedef struct
{
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char genero[TAM_GENERO];
	char nascimento[TAM_NASCIMENTO]; /* DD/MM/AAAA */
	char celular[TAM_CELULAR];
	char veiculo[TAM_VEICULO];
	char placa[TAM_PLACA];
	char trajeto[TAM_TRAJETO];
	char data[TAM_DATA];   /* DD/MM/AA, ex: 24/09/19 */
	char hora[TAM_HORA];   /* HH:MM, ex: 07:30 */
	char valor[TAM_VALOR]; /* 999.99, ex: 004.95 */
	char vagas[TAM_VAGAS];
} Carona;

/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um nó  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;  /* numero de chaves armazenadas*/
	Chave_ip *chave; /* vetor das chaves e rrns [m-1]*/
	int *desc;		 /* ponteiros para os descendentes, *desc[m]*/
	char folha;		 /* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;  /* numero de chaves armazenadas*/
	Chave_is *chave; /* vetor das chaves e rrns [m-1]*/
	int *desc;		 /* ponteiros para os descendentes, *desc[m]*/
	char folha;		 /* flag folha da arvore*/
} node_Btree_is;

typedef struct
{
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
char ARQUIVO_IP[2000 * sizeof(Chave_ip)];
char ARQUIVO_IS[2000 * sizeof(Chave_is)];
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros.
 *done */
int carregar_arquivo();

/* (Re)faz o Cria iprimary
doine*/
void criar_iprimary(Indice *iprimary);
/* (Re)faz o índice de Caronas
doine */
void criar_iride(Indice *iride);

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata
* done */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
/*  adcionei um ponteiro por onde vou retornar a struc, senguindo o padrão da função write_btree
* done */
void *read_btree(void * retorno, int rrn, int ip);

/* Aloca dinamicamente os vetores de chaves e descendentes
DONE :3 */
void *criar_no(char ip);

/* Percorre a arvore e retorna o RRN da chave informada.  Retorna -1, caso não
 * encontrada.
 * done */
int buscar_chave_ip(int noderrn, char *pk, int exibir_caminho);

/* Percorre a arvore e retorna a pk da string destino/data-hora informada. Retorna -1, caso não
 * encontrada.
 * done */
char *buscar_chave_is(const int noderrn, const char *titulo, const int exibir_caminho);

/* Realiza percurso em-ordem imprimindo todas as caronas na ordem lexicografica do destino e data/hora e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_is(int noderrn, int nivel);

/*Gera Chave da struct Carona
* done */
void gerarChave(Carona *novo);
char * gerarTitulo( Carona j);
/* Função auxiliar que ordena as chaves em esq + a chave a ser inserida e divide
 * entre os nós esq e dir. Retorna o novo nó à direita, a chave promovida e seu
 * descendente direito, que pode ser nulo, caso a nó seja folha.
 * done*/
int divide_no_ip(int rrnesq, Chave_ip *chave, int desc_dir_rrn);
int divide_no_is(int rrnesq, Chave_is *chave, int desc_dir_rrn);

/* Lista todos os registros não marcados para remoção */
void listar(Indice iprimary, Indice iride);

/* Realiza percurso pré-ordem imprimindo as chaves primárias dos registros e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_ip(int noderrn, int nivel);

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona
* done */
Carona recuperar_registro(int rrn);

/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice *iprimary, Indice *iride);

int alterar(Indice iprimary);

void buscar(Indice iprimary, Indice iride);

void listar(Indice iprimary, Indice iride);

/*******************************************************/

void libera_no(void *node, char ip);

/*Realiza os scanfs na struct Carona*/
void ler_entrada(char *registro, Carona *novo);

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *iride, Carona j);

/* Insere um novo registro na Árvore B
done */
void insere_chave_ip(Indice *iprimary, Chave_ip chave);
void insere_chave_is(Indice *iride, Chave_is chave);

/* Função auxiliar para ser chamada recursivamente, inserir as novas chaves nas
 * folhas e tratar overflow no retorno da recursão.
 * done */
int insere_aux_ip(int noderrn,  Chave_ip * chave);
int insere_aux_is(int noderrn, Chave_is *chave);

/* VOCÊS NÃO NECESSARIAMENTE PRECISAM USAR TODAS ESSAS FUNÇÕES, É MAIS PARA TEREM UMA BASE MESMO,
 * PODEM CRIAR SUAS PRÓPRIAS FUNÇÕES SE PREFERIREM */


int main()
{
	char *p; /* # */
			 /* Arquivo */
	int carregarArquivo = 0;
	nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip * 3 + 4 + (-1 + ordem_ip) * 14;
	tamanho_registro_is = ordem_is * 3 + 4 + (-1 + ordem_is) * (TAM_STRING_INDICE + 10);

	/* Índice primário */
	Indice iprimary;
	iprimary.raiz = -1;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Caronas */
	Indice iride;
	iride.raiz = -1;
	criar_iride(&iride);

	/* Execução do programa */
	int opcao = 0;
	while (1)
	{
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
		case 1: /* Cadastrar uma nova Carona */
			cadastrar(&iprimary, &iride);
			break;
		case 2: /* Alterar a qtd de vagas de uma Carona */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar uma Carona */
			printf(INICIO_BUSCA);
			buscar(iprimary, iride);
			break;
		case 4: /* Listar todos as Caronas */
			printf(INICIO_LISTAGEM);
			listar(iprimary, iride);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if (!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IP; *p != '\0'; p += tamanho_registro_ip)
				{
					fwrite(p, 1, tamanho_registro_ip, stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if (!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IS; *p != '\0'; p += tamanho_registro_is)
				{
					fwrite(p, 1, tamanho_registro_is, stdout);
					puts("");
				}
			printf("%s\n", ARQUIVO_IS);
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]%*c", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe a Carona */
int exibir_registro(int rrn)
{
	if (rrn < 0)
		return 0;

	Carona j = recuperar_registro(rrn);
	char *traj, trajeto[TAM_TRAJETO];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.genero);
	printf("%s\n", j.nascimento);
	printf("%s\n", j.celular);
	printf("%s\n", j.veiculo);
	printf("%s\n", j.placa);
	printf("%s\n", j.data);
	printf("%s\n", j.hora);
	printf("%s\n", j.valor);
	printf("%s\n", j.vagas);

	strcpy(trajeto, j.trajeto);

	traj = strtok(trajeto, "|");

	while (traj != NULL)
	{
		printf("%s", traj);
		traj = strtok(NULL, "|");
		if (traj != NULL)
		{
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn)
{

	char temp[257], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[256] = '\0';
	Carona j;

	p = strtok(temp, "@");
	strcpy(j.nome, p);
	p = strtok(NULL, "@");
	strcpy(j.genero, p);
	p = strtok(NULL, "@");
	strcpy(j.nascimento, p);
	p = strtok(NULL, "@");
	strcpy(j.celular, p);
	p = strtok(NULL, "@");
	strcpy(j.veiculo, p);
	p = strtok(NULL, "@");
	strcpy(j.placa, p);
	p = strtok(NULL, "@");
	strcpy(j.trajeto, p);
	p = strtok(NULL, "@");
	strcpy(j.data, p);
	p = strtok(NULL, "@");
	strcpy(j.hora, p);
	p = strtok(NULL, "@");
	strcpy(j.valor, p);
	p = strtok(NULL, "@");
	strcpy(j.vagas, p);

	gerarChave(&j);

	return j;
}

/*Gera Chave da struct Carona*/
void gerarChave(Carona *novo){
	/* Não tenho certeza se essa associação vai funcionar, talvez eu tenha que acessar as str */
	novo->pk[0] = toupper(novo->nome[0]);
	novo->pk[1] = toupper(novo->placa[0]);
	novo->pk[2] = toupper(novo->placa[1]);
	novo->pk[3] = toupper(novo->placa[2]);
	novo->pk[4] = toupper(novo->data[0]);
	novo->pk[5] = toupper(novo->data[1]);
	novo->pk[6] = toupper(novo->data[3]);
	novo->pk[7] = toupper(novo->data[4]);
	novo->pk[8] = toupper(novo->hora[0]);
	novo->pk[9] = toupper(novo->hora[1]);
	novo->pk[10] = '\0';
}

char * gerarTitulo( Carona j){
	char titulo[52];
	char * aux;
	aux = strtok(titulo, "|");
	while(aux != NULL){
		aux = strtok(NULL, "|");
		strcpy(titulo, aux);
	}

	return titulo;
}	

/* (Re)faz o Cria iprimary
* Recebe a raiz e cria, uma nova arvore lendo o arquivo de dados */
void criar_iprimary(Indice *iprimary){
	int i;
	Carona aux_carona;
	Chave_ip * aux_chave;
	/* ler todas as caronas do arquivo */
	for(i=0; i < nregistros; i++){
		/*  testar se a carona existe
		 * pode haver o registro mas a carona não existir? */
		aux_carona = recuperar_registro(i);
		aux_chave = malloc(sizeof(Chave_ip));

		/* criar a chave com pk, e titulo */
		strcpy(aux_chave->pk, aux_carona.pk);
		aux_chave->rrn = i;

		insere_chave_ip( iprimary, * aux_chave);
	}
}

/* (Re)faz o índice de Caronas
// Recebe a raiz e cria, uma nova arvore lendo o arquivo de dados */
void criar_iride(Indice *iride){
	int i;
	Carona aux_carona;
	Chave_is * aux_chave;
	char * aux_titulo;
	/* ler todas as caronas do arquivo */
	for(i=0; i < nregistros; i++){
		/*  testar se a carona existe
		 * pode haver o registro mas a carona não existir? */
		aux_carona = recuperar_registro(i);
		aux_chave = malloc(sizeof(Chave_is));

		/* criar a chave com pk, e rrn */
		strcpy(aux_chave->pk, aux_carona.pk);
		aux_titulo = gerarTitulo(aux_carona);
		strcpy(aux_chave->string, aux_titulo );
		insere_chave_is( iride, * aux_chave);
	}
}

/*  Escrever o nó quando já estiver sido encontrado
// na arvore apenas transcrever pro arquivo
// já é garantido que é nesse nó que deve ser escrito( e já esta organizado)
// salvar é uma chave
ip == 1? primario : secundario */
void write_btree(void *salvar, int rrn, char ip){
	int i, j;
	char temp[20];


	if(ip == '1'){
		char * aux_node = (char *) malloc(sizeof(char) * tamanho_registro_ip + 1);
		/* casting no node para o indice certo */
		node_Btree_ip * salvar_ip = (node_Btree_ip *) criar_no('1');
		salvar_ip = salvar;

		/* preencher os registro com '#' e '*' */
		j = 0;
		 while( j < tamanho_registro_ip - ordem_ip * 3){
			aux_node[j] = '#';
			j++;
		}
		while( j < tamanho_registro_ip){
			aux_node[j] = '*';
			j++;
		}

		sprintf(temp,"%03d", salvar_ip->num_chaves);
		strncpy(aux_node, temp, 3);

		 for(i = 0; i < salvar_ip->num_chaves;i++){
			/*  somar os 3B para o num_chaves + os B das chaves já colocadas */
			strncpy(aux_node +  3 + i * 14 , salvar_ip->chave[i].pk, 10 );

			/* somar + 10 bytes da PK dessa chave
			rrn é um inteiro então tem que ser convertido
			rrn do arquivo */

			/*usar strncpy porque o strcpy preenche com 0*/
			sprintf(temp,"%04d", salvar_ip->chave[i].rrn);
			strncpy(aux_node + 3 + (i + 1)*(10) + i * 4, temp, 4);

		}

		int aux_ponteiro = 3 + (ordem_ip - 1) * 14;

		/*  Salvar se é folha ou não
		// essa conta do aux_ponteiro tem que ser testada */
		aux_node[aux_ponteiro] = salvar_ip->folha;
		aux_ponteiro++;
		/* salvar os RRNs das folhas, RRNs tem que estar na posição correta */

		/*  descendentes precisam do tratamento pro casao de não ter o filho da direita */
		 for(i = 0; i < ordem_ip ;i++){
				if (salvar_ip->desc[i] == -1)
					break;
				sprintf(temp,"%03d", salvar_ip->desc[i]);
				strncpy(aux_node + aux_ponteiro + i * 3, temp,3);

		}

		strncpy(ARQUIVO_IP + (rrn) * tamanho_registro_ip, aux_node, tamanho_registro_ip);
		free(aux_node);

	}
	 else{
		char * aux_node = (char *) malloc(sizeof(char) * tamanho_registro_is + 1);

		/* preencher os registro com '#' e '*' */
		j = 0;
		while( j > tamanho_registro_is - ordem_is * 3){
			aux_node[j] = '#';
			j++;
		}
		while( j > tamanho_registro_is){
			aux_node[j] = '*';
			j++;
		}

		/*  casting no node para o indice certo */
		node_Btree_is * salvar_is = (node_Btree_is *) malloc(sizeof(node_Btree_is));
		salvar_is = salvar;

		/*  o num de chaves já tem que estar atualizado
		* verificar se sscanf esta funcinando como esperado com o debugger
		* Salvando numero de chaves */
		sprintf(temp,"%d", salvar_is->num_chaves);
		strncpy(aux_node, temp, 3);


		for(i = 0; i < salvar_is->num_chaves;i++){
			/*  somar os 3B para o num_chaves + os B das chaves já colocadas */
			strncpy(aux_node +  3 + i * (10) + i * TAM_STRING_INDICE , salvar_is->chave[i].pk, 10 );

			/* somar + 10 bytes da PK dessa chave
			* chaves já vem formatadas pra no formato DESTINO$AAMMDD$HHMM */
			strncpy(aux_node + 3 + (i + 1)*(10) + i * (TAM_STRING_INDICE), salvar_is->chave[i].string , sizeof(salvar_is->chave[i].string));

		}


		int aux_ponteiro = 3 + (ordem_is - 1) * 51;

		/* Salvar se é folha ou não
		 essa conta do aux_ponteiro tem que ser testada */
		aux_node[aux_ponteiro] = salvar_is->folha;
		aux_ponteiro++;

		/*  descendentes precisam do tratamento pro casao de não ter o filho da direita */
		for(i = 0; i < ordem_is ;i++){
				if (salvar_is->desc[i] == -1)
					break;
				sprintf(temp,"%d", salvar_is->desc[i]);
				strncpy(aux_node + aux_ponteiro + i * 3, temp, 3);
		}

		strncpy(ARQUIVO_IS + (rrn) * tamanho_registro_is, aux_node, tamanho_registro_is );
		
		free(aux_node);

	}

}

/*Lê um nó do arquivo de índice e retorna na estrutura
// o ponteiro retorno já tem que ter sido alocado com a estrutura certa.
// ip == 1? primario : secundario */
void * read_btree( void * retorno, int rrn, int ip){

	if(ip == 1){

		node_Btree_ip * retorno_ip = (node_Btree_ip *) criar_no('1');
		char * aux_node = (char *) malloc(sizeof(char) *tamanho_registro_ip + 1 );
		char temp[20];
		char temp2[4];
		temp2[3] = '\0';
		int i;
		retorno_ip = (node_Btree_ip *) retorno;
		Chave_ip * aux_chave;


		strncpy(aux_node, ARQUIVO_IP + (rrn) * tamanho_registro_ip, tamanho_registro_ip);

		/*  Num de chaves tem que ser convertido pra int */
		strncpy(temp2, aux_node,3);
		sscanf(temp2, "%d", &retorno_ip->num_chaves);

		for(i = 0; i < retorno_ip->num_chaves;i++){
			/*  somar os 3B para o num_chaves + os B das chaves já colocadas */
			aux_chave = (Chave_ip *) malloc(sizeof(Chave_ip));
			strncpy(aux_chave->pk ,aux_node +  3 + i * 14, 10);


			/* somar + 10 bytes da PK dessa chave
			*rrn é um inteiro então tem que ser convertido */
			strncpy(temp, aux_node + 3 + (i + 1)*(10) + i * 4, 4);
			sscanf(temp, "%d", &aux_chave->rrn);

			/*  pode dar problema aqui */
			retorno_ip->chave[i] = * aux_chave;
		}


		int aux_ponteiro = 3 + (ordem_ip  - 1 ) * 14;

		/*  Salvar se é folha ou não
		* essa conta do aux_ponteiro tem que ser testada */
		retorno_ip->folha = aux_node[aux_ponteiro];
		aux_ponteiro++;

		

		/* salvar os RRNs das folhas, RRNs tem que estar na posição correta
		* descendentes precisam do tratamento pro casao de não ter o filho da direita */
		for(i = 0; i < ordem_ip ;i++){
				if(retorno_ip->folha == 'T')
					break;
				strncpy(temp2, aux_node + aux_ponteiro + i * 3, 3);
				if(0 != strncmp("***", temp2, 3))
					sscanf(temp2, "%d",  &retorno_ip->desc[i]);
		}
		return  retorno_ip;



	}else{
		node_Btree_is * retorno_is;
		char * aux_node = malloc(sizeof(char) *tamanho_registro_is);
		char  temp[3];
		retorno_is = retorno;
		Chave_is * aux_chave;
		int i = 0;

		/* vetor de chaves vai ser allocado dinamicamente com todas as posições */
		retorno_is->chave =  malloc(sizeof(Chave_is *) * (ordem_is-1) );
		/* vetor de destinos também vai ser alocado dinamicamente com todas posições */
		retorno_is->desc =  malloc(sizeof(int) * (ordem_is));

		strcpy(aux_node, ARQUIVO_IS + (rrn) * tamanho_registro_is);

		/* num de chaves tem que ser convertido pra int */
		strncpy(temp, aux_node,3);
		sscanf(temp, "%d" ,  &retorno_is->num_chaves);


		for(i = 0; i < retorno_is->num_chaves;i++){
			/*  somar os 3B para o num_chaves + os B das chaves já colocadas */
			aux_chave = malloc(sizeof(Chave_is));
			strcpy(aux_chave->pk ,aux_node +  3 + i * (10) + i * TAM_STRING_INDICE);


			/* somar + 10 bytes da PK dessa chave */
			strcpy(aux_chave->string, aux_node +  3 + (i+1) * (10) + i * TAM_STRING_INDICE);

			/*  pode dar problema aqui */
			retorno_is->chave[i] = * aux_chave;
		}

		int aux_ponteiro = 3 + (ordem_is - 1)* 51;
		/*  Salvar se é folha ou não
		 essa conta do aux_ponteiro tem que ser testada */
		retorno_is->folha = aux_node[aux_ponteiro];
		aux_ponteiro++;

		/* salvar os RRNs das folhas, RRNs tem que estar na posição correta
		 descendentes precisam do tratamento pro casao de não ter o filho da direita */
		for(i = 0; i <= retorno_is->num_chaves ;i++){

				strcpy(temp, aux_node + aux_ponteiro + i * 3);
				sscanf(temp,"%d", &retorno_is->desc[i]);
		}

	}
}
/* Percorre a arvore e retorna o RRN da chave informada.  Retorna -1, caso não
 * encontrada.
 * 3 param: 1 exibir caminho, 0 não */
int buscar_chave_ip(int noderrn, char *pk, int exibir_caminho){

	int i = 1;

	/* recuperar nó */
	node_Btree_ip * aux_node = (node_Btree_ip *) criar_no('1');
	read_btree( aux_node, noderrn, 1);

	/* imprimir a caminho */
	while( i <= aux_node->num_chaves && 0 < strcmp( pk, aux_node->chave[i-1].pk)){
		if(exibir_caminho)
			printf("%s ",aux_node->chave[i-1].pk);
		i++;
	}

	if( i <= aux_node->num_chaves && 0 == strcmp(pk, aux_node->chave[i].pk))
		return aux_node->chave[i-1].rrn;

	if(aux_node->folha)
		return -1;
	else
		return buscar_chave_ip( aux_node->desc[i-1] , pk, exibir_caminho);
}

char * buscar_chave_is(const int noderrn, const char *titulo, const int exibir_caminho){
	int i = 1;
	node_Btree_is * aux_node = malloc(sizeof(node_Btree_is));
	read_btree(aux_node, noderrn, 0);

	while( i <= aux_node->num_chaves && 0 < strcmp(titulo, aux_node->chave[i-1].string)){
		if(exibir_caminho)
		/* tem que marcar a ultima chave de cada nó com \n ao invés de virgula */
			printf("%s ",aux_node->chave[i-1].string);
		i++;
	}
	if(i <= aux_node->num_chaves && 0 == strcmp(titulo, aux_node->chave[i-1].string))
		return aux_node->chave[i-1].pk;

	if(aux_node->folha)
		return "-1";
	else{
		return buscar_chave_is(aux_node->desc[i-1], titulo, exibir_caminho);
	}
}

void cadastrar(Indice *iprimary, Indice *iride){

}

int alterar(Indice iprimary){
	return -1;
}

void buscar(Indice iprimary, Indice iride){

}

void listar(Indice iprimary, Indice iride){

}

/* Insere um novo registro na Árvore B */
void insere_chave_ip(Indice *iprimary, Chave_ip chave){
	int i = 0;
	node_Btree_ip * aux_node = (node_Btree_ip *) criar_no('1');
	aux_node->num_chaves = 0;
	i = 0;
	if( iprimary->raiz == -1){
		aux_node->chave[0] = chave;
		aux_node->num_chaves++;
		aux_node->folha = 'T';
		write_btree(aux_node, nregistrosip, '1');
		iprimary->raiz = 0;
		free(aux_node);

	}else{
		/* chave vai ser o filho direito no retorno*/
		int filho_direito = insere_aux_ip(iprimary->raiz, &chave);
		if(filho_direito != -1){
			aux_node->folha = 'F';
			aux_node->num_chaves++;
			aux_node->chave[0] = chave;
			aux_node->desc[0] = iprimary->raiz;
			aux_node->desc[1] = filho_direito;
			nregistrosip++;
			write_btree(aux_node, nregistrosip, '1');
			
			iprimary->raiz = nregistrosip;
		}
	}


}
void insere_chave_is(Indice *iride, Chave_is chave){

	node_Btree_is * aux_node = malloc(sizeof( node_Btree_is));

	if( iride->raiz == -1){
		aux_node->chave[0] = chave;
		aux_node->num_chaves++;
		aux_node->folha = 'T';
		write_btree(aux_node, nregistrosis, '0');
		iride->raiz = 0;

	}else{
		/* chave vai ser o filho direito no retorno*/
		int filho_direito = insere_aux_is(iride->raiz, &chave);
		if(filho_direito != -1){
			aux_node->folha = 'F';
			aux_node->num_chaves++;
			aux_node->chave[0] =  chave;
			aux_node->desc[0] = iride->raiz;
			aux_node->desc[1] = filho_direito;
			iride->raiz = nregistrosis;
			write_btree(aux_node, nregistrosis, '0');


		}
	}
}

/* Função auxiliar para ser chamada recursivamente, inserir as novas chaves nas
 * folhas e tratar overflow no retorno da recursão.
 * 0 sem problemas, 1 overflow, -1 erro
 * nope, o retorno aqui tem que ser o RRN*/
int insere_aux_ip(int noderrn,  Chave_ip * chave){
	int i;
	int filho_direita;
	Chave_ip chavinha;
	node_Btree_ip * aux_node = (node_Btree_ip *) criar_no('1');
	read_btree(aux_node, noderrn, 1);
	/* um monte de duvida sobre como segurar o rrn*/


	if(aux_node->folha == 'T'){

		if(aux_node->num_chaves < ordem_ip -1){
			i = aux_node->num_chaves;

			while( i > 0 && 0 > strcmp(chave->pk, aux_node->chave[i-1].pk)){
				aux_node->chave[i] = aux_node->chave[i-1];
				i--;
			}
			aux_node->chave[i] = *chave;
			chavinha = aux_node->chave[1];

			aux_node->num_chaves++;
			write_btree(aux_node, noderrn, '1');
			chave = NULL;

			return -1;
		}else
		{
			return divide_no_ip(noderrn, chave, -1);
		}
	}else
	{
		i = aux_node->num_chaves ;
		while( i > 0 && 0 > strcmp(chave->pk, aux_node->chave[i-1].pk))
			i--;
		/* Quando não encontra*/
		filho_direita = insere_aux_ip(aux_node->desc[i], chave);

		if(filho_direita != -1){

			/*problema da divisão da propagação, os filhos não estão sendo copiados
			*
			*
			* CONTINUE?
			*
			*/
			if(aux_node->num_chaves < ordem_ip -1){
				i = aux_node->num_chaves;
				while( i > 0 && 0 > strcmp(chave->pk, aux_node->chave[i-1].pk)){
					aux_node->desc[i+1] = aux_node->desc[i];
					aux_node->chave[i] = aux_node->chave[i-1];
					i--;
				}
				aux_node->chave[i] = * chave;
				aux_node->desc[i+1] = filho_direita;
				aux_node->num_chaves++;
				write_btree(aux_node, noderrn, '1');
				chave = NULL;
				return -1;

			}else{
				return divide_no_ip(noderrn, chave, filho_direita);
			}

		}else
			/*caso onde da merda*/
			return -1;
	}
}


int insere_aux_is(int noderrn, Chave_is *chave){
	int i;
	int filho_direita;
	node_Btree_is * aux_node = malloc(sizeof(node_Btree_is));
	/* um monte de duvida sobre como segurar o rrn*/
	read_btree(aux_node, noderrn, 0);

	if(aux_node->folha == 'T'){

		if(aux_node->num_chaves < ordem_is -1){
			i = aux_node->num_chaves;

			while( i > 0 && 0 > strcmp(chave->string, aux_node->chave[i].string)){
				aux_node->chave[i] = aux_node->chave[i+1];
				i--;
			}
			aux_node->chave[i] = * chave;
			aux_node->num_chaves++;
			write_btree(aux_node, noderrn, '0');
			chave = NULL;

			return -1;
		}else
		{
			return divide_no_is(noderrn, chave, -1);
		}
	}else
	{
		i = aux_node->num_chaves;
		while( i > 0 && 0 < strcmp(chave->string, aux_node->chave[i].string))
			i--;
		i++;
		/* Quando não encontra*/
		filho_direita = insere_aux_is(aux_node->desc[i], chave);

		if(filho_direita != -1){

			if(aux_node->num_chaves < ordem_ip -1){
				i = aux_node->num_chaves;
				while( i > 0 && 0 > strcmp(chave->string, aux_node->chave[i].string)){
					aux_node->desc[i] = aux_node->desc[i+1];
					aux_node->chave[i] = aux_node->chave[i+1];
					i--;
				}
				aux_node->chave[i] = * chave;
				aux_node->desc[i] = filho_direita;
				aux_node->num_chaves++;
				write_btree(aux_node, noderrn, '0');
				chave = NULL;
				return -1;

			}else{
				return divide_no_is(noderrn, chave, filho_direita);
			}

		}else
			/*caso onde da merda*/
			return -1;
	}
}

/* Função auxiliar que ordena as chaves em esq + a chave a ser inserida e divide
 * entre os nós esq e dir. Retorna o novo nó à direita, a chave promovida e seu
 * descendente direito, que pode ser nulo, caso a nó seja folha. */
int divide_no_ip(int rrnesq, Chave_ip *chave, int desc_dir_rrn){
	int i = 0, j = 0;
	int chave_alocada = 0;
	/* recuperando o nó que vai ser dividido */
	node_Btree_ip * aux_node = (node_Btree_ip *) criar_no('1');
	node_Btree_ip *  new_node = (node_Btree_ip *) criar_no('1');
	read_btree(aux_node, rrnesq, 1);

	i = aux_node->num_chaves;
	new_node->folha = aux_node->folha;
	new_node->num_chaves = (ordem_ip -1) /2;

	for(j = new_node->num_chaves; j > 0; j--){
		if( !chave_alocada && 0 < strcmp(chave->pk, aux_node->chave[i-1].pk)){
			new_node->chave[j-1] = * chave;
			new_node->desc[j] = desc_dir_rrn;
			chave_alocada = 1;
		}else
		{
			new_node->chave[j-1] = aux_node->chave[i-1];
			new_node->desc[j] = aux_node->desc[i];
			i--;
		}
	}

	if(!chave_alocada){
		while (i >= 1 && 0 > strcmp(chave->pk, aux_node->chave[i-1].pk) ){
			aux_node->chave[i] = aux_node->chave[i-1];
			aux_node->desc[i+1] = aux_node->desc[i];
			i--;

		}
		aux_node->chave[i] = *chave;
		aux_node->desc[i+1] = desc_dir_rrn;
	}
	i = (ordem_ip/2);

	/*chave é o nó promovido*/
	* chave = aux_node->chave[i];

	new_node->desc[0] = aux_node->desc[( i / 2) + 2];
	aux_node->num_chaves = i;

	write_btree(aux_node, rrnesq, '1');

	nregistrosip++;
	write_btree(new_node, nregistrosip, '1');
	

	/* nregistros vai ser o rrn do filho_direito*/
	return nregistrosip;
	/* tem que retornar o RRN no new_node quando ele for escrito no indice*/
}
int divide_no_is(int rrnesq, Chave_is *chave, int desc_dir_rrn){
	int i = 0, j = 0;
	int chave_alocada = 0;
	/* recuperando o nó que vai ser dividido */
	node_Btree_is * aux_node = malloc(sizeof(node_Btree_is));
	node_Btree_is *  new_node = malloc(sizeof(node_Btree_is));
	read_btree(aux_node, rrnesq, 0);

	i = aux_node->num_chaves;
	new_node->folha = aux_node->folha;
	new_node->num_chaves = ordem_is/2;

	for(j = new_node->num_chaves; j > 0; j--){
		if( !chave_alocada && 0 < strcmp(chave->string, aux_node->chave[i-1].string)){
			new_node->chave[j-1] = * chave;
			new_node->desc[j] = desc_dir_rrn;
			chave_alocada = 1;
		}else
		{
			new_node->chave[j-1] = aux_node->chave[i-1];
			new_node->desc[j] = aux_node->desc[i];
			i--;
		}
	}

	if(!chave_alocada){
		while (i >= 1 && 0 > strcmp(chave->string, aux_node->chave[i-1].string) ){
			aux_node->chave[i] = aux_node->chave[i-1];
			aux_node->desc[i+1] = aux_node->desc[i];
		}
		aux_node->chave[i] = *chave;
		aux_node->desc[i+1] = desc_dir_rrn;
	}
	i = (ordem_is/2);

	/*chave é o nó promovido*/
	* chave = aux_node->chave[i];
	new_node->desc[0] = aux_node->desc[i+1];
	aux_node->num_chaves = i;

	write_btree(new_node, nregistrosip, '0');

	/* nregistros vai ser o rrn do filho_direito*/
	return nregistrosis - 1 ;
	/* tem que retornar o RRN no new_node quando ele for escrito no indice*/
}

void *criar_no(char ip){
	int i;
	if(ip == '1'){
		node_Btree_ip * aux_node = (node_Btree_ip *)  malloc(sizeof(node_Btree_ip));
		aux_node->chave = (Chave_ip *) malloc(sizeof(Chave_ip ) * (ordem_ip - 1));
		aux_node->desc = (int*) malloc(sizeof(int) *ordem_ip);
		for (i = 0; i < ordem_ip; i++)
			aux_node->desc[i] = -1;

		return aux_node;
	}else{
		node_Btree_is * aux_node = (node_Btree_is *) malloc(sizeof(node_Btree_is));
		aux_node->chave = (Chave_is *) malloc(sizeof(Chave_is) * (ordem_is - 1));
		aux_node->desc = (int *) malloc(sizeof(int) * ordem_is);
		for ( i = 0; i < ordem_is; i++)
			aux_node->desc[i] = -1;
		return aux_node;
	}

}
