#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VIRUS_NAME_LEN 16
#define MAGIC_LEN 4

typedef struct virus {
    unsigned short SigSize;
    unsigned char* VirusName;
    unsigned char* Sig;
} virus;

typedef struct link link;
struct link {
    link *nextVirus;
    virus *vir;
};

struct fun_desc_link {
    char *name;
    link* (*fun)(link*);
};

struct fun_desc_void {
    char *name;
    void (*fun)(link*, char*);
};

void free_virus(virus* v) {
    if (v != NULL) {
        free(v->VirusName);
        free(v->Sig);
        free(v);
    }
}

int read_magic(FILE* file) {
    unsigned char magic_number[MAGIC_LEN];
    if (fread(magic_number, 1, MAGIC_LEN, file) != MAGIC_LEN) {
        fprintf(stderr, "Error: Failed to read magic number.\n");
        return -1;
    }
    
    if (memcmp(magic_number, "VIRL", MAGIC_LEN) == 0) {
        return 0;
    }

    else if (memcmp(magic_number, "VIRB", MAGIC_LEN) == 0) {
        return 1;
    }

    else {
        fprintf(stderr, "Error: Invalid magic number.\n");
        return -1;
    }
}

int init_virus_name(FILE* file, unsigned char* empty_virus_name) {
    if (fread(empty_virus_name, 1, VIRUS_NAME_LEN, file) != VIRUS_NAME_LEN) {
        fprintf(stderr, "Error: VirusName must be %d bytes.\n", VIRUS_NAME_LEN);
        return 0;
    }
    return 1;
}

int init_virus_sig(FILE* file, unsigned char* empty_virus_sig, unsigned short sig_size) {
    if (fread(empty_virus_sig, 1, sig_size, file) != sig_size) {
        fprintf(stderr, "Error: Virus signature init failed.\n");
        return 0;
    }
    return 1;
}

virus* readVirus(FILE* file, int is_big_endian) {
    unsigned char sig_size[2];
    size_t bytesRead = fread(sig_size, 1, 2, file);
    if (bytesRead  != 2) {
        if (bytesRead == 1) {
            fprintf(stderr, "Error: Failed to read SigSize.\n");
        }
        return NULL;
    }

    virus* v  = malloc(sizeof(virus));
    if (v == NULL) {
        fprintf(stderr, "Error: Memory allocation for virus failed.\n");
        return NULL;
    }

    if (is_big_endian) {
        v->SigSize = (sig_size[0] << 8) | sig_size[1];
    }
    else {
        v->SigSize = (sig_size[1] << 8) | sig_size[0];
    }

    v->VirusName = (unsigned char*) calloc(VIRUS_NAME_LEN, sizeof(unsigned char));
    if (v->VirusName == NULL) {
        free_virus(v);
        fprintf(stderr, "Error: Memory allocation failed for virus name.\n");
        return NULL;
    }

    if (init_virus_name(file, v->VirusName) == 0) {
        free_virus(v);
        return NULL;
    }

    v->Sig = (unsigned char*) malloc(v->SigSize);
    if (v->Sig == NULL) {
        free_virus(v);
        fprintf(stderr, "Error: Memory allocation failed for virus sig.\n");
        return NULL;
    }

    if (init_virus_sig(file, v->Sig, v->SigSize) == 0) {
        free_virus(v);
        return NULL;
    }

    return v;
}

void printVirus(virus* virus, FILE* output) {
    if (virus == NULL) {
        fprintf(stderr, "Error: Can't print NULL virus.\n");
        return;
    }

    if (virus->VirusName == NULL) {
        fprintf(stderr, "Error: Can't print NULL VirusName.\n");
        return;
    }
    fprintf(output, "Virus name: %s\n", virus->VirusName);

    fprintf(output, "Virus size: %hu\n", virus->SigSize);

    if (virus->Sig == NULL) {
        fprintf(stderr, "Error: Can't print NULL Sig.\n");
        return;
    }
    
    fprintf(output, "signature:\n");
    for (int i = 0; i < virus->SigSize; i++) {
        fprintf(output, "%02X ", virus->Sig[i]);
    }
    fprintf(output, "\n\n");
}

void list_print(link *virus_list, FILE* file) {
    link *curr_link = virus_list;
    while (curr_link != NULL) {
        printVirus(curr_link->vir, file);
        curr_link = curr_link->nextVirus;
    }    
}

link* list_append(link* virus_list, virus* data) {
    if (data == NULL) {
        fprintf(stderr, "Error: cant append NULL link to virus list.\n");
        return virus_list;
    }

    link *append_link = (link*) malloc(sizeof(link));
    if (append_link == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for new list link.\n");
        return virus_list;
    }

    append_link->vir = data;
    append_link->nextVirus = NULL;
    if (virus_list == NULL) {
        return append_link;
    }

    link *curr_link = virus_list;
    while (curr_link->nextVirus != NULL) {
        curr_link = curr_link->nextVirus;
    } 
    curr_link->nextVirus = append_link;
    return virus_list;
}

void list_free(link *virus_list) {
    link *curr_link = virus_list;
    while (curr_link != NULL) {
        link *next = curr_link->nextVirus;
        free_virus(curr_link->vir);
        free(curr_link);  
        curr_link = next;
    }   
}

void print_menu(struct fun_desc_link menu[]) {
    for (int i = 0; menu[i].name != NULL; i++) {
        printf("%d) %s\n", i + 1, menu[i].name);
    }
    printf("option : ");
}

FILE* open_cmd_file(char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: cannot open file %s\n", fileName);
        return NULL;
    }
    return file;
}

link* load_signatures(link* virus_list) {
    list_free(virus_list);
    virus_list = NULL;
    char fileName[256];
    printf("Enter signature file name: ");
    if (fgets(fileName, sizeof(fileName), stdin) == NULL) {
        fprintf(stderr, "Error: error getting signature file name.\n");
        return NULL;
    }
    fileName[strcspn(fileName, "\n")] = '\0';
    FILE* file = open_cmd_file(fileName);
    if (file == NULL) {
        return NULL;
    }

    int is_big_endian = read_magic(file);
    if (is_big_endian == -1) {
        fclose(file);
        return NULL;
    }

    virus* v = NULL;
    while ((v = readVirus(file, is_big_endian)) != NULL) {
        virus_list = list_append(virus_list, v);
    }

    fclose(file);
    return virus_list;
}

link* print_signatures(link* virus_list) {
    if (virus_list != NULL) {
        list_print(virus_list, stdout);
    }
    return virus_list;
}

void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    link *curr = virus_list;
    while (curr != NULL) {
        virus *v = curr->vir;
        if (!(v->SigSize > size)) {
            for (unsigned int i = 0; i <= size - v->SigSize; i++) {
                if (memcmp(buffer + i, v->Sig, v->SigSize) == 0) {
                    printf("Virus detected!\n");
                    printf("Starting byte location: %u\n", i);
                    printf("Virus name: %s\n", v->VirusName);
                    printf("Virus signature size: %d\n\n", v->SigSize);
                }
            }
        }
        curr = curr->nextVirus;
    }
}

unsigned int read_file(link* virus_list, FILE *file, char *buffer, size_t buffer_size) {
    if (virus_list == NULL) {
        fprintf(stderr, "Error: no virus signatures loaded. Please load signatures first.");
        return 0;
    }

    unsigned int bytesRead;
    if ((bytesRead = fread(buffer, 1, buffer_size, file)) == 0) {
        fprintf(stderr, "Error: failed to read from input file\n");
        return 0;
    }
    return bytesRead;
}

void detect_viruses_call(link* virus_list, char *fileName) {
    FILE* file = open_cmd_file(fileName);
    if (file == NULL) {
        return;
    }

    char buffer[10000];
    unsigned int size = read_file(virus_list, file, buffer, sizeof(buffer));
    if (size > 0) {
        detect_virus(buffer, size, virus_list);
    }
    fclose(file);
}

void neutralize_virus(char *fileName, int signatureOffset) {
    FILE* file = fopen(fileName, "r+b");
    if (file == NULL) {
        return;
    }

    if (fseek(file, signatureOffset, SEEK_SET) != 0) {
        fprintf(stderr, "Error: failed to offset file pointer.\n");
        return;
    }

    unsigned char byte_to_change[1] = { 0xC3 };
    if(fwrite(byte_to_change, sizeof(unsigned char), 1, file) != 1) {
        fprintf(stderr, "Error: failed to writw to file.\n");
        return;
    }
    fclose(file);
}

void fix_file(link* virus_list, char *fileName) {
    FILE* file = open_cmd_file(fileName);
    if (file == NULL) {
        return;
    }

    char buffer[10000];
    unsigned int size = read_file(virus_list, file, buffer, sizeof(buffer));
    fclose(file);
    if (size == 0) {
        return;
    }

    link *curr = virus_list;
    while (curr != NULL) {
        virus *v = curr->vir;
        if (!(v->SigSize > size)) {
            for (unsigned int i = 0; i <= size - v->SigSize; i++) {
                if (memcmp(buffer + i, v->Sig, v->SigSize) == 0) {
                    neutralize_virus(fileName, i);
                }
            }
        }
        curr = curr->nextVirus;
    }
}

link* quit(link* virus_list) {
    list_free(virus_list);
    virus_list = NULL;
    return virus_list;
}

int main(int argc, char **argv){
    if (argc != 2) {
        fprintf(stderr, "Error: argc should equal 2\n");
        exit(1);
    }
    char *fileName = argv[1];

    struct fun_desc_link main_menu[] = { { "Load signatures", load_signatures }, { "Print signatures", print_signatures }, {"Detect viruses", NULL}, {"Fix file", NULL}, {"Quit", quit}, { NULL, NULL } };
    struct fun_desc_void void_menu[] = { {"Detect viruses", detect_viruses_call }, {"Fix file", fix_file}, { NULL, NULL } };
    int menu_length = 0;
    while (main_menu[menu_length].name != NULL) {
        menu_length++;
    }
    char buffer[50];
    link* virus_list = NULL;

    while (!feof(stdin)) {
        printf("Select operation from the following menu (ctrl^D for exit):\n");
        print_menu(main_menu);
  
        char *num = fgets(buffer, sizeof(buffer), stdin);
        if (feof(stdin)) {
            printf("\n");
            break;
        }  
        int func_num = atoi(num) - 1;
        printf("\n");
  
        if (func_num >= 0 && func_num < menu_length) {
            printf("Within bounds\n");
            if (main_menu[func_num].fun == quit) {
                printf("Quitting...\n");
                break;
            }

            if (func_num >= 2 && func_num <= 3) {
                void_menu[func_num - 2].fun(virus_list, fileName);
            }

            else {
                virus_list = main_menu[func_num].fun(virus_list);
            }
            printf("DONE.\n\n");
        }
  
        else {
            printf("Not within bounds\n");
            break;
        }
    }
    virus_list = quit(virus_list);
    return 0;
  }
  