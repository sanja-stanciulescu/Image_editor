//STANCIULESCU ANA 312CA - 28.12.2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct image {
	char magicnr[3];
	int linii, coloane, rgb, size;
	unsigned char **data;
	int x1, x2, y1, y2;
} image_t;

//free the image memory
void free_image(image_t *image)
{
	for (int i = 0; i < image->linii; i++)
		free(image->data[i]);
	free(image->data);
	image->data = NULL;
}

//function to load the image
int LOAD(image_t *image, char *filename)
{
	//free the memory, if used
	if (image->data)
		free_image(image);
	//open the file and copy the data
	char line[256];
	FILE *f = fopen(filename, "rb");
	if (!f) {
		fprintf(stdout, "Failed to load %s\n", filename);
		return 0;
	}
	fscanf(f, "%2s\n", image->magicnr);
	fpos_t pos;
	do {
		fgetpos(f, &pos);
		fgets(line, sizeof(line), f);
	} while (line[0] == '#');
	fsetpos(f, &pos);
	fscanf(f, "%d %d %d", &image->coloane, &image->linii, &image->rgb);
	fgetc(f);
	image->x1 = 0; image->x2 = image->coloane;
	image->y1 = 0; image->y2 = image->linii;
	//copy the pixel matrix based on the image type
	if (strcmp(image->magicnr, "P2") == 0 ||
		strcmp(image->magicnr, "P5") == 0) {
		image->size = image->coloane * image->linii;
		image->data = (unsigned char **)malloc(image->linii *
					  sizeof(unsigned char *));
		if (!image->data) {
			fprintf(stderr, "Eroarea alocare memorie\n");
			exit(-1);
		}
		for (int i = 0; i < image->linii; i++) {
			image->data[i] = (unsigned char *)malloc(image->coloane);
			if (!image->data[i]) {
				fprintf(stderr, "Eroare alocare memorie\n");
				for (int j = i - 1; j >= 0; j--)
					free(image->data[j]);
				free(image->data);
				exit(-1);
			}
			if (strcmp(image->magicnr, "P2") == 0)
				for (int j = 0; j < image->coloane; j++)
					fscanf(f, "%hhu", &image->data[i][j]);
			else
				fread(image->data[i], 1, image->coloane, f);
		}
	}

	if (strcmp(image->magicnr, "P3") == 0 ||
		strcmp(image->magicnr, "P6") == 0) {
		image->size = 3 * image->coloane * image->linii;
		image->data = (unsigned char **)malloc(image->linii *
											  sizeof(unsigned char *));
		if (!image->data) {
			fprintf(stderr, "Eroarea alocare memorie\n");
			exit(-1);
		}
		for (int i = 0; i < image->linii; i++) {
			image->data[i] = (unsigned char *)malloc(3 * image->coloane);
			if (!image->data[i]) {
				fprintf(stderr, "Eroare alocare memorie\n");
				for (int j = i - 1; j >= 0; j--)
					free(image->data[j]);
				free(image->data);
				exit(-1);
			}
			if (strcmp(image->magicnr, "P3") == 0)
				for (int j = 0; j < image->coloane; j++)
					fscanf(f, "%hhu %hhu %hhu", &image->data[i][j * 3],
						   &image->data[i][j * 3 + 1],
						   &image->data[i][j * 3 + 2]);
			else
				fread(image->data[i], 1, (image->coloane * 3), f);
		}
	}
	printf("Loaded %s\n", filename);
	fclose(f);
	return 1;
}

//function to select a certain area
int SELECT(image_t *image, char *parameters)
{
	int x1, x2, y1, y2;
	char all1[4], all2[20];
	if (sscanf(parameters, "%s %[^\n]", all1, all2) > 1) {
		if (sscanf(parameters, "%d %d %d %d", &x1, &y1, &x2, &y2) == 4)
			sscanf(parameters, "%d %d %d %d", &x1, &y1, &x2, &y2);
		else
			return -1;
	} else {
		if (strcmp(all1, "ALL") == 0)
			return 3;
	}
	if (!image->data)
		return 0;
	//make sure the parameters are in order
	if (x1 > x2) {
		int aux = x1;
		x1 = x2;
		x2 = aux;
	}
	if (y1 > y2) {
		int aux = y1;
		y1 = y2;
		y2 = aux;
	}
	if (x1 < 0 || x1 > image->coloane)
		return 1;
	if (x2 < 0 || x2 > image->coloane)
		return 1;
	if (y1 < 0 || y1 > image->linii)
		return 1;
	if (y2 < 0 || y2 > image->linii)
		return 1;
	if ((x2 - x1) == 0 || (y2 - y1) == 0)
		return 1;
	//save selection parameters
	image->x1 = x1; image->x2 = x2;
	image->y1 = y1; image->y2 = y2;
	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
	return 2;
}

//function to select the entire image
void SELECT_ALL(image_t *image)
{
	if (!image->data) {
		printf("No image loaded\n");
	} else {
		//revert selection parameters to the initial size
		image->x1 = 0; image->y1 = 0;
		image->x2 = image->coloane;
		image->y2 = image->linii;
		printf("Selected ALL\n");
	}
}

//function to crop the image
int CROP(image_t *image)
{
	//create a new structure with the wanted size
	image_t crop;
	if (!image->data)
		return 0;
	crop.linii = image->y2 - image->y1;
	crop.coloane = image->x2 - image->x1;
	crop.data = (unsigned char **)malloc(image->linii *
										sizeof(unsigned char *));
	if (!crop.data) {
		fprintf(stderr, "Eroarea alocare memorie\n");
		exit(-1);
	}
	for (int i = 0; i < crop.linii; i++) {
		if (strcmp(image->magicnr, "P2") == 0 ||
			strcmp(image->magicnr, "P5") == 0) {
			crop.data[i] = (unsigned char *)malloc(crop.coloane);
			if (!crop.data[i]) {
				fprintf(stderr, "Eroare alocare memorie\n");
				for (int j = i - 1; j >= 0; j--)
					free(crop.data[j]);
				free(crop.data);
				exit(-1);
			}
			for (int j = 0; j < crop.coloane; j++)
				crop.data[i][j] = image->data[image->y1 + i][image->x1 + j];
		}
		if (strcmp(image->magicnr, "P3") == 0 ||
			strcmp(image->magicnr, "P6") == 0) {
			crop.data[i] = (unsigned char *)malloc(3 * image->coloane);
			if (!crop.data[i]) {
				fprintf(stderr, "Eroare alocare memorie\n");
				for (int j = i - 1; j >= 0; j--)
					free(crop.data[j]);
				free(crop.data);
				exit(-1);
			}
			for (int j = 0; j < crop.coloane; j++) {
				crop.data[i][j * 3] =
				image->data[image->y1 + i][(image->x1 + j) * 3];
				crop.data[i][j * 3 + 1] =
				image->data[image->y1 + i][(image->x1 + j) * 3 + 1];
				crop.data[i][j * 3 + 2] =
				image->data[image->y1 + i][(image->x1 + j) * 3 + 2];
			}
		}
	}
	//copy the header into the new structure
	strcpy(crop.magicnr, image->magicnr);
	crop.rgb = image->rgb;
	crop.x1 = 0; crop.y1 = 0;
	crop.x2 = crop.coloane;
	crop.y2 = crop.linii;
	//switch between the new structures
	int size = sizeof(image_t);
	void *temp = malloc(size);
	if (!temp) {
		printf("Eroare alocare memorie\n");
		exit(-1);
	}
	memcpy(temp, image, size);
	memcpy(image, &crop, size);
	memcpy(&crop, temp, size);
	free(temp);
	free_image(&crop);
	printf("Image cropped\n");
	return 1;
}

//copy the pixel matrix when the ascii format is wanted
void text_write(image_t *image, FILE *f)
{
	for (int i = 0; i < image->linii; i++) {
		for (int j = 0; j < image->coloane; j++) {
			if (strcmp(image->magicnr, "P2") == 0 ||
				strcmp(image->magicnr, "P5") == 0)
				fprintf(f, "%hhu ", image->data[i][j]);
			else
				fprintf(f, "%hhu %hhu %hhu ", image->data[i][j * 3],
						image->data[i][j * 3 + 1], image->data[i][j * 3 + 2]);
		}
		fprintf(f, "\n");
	}
}

//copy the pixel matrix when the binary format is needed
void binary_write(image_t *image, FILE *f)
{
	for (int i = 0; i < image->linii; i++) {
		if (strcmp(image->magicnr, "P2") == 0 ||
			strcmp(image->magicnr, "P5") == 0)
			fwrite(image->data[i], 1, image->coloane, f);
		else
			fwrite(image->data[i], 1, (image->coloane * 3), f);
	}
}

//function to save a new file
int SAVE(image_t *image, char *parameters)
{
	char filename[40], ascii[6]; int n;
	if (!image->data)
		return 0;
	n = sscanf(parameters, "%s", filename);
	n--;
	FILE *f = fopen(filename, "w");
		if (!f) {
			fprintf(stderr, "Eroare la deschidere fisier\n");
			exit(-1);
		}
	//copy the header and pixel matrix respecting the format
	if (sscanf(parameters, "%s %[^\n]", filename, ascii) > 1) {
		if (strcmp(image->magicnr, "P2") == 0 ||
			strcmp(image->magicnr, "P3") == 0) {
			fprintf(f, "%s\n%d %d\n%d\n", image->magicnr,
					image->coloane, image->linii, image->rgb);
		} else {
			if (strcmp(image->magicnr, "P5") == 0)
				fprintf(f, "%s\n%d %d\n%d\n", "P2", image->coloane,
						image->linii, image->rgb);
			else
				fprintf(f, "%s\n%d %d\n%d\n", "P3", image->coloane,
						image->linii, image->rgb);
		}
		text_write(image, f);
		fclose(f);
	} else {
		if (strcmp(image->magicnr, "P5") == 0 ||
			strcmp(image->magicnr, "P6") == 0) {
			fprintf(f, "%s\n%d %d\n%d\n", image->magicnr,
					image->coloane, image->linii, image->rgb);
		} else {
			if (strcmp(image->magicnr, "P2") == 0)
				fprintf(f, "%s\n%d %d\n%d\n", "P5", image->coloane,
						image->linii, image->rgb);
			else
				fprintf(f, "%s\n%d %d\n%d\n", "P6", image->coloane,
						image->linii, image->rgb);
		}
		fclose(f);
		FILE *againf = fopen(filename, "ab");
		binary_write(image, againf);
		fclose(againf);
	}
	printf("Saved %s\n", filename);
	return 1;
}

int clamp(int sum)
{
	if (sum < 0)
		return 0;
	if (sum > 255)
		return 255;
	return sum;
}

double clamp2(double sum)
{
	if (sum < 0.0)
		return 0.0;
	if (sum > 255.0)
		return 255.0;
	return sum;
}

int produs_scalar(image_t *image, int ker[3][3], int i, int j)
{
	int sum = 0;
	int y = image->y1 + i - 1, x;
	for (int a = 0; a < 3; a++) {
		x = j - 3;
		for (int b = 0; b < 3; b++) {
			sum = sum + ker[a][b] * image->data[y][x];
			x = x + 3;
		}
		y++;
	}
	return sum;
}

int SHARPEN(image_t *image)
{
	//create new matrix for saving the edited values
	unsigned char **sharp_data;
	int ker[][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}; int sum;
	sharp_data = (unsigned char **)malloc((image->y2 - image->y1) *
										  sizeof(unsigned char *));
	if (!sharp_data) {
		fprintf(stderr, "Eroare alocare memorie\n");
		exit(-1);
	}
	int linii = image->y2 - image->y1;
	int coloane = image->x2 - image->x1;
	for (int i = 0; i < linii; i++) {
		sharp_data[i] = (unsigned char *)malloc(coloane * 3);
		if (!sharp_data[i]) {
			fprintf(stderr, "Eroare alocare memorie\n");
			for (int j = i - 1; j >= 0; j--)
				free(sharp_data[j]);
			free(sharp_data);
			exit(-1);
		}
		for (int j = 0; j < coloane; j++) {
			int size = image->x1 + j;
			//check if the pixel is on the border or not
			if ((image->x1 + j) == 0 || (image->y1 + i) == 0 ||
				(image->x1 + j) == (image->coloane - 1) ||
				(image->y1 + i) == (image->linii - 1)) {
				sharp_data[i][j * 3] =
				image->data[image->y1 + i][(image->x1 + j) * 3];
				sharp_data[i][j * 3 + 1] =
				image->data[image->y1 + i][(image->x1 + j) * 3 + 1];
				sharp_data[i][j * 3 + 2] =
				image->data[image->y1 + i][(image->x1 + j) * 3 + 2];
			} else {
				sum = clamp(produs_scalar(image, ker, i, size * 3));
				sharp_data[i][j * 3] = (unsigned char)sum;
				sum = clamp(produs_scalar(image, ker, i, size * 3 + 1));
				sharp_data[i][j * 3 + 1] = (unsigned char)sum;
				sum = clamp(produs_scalar(image, ker, i, (size * 3 + 2)));
				sharp_data[i][j * 3 + 2] = (unsigned char)sum;
			}
		}
	}
	//copy the changed values back into the original matrix
	for (int i = 0; i < linii; i++)
		for (int j = 0; j < coloane; j++) {
			image->data[image->y1 + i][(image->x1 + j) * 3] =
			sharp_data[i][j * 3];
			image->data[image->y1 + i][(image->x1 + j) * 3 + 1] =
			sharp_data[i][j * 3 + 1];
			image->data[image->y1 + i][(image->x1 + j) * 3 + 2] =
			sharp_data[i][j * 3 + 2];
		}
	//free the memory used for the new matrix
	for (int i = 0; i < linii; i++)
		free(sharp_data[i]);
	free(sharp_data);
	return 3;
}

int EDGE(image_t *image)
{
	//create new matrix for saving the edited values
	unsigned char **edge_data;
	int ker[][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}}, sum;
	int linii = image->y2 - image->y1;
	int coloane = image->x2 - image->x1;
	edge_data = (unsigned char **)malloc((image->y2 - image->y1) *
										 sizeof(unsigned char *));
	if (!edge_data) {
		fprintf(stderr, "Eroare alocare memorie\n");
		exit(-1);
	}
	for (int i = 0; i < linii; i++) {
		edge_data[i] = (unsigned char *)malloc(coloane * 3);
		if (!edge_data[i]) {
			fprintf(stderr, "Eroare alocare memorie\n");
			for (int j = i - 1; j >= 0; j--)
				free(edge_data[j]);
			free(edge_data);
			exit(-1);
		}
		for (int j = 0; j < coloane; j++) {
			//check if the pixel is on the border or not
			if ((image->y1 + i) == 0 || (image->x1 + j) == 0 ||
				(image->y1 + i) == (image->linii - 1) ||
				(image->x1 + j) == (image->coloane - 1)) {
				edge_data[i][j * 3] =
				image->data[image->y1 + i][(image->x1 + j) * 3];
				edge_data[i][j * 3 + 1] =
				image->data[image->y1 + i][(image->x1 + j) * 3 + 1];
				edge_data[i][j * 3 + 2] =
				image->data[image->y1 + i][(image->x1 + j) * 3 + 2];
			} else {
				int size = image->x1 + j;
				sum = clamp(produs_scalar(image, ker, i, size * 3));
				edge_data[i][j * 3] = (unsigned char)sum;
				sum = clamp(produs_scalar(image, ker, i, size * 3 + 1));
				edge_data[i][j * 3 + 1] = (unsigned char)sum;
				sum = clamp(produs_scalar(image, ker, i, size * 3 + 2));
				edge_data[i][j * 3 + 2] = (unsigned char)sum;
			}
		}
	}
	//copy the changed valuea back into the original matrix
	for (int i = 0; i < linii; i++)
		for (int j = 0; j < coloane; j++) {
			int size1 = image->y1 + i, size2 = image->x1 + j;
			image->data[size1][size2 * 3] = edge_data[i][j * 3];
			image->data[size1][size2 * 3 + 1] = edge_data[i][j * 3 + 1];
			image->data[size1][size2 * 3 + 2] = edge_data[i][j * 3 + 2];
		}
	//free the memory
	for (int i = 0; i < linii; i++)
		free(edge_data[i]);
	free(edge_data);
	return 3;
}

int BLUR(image_t *image)
{
	//create new matrix for saving the edited values
	unsigned char **blur_data;
	int ker[][3] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
	double sum;
	int linii = image->y2 - image->y1;
	int coloane = image->x2 - image->x1;
	blur_data = (unsigned char **)malloc((image->y2 - image->y1) *
				 sizeof(unsigned char *));
	if (!blur_data) {
		fprintf(stderr, "Eroare alocare memorie\n");
		exit(-1);
	}
	for (int i = 0; i < linii; i++) {
		blur_data[i] = (unsigned char *)malloc(coloane * 3);
		if (!blur_data[i]) {
			fprintf(stderr, "Eroare alocare memorie\n");
			for (int j = i - 1; j >= 0; j--)
				free(blur_data[j]);
			free(blur_data);
			exit(-1);
		}
		for (int j = 0; j < coloane; j++) {
			int size1 = image->y1 + i, size2 = image->x1 + j;
			//check if the pixel is on the border or not
			if ((image->y1 + i) == 0 || (image->x1 + j) == 0 ||
				(image->y1 + i) == (image->linii - 1) ||
				(image->x1 + j) == (image->coloane - 1)) {
				blur_data[i][j * 3] = image->data[size1][size2 * 3];
				blur_data[i][j * 3 + 1] = image->data[size1][size2 * 3 + 1];
				blur_data[i][j * 3 + 2] = image->data[size1][size2 * 3 + 2];
			} else {
				sum = (double)produs_scalar(image, ker, i, size2 * 3) / 9;
				blur_data[i][j * 3] = (unsigned char)round(clamp2(sum));
				sum = (double)produs_scalar(image, ker, i, size2 * 3 + 1) / 9;
				blur_data[i][j * 3 + 1] = (unsigned char)round(clamp2(sum));
				sum = (double)produs_scalar(image, ker, i, size2 * 3 + 2) / 9;
				blur_data[i][j * 3 + 2] = (unsigned char)round(clamp2(sum));
			}
		}
	}
	//copy the new values back into the original matrix
	for (int i = 0; i < linii; i++)
		for (int j = 0; j < coloane; j++) {
			int size = image->x1 + j;
			image->data[image->y1 + i][size * 3] = blur_data[i][j * 3];
			image->data[image->y1 + i][size * 3 + 1] = blur_data[i][j * 3 + 1];
			image->data[image->y1 + i][size * 3 + 2] = blur_data[i][j * 3 + 2];
		}
	//free the memory
	for (int i = 0; i < linii; i++)
		free(blur_data[i]);
	free(blur_data);
	return 3;
}

int GAUSSIAN(image_t *image)
{
	//create new matrix for saving the edited values
	unsigned char **gaus_data;
	int ker[][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
	double sum;
	int linii = image->y2 - image->y1;
	int coloane = image->x2 - image->x1;
	gaus_data = (unsigned char **)malloc((image->y2 - image->y1) *
					 sizeof(unsigned char *));
	if (!gaus_data) {
		fprintf(stderr, "Eroare alocare memorie\n");
		exit(-1);
	}
	for (int i = 0; i < linii; i++) {
		gaus_data[i] = (unsigned char *)malloc(coloane * 3);
		if (!gaus_data[i]) {
			fprintf(stderr, "Eroare alocare memorie\n");
			for (int j = i - 1; j >= 0; j--)
				free(gaus_data[j]);
			free(gaus_data);
			exit(-1);
		}
		for (int j = 0; j < coloane; j++) {
			//check if the pixel is on the border or not
			if ((image->y1 + i) == 0 || (image->x1 + j) == 0 ||
				(image->y1 + i) == (image->linii - 1) ||
				(image->x1 + j) == (image->coloane - 1)) {
				gaus_data[i][j * 3] =
				image->data[image->y1 + i][(image->x1 + j) * 3];
				gaus_data[i][j * 3 + 1] =
				image->data[image->y1 + i][(image->x1 + j) * 3 + 1];
				gaus_data[i][j * 3 + 2] =
				image->data[image->y1 + i][(image->x1 + j) * 3 + 2];
			} else {
				int size = image->x1 + j;
				sum = (double)produs_scalar(image, ker, i, size * 3) / 16;
				gaus_data[i][j * 3] = (unsigned char)round(clamp2(sum));
				sum = (double)produs_scalar(image, ker, i, size * 3 + 1) / 16;
				gaus_data[i][j * 3 + 1] = (unsigned char)round(clamp2(sum));
				sum = (double)produs_scalar(image, ker, i, size * 3 + 2) / 16;
				gaus_data[i][j * 3 + 2] = (unsigned char)round(clamp2(sum));
			}
		}
	}
	//copy the new values back into the original matrix
	for (int i = 0; i < linii; i++)
		for (int j = 0; j < coloane; j++) {
			int size = image->x1 + j;
			image->data[image->y1 + i][size * 3] = gaus_data[i][j * 3];
			image->data[image->y1 + i][size * 3 + 1] = gaus_data[i][j * 3 + 1];
			image->data[image->y1 + i][size * 3 + 2] = gaus_data[i][j * 3 + 2];
		}
	//free the memory
	for (int i = 0; i < linii; i++)
		free(gaus_data[i]);
	free(gaus_data);
	return 3;
}

//function to spot possible errors and redirect to needed functions
int APPLY(image_t *image, char *parameters)
{
	char param[14], gaussian[5];
	int check = -1;
	if (!image->data)
		return 0;
	if (strcmp(image->magicnr, "P2") == 0 || strcmp(image->magicnr, "P5") == 0)
		return 1;
	if (sscanf(parameters, "%s %s", param, gaussian) > 1)
		return 2;
	if (strcmp(param, "EDGE") == 0)
		check = EDGE(image);
	if (strcmp(param, "SHARPEN") == 0)
		check = SHARPEN(image);
	if (strcmp(param, "BLUR") == 0)
		check = BLUR(image);
	if (strcmp(param, "GAUSSIAN_BLUR") == 0)
		check = GAUSSIAN(image);
	if (check == -1)
		return 2;
	printf("APPLY %s done\n", param);
	return 3;
}

int EQUALIZE(image_t *image)
{
	//check for errors
	if (!image->data)
		return 0;
	if (strcmp(image->magicnr, "P3") == 0 || strcmp(image->magicnr, "P6") == 0)
		return 1;
	unsigned char **equal_data;
	equal_data = (unsigned char **)malloc(image->linii *
										sizeof(unsigned char *));
	if (!equal_data) {
		fprintf(stderr, "Eroare alocare memorie\n");
		exit(-1);
	}
	//create a frequency array and a new matrix
	double sum;
	int frequence[256] = {0};
	image->size = image->linii * image->coloane;
	for (int i = 0; i < image->linii; i++) {
		equal_data[i] = (unsigned char *)malloc(image->coloane);
		if (!equal_data[i]) {
			fprintf(stderr, "Eroare alocare memorie\n");
			for (int j = i - 1; j >= 0; j--)
				free(equal_data[j]);
			free(equal_data);
			exit(-1);
		}
		for (int j = 0; j < image->coloane; j++) {
			int val = (int)image->data[i][j];
			frequence[val]++;
		}
	}
	//calculate the new pixel values
	for (int i = 0; i < image->linii; i++)
		for (int j = 0; j < image->coloane; j++) {
			sum = 0.0;
			for (int k = 0; k <= image->data[i][j]; k++)
				sum = sum + (double)frequence[k];
			equal_data[i][j] = (unsigned char)clamp2(round(255 * sum /
													image->size));
		}
	//copy the new values back into the original matrix
	for (int i = 0; i < image->linii; i++)
		for (int j = 0; j < image->coloane; j++)
			image->data[i][j] = equal_data[i][j];
	//free the memory
	for (int i = 0; i < image->linii; i++)
		free(equal_data[i]);
	free(equal_data);
	printf("Equalize done\n");
	return 3;
}

int HISTOGRAM(image_t *image, char *parameters)
{
	//check for errors
	if (!image->data)
		return 0;
	int x, y, n; char test[20];
	if (sscanf(parameters, "%d %[^\n]", &x, test) == 2) {
		if (sscanf(parameters, "%d %d %[^\n]", &x, &y, test) == 3)
			return -1;
		n = sscanf(parameters, "%d %d", &x, &y);
		n--;
		if (y % 2 == 1)
			return -1;
	} else {
		return -1;
	}
	if (strcmp(image->magicnr, "P3") == 0 || strcmp(image->magicnr, "P6") == 0)
		return 1;
	memset(test, 0, 20);
	//create a frequency array and an array with the number of stars
	int frequence[256] = {0}, val, max = 0;
	for (int i = 0; i < image->linii; i++)
		for (int j = 0; j < image->coloane; j++) {
			val = (int)image->data[i][j];
			frequence[val]++;
		}
	int *stars = calloc(y, sizeof(int)), sum;
	//calculate the number of stars
	for (int i = 0; i < y; i++) {
		sum = 0;
		for (int j = i * (256 / y); j < (i + 1) * (256 / y); j++)
			sum = sum + frequence[j];
		stars[i] = sum;
		if (sum > max)
			max = sum;
	}
	//return the histogram into stdout
	for (int i = 0; i < y; i++)
		stars[i] = (stars[i] * x) / max;
	for (int i = 0; i < y; i++) {
		printf("%d\t|\t", stars[i]);
		for (int j = 0; j < stars[i]; j++)
			printf("*");
		printf("\n");
	}
	//free memory
	free(stars);
	return 3;
}

//function to rotate the entire image
void rotate_all(image_t *image, int a)
{
	int linii = image->y2 - image->y1, coloane = image->x2 - image->x1;
	//create a new structure for the rotated image + copy the header
	image_t rotate;
	strcpy(rotate.magicnr, image->magicnr);
	rotate.linii = image->coloane; rotate.coloane = image->linii;
	rotate.rgb = image->rgb;
	rotate.x1 = 0; rotate.y1 = 0;
	rotate.x2 = rotate.coloane;
	rotate.y2 = rotate.linii;
	rotate.data = (unsigned char **)malloc(coloane *
										  sizeof(unsigned char *));
	if (!rotate.data) {
		printf("Eroare alocare memorie\n");
		exit(-1);
	}
	for (int i = 0; i < coloane; i++) {
		int size = linii * a;
		rotate.data[i] = (unsigned char *)malloc(size);
		if (!rotate.data[i]) {
			fprintf(stderr, "Eroare alocare memorie\n");
			for (int j = i - 1; j >= 0; j--)
				free(rotate.data[j]);
			free(rotate.data);
			exit(-1);
		}
	}
	//create the new matrix
	for (int i = 0; i < linii; i++)
		for (int j = 0; j < coloane; j++) {
			if (a == 1) {
				rotate.data[j][linii - 1 - i] =
				image->data[image->y1 + i][image->x1 + j];
			} else {
				rotate.data[j][3 * (linii - 1 - i)] =
				image->data[image->y1 + i][3 * (image->x1 + j)];
				rotate.data[j][3 * (linii - 1 - i) + 1] =
				image->data[image->y1 + i][3 * (image->x1 + j) + 1];
				rotate.data[j][3 * (linii - 1 - i) + 2] =
				image->data[image->y1 + i][3 * (image->x1 + j) + 2];
			}
	}
	//switch between the two structures
	int size = sizeof(image_t);
	void *temp = malloc(size);
	if (!temp) {
		printf("Eroare alocare memorie\n");
		exit(-1);
	}
	memcpy(temp, image, size);
	memcpy(image, &rotate, size);
	memcpy(&rotate, temp, size);
	free(temp);
	free_image(&rotate);
}

//function to rotate only the selection
void rotate_part(image_t *image, int a)
{
	int linii = image->y2 - image->y1, coloane = image->x2 - image->x1;
	//create new matrix for the rotated subimage
	unsigned char **rotate_data;
	rotate_data = (unsigned char **)malloc(coloane *
										  sizeof(unsigned char *));
	if (!rotate_data) {
		printf("Eroare alocare memorie\n");
		exit(-1);
	}
	for (int i = 0; i < coloane; i++) {
		int size = linii * a;
		rotate_data[i] = (unsigned char *)malloc(size);
		if (!rotate_data[i]) {
			fprintf(stderr, "Eroare alocare memorie\n");
			for (int j = i - 1; j >= 0; j--)
				free(rotate_data[j]);
			free(rotate_data);
			exit(-1);
		}
	}
	for (int i = 0; i < linii; i++)
		for (int j = 0; j < coloane; j++) {
			if (a == 1) {
				rotate_data[j][linii - 1 - i] =
				image->data[image->y1 + i][image->x1 + j];
			} else {
				rotate_data[j][3 * (linii - 1 - i)] =
				image->data[image->y1 + i][3 * (image->x1 + j)];
				rotate_data[j][3 * (linii - 1 - i) + 1] =
				image->data[image->y1 + i][3 * (image->x1 + j) + 1];
				rotate_data[j][3 * (linii - 1 - i) + 2] =
				image->data[image->y1 + i][3 * (image->x1 + j) + 2];
			}
		}
	//copy the rotated subimage back into the original matrix
	for (int i = 0; i < linii; i++)
		for (int j = 0; j < coloane; j++) {
			int size = image->x1 + j;
			//check if it's grayscale or color
			if (a == 1) {
				image->data[image->y1 + i][size] = rotate_data[i][j];
			} else {
				image->data[image->y1 + i][size * 3] =
				rotate_data[i][j * 3];
				image->data[image->y1 + i][size * 3 + 1] =
				rotate_data[i][j * 3 + 1];
				image->data[image->y1 + i][size * 3 + 2] =
				rotate_data[i][j * 3 + 2];
			}
		}
	//free the memory
	for (int i = 0; i < linii; i++)
		free(rotate_data[i]);
	free(rotate_data);
}

//function to rotate 90 degrees to the right
void rotate_p90(image_t *image, int a)
{
	image->size = image->linii * image->coloane;
	if ((image->x2 - image->x1) * (image->y2 - image->y1) != image->size)
		rotate_part(image, a);
	else
		rotate_all(image, a);
}

//function that checks for errors and redirects to the other functions
int ROTATE(image_t *image, char *parameters)
{
	if (!image->data)
		return 0;
	if (image->x1 != 0 && image->y1 != 0 &&
		image->x2 != image->coloane && image->y2 != image->linii)
		if ((image->x2 - image->x1) != (image->y2 - image->y1))
			return 1;
	int x, n; char test[30] = {0};
	if (sscanf(parameters, "%d %[^\n]", &x, test) != 1) {
		memset(test, 0, 130);
		return -1;
	}
	memset(test, 0, 30);
	n = sscanf(parameters, "%d", &x);
	n--;
	if (x != 0 && x != 90 && x != -90 && x != 180 && x != -180 &&
		x != 270 && x != -270 && x != 360 && x != -360)
		return 2;
	int y;
	//convert the angles to the 4 possible cases
	if (x == 270 || x == -90) {
		y = -90;
	} else {
		if (x == -270 || x == 90)
			y = 90;
		else
			if (x == -180 || x == 180)
				y = 180;
			else
				if (x == 0 || x == 360 || x == -360)
					y = 360;
	}
	int a;
	if (strcmp(image->magicnr, "P2") == 0 || strcmp(image->magicnr, "P5") == 0)
		a = 1;
	else
		a = 3;
	//rotate depending on angle
	switch (y) {
	case 90:
		rotate_p90(image, a);
		break;
	case -90:
		rotate_p90(image, a);
		rotate_p90(image, a);
		rotate_p90(image, a);
		break;
	case 180:
		rotate_p90(image, a);
		rotate_p90(image, a);
		break;
	default:
		break;
	}
	printf("Rotated %d\n", x);
	return 3;
}

//function to process the input line and redirect to needed functions
void process_command(char *line, image_t *image)
{
	char command[11], parameters[80];
	int check = -1;
	if (sscanf(line, "%s %[^\n]", command, parameters) >= 1) {
		if (strcmp(command, "LOAD") == 0)
			check = LOAD(image, parameters);
		if (strcmp(command, "SELECT") == 0) {
			check = SELECT(image, parameters);
			if (check == 0)
				printf("No image loaded\n");
			if (check == 1)
				printf("Invalid set of coordinates\n");
			if (check == 3)
				SELECT_ALL(image);
		}
		if (strcmp(command, "CROP") == 0) {
			check = CROP(image);
			if (check == 0)
				printf("No image loaded\n");
		}
		if (strcmp(command, "SAVE") == 0) {
			check = SAVE(image, parameters);
			if (check == 0)
				printf("No image loaded\n");
		}
		if (strcmp(command, "APPLY") == 0) {
			if (!image->data) {
				printf("No image loaded\n");
				check = 0;
			}
			if (sscanf(line, "%s %[^\n]", command, parameters) == 2) {
				check = APPLY(image, parameters);
				if (check == 1)
					printf("Easy, Charlie Chaplin\n");
				if (check == 2)
					printf("APPLY parameter invalid\n");
			}
		}
		if (strcmp(command, "EQUALIZE") == 0) {
			check = EQUALIZE(image);
			if (check == 0)
				printf("No image loaded\n");
			if (check == 1)
				printf("Black and white image needed\n");
		}
		if (strcmp(command, "HISTOGRAM") == 0) {
			check = HISTOGRAM(image, parameters);
			if (check == 0)
				printf("No image loaded\n");
			if (check == 1)
				printf("Black and white image needed\n");
		}
		if (strcmp(command, "ROTATE") == 0) {
			check = ROTATE(image, parameters);
			if (check == 0)
				printf("No image loaded\n");
			if (check == 1)
				printf("The selection must be square\n");
			if (check == 2)
				printf("Unsupported rotation angle\n");
		}
		if (strcmp(command, "EXIT") == 0) {
			if (!image->data) {
				printf("No image loaded\n");
				check = 0;
			} else {
				free_image(image);
				check = 0;
			}
		}
		if (check == -1)
			printf("Invalid command\n");
	} else {
		fprintf(stderr, "Invalid input format\n");
	}
	memset(parameters, 0, 80);
}

int main(void)
{
	image_t image; char command[11], line[256];
	image.data = NULL;
	int n;
	while (1) {
		fgets(line, sizeof(line), stdin);
		n = sscanf(line, "%s", command);
		n--;
		process_command(line, &image);
		if (strcmp(command, "EXIT") == 0)
			break;
	}
	return 0;
}
