#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	float f = 0.0;
	long double d = 1.55;
	unsigned char *c = reinterpret_cast<unsigned char *>(&f);
	cout << c[R] << endl;

//	printf("\nf = %f\n", f);
//	printf("d = %Lf\n\n", d);
	cout << f << endl;
	cout << d << endl;

	return 0;
}
