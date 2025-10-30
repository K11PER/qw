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

int extration(unsigned char* pict, unsigned int h, unsigned int w){
    srand(KEY);
    
    int used_pos[5400] = {0};
    int used_count = 0;
    
    
    int msg_len = 0;
    for(size_t j = 0; j < 32; j++){
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
        unsigned char b = pict[r] & 1;
        msg_len |= (b << (31 - j));
    }
    
    printf("Длина сообщения: %d\n", msg_len);
    
    
    if (msg_len <= 0 || msg_len > 675) {
        printf("Ошибка, слишком длинное сообщение\n");
        return -1;
    }
    
    
    unsigned char msgout[676];
    for(size_t i = 0; i < msg_len; i++){
        msgout[i] = 0;
        for(size_t j = 0; j < 8; j++){
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
            unsigned char b = pict[r] & 1;
            msgout[i] |= (b << (7 - j));
        }
    }
    msgout[msg_len] = '\0';
    
    printf("Сообщение: %s\n", msgout);
    return 0;
}

int main(){
    struct BitMapFileHeader file_header;
    struct BitMapInfoHeader info_header;
  
    FILE* airp = fopen("newpict.bmp", "rb");
    if (airp == NULL) {
        printf("Ошибка, не удалось открыть файл\n");
        return 1;
    }

    fread(&file_header, sizeof(file_header), 1, airp);
    fread(&info_header, sizeof(info_header), 1, airp);

    unsigned int height = info_header.height;
    unsigned int width = info_header.width;
    unsigned char pict[width*height*3];

    fread(pict, sizeof(pict), 1, airp);

    extration(pict, height, width);

    fclose(airp);
    return 0;
}