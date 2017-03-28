#include <math.h>
#include <fcntl.h>
#include <time.h>
#include <C_General.hpp>
#include <C_Trace.hpp>
#include <C_File.hpp>
#include <C_Arguments.hpp>
#include <C_Matrix.hpp>
#include <C_Image.hpp>
#include <iostream>
#include <string>

void Pause();

int pretratamiento(C_Image image);
int cantidad(C_Image image, C_Image image2);
int dia(C_Image image, C_Image image2, C_Image imageFinal, int tope, int contraccion, int expansion);
void noche(C_Image image, C_Image image2, C_Image imageFinal, int tope, int minimo, double pre1, double pre2, int contraccion, int expansion);
C_Image busquedaMejor(C_Image image, C_Image image2, int minimo, double i);
C_Image erosionn(C_Image imageFinal, int erosionTotal);
C_Image dilatacionn(C_Image erosion, int erosionTotal);
int fondo(C_Image image, C_Image image2, C_Image image3, C_Image image4, C_Image image5, C_Image image6, C_Image image7, C_Image imageFinal);

using namespace std;

int segmentacion(int argc, char **argv) {
	//comenzamos leyendo la foto sin movimiento
	//ahora declaramos las filas y columnas, luego definimos las imagenes
	//C_Image::IndexT primero, segundo, tercero, cuarto;
	string original;
	string movimiento;
	int expansion = 2;
	int contraccion = 2;
	int opcion = 0;
	cout << "Este programa oferta diversas funciones.\nSi desea obtener el movimiento que hay entre dos imagenes pulse 1.\nSi desea obtener el fondo de una imagen usando un total de 7 imagenes pulse 2.\n";
	cin >> opcion;
	if (opcion == 1) {
		C_Image image, image2, imageFinal;
		cout << "Has seleccionado la opcion numero 1.\n";
		cout << "NOTA: Recuerde que la imagen tiene que estar en la misma carpeta donde ejecuta el archivo ejecutable .exe.\n Si no es asi debes introducir la ruta entera\n";
		cout << "Introduce la imagen que quiere usar como referencia (con extension BMP)" << endl;
		cin >> original;
		cout << "Introduce la imagen en la que hay movimiento con respecto la primera (con extension BMP)" << endl;
		cin >> movimiento;
		cout << "¿Quiere usar la configuracion TIPICA, o una configuracion MANUAL?\n" << "La configuracion tipica incluye:\n" << "Erosion = 2\n" << "Dilatacion = 2\n";
		cout << "Introduzca el numero '1' si quiere la configuracion tipica, o el '2' si quiere la avanzada:\n";
		cin >> opcion;
		cout << "Cargando...\n";
		if (opcion == 2) {
			cout << "Introduzca la erosion que quiere:\n";
			cin >> contraccion;
			cout << "Introduzca la dilatacion que quiere:\n";
			cin >> expansion;
			cout << "Cargando...\n";
		}

		const char * cadenaOriginal = original.c_str();
		const char * cadenaMovimiento = movimiento.c_str();

		image.ReadBMP(cadenaOriginal);
		image2.ReadBMP(cadenaMovimiento);
		imageFinal.ReadBMP(cadenaOriginal);

		//MI CODIGO DE VERDAD
		//nos dice si la diferencia de pixeles entre la primera imagen y la segunda es muy brusco o no
		//double pre3 = pretratamiento(imageFinal);
		double pre1 = pretratamiento(image);
		double pre2 = pretratamiento(image2);
		int tope = cantidad(image, image2);
		int minimo = pre1 - pre2;

		if (minimo > -10 && minimo < 10) {
			dia(image, image2, imageFinal, tope, contraccion, expansion);
		}
		else {
			noche(image, image2, imageFinal, tope, minimo, pre1, pre2, contraccion, expansion);
		}
	}
	else if (opcion == 2) {
		C_Image image, image2, image3, image4, image5, image6, image7, imageFinal;
		string uno, dos, tres, cuatro, cinco, seis, siete, final;

		cout << "Ha seleccionado la opcion numero 2.\n";
		cout << "NOTA: Se le va a ir pidiendo 7 imagenes, tenga en cuenta que si las imagenes estan movidas es posible que no salga\n correctamente, ademas si sale el mismo objeto o varios parecidos en la misma posicion el programa fallara\n";
		cout << "Por favor introduzca el nombre de la primera imagen (con extension BMP)" << endl;
		cin >> uno;
		cout << "Por favor introduzca el nombre de la segunda imagen (con extension BMP)" << endl;
		cin >> dos;
		cout << "Por favor introduzca el nombre de la tercera imagen (con extension BMP)" << endl;
		cin >> tres;
		cout << "Por favor introduzca el nombre de la cuarta imagen (con extension BMP)" << endl;
		cin >> cuatro;
		cout << "Por favor introduzca el nombre de la quinta imagen (con extension BMP)" << endl;
		cin >> cinco;
		cout << "Por favor introduzca el nombre de la sexta imagen (con extension BMP)" << endl;
		cin >> seis;
		cout << "Por favor introduzca el nombre de la septima imagen (con extension BMP)" << endl;
		cin >> siete;
		cout << "Cargando...";

		//pasamos a char
		const char * cadenaUno = uno.c_str();
		const char * cadenaDos = dos.c_str();
		const char * cadenaTres = tres.c_str();
		const char * cadenaCuatro = cuatro.c_str();
		const char * cadenaCinco = cinco.c_str();
		const char * cadenaSeis = seis.c_str();
		const char * cadenaSiete = siete.c_str();

		//SACAR EL FONDO
		image.ReadBMP(cadenaUno);
		image2.ReadBMP(cadenaDos);
		image3.ReadBMP(cadenaTres);
		image4.ReadBMP(cadenaCuatro);
		image5.ReadBMP(cadenaCinco);
		image6.ReadBMP(cadenaSeis);
		image7.ReadBMP(cadenaSiete);
		imageFinal.ReadBMP(cadenaUno);

		fondo(image, image2, image3, image4, image5, image6, image7, imageFinal);
	}
	else {
		cout << "ERROR. A seleccionado una opcion inexistente. Se cerrara el programa.";
		Pause();
	}
	return 0;
}

int pretratamiento(C_Image imagen) {
	C_Image::IndexT row, col;
	//Me calculo la media de tonos grises que hay en la imagen
	//para la primera imagen
	int calculo = 0;
	int dividendo = 0;
	int divisor = 0;

	for (row = imagen.FirstRow(); row <= imagen.LastRow(); row++) {
		for (col = imagen.FirstCol(); col <= imagen.LastCol(); col++) {
			dividendo = dividendo + imagen(row, col);
			divisor++;
		}
	}

	calculo = dividendo / divisor;

	return calculo;
}

int cantidad(C_Image image, C_Image image2) {
	//Para saber la cantidad de pixeles dque hay entre diferentes 
	C_Image::IndexT row, col;

	int primero = 0;
	int segundo = 0;
	int tercero = 0;
	int cuarto = 0;
	int quinto = 0;
	int sexto = 0;
	int septimo = 0;
	int octavo = 0;
	int noveno = 0;
	int decimo = 0;
	int mayor = 0;

	for (row = image.FirstRow(); row <= image.LastRow(); row++) {
		for (col = image.FirstCol(); col <= image.LastCol(); col++) {
			if (image(row, col) - image2(row, col) == 0 || image2(row, col) - image(row, col) == 0) {
				//no se hace nada
			}
			else if (image(row, col) - image2(row, col) > 0 && image(row, col) - image2(row, col) <= 10) {
				primero++;
			}
			else if (image(row, col) - image2(row, col) > 10 && image(row, col) - image2(row, col) <= 20) {
				segundo++;
			}
			else if (image(row, col) - image2(row, col) > 20 && image(row, col) - image2(row, col) <= 30) {
				tercero++;
			}
			else if (image(row, col) - image2(row, col) > 30 && image(row, col) - image2(row, col) <= 40) {
				cuarto++;
			}
			else if (image(row, col) - image2(row, col) > 40 && image(row, col) - image2(row, col) <= 50) {
				quinto++;
			}
			else if (image(row, col) - image2(row, col) > 50 && image(row, col) - image2(row, col) <= 60) {
				sexto++;
			}
			else if (image(row, col) - image2(row, col) > 60 && image(row, col) - image2(row, col) <= 70) {
				septimo++;
			}
			else if (image(row, col) - image2(row, col) > 70 && image(row, col) - image2(row, col) <= 80) {
				octavo++;
			}
			else if (image(row, col) - image2(row, col) > 80 && image(row, col) - image2(row, col) <= 90) {
				noveno++;
			}
			else if (image(row, col) - image2(row, col) > 90 && image(row, col) - image2(row, col) <= 100) {
				decimo++;
			}
			else {
				mayor++;
			}
		}
	}
	int Vector[11];
	Vector[0] = primero;
	Vector[1] = segundo;
	Vector[2] = tercero;
	Vector[3] = cuarto;
	Vector[4] = quinto;
	Vector[5] = sexto;
	Vector[6] = septimo;
	Vector[7] = octavo;
	Vector[8] = noveno;
	Vector[9] = decimo;
	Vector[10] = mayor;

	//nos muestra la cantidad de cada 1, entre 0 y 10, 11 y 20 etc...
/*
	int j = 0;
	int k = 10;
	for (size_t i = 0; i < 11; i++)
	{
		if (i == 10) {
			cout << "Mas de " << j << ": " << Vector[i] << "\n";
		}
		else {
			cout << "Entre " << j << " y " << k << ": " << Vector[i] << "\n";
		}
		j += 10;
		k += 10;
	}
*/

	int calculo = 0;
	int t = 10;
	//sumamos todos
	for (int i = 1; i < 10; i++)
	{
		calculo += Vector[i];
	}
	//entre 8 ya que son desde 1 a 9 (8 valores)
	calculo = calculo / 8;
	//cout << "Calculo vale: " << calculo << "\n";

	for (int i = 1; i < 10; i++)
	{
		t += 10;
		if (Vector[i] < calculo) {
			break;
		}
	}
	return t;
}

int dia(C_Image image, C_Image image2, C_Image imageFinal, int tope, int contraccion, int expansion) {
	C_Image::IndexT row, col;
	C_Image erosion, dilatacion;

	//nuestro algoritmo PARA CUANDO TIENE LA MISMA LUMINOSIDAD (pero movimiento)
	//para cuando es de noche habria que restarle los pixeles de diferencia.
	for (row = image.FirstRow(); row <= image.LastRow(); row++) {
		for (col = image.FirstCol(); col <= image.LastCol(); col++) {
			if (image(row, col) - image2(row, col) >= tope || image2(row, col) - image(row, col) >= tope) {
				//se consedera que es movimiento
				imageFinal(row, col) = 255;
			}
			else {
				//se considera que es igual
				imageFinal(row, col) = 0;
			}
		}
	}
	cout << "73%...";
	imageFinal.WriteBMP("resultadoSinProcesar.bmp");


	for (int i = 0; i < contraccion; i++)
	{
		erosion = erosionn(imageFinal, 0);
		imageFinal = erosion;
	}
	erosion.WriteBMP("resultadoErosion.bmp");
	cout << "80%...";
	for (int i = 0; i < expansion; i++)
	{
		dilatacion = dilatacionn(erosion, 255);
		erosion = dilatacion;
	}
	cout << "100%!";
	dilatacion.WriteBMP("resultadoProcesado.bmp");
	cout << "Finalizado!\n";
	Pause();
	return 0;
}

void noche(C_Image image, C_Image image2, C_Image imageFinal, int tope, int minimo, double pre1, double pre2, int contraccion, int expansion) {
	double division = 0;
	if (pre1 < pre2) {

		division = pre1 / pre2;
		//cout << "pasa el if" << division;
	}
	else {

		division = pre2 / pre1;
		//cout << "pre2/pre1\n" << division;
	}
	//Pause();
	image2 = busquedaMejor(image, image2, minimo, division);
	//cout << "pasa busquedaMejor\n";
	//Pause();
	image2.WriteBMP("resultadoAclarado.BMP");
	cout << "25%...";
	dia(image, image2, imageFinal, tope, contraccion, expansion);
	//strcat;
	//itoa;
	//String s;
}

C_Image busquedaMejor(C_Image image, C_Image image2, int minimo, double i) {
	C_Image::IndexT row, col;
	//cout << "Va a empezar busquedaMejor";
	//Pause();
	for (row = image.FirstRow(); row <= image.LastRow(); row++) {
		for (col = image.FirstCol(); col <= image.LastCol(); col++) {
			if (minimo < 0) {
				//cout << "se multiplica * i\n" << i;
				image2(row, col) = image2(row, col) * i;
				if (image2(row, col) < 0) {
					image2(row, col) = 0;
				}

			}
			else {
				image2(row, col) = image2(row, col) / i;
				if (image2(row, col) > 255) {
					image2(row, col) = 255;
				}
			}
		}
	}
	//image2.WriteBMP("busquedaMejor.BMP");
	//cout << "termina busquedaMejor\n";
	//Pause();
	return image2;
}

C_Image erosionn(C_Image imageFinal, int erosionTotal) {
	//dilatamos los pixeles negros
	C_Image::IndexT row, col;
	C_Image erosion;
	erosion = imageFinal;
	if (erosionTotal > 0) {
		cout << "10%...";
		for (int i = 0; i < 256; i++) {
			for (row = imageFinal.FirstRow(); row <= imageFinal.LastRow(); row++) {
				for (col = imageFinal.FirstCol(); col <= imageFinal.LastCol(); col++) {
					if (imageFinal(row, col) == erosionTotal) {
						//pixel de arriba
						if (row > imageFinal.FirstRow()) {
							erosion(row - 1, col) = erosionTotal;
						}
						//pixel de abajo
						if (row < imageFinal.LastRow()) {
							erosion(row + 1, col) = erosionTotal;
						}
						//pixel de la izquierda
						if (col > imageFinal.FirstCol()) {
							erosion(row, col - 1) = erosionTotal;
						}
						//pixel de derecha
						if (col < imageFinal.LastCol()) {
							erosion(row, col + 1) = erosionTotal;
						}
					}
				}
				erosionTotal++;

				if (erosionTotal == 128) {
					cout << "50%...";
				}
			}
		}
	}
	else {
		erosionTotal = 0;
		for (row = imageFinal.FirstRow(); row <= imageFinal.LastRow(); row++) {
			for (col = imageFinal.FirstCol(); col <= imageFinal.LastCol(); col++) {
				if (imageFinal(row, col) == erosionTotal) {
					//pixel de arriba
					if (row > imageFinal.FirstRow()) {
						erosion(row - 1, col) = erosionTotal;
					}
					//pixel de abajo
					if (row < imageFinal.LastRow()) {
						erosion(row + 1, col) = erosionTotal;
					}
					//pixel de la izquierda
					if (col > imageFinal.FirstCol()) {
						erosion(row, col - 1) = erosionTotal;
					}
					//pixel de derecha
					if (col < imageFinal.LastCol()) {
						erosion(row, col + 1) = erosionTotal;
					}
				}
			}
		}
		cout << "75%\n";
		return erosion;
	}
	return erosion;
}

C_Image dilatacionn(C_Image erosion, int erosionTotal) {
	C_Image::IndexT row, col;
	C_Image dilatacion;
	dilatacion = erosion;

	//hacemos dilatacion
	//dilatamos los pixeles blancos
	for (row = erosion.FirstRow(); row <= erosion.LastRow(); row++) {
		for (col = erosion.FirstCol(); col <= erosion.LastCol(); col++) {
			if (erosion(row, col) == 255) {
				//pixel de arriba
				if (row > erosion.FirstRow()) {
					dilatacion(row - 1, col) = 255;
				}
				//pixel de abajo
				if (row < erosion.LastRow()) {
					dilatacion(row + 1, col) = 255;
				}

				//pixel de la izquierda
				if (col > erosion.FirstCol()) {
					dilatacion(row, col - 1) = 255;
				}
				//pixel de derecha
				if (col < erosion.LastCol()) {
					dilatacion(row, col + 1) = 255;
				}
			}
		}
	}
	return dilatacion;
}

int fondo(C_Image image, C_Image image2, C_Image image3, C_Image image4, C_Image image5, C_Image image6, C_Image image7, C_Image imageFinal) {
	C_Image::IndexT row, col;
	//en Array[] guardamos la escala de gris
	int Array[7];
	//en Aux[] guardamos el calculo para saber cual es el mas proximo
	int Aux[7];
	for (row = image.FirstRow(); row <= image.LastRow(); row++) {
		for (col = image.FirstCol(); col <= image.LastCol(); col++) {
			int calculo = 0;
			int proximo = 256;
			int pos = 0;

			Array[0] = image(row, col);
			Array[1] = image2(row, col);
			Array[2] = image3(row, col);
			Array[3] = image4(row, col);
			Array[4] = image5(row, col);
			Array[5] = image6(row, col);
			Array[6] = image7(row, col);

			for (int i = 0; i < 7; i++)
			{
				calculo = calculo + Array[i];
			}
			calculo = calculo / 7;

			for (int i = 0; i < 7; i++) {
				Aux[i] = Array[i] - calculo;
				if (Aux[i] < 0) {
					Aux[i] = -Aux[i];
				}
				//cout << "Calculo = " << calculo <<" Aux[i] = "<< Aux[i] << " Array[i] = " << Array[i] << "\n";
			}

			for (int i = 0; i < 7; i++) {
				if (proximo > Aux[i]) {
					proximo = Aux[i];
					pos = i;
				}
				//cout << "Proximo = " << proximo << " cuando Aux[i] vale: " << Aux[i] << "\n";
			}
			//Pause();
			imageFinal(row, col) = Array[pos];
		}
	}
	imageFinal = erosionn(imageFinal, 1);

	imageFinal.WriteBMP("resultadoFondo.bmp");
	cout << "Finalizado!\n";
	Pause();
	return 0;
}
