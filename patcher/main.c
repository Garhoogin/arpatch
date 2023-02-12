#include <stdio.h>
#include <stdint.h>

#define ASOUT_OUT "zzzpatch.out"
#define ASOUT_ELF "zzzpatch.elf"
#define ASOUT_BIN "zzzpatch.bin"
#define LDSCRIPT  "zzzpatch.x"

void writeFileToAddress(const char *path, uint32_t address) {
	int nCodeBytes, nCodeWords, nCodeLines, i;
	FILE *fp;

	//read code patch and emit AR code
	fp = fopen(path, "rb");
	fseek(fp, 0, SEEK_END);
	nCodeBytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	nCodeWords = (nCodeBytes + 3) / 4;
	nCodeLines = (nCodeWords + 1) / 2;
	uint32_t *code = (uint32_t *) calloc(nCodeLines, 8);
	fread(code, nCodeBytes, 1, fp);
	fclose(fp);
	
	//if one word or less, only emit a 0, 1, 2-code type
	if (nCodeWords <= 1) {
		//output depending on code size.
		switch (nCodeBytes) {
			case 0:
				break;
			case 1:
				printf("%08X %08X\n", 0x20000000 | address, *(uint8_t *) code);
				break;
			case 2:
				printf("%08X %08X\n", 0x10000000 | address, *(uint16_t *) code);
				break;
			case 3:
				printf("%08X %08X\n", 0x10000000 | address, *(uint16_t *) code);
				printf("%08X %08X\n", 0x20000000 | (address + 2), *(((uint8_t *) code) + 2));
				break;
			case 4:
				printf("%08X %08X\n", address, *code);
				break;
		}
	} else {
		//emit E-type write
		printf("%08X %08X\n", 0xE0000000 | address, nCodeBytes);
		for (i = 0; i < nCodeLines; i++) {
			printf("%08X %08X\n", code[i * 2], code[i * 2 + 1]);
		}
	}
	free(code);
}

int assemble(char *lineAsm, int emitThumb, uint32_t addr) {
	char textBuffer[1024];
	int result;
	FILE *pp;
	
	//assemble
	sprintf(textBuffer, "arm-none-eabi-as -mthumb-interwork -o " ASOUT_OUT " %s", emitThumb ? "-mthumb" : "");
	pp = popen(textBuffer, "w");
	fprintf(pp, ".global _start\n_start:\n%s\n", lineAsm);
	result = pclose(pp);
	if (result) return result;
	
	//link
	sprintf(textBuffer, "arm-none-eabi-ld " ASOUT_OUT " -o " ASOUT_ELF " -T " LDSCRIPT " -Ttext %08X", addr);
	pp = popen(textBuffer, "w");
	result = pclose(pp);
	if (result) return result;
	
	//objcopy
	pp = popen("arm-none-eabi-objcopy -O binary " ASOUT_ELF " " ASOUT_BIN, "w");
	result = pclose(pp);
	if (result) return result;
	
	//remove intermediates
	pp = popen("rm " ASOUT_OUT " " ASOUT_ELF, "w");
	pclose(pp);
	
	//success
	return 0;
}

void writeHooksFromFile(const char *path) {
	int lineLength, exitStatus, emitThumb;
	char *lineAsm, c;
	uint32_t addr;
	FILE *fp = fopen(path, "r");
	
	//parse by line. First part of a line is the address in hex. Rest is assembly.
	char line[1024];
	do {
		emitThumb = lineLength = 0;
		memset(line, 0, sizeof(line));
		while (c = getc(fp), c != EOF && c != '\r' && c != '\n') {
			line[lineLength++] = c;
		}
		
		//check length of line. If 0 skip
		if (lineLength == 0) continue;
		
		//hex integer at the beginning of the line is the address
		addr = strtol(line, &lineAsm, 16);
		
		//if next char is t or T, emit thumb code
		if (*lineAsm == 't' || *lineAsm == 'T') {
			emitThumb = 1;
			lineAsm++;
		}
		lineAsm++;
		
		//assemble to zzzpatch.bin
		exitStatus = assemble(lineAsm, emitThumb, addr);
		if (exitStatus) {
			printf("!!! Assembly failure: hook %08X\n", addr);
			break;
		}
		
		//insert
		writeFileToAddress(ASOUT_BIN, addr);
		system("rm " ASOUT_BIN);
	} while (c != EOF);
	
	fclose(fp);
}

void writeLinkerScriptFromSymbols(const char *path) {
	char *symName, c, symType;
	int lineLength;
	uint32_t addr;
	FILE *fp = fopen(path, "r");
	FILE *out = fopen(LDSCRIPT, "w");
	
	char line[1024];
	do {
		lineLength = 0;
		memset(line, 0, sizeof(line));
		while (c = getc(fp), c != EOF && c != '\r' && c != '\n') {
			line[lineLength++] = c;
		}
		//printf("L: %s\n", line);
		
		//check length of line. If 0 skip
		if (lineLength == 0) continue;
		
		//starts with address
		addr = strtol(line, &symName, 16);
		symName++;
		symType = *symName;
		symName += 2;
		if (symType >= 'A' && symType <= 'Z') { //global symbols uppercase
			fprintf(out, "%s = 0x%08X;\n", symName, addr);
		}
		
	} while (c != EOF);
	
	fclose(out);
	fclose(fp);
}

int main(int argc, char **argv) {
	//usage: arpatcher <address> <bin> <hooks> <symbol listing>
	if (argc < 5) return -1;
	
	//read symbols and create temporary linker script
	writeLinkerScriptFromSymbols(argv[4]);
	
	//read code address
	uint32_t baseAddress = strtol(argv[1], NULL, 16);
	
	//read code patch and emit AR code
	writeFileToAddress(argv[2], baseAddress);
	
	//parse hooks and emit code
	writeHooksFromFile(argv[3]);
	
	//remove temporary linker script
	system("rm " LDSCRIPT);
	
	return 0;
}