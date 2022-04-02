#include "img.h"
#include <stdio.h>

int main( int argc, char* argv[] ) {

    if(argc != 3) {
        printf("Arguments pas bons..\nreveler image sortie\n");
        return 0;
    }

    image* img = importer_image( argv[1] );

    if( img == NULL ) {
        printf("Impossible d'ouvrir %s\n", argv[1]);
        return 0;
    }

    image* sortie = relever(img);
    exporter_image(sortie, argv[2]);

    detruire_image(sortie);
    detruire_image(img);

    return 0;
}