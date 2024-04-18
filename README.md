#Ana Stanciulescu 312CA - 28.12.2023

#####Image editor#####

###Task 1 - LOAD
	1. Se elibereaza memoria ocupata de imaginea precedenta;
	2. Se deschide noul fisier si se copiaza informatia din header pentru a stabili formatul imaginii;
	3. In functie de tip (grayscale sau color) se aloca o anumita memorie pentru matricea de pixeli:
		- grayscale: atatea celule in matrice cati pixeli sunt;
		- color: de 3 ori mai multe celule decat pixeli pentru a retine valorile RGB;
	4. In functie de format (ascii sau binar) se folosesc functiile specifice pentru a copia datele din fisier;

###Task 2 - SELECT
	1. Se citesc parametrii de dupa comanda "SELECT" pentru a determina daca comanda este valida;
	2. Daca singurul parametru este "ALL" atunci se apeleaza functia SELECT_ALL;
	3. Daca parametrii respecta cerinta, se interschimba parametrii pentru a fi in ordine crescatoare si se verifica
ca acestia nu depasesc dimensiunea imaginii;
	4. Se copiaza in structura acesti parametri;

###Task 3 - SELECT ALL
	1. Se verifica daca in memorie este salvata deja o imagine;
	2. Se modifica valorile din structura, specifice selectiei, pentru a cuprinde capetele imaginii;

###Task 4 - CROP
	1. Deoarece se modifica memoria alocata imaginii, se creeaza o noua structura, numita crop;
	2. Dimensiunile noii matrici de pixeli se stabilesc in functie de tipul imaginii (grayscale sau color),
ca in functia LOAD;
	3. In noua matrice se copiaza pur si simplu valorile pixelilor din imagine care se incadreaza in selectie;
	4. La final, se interschimba zonele de memorie ale celor doua structuri, astfel incat in structura image sa se afle noile
date si in crop, cele vechi;
	5. Se elibereaza memoria din crop, adica vechea matrice;

###Task 5 - SAVE
	1. Se citesc parametrii comenzii pentru a afla numele noului fisier care trebuie creat si formatul in care se salveaza imaginea
(ascii sau binar);
	2. In functie de acest format, se copiaza in noul fisier headerul, modificand daca e cazul magic number-ul;
	3. Se inchide fisierul in modul text si se redeschide in:
		- modul text: daca fisierul trebuie salvat tot ca ascii;
		- modul binar: daca fisierul trebuie salvat ca binar;
	4. Dupa se apeleaza o functie care copiaza datele din matrice:
		- modul text: text_write() -> in functie de tipul imaginii, copiaza celula cu celula datele din matrice (grayscale)
 sau copiaza cate 3 celule, responsabile cu valorile de RGB (color);
		- modul binar: binary_write() -> urmareste acelasi principiu ca text_write(), singura diferenta fiind
faptul ca functia fwrite() poate copia intreaga linie si nu e nevoie de parcurs matricea celula cu celula;
	5. Se inchide fisierul indiferent de modul in care a fost deschis;

###Task 6 - APPLY
	1. Aici, functia APPLY se ocupa mai mult cu gestionrea erorilor, procesarea parametrilor si apelarea altor functii;
	2. Daca nu se intampina erori, in functie de parametru se apeleaza:
		- EDGE
		- BLUR
		- SHARPEN
		- GAUSSIAN_BLUR
	3. Toate aceste functii au acelasi principiu de functionare, cu mici diferente in functie de kernelul lor:
		- se genereaza kernelul;
		- se aloca memorie pentru o noua matrice in functie de dimensiunile din selectie;
		- deoarece imaginea e mereu color, se considera doar situatia cand fiecare pixel are 3 celule in matrice;
		- daca vreo portiune din selectie coincide cu marginea imaginii, datele pixelului respectiv se copiaza nemodificate;
		- altfel, se apeleaza functia produs_scalar() pentru a calcula produsul scalar dintre kernel si matricea 3x3 din jurul pixelului curent;
		- pe suma obtinuta se aplica functia clamp() pentru a ne asigura ca aceasta nu depaseste intervalul [0, 255];
		- in cazul BLUR si GAUSSIAN_BLUR, produsul scalar se inmulteste cu 1/9 sau 1/16 si dupa se aplica functia round() pentru a obtine un intreg;
		- acest produs scalar se copiaza in noua matrice;
		- dupa ce noua matrice este complet creata, se copiaza inapoi in matricea originala aceste noi valori, 
inlocuindu-le pe cele vechi;
		- se elibereaza memoria noii matrici;

###Task 7 - EQUALIZE
	1. Se aloca memorie pentru o matrice noua, in care se vor memora valorile in urma operatiilor;
	2. Odata cu alocarea de memorie, se parcurge si matricea imaginii pentru a crea un vector de frecventa pentru fiecare valoare de gri;
	3. Parcurgand din nou dimensiunile imaginii, se calculeaza suma frecventelor, inmultind-o cu valoarea maxima (255),
impartind-o cu suprafata imaginii si apeland functiile round() si clamp2();
	4. Rezultatul se copiaza in noua matrice, ca mai apoi se treaca in matricea initiala;
	5. Se elibereaza memoria noii matrici;

###Task 8 - HISTOGRAM
	1. Se citesc parametrii comenzii si se verifica daca exista erori la nivelul valorilor;
	2. Se creeaza un vector de frecventa pentru fiecare valoare de gri;
	3. Se aloca memorie pentru un vector in care se va retine numarul de stele din fiecare bin;
	4. In functie de numarul de bin-uri si valoarea maxima de stele, se parcurge vectorul de frecventa
si se aduna valorile din el in vectorul stars;
	5. Dupa aceea, fiecare celula din vectorul stars se imparte la numarul maxim de aparitii;
	6. Se afiseaza histograma si se elibereaza memoria alocata vectorului stars;

###Task 9 - ROTATE
	1. In prima parte a functiei se citesc parametrii comenzii si se verifica erorile;
	2. In functie de valoarea unghiului ne putem folosi de cercul trigonometric pentru a reduce cazurile posibile de la 9 la 3;
	3. Se creeaza un case pentru:
		- +90: se apeleaza o singura data functia rotate_p90(), care roteste la dreapta matricea;
		- -90: se apeleaza de 3 ori functia rotate_p90();
		- 180: se apeleaza de 2 ori functia rotate_p90();
	4. In functia rotate_p90(), se verifica selectia care trebuie rotita (e nevoie de acest lucru din cauza memoriei):
		- daca e o submatrice a imaginii, se apeleaza functia rotate_part();
		- daca e toata imaginea, se apeleaza functia rotate_all();
	5. Si rotate_part() si rotate_all() functioneaza pe acelasi principiu, insa atunci cand lucram cu toata matricea, ca la crop,
este necesar sa cream o noua structura pentru a putea interschimba suprafetele de memorie;
	6. Se aloca memorie pentru o noua matrice de date, se verifica tipul imaginii (grayscale sau color), se aplica formula
necesara rotirii matricii in functie de tip si se copiaza noile valori inapoi in imagine;
	7. Se elibereaza memoria alocata pentru noua matrice sau vechea structura;

###Task 10 - EXIT
	1. Se apeleaza functia free_image() care elibereaza memoria alocata matricii de pixeli;
	2. In main(), odata intalnita aceasta comanda se da break din loop-ul infinit;

###Functia process_command()
	1. Se ocupa cu impartirea liniei de input din main in command si parameters;
	2. In functie de comanda, se acceseaza functiile specifice sau afiseaza "Invalid command" daca
nu exista;
	3. Aproape toate functiile sunt de tipul int si returneaza anumite numere in functie de erorile intalnite
sau daca functia a fost parcursa cu succes;
	4. De aceea, functia process_command preia aceste output-uri si afiseaza erorile corespunzatoare;

###Functia int main()
	1. Intr-un loop infinit, in main se citeste de la stdin cate o line de comanda si se apeleaza process_command()
pentru a fi analizata linia; 
