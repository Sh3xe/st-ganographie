#include "img.h"

void test_p2() {

	image* img0 = importer_image("./images/vangogh.ppm");
	image* img1 = importer_image("./images/mars.ppm");

	mirroir(img0);
	img0 = redim(img0, 400, 300);

	cacher(img1, img0);
	image* img2 = relever(img1);

	exporter_image(img1, "./export0.ppm");
	exporter_image(img2, "./export1.ppm");

	detruire_image(img0);
	detruire_image(img1);
	detruire_image(img2);

}

void test_p3() {

	image* img0 = importer_image("./images/vangogh.pgm");
	image* img1 = importer_image("./images/mars.pgm");

	mirroir(img0);
	img0 = redim(img0, 400, 300);

	cacher(img1, img0);
	image* img2 = relever(img1);

	exporter_image(img1, "./export0.pgm");
	exporter_image(img2, "./export1.pgm");

	detruire_image(img0);
	detruire_image(img1);
	detruire_image(img2);

}

int main() {

	return 0;
}