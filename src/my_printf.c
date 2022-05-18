#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

void my_putchar(int c) {
	write(1, &c, 1);
}

void my_putString(char* s) {
	while (*s) {
		write(1, s++, 1);
	}
}

int getNumberSize(long int number) {
	unsigned int size = 0;

	if (number == 0) {
		size = 1;
		return size;
	}
	if (number < 0) {
		size += 1;
	}
	while (number > 0) {
		number /= 10;
		size++;
	}
	return size;
}

int printPointerAddress(long int memAddress) {
	int count = 2;
	int index = 2;
	int j, temp;
	int base = 16;
	int pointerBuffer[14];

	while (memAddress != 0) {
		temp = memAddress % base;
		if (temp < 10) {
			pointerBuffer[index] = temp + '0';
			index++;
		} else {
			pointerBuffer[index] = temp + 87;
			index++;
		}
		memAddress /= 16;
	}
	my_putchar('0');
	my_putchar('x');
	for (j = index - 1; j >= 2; j--) {
		my_putchar(pointerBuffer[j]);
		count++;
	}
	return count;
}

void myPutnbrLongInt(long int n)
{
	unsigned int numb;

	if (n < 0) {
		numb = -n;
		my_putchar('-');
	} else
		numb = n;
	if (numb < 10) {
		my_putchar(numb + '0');
	} else {
		myPutnbrLongInt(numb / 10);
		my_putchar(numb % 10 + '0');
	}
}

void my_putnbr(signed int n)
{
	unsigned int numb;

	if (n < 0) {
		numb = -n;
		my_putchar('-');
	} else
		numb = n;
	if (numb < 10) {
		my_putchar(numb + '0');
	} else {
		my_putnbr(numb / 10);
		my_putchar(numb % 10 + '0');
	}
}

int convertToHexidecimal(long int value) {
	int index = 0;
	int j, temp;
	int base = 16;
	int hexBuffer[16];
	int hexCount;
	int zero = 1;

	if (value == 0) {
		my_putchar('0');
		return zero;
	}

	while (value != 0) {
		temp = value % base;
		if (temp < 10) {
			hexBuffer[index] = temp + 48;
			index++;
		} else {
			hexBuffer[index] = temp + 87;
			index++;
		}
		value /= 16;
	}

	for (j = index - 1; j >= 0; j--) {
		my_putchar(hexBuffer[j]);
	}
	hexCount = index;
	return hexCount;
}

int convertToOctal(long int decimalNumber) {
	int j;
	int index = 0;
	long int buffer[10];
	int zero = 1;

	if (decimalNumber == 0) {
		my_putnbr(0);
		return zero;
	}

	while (decimalNumber != 0) {
		buffer[index] = decimalNumber % 8;
		decimalNumber /= 8;
		index++;
	}
	for (j = index - 1; j >= 0; j--)
		my_putnbr(buffer[j]);
	return index;
}

int my_printf(char* restrict format, ...) {
	char* traverse = NULL;
	void* memAddress;
	long oval;
	signed int dValue;
	char* svalue;
	int dNumberCount;
	int characterCount = 0;
	unsigned int uValue;
	int pCount, xCount, uCount, oCount;

	traverse = format;
	va_list arg;
	va_start(arg, format);
	while (*traverse != '\0') {
		if (*traverse != '%') {
			my_putchar(*traverse);
			traverse++;
			characterCount++;
		}
		if (*traverse == '%') {
			traverse++;
			if (*traverse == 'd') {
				dValue = va_arg(arg, signed int);
				if (dValue < 0) {
					characterCount += 1;
				}
				dNumberCount = getNumberSize(abs(dValue));
				my_putnbr(dValue);
				characterCount += dNumberCount;
			} else if (*traverse == 'o') {
				oval = va_arg(arg, unsigned int);
				oCount = convertToOctal(oval);
				characterCount += oCount;
				oCount = 0;
			} else if (*traverse == 'u') {
				uValue = va_arg(arg, unsigned int);
				uCount = getNumberSize((unsigned int)uValue);
				myPutnbrLongInt(uValue);
				characterCount += uCount;
			} else if (*traverse == 'x') {
				xCount = convertToHexidecimal(va_arg(arg, unsigned int));
				characterCount += xCount;
				xCount = 0;
			} else if (*traverse == 'c') {
				my_putchar(va_arg(arg, unsigned int));
				characterCount++;
			} else if (*traverse == 's') {
				svalue = va_arg(arg, char*);
				if (svalue != NULL) {
					while (*svalue != '\0') {
						my_putchar(*svalue);
						svalue++;
						characterCount++;
					}
				} else {
					my_putString("(null)");
					characterCount += 6;
				}
			} else {
				memAddress = va_arg(arg, void*);
				pCount = printPointerAddress((long int)memAddress);
				characterCount += pCount;
				pCount = 0;
			}
			traverse++;
		}
	}
	va_end(arg);
	return characterCount;
}

/*int main() {
	int value, correct_value;

	printf("---------Test d-----------\n");
	correct_value = printf("|%d|, |%d|, |%d|, |%d|\n", 5, -9876473, 87, 0);
	value = my_printf("|%d|, |%d|, |%d|, |%d|\n", 5, -9876473, 87, 0);
	printf("My value is: %d\nCorrect value is: %d\n", value, correct_value);
	printf("\n");
	printf("---------Test o-----------\n");
	value = my_printf("|%o|, |%o|, |%o|, |%o|\n", 5, -9876473, 87, 0);
	correct_value = printf("|%o|, |%o|, |%o|, |%o|\n", 5, -9876473, 87, 0);
	printf("My value is: %d\nCorrect value is: %d\n", value, correct_value);
	printf("\n");
	printf("---------Test u-----------\n");
	value = my_printf("|%u|, |%u|, |%u|, |%u|\n", 5, -9876473, 87, 0);
	correct_value = printf("|%u|, |%u|, |%u|, |%u|\n", 5, -9876473, 87, 0);
	printf("My value is: %d\nCorrect value is: %d\n", value, correct_value);
	printf("\n");
	printf("---------Test x-----------\n");
	value = my_printf("|%x|, |%x|, |%x|, |%x|\n", 5, -9876473, 87, 0);
	correct_value = printf("|%x|, |%x|, |%x|, |%x|\n", 5, -9876473, 87, 0);
	printf("My value is: %d\nCorrect value is: %d\n", value, correct_value);
	printf("\n");
	printf("---------Test c-----------\n");
	value = my_printf("|%c|, |%c|, |%c|, |%c|\n", v, n, i, r);
	correct_value = printf("|%c|, |%c|, |%c|, |%c|\n", v, n, i, r);
	printf("My value is: %d\nCorrect value is: %d\n", value, correct_value);
	printf("\n");
	printf("---------Test p-----------\n");
	value = my_printf("|%p|, |%p|, |%p|, |%p|\n", &v, &n, &i, &r);
	correct_value = printf("|%p|, |%p|, |%p|, |%p|\n", &v, &n, &i, &r);
	printf("My value is: %d\nCorrect value is: %d\n", value, correct_value);
	printf("\n");
	printf("---------Test s-----------\n");
	value = my_printf("NULL STRING %s!\n", (char*)NULL);
	correct_value = printf("NULL STRING %s!\n", (char*)NULL);
	printf("My value is: %d\nCorrect value is: %d\n", value, correct_value);
	printf("\n");

	return 0;
}*/

