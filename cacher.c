#include "img.h"
#include <stdio.h>

int main( int argc, char* argv[] ) {

    if(argc != 4) {
        printf("Arguments pas bons..\ncacher image secret sortie\n");
        return 0;
    }

    image* img = importer_image( argv[1] );

    if( img == NULL ) {
        printf("Impossible d'ouvrir %s\n", argv[1]);
        return 0;
    }

    image* img2 = importer_image( argv[2] );

    if( img2 == NULL ) {
        printf("Impossible d'ouvrir %s\n", argv[2]);
        return 0;
    }

    cacher(img, img2);
    exporter_image(img, argv[3]);

    detruire_image(img);
    detruire_image(img2);

    return 0;
}