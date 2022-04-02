#include <stdint.h>
#include <stdbool.h>

typedef enum mode_image {
    PGM_P2,
    PGM_P3
} mode_image;

typedef struct image {
    int hauteur;
    int largeur;
    uint8_t* pixels;

    int intervalle_valeurs;
    mode_image mode;
} image;


image* creer_image( int l, int h, mode_image mode );
void detruire_image( image* img );

image* copier( image* img );

/* converti img au format nv_mode ET DETRUIT LE POINTEUR img */
image* convertir_format( image* img, mode_image nv_mode );

bool exporter_image( image* img, char* chemin );
image* importer_image( char* chemin );

/* effectue une symetrie axiale verticale */
void mirroir( image* img );

/* rend img flou, "niveau" ~ 1-10 */
void flou( image* img, int niveau );

/* redimensionne img ET DETRUIT LE POINTEUR img */
image* redim( image* img, int l, int h );

/* supperpose coeff% de im1 avec 1 - coeff% de img2 */
//image* superposer( image* img1, image* img2, float coeff );

/* rogne l'image au rectangle (x,y) de taille (l, h) ET DETRUIT LE POINTEUR img */
//image* rogner( image* img, int x, int y, int l, int h );

/* cache secret dans support */
void cacher( image* support, image* secret);

/* relève une image caché dans support */
image* relever( image* support );
