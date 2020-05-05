#include <iostream>
#include <bitset>
#include <climits>
#include <limits>
#include <iomanip>


typedef union // used as data input conversion
{
	double input; // assumes sizeof(double) == sizeof(long int)
	long int output;
}dataDouble;

typedef union // used as data output conversion
{
	int input;
	float output;
}dataFloat;


void printFloat(const std::bitset<sizeof(float) * CHAR_BIT> &bitsF)
{
	for(int i = 31; i >= 0 ; i--)
	{
		if(i == 30 || i == 22) 
			std::cout << "|";
		std::cout << bitsF[i];
	}
	std::cout << std::endl;

}

void printDouble(const std::bitset<sizeof(double) * CHAR_BIT> &bitsD)
{
	for(int i = 63; i >= 0 ; i--)
	{
		if(i == 62 || i == 51) 
			std::cout << "|";
		std::cout << bitsD[i];
	}
	std::cout << std::endl;
}

void doubleToFloatSimple(const double &value)
{
	dataFloat dataF;
	dataDouble dataD;	
	dataD.input = value;
	
	std::bitset<sizeof(double) * CHAR_BIT> bitsD(dataD.output);
	std::bitset<sizeof(float) * CHAR_BIT> bitsF(0);
	std::bitset<11> bitsED;

	bitsF[31] = bitsD[63]; // signal transfer
	
	for(int i = 0; i < 23; i++) // mantissa transfer
		bitsF[22-i] = bitsD[51-i];

	for(int i = 0; i < 11; i++)
		bitsED[i] = bitsD[52 + i];

	//expoent conversion
	int exp = (int)bitsED.to_ulong();
	std::cout << exp << std::endl;
	exp = exp - 896;
	std::bitset<8> bitsEF(exp);

	for(int i = 0; i < 8; i++)
		bitsF[23 + i] = bitsEF[i]; 
	
	printFloat(bitsF);

	std::cout << std::endl;

	dataF.input = (int)bitsF.to_ulong();
	std::cout << dataF.output << std::endl;
	
}


void doubleToFloat(const double &value)
{
	dataFloat dataF;
	dataDouble dataD;	
	dataD.input = value;
	
	std::bitset<sizeof(double) * CHAR_BIT> bitsD(dataD.output);
	std::bitset<sizeof(float) * CHAR_BIT> bitsF(0);
	std::bitset<11> bitsED;

	printDouble(bitsD);

	bitsF[31] = bitsD[63]; // signal transference

	for(int i = 0; i < 11; i++)
		bitsED[i] = bitsD[52 + i];

	bitsD[63] = 0; // To simplify zero check

	if(bitsD.none()) // ZERO
	{
		for(int i = 0; i < 30; i++)
			bitsF[i] = 0;
	}
	else if(bitsED.all() || bitsED.none()) // infinity or NaN or denormalized
	{
		int infAndNanOrDenorm = bitsED.all() ? 1 : 0;

		for(int i = 0; i < 8; i++)
			bitsF[23 + i] = infAndNanOrDenorm;

		for(int i = 0; i < 23; i++) 
			bitsF[22-i] = bitsD[51-i];
	}
	else if(bitsED[10] == 1 && (bitsED[7] | bitsED[8] | bitsED[9]) != false) // greater than max expoent value 
	{
		for(int i = 1; i < 8; i++)
			bitsF[23 + i] = 1;	
		bitsF[23] = 0;

		for(int i = 0; i < 23; i++) 
			bitsF[i] = 1; 
	}
	else if(bitsED[10] == 0 && (bitsED[7] & bitsED[8] & bitsED[9]) != true)// less than the minimum expoent value 
	{
		for(int i = 1; i < 8; i++)
			bitsF[23 + i] = 0;	
		bitsF[23] = 1;
		
		for(int i = 0; i < 23; i++) 
			bitsF[i] = 0; 
	}
	else{ // fit
		
		bitsF[30] = bitsED[10];
		for(int i = 0; i < 7; i++)
			bitsF[23 + i] = bitsED[i]; 

		for(int i = 0; i < 23; i++) 
			bitsF[22-i] = bitsD[51-i];
	}

	printFloat(bitsF);

	dataF.input = (int)bitsF.to_ulong();
	std::cout << ((value == dataF.output) ? "True" : "False")  << std::endl;
	std::cout << std::setprecision(20) << value << std::endl << dataF.output << std::endl << std::endl;
}


void floatToDouble(const float &value)
{
	dataFloat dataF;
	dataDouble dataD;
	dataF.output = value;

	std::bitset<sizeof(float) * CHAR_BIT> bitsF(dataF.input);
	std::bitset<sizeof(double) * CHAR_BIT> bitsD;
	std::bitset<8> bitsEF;

	printFloat(bitsF);

	bitsD[63] = bitsF[31];	

	for(int i = 0; i < 8; i ++)
		bitsEF[i] = bitsF[23 + i];
	
	bitsF[31] = 0; // simplify zero check

	if(bitsF.none()) // ZERO
	{
		for(int i = 0; i < 31; i++)
			bitsD[i] = 0;
	}
	else if(bitsEF.all() || bitsEF.none()) // Infinity or NaN or Denormalized
	{
		int infAndNanOrDenorm = bitsEF.all() ? 1 : 0;

		for(int i = 0; i < 11; i++)
			bitsD[52 + i] = infAndNanOrDenorm;

		for(int i = 0; i < 23; i++) 
			bitsD[51-i] = bitsF[22-i];

		// fills in the rest of the fraction
		for(int i = 0; i < 29; i++)
			bitsD[28 - i] = 0;
		
	}
	else{
		int negativeOrPositive = bitsEF[7] == 1 ? 1 : 0;
			
		bitsD[62] = bitsEF[7];
		for(int i = 0; i < 3; i++)
			bitsD[61 - i] = !negativeOrPositive;
		
		for(int i = 0; i < 7; i++)
			bitsD[58 - i] = bitsEF[6 - i];
		
		for(int i = 0; i < 23; i++) 
			bitsD[51-i] = bitsF[22-i];

		for(int i = 0; i < 29; i++)
			bitsD[28 - i] = 0;
	}
	
	//printDouble(bitsD);

	dataD.output = bitsD.to_ulong();
	std::cout << ((value == dataD.input) ? "True" : "False")  << std::endl;
	//std::cout << std::setprecision(20) << value << std::endl << dataD.input << std::endl << std::endl;
}

int main()
{
    // std::cout << std::endl;
    // std::cout << "float size = " << sizeof(float) * CHAR_BIT << std::endl;
    // std::cout << "double size = " << sizeof(double) * CHAR_BIT << std::endl;
    // std::cout << "int size = " << sizeof(int) * CHAR_BIT << std::endl;
    // std::cout << "long int size = " << sizeof(long int) * CHAR_BIT << std::endl;
    // std::cout << std::endl;

    doubleToFloat(0.0);
    doubleToFloat(5.0);
    doubleToFloat(-1.5);
    doubleToFloat(-2.25125);
    doubleToFloat(1.175494350822287508e-38); // min(float)
    doubleToFloat(std::numeric_limits<float>::min());
    doubleToFloat(std::numeric_limits<float>::max());
    doubleToFloat(3.40282346639e-100);
    doubleToFloat(std::numeric_limits<float>::infinity());
    doubleToFloat(std::numeric_limits<double>::infinity());
    doubleToFloat(-std::numeric_limits<float>::infinity());

    // floatToDouble(0.0);
    // floatToDouble(1.0);
    // floatToDouble(5.0);
    // floatToDouble(-1.5);
    // floatToDouble(-2.25125);
    // floatToDouble(1.175494350822287508e-38); // min(float)
    // floatToDouble(2.35098870164e-38); // next value greater than min(float)  
    // floatToDouble(std::numeric_limits<float>::min());
    // floatToDouble(std::numeric_limits<float>::max());
    // floatToDouble(std::numeric_limits<float>::infinity());
    // floatToDouble(-std::numeric_limits<float>::infinity());
}
