
#include "libtcc.h"
//#include "patcher/jitHooks.hpp"
//#include "wrap.h"
//#include "tcc.h"
#include <asmjit_lib/asmjit.h>
#include <asmjit_lib/asmtk/asmtk.h>
//#include <iostream>

using namespace asmjit;
using namespace asmtk;

size_t blentotal;

extern "C" size_t sstrlen (const char* s) 
{
	register const char* i;
	for(i=s; *i != '\n'; ++i);
	return (i-s);
}


uint8_t tallymarker_hextobin(const char * str, uint8_t * bytes, size_t blen)
{
	uint8_t  pos;
	uint8_t  idx0;
	uint8_t  idx1;

	// mapping of ASCII characters to hex values
	const uint8_t hashmap[] =
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  !"#$%&'
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ()*+,-./
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // 01234567
		0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 89:;<=>?
		0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // @ABCDEFG
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HIJKLMNO
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // PQRSTUVW
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // XYZ[\]^_
		0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, // `abcdefg
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // hijklmno
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // pqrstuvw
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // xyz{|}~.
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // ........
	};

	memset(bytes, 0, blen);
	for (pos = 0; ((pos < (blen*2)) && (pos < strlen(str))); pos += 2)
	{
		idx0 = (uint8_t)str[pos+0];
		idx1 = (uint8_t)str[pos+1];
		bytes[pos/2] = (uint8_t)(hashmap[idx0] << 4) | hashmap[idx1];
	};

	return(0);
}

extern "C" unsigned char* jit_assemble(TCCState *s1, char* input)
{
	//std::cout<<"hello"<<std::endl;
	//    if (ind1 > cur_text_section->data_allocated)
	//section_realloc(cur_text_section, 100);
	//memset(&cur_text_section->data[0], 0x90, 30);
	//cur_text_section->data[ind] = 0x90;

	//ind = 30;

	StringLogger logger;
	logger.addOptions(Logger::kOptionBinaryForm);

	CodeInfo ci(ArchInfo::kTypeX86, 0, Globals::kNoBaseAddress);
	CodeHolder code;

	code.init(ci);
	code.setLogger(&logger);

	X86Assembler a(&code);
	AsmParser p(&a);
	CodeBuffer buffer;
	logger.clearString();
	Error err = p.parse(input);

	unsigned char* bytecode = nullptr;

	if (err == kErrorOk)
	{
		const char* log = logger.getString();
		int len = logger.getLength();
		size_t i;
		bytecode = (unsigned char*) malloc(len);
		//printf("%d\n", len);
		//printf("%s\n", log);

		// Skip the instruction part, and keep only the comment part.
		for (i = 0; i < len; i++)
		{
			if (log[i] == ';')
			{
				i += 2;
				int step_bytes_length = sstrlen(log+i)/2;
				char temp1[step_bytes_length];
				sprintf(&temp1[0], "%.*s", (int)(step_bytes_length*2), log + i);
				printf("%s\n", &temp1[0]);
				//printf("%d\n", step_bytes_length);
				tallymarker_hextobin(temp1, bytecode+blentotal, step_bytes_length);
				blentotal+=step_bytes_length;
			}
		}

		//	ci._baseAddress = ci._baseAddress + 3;
		code.init(ci);
		code.sync();

	}
	else
	{
		a.resetLastError();
		fprintf(stdout, "ERROR: 0x%08X: %s %s\n", err, DebugUtils::errorAsString(err), input);
	}
	//printf("%d\n", bytecode[0]);
	return bytecode;
}


extern "C" void wrap_free(unsigned char* p)
{
	free(p);
}










