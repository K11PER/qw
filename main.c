#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
unsigned int KEY = 3876472784;

struct BitMapFileHeader
{
    uint16_t type;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t data_offset;
};

struct BitMapInfoHeader
{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    int32_t xpm;
    int32_t ypm;
    uint32_t colors_used;
    uint32_t colors_important;
};

#pragma pack(pop)

int secure(unsigned char* pict, unsigned char* msg, unsigned int h, unsigned int w){
    srand(KEY);
    
    int msg_len = strlen((char*)msg);
    printf("Длина сообщения: %d \n", msg_len);
    
    
    if (msg_len * 8 > w*h*3) {
        printf("Слишком длинное сообщение\n");
        return -1;
    }
    
    int used_pos[5400] = {0};
    int used_count = 0;

    
    for(size_t j = 0; j < 32; j++){
        int b = (msg_len >> (31 - j)) & 1;
        
        int r;
        int u;
        do {
            u = 1;
            r = rand() % (h*w*3);
            for(int k = 0; k < used_count; k++) {
                if (used_pos[k] == r) {
                    u = 0;
                    break;
                }
            }
        } while (!u);
        
        used_pos[used_count++] = r;
        pict[r] = (pict[r] & 0b11111110) | b;
    }

    
    for(size_t i = 0; i < msg_len; i++){
        for(size_t j = 0; j < 8; j++){
            int bit = (msg[i] >> (7 - j)) & 1;
            
            int r;
            int u;
            do {
                u = 1;
                r = rand() % (w*h*3);
                for(int k = 0; k < used_count; k++) {
                    if (used_pos[k] == r) {
                        u = 0;
                        break;
                    }
                }
            } while (!u);
            
            used_pos[used_count++] = r;
            pict[r] = (pict[r] & 0b11111110) | bit;
        }
    }
    
    return 0;
}

int main(){
    struct BitMapFileHeader file_header;
    struct BitMapInfoHeader info_header;

    

    char file[100];

    printf("Введите название файла с раширением .bmp: ");

    fgets(file, sizeof(file), stdin);
    file[strcspn(file, "\n")] = 0;
    
    char msg[676];
    
    printf("Введите сообщение: ");
    fgets(msg, sizeof(msg), stdin);
    msg[strcspn(msg, "\n")] = 0;


    int len = strlen(file);
    if (len >= 4 && file[len-1] == 'p' && file[len-2] == 'm' && file[len-3] == 'b' && file[len-4] == '.') {
        printf("");
    } else {
        strcat(file, ".bmp");
    }
        
    
    FILE* airp = fopen(file, "rb");
    if (airp == NULL) {
        printf("Ошибка, не удалось открыть файл\n");
        return 1;
    }
    // FILE* airp12 = fopen(file, "rb");
    // if (airp == NULL) {
    //     printf("Ошибка, не удалось открыть файл\n");
    //     return 1;
    // }



    fread(&file_header, sizeof(file_header), 1, airp);
    fread(&info_header, sizeof(info_header), 1, airp);

    unsigned int height = info_header.height;
    unsigned int width = info_header.width;
    unsigned char pict[width*height*3];
    unsigned char pict1[width*height*3];

    fread(pict, sizeof(pict), 1, airp);
    memcpy(&pict1, &pict, sizeof(pict));

 

    if (secure(pict, (unsigned char*)msg, height, width) == 0) {
        FILE* airp1 = fopen("newpict.bmp", "wb");
        fwrite(&file_header, sizeof(file_header), 1, airp1);
        fwrite(&info_header, sizeof(info_header), 1, airp1);
        fwrite(pict, sizeof(pict), 1, airp1);
        for(size_t i = 0; i < sizeof(pict1); i++){
            pict1[i] -= pict[i];
            pict1[i] *= 200; 
        }

        FILE* airp2 = fopen("newpictraz.bmp", "wb");
        fwrite(&file_header, sizeof(file_header), 1, airp2);
        fwrite(&info_header, sizeof(info_header), 1, airp2);
        fwrite(pict1, sizeof(pict1), 1, airp2);
        fclose(airp1);
        fclose(airp2);
        printf("Успех\n");
    }
    
    fclose(airp);
    return 0;
}
