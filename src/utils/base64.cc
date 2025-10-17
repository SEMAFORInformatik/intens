#include "base64.h"

char bstr[] =
	"ABCDEFGHIJKLMNOPQ"
	"RSTUVWXYZabcdefgh"
	"ijklmnopqrstuvwxy"
	"z0123456789+/";

char rstr[] = {
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  62,   0,   0,   0,  63, 
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61,   0,   0,   0,   0,   0,   0, 
	  0,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14, 
	 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   0,   0,   0,   0,   0, 
	  0,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40, 
	 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,   0,   0,   0,   0,   0};

void base64encode(const unsigned char* input,size_t l,std::string& output, bool add_crlf){
    size_t i = 0;
    size_t o = 0;
    
    output = "";
    while (i < l){
	size_t remain = l - i;
	if (add_crlf && o && o % 76 == 0)
	    output += "\n";
	switch (remain)	{
	    case 1:
		output += bstr[ ((input[i] >> 2) & 0x3f) ];
		output += bstr[ ((input[i] << 4) & 0x30) ];
		output += "==";
		break;
	    case 2:
		output += bstr[ ((input[i] >> 2) & 0x3f) ];
		output += bstr[ ((input[i] << 4) & 0x30) + ((input[i + 1] >> 4) & 0x0f) ];
		output += bstr[ ((input[i + 1] << 2) & 0x3c) ];
		output += "=";
		break;
	    default:
		output += bstr[ ((input[i] >> 2) & 0x3f) ];
		output += bstr[ ((input[i] << 4) & 0x30) + ((input[i + 1] >> 4) & 0x0f) ];
		output += bstr[ ((input[i + 1] << 2) & 0x3c) + ((input[i + 2] >> 6) & 0x03) ];
		output += bstr[ (input[i + 2] & 0x3f) ];
	}
	o += 4;
	i += 3;
    }
}

/*
 * return true if input has been processed and output's size is sufficient
 */
bool base64decode(const std::string& input, unsigned char *output, size_t& sz) {
    size_t i = 0;
    size_t l = input.size();
    size_t j = 0;
    
    while (i < l){
	while (i < l && (input[i] == 13 || input[i] == 10))
	    i++;
	if (i < l){
	    unsigned char b1 = (unsigned char)((rstr[(int)input[i]] << 2 & 0xfc) +
					       (rstr[(int)input[i + 1]] >> 4 & 0x03));
	    if (output){
		if( j>=sz ) return false;
		output[j] = b1;
	    }
	    j++;
	    if (input[i + 2] != '='){
		unsigned char b2 = (unsigned char)((rstr[(int)input[i + 1]] << 4 & 0xf0) +
						   (rstr[(int)input[i + 2]] >> 2 & 0x0f));
		if (output){
		    if( j>=sz ) return false;
		    output[j] = b2;
		}
		j++;
	    }
	    if (input[i + 3] != '='){
		unsigned char b3 = (unsigned char)((rstr[(int)input[i + 2]] << 6 & 0xc0) +
						   rstr[(int)input[i + 3]]);
		if (output){
		    if( j>=sz ) return false;
		    output[j] = b3;
		}
		j++;
	    }
	    i += 4;
	}
    }
    sz = j;
    return true;
}

