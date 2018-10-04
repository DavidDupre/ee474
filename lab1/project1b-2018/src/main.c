#include <stdio.h>

//  this is a simple routine that demonstrates how to fill and diaplay an array of characters

// Main was void instead of int, also took parameter void
int main()
{
	int i = 0;									//  declare a working variable

	// The problem was that the array was not long enough.
	// This problem was identified after examining all constants in the file.
	char myArray[6];							//  declare a character array

	for (i = 0; i <= 5; i++)					//  fill array with characters
	{
		//  fill with the ascii characters A..F
		//  65 is the ascii value for A

		myArray[i]= 65+i;
	}

	for (i = 0; i <= 5; i++)					//  display the array
	{
		printf("%c \n", myArray[i]);
	}

	printf("\n");

	// Now set to retun 0
	return 0;
}