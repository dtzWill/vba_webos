#include <iostream>
#include <iomanip>
using std::cout;

#include <stdint.h>

/*
 * Generates a lookup table for 8 bit numbers that basically multiplies the
 * index of each bit by 2. This allows the LUT to be used to interleave the
 * high and low bits of each 8 pixel row in the GB's VRAM tile data into 8
 * contiguous 2-bit numbers.
 */
int main(void)
{
	cout << "uint16_t repackTable[256] = {";
	// Iterate over each entry in the LUT
	for (unsigned int i = 0; i < 256; ++i)
	{
		if (i%8 == 0)
			cout << "\n\t";
		// If i were, for example, 0x56 (01010110 in binary), new bits would be
		// inserted as follows: -0-1-0-1-0-1-1-0, so the output would be
		// 0x1114, or 0001000100010100.
		uint16_t output = 0;
		for (int j = 0; j < 8; ++j)
			output += (i & 1<<j) << j;

		cout << "0x"
		     << std::hex
		     << std::setw(4)
		     << std::setfill('0')
		     << output;
		if (i < 255) cout << ",";
		if ( (i+1) % 8 != 0) cout << " ";
	}
	cout << "\n};\n";
}
