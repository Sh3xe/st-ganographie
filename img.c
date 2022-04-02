
#include "img.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct pixel_rgb {
	uint8_t r, g, b;
} pixel_rgb;

// FONCTIONS UTILITAIRES

image* creer_image_complet( int l, int h, int intervalle, mode_image mode ) {

	image* img = (image*)malloc( sizeof( image ) );

	img->intervalle_valeurs = intervalle;
	img->largeur = l;
	img->hauteur = h;
	img->mode = mode;

	int taille = 1;
	switch( img->mode ) {
		case PGM_P2:
			taille = 1;
			break;
		case PGM_P3:
			taille = 3;
			break;
		default:
			return NULL;
	}

	img->pixels = (uint8_t*)malloc( l * h * taille * sizeof(uint8_t) );
	for( int i = 0; i < l*h*taille; i++ )
		img->pixels[i] = 0;


	return img;
}

uint8_t recup_pixel( image* img, int i, int j ) {
	assert( i < img->largeur && j < img->hauteur );
	return img->pixels[ i + j*img->largeur ];
}

void modif_pixel( image* img, int i, int j, uint8_t pixel ) {
	assert( i < img->largeur && j < img->hauteur );
	img->pixels[ i + j*img->largeur ] = pixel;
}

pixel_rgb recup_pixel_rgb( image* img, int i, int j ) {

	assert( i < img->largeur && j < img->hauteur );

	pixel_rgb p = {
		img->pixels[ (i + j*img->largeur) * 3 ],
		img->pixels[ (i + j*img->largeur) * 3 + 1 * sizeof(uint8_t)],
		img->pixels[ (i + j*img->largeur) * 3 + 2 * sizeof(uint8_t)]
	};

	return p;
}

void modif_pixel_rgb( image* img, int i, int j, pixel_rgb pixel ) {
	assert( i < img->largeur && j < img->hauteur );
	img->pixels[ (i + j*img->largeur) * 3 ] = pixel.r;
	img->pixels[ (i + j*img->largeur) * 3 + 1 * sizeof(uint8_t)] = pixel.g;
	img->pixels[ (i + j*img->largeur) * 3 + 2 * sizeof(uint8_t)] = pixel.b;
}

/* renvoie la moyenne des pixels voisins de distance <= l autour de (i, j) */
uint8_t moyenne_pixels_voisins( image* img, int i, int j, int l ) {

	float total = 0.0f;
	int nb_voisins = 0;

	for( int x = i - l; x <= i + l; x++ )
	for( int y = j - l; y <= j + l; y++ ) {
		if( x < 0 || y < 0 || x >= img->largeur || y >= img->hauteur ) continue;

		uint8_t pixel = recup_pixel(img, x, y);
		nb_voisins++;
		total += (float)pixel;
		
	}

	return total / nb_voisins;
}

/* renvoie la moyenne des pixels voisins de distance <= l autour de (i, j) */
pixel_rgb moyenne_pixels_voisins_rgb( image* img, int i, int j, int l ) {

	int total_r = 0, total_g = 0, total_b = 0;
	int nb_voisins = 0;

	for( int x = i - l; x <= i + l; x++ )
	for( int y = j - l; y <= j + l; y++ ) {
		if( x < 0 || y < 0 || x >= img->largeur || y >= img->hauteur ) continue;

		pixel_rgb pixel = recup_pixel_rgb(img, x, y);
		nb_voisins++;

		total_r += pixel.r;
		total_g += pixel.g;
		total_b += pixel.b;
		
	}

	pixel_rgb total = {
		total_r / nb_voisins,
		total_g / nb_voisins,
		total_b / nb_voisins
	};


	return total;
}


// INTERFACE

image* creer_image( int l, int h, mode_image mode ) {
	return creer_image_complet(l, h, 255, mode);
}

void detruire_image( image* img ) {
	free( img->pixels );
	free( img );
}

image* convertir_format( image* img, mode_image nv_mode ) {

	assert( img != NULL && "opération impossible sur un pointeur NULL ");
	
	if( img->mode == nv_mode ) return img;

	image* nv_img = creer_image_complet(
		img->largeur,
		img->hauteur,
		img->intervalle_valeurs,
		nv_mode );

	if( img->mode == PGM_P2 && nv_mode == PGM_P3) {
		for( int i = 0; i < img->largeur; i++ )
		for( int j = 0; j < img->hauteur; j++ ) {
			uint8_t pixel = recup_pixel(img, i, j);
			pixel_rgb p = { pixel, pixel, pixel };
			modif_pixel_rgb( nv_img, i, j, p);
		}
	}
	else if( img->mode == PGM_P3 && nv_mode == PGM_P2 ) {
		for( int i = 0; i < img->largeur; i++ )
		for( int j = 0; j < img->hauteur; j++ ) {
			pixel_rgb pixel = recup_pixel_rgb(img, i, j);
			uint8_t p = (pixel.r + pixel.g + pixel.b) / 3;
			modif_pixel( nv_img, i, j, p);
		}
	}
	else
		assert( false && "Format non supporté" );


	detruire_image(img);
	return nv_img;
}

bool exporter_image( image* img, char* chemin ) {

	assert( img != NULL && "opération impossible sur un pointeur NULL ");

	FILE* fichier = fopen(chemin, "w");
	if( fichier == NULL ) return false;

	switch ( img->mode )
	{
	case PGM_P2:
		fprintf(fichier, "P2\n");
		break;
	case PGM_P3:
		fprintf(fichier, "P3\n");
		break;
	default:
		assert( false && "Format non supporté" );
		break;
	}

    fprintf( fichier, "%d %d\n%d\n", img->largeur, img->hauteur, img->intervalle_valeurs );

	switch( img->mode ) {
	case PGM_P2:
		for( int j  = 0; j < img->hauteur; j++) {
			for( int i  = 0; i < img->largeur; i++) {
				fprintf(fichier, "%d ", recup_pixel(img, i, j));
			}
			fprintf(fichier, "\n");
		}
		break;
	case PGM_P3:
		for( int j  = 0; j < img->hauteur; j++) {
			for( int i  = 0; i < img->largeur; i++) {
				pixel_rgb p = recup_pixel_rgb(img, i, j);
				fprintf(fichier, "%d %d %d ", p.r, p.g, p.b);
			}
			fprintf(fichier, "\n");
		}
		break;
	default:
		assert(false && "Format non supporté");
		break;
	}

	fclose( fichier );
}

image* importer_image( char* chemin ) {

	FILE* fichier = fopen( chemin, "r");
	if( fichier == NULL ) return NULL;

	char mode_str[4];
	int intervalle = 255, largeur = 0, hauteur = 0;
	fscanf( fichier, "%s\n%d %d\n %d", mode_str, &largeur, &hauteur, &intervalle );

	mode_image mode = PGM_P2;
	if( mode_str[0] == 'P' && mode_str[1] == '3' ) mode = PGM_P3;

	image* img = creer_image_complet(largeur, hauteur, intervalle, mode);

	switch( mode ) {
	case PGM_P2:
		for( int j  = 0; j < img->hauteur; j++) {
			for( int i  = 0; i < img->largeur; i++) {
				int pixel;
				fscanf(fichier, "%d", &pixel);
				modif_pixel(img, i, j, (uint8_t)pixel);
			}
		}
		break;
	case PGM_P3:
		for( int j  = 0; j < img->hauteur; j++) {
			for( int i  = 0; i < img->largeur; i++) {
				pixel_rgb pixel;
				fscanf(fichier, "%hhd %hhd %hhd ", &pixel.r, &pixel.g, &pixel.b);
				modif_pixel_rgb(img, i, j, pixel);
			}
		}
		break;
	default:
		assert( false && "format non supporté");
		break;
	}

	fclose( fichier );
	return img;
}

void mirroir( image* img ) {

	assert( img != NULL && "opération impossible sur un pointeur NULL ");

	int millieu = img->largeur / 2;

	switch( img->mode ) {
	case PGM_P2:
		for( int j  = 0; j < img->hauteur; j++) 
		for( int i  = 0; i < millieu; i++) {
			uint8_t p1 = recup_pixel(img, i, j);
			uint8_t p2 = recup_pixel(img, img->largeur - 1 - i, j);
			modif_pixel(img, i, j, p2);
			modif_pixel(img, img->largeur - 1 - i, j, p1);
		}
		break;
	case PGM_P3:
		for( int j  = 0; j < img->hauteur; j++) 
		for( int i  = 0; i < millieu; i++) {
			pixel_rgb p1 = recup_pixel_rgb(img, i, j);
			pixel_rgb p2 = recup_pixel_rgb(img, img->largeur - 1 - i, j);
			modif_pixel_rgb(img, i, j, p2);
			modif_pixel_rgb(img, img->largeur - 1 - i, j, p1);
		}
		break;
	default: 
		assert( false && "format non supporté");
		break;
	}
}

image* redim( image* img, int l, int h ) {

	assert( img != NULL && "opération impossible sur un pointeur NULL ");

	image* nvl_img = creer_image_complet(l, h, img->intervalle_valeurs, img->mode);

	int h0 = img->hauteur * ((float)l / (float)img->largeur);
	int l1 = img->largeur * ((float)h / (float)img->hauteur);
	
	// la dimension du rectangle sur lequel on va envoyer l'image
	int X = l;
	int Y = h;

	if( h0 > h ) {
		X = l1;
		Y = h;
	}
	if( l1 > l ) {
		X = l;
		Y = h0;
	}
	

	switch( img->mode ) {
	case PGM_P2:
		for( int j  = 0; j < Y; j++)
		for( int i  = 0; i < X; i++) {
			int x = i * ((float)img->largeur / X);
			int y = j * ((float)img->hauteur / Y);
			modif_pixel(nvl_img, i, j, recup_pixel(img, x, y));
		}
		break;
	case PGM_P3:
		for( int j  = 0; j < Y; j++)
		for( int i  = 0; i < X; i++) {
			int x = i * ((float)img->largeur / X);
			int y = j * ((float)img->hauteur / Y);
			modif_pixel_rgb(nvl_img, i, j, recup_pixel_rgb(img, x, y));
		}
		break;
	default:
		assert( false && "Format non supporté" );
		break;
	}

	detruire_image(img);
	return nvl_img;
}

image* copier( image* img ) {

	assert( img != NULL && "opération impossible sur un pointeur NULL ");

	image* copie = creer_image_complet(img->largeur, img->hauteur, img->intervalle_valeurs, img->mode);

	switch( img->mode ) {
	case PGM_P2:
		for( int j = 0; j < img->hauteur; j++)
		for( int i  = 0; i < img->largeur; i++)
			modif_pixel(copie, i, j, recup_pixel(img, i, j));
		break;
	case PGM_P3:
		for( int j = 0; j < img->hauteur; j++)
		for( int i  = 0; i < img->largeur; i++)
			modif_pixel_rgb(copie, i, j, recup_pixel_rgb(img, i, j));
		break;
	default:
		assert( false && "Format non supporté ");
		break;
	}

	return copie;
}

void cacher( image* support, image* secret) {

	assert( support != NULL && secret != NULL && support->mode == secret->mode );

	image* secret_redim = copier( secret );
	secret_redim = redim( secret_redim, support->largeur / 2, support->hauteur / 2 );


	switch( support-> mode ) {
	case PGM_P2:
		for( int j = 0; j < secret_redim->hauteur; j++ )
		for( int i = 0; i < secret_redim->largeur; i++ ) {
			uint8_t pixel = recup_pixel(secret_redim, i, j);

			modif_pixel(support, i*2  , j*2  , (recup_pixel(support, i*2  , j*2  ) & 0b11111100) | ((pixel & 0b11000000) >> 6));
			modif_pixel(support, i*2  , j*2+1, (recup_pixel(support, i*2  , j*2+1) & 0b11111100) | ((pixel & 0b00001100) >> 2));
			modif_pixel(support, i*2+1, j*2  , (recup_pixel(support, i*2+1, j*2  ) & 0b11111100) | ((pixel & 0b00110000) >> 4));
			modif_pixel(support, i*2+1, j*2+1, (recup_pixel(support, i*2+1, j*2+1) & 0b11111100) | ((pixel & 0b00000011) >> 0));
		}
		break;
	case PGM_P3:
		for( int j = 0; j < secret_redim->hauteur; j++ )
		for( int i = 0; i < secret_redim->largeur; i++ ) {
			pixel_rgb pixel_secret = recup_pixel_rgb(secret_redim, i, j);

			pixel_rgb pixel_support = recup_pixel_rgb(support, i*2, j*2);
			pixel_rgb p0 = {
				(pixel_support.r & 0b11111100) | ((pixel_secret.r & 0b11000000) >> 6),
				(pixel_support.g & 0b11111100) | ((pixel_secret.g & 0b11000000) >> 6),
				(pixel_support.b & 0b11111100) | ((pixel_secret.b & 0b11000000) >> 6),
			};

			modif_pixel_rgb(support, i*2, j*2, p0);

			pixel_support = recup_pixel_rgb(support, i*2, j*2);
			pixel_rgb p1 = {
				(pixel_support.r & 0b11111100) | ((pixel_secret.r & 0b00001100) >> 2),
				(pixel_support.g & 0b11111100) | ((pixel_secret.g & 0b00001100) >> 2),
				(pixel_support.b & 0b11111100) | ((pixel_secret.b & 0b00001100) >> 2),
			};

			modif_pixel_rgb(support, i*2, j*2+1, p1);

			pixel_support = recup_pixel_rgb(support, i*2, j*2);
			pixel_rgb p2 = {
				(pixel_support.r & 0b11111100) | ((pixel_secret.r & 0b00110000) >> 4),
				(pixel_support.g & 0b11111100) | ((pixel_secret.g & 0b00110000) >> 4),
				(pixel_support.b & 0b11111100) | ((pixel_secret.b & 0b00110000) >> 4),
			};

			modif_pixel_rgb(support, i*2+1, j*2, p2);

			pixel_support = recup_pixel_rgb(support, i*2, j*2);
			pixel_rgb p3 = {
				(pixel_support.r & 0b11111100) | ((pixel_secret.r & 0b00000011) >> 0),
				(pixel_support.g & 0b11111100) | ((pixel_secret.g & 0b00000011) >> 0),
				(pixel_support.b & 0b11111100) | ((pixel_secret.b & 0b00000011) >> 0),
			};

			modif_pixel_rgb(support, i*2+1, j*2+1, p3);
			
		}
		break;

	}

	detruire_image(secret_redim);
}

image* relever( image* support ) {

	assert( support != NULL && "opération impossible sur un pointeur NULL ");

	image* secret = creer_image_complet( support->largeur / 2, support->hauteur / 2, support->intervalle_valeurs, support->mode );

	switch( support->mode ) {
	case PGM_P2:
		for( int i = 0; i < secret->largeur; i++ )
		for( int j = 0; j < secret->hauteur; j++ ) {
			uint8_t pixel = 0;

			pixel = pixel | ((recup_pixel(support, i*2  , j*2  ) & 0b00000011) << 6);
			pixel = pixel | ((recup_pixel(support, i*2  , j*2+1) & 0b00000011) << 2);
			pixel = pixel | ((recup_pixel(support, i*2+1, j*2  ) & 0b00000011) << 4);
			pixel = pixel | ((recup_pixel(support, i*2+1, j*2+1) & 0b00000011) << 0);

			modif_pixel( secret, i, j, pixel );
		}
		break;
	case PGM_P3:
		for( int i = 0; i < secret->largeur; i++ )
		for( int j = 0; j < secret->hauteur; j++ ) {
			pixel_rgb pixel = {0, 0, 0};

			pixel_rgb pixel_support0 = recup_pixel_rgb(support, i*2, j*2);
			pixel_rgb pixel_support1 = recup_pixel_rgb(support, i*2, j*2+1);
			pixel_rgb pixel_support2 = recup_pixel_rgb(support, i*2+1, j*2);
			pixel_rgb pixel_support3 = recup_pixel_rgb(support, i*2+1, j*2+1);

			pixel.r =
				((pixel_support0.r & 0b00000011) << 6) |
				((pixel_support1.r & 0b00000011) << 2) |
				((pixel_support2.r & 0b00000011) << 4) |
				((pixel_support3.r & 0b00000011) << 0);

			pixel.g =
				((pixel_support0.g & 0b00000011) << 6) |
				((pixel_support1.g & 0b00000011) << 2) |
				((pixel_support2.g & 0b00000011) << 4) |
				((pixel_support3.g & 0b00000011) << 0);

			pixel.b =
				((pixel_support0.b & 0b00000011) << 6) |
				((pixel_support1.b & 0b00000011) << 2) |
				((pixel_support2.b & 0b00000011) << 4) |
				((pixel_support3.b & 0b00000011) << 0);

			modif_pixel_rgb( secret, i, j, pixel );
		}
		break;
	default: 
		assert( false && "Format non supporté" );
		break;
	}

	return secret;
}

void flou( image* img, int niveau ) {

	assert( img != NULL && "opération impossible sur un pointeur NULL ");

	switch( img->mode ) {
	case PGM_P2:
		for( int i = 0; i < img->largeur; i++ )
		for( int j = 0; j < img->hauteur; j++ ) {
			uint8_t moyenne = moyenne_pixels_voisins(img, i, j, niveau);
			modif_pixel(img, i, j, moyenne );
		}
		break;
	case PGM_P3:
		for( int i = 0; i < img->largeur; i++ )
		for( int j = 0; j < img->hauteur; j++ ) {
			pixel_rgb moyenne = moyenne_pixels_voisins_rgb(img, i, j, niveau);
			modif_pixel_rgb(img, i, j, moyenne );
		}
		break;
	default: 
		assert( false && "Format non supporté" );
		break;
	}

}

/*
image* superposer( image* img1, image* img2, float coeff ) {

	assert( img1 != NULL && img2 != NULL && "opération impossible sur un pointeur NULL ");
	assert( img1-> mode == img2->mode );



	switch( img1->mode ) {
	case PGM_P2:
		for( int i = 0; i < img1->largeur; i++ )
		for( int j = 0; j < img1->hauteur; j++ ) {
			
		}
		break;
	case PGM_P3:

		break;
	default:
		assert( false && "Format non supporté" );
		break;
	}

	return NULL;
}

image* rogner( image* img, int x, int y, int l, int h ) {

	assert( img != NULL && "opération impossible sur un pointeur NULL ");

	return NULL;
}
*/


