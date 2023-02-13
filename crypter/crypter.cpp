// SeqCrypter - crypter for SequoiaEngine

// Includes
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>

#define USE_VECTORS

// Uncomment if you want to measure operation time
// #define MEASURE_TIME

// #ifdef USE_VECTORS
#include <immintrin.h>
// #endif // USE_VECTORS

// Version
#define MAJOR 2
#define MINOR 0

// -d = decrypt
// -e = encrypt
// -p = pack
// -l = limit per pack
// -u = unpack

// Defined constants
#define ENCRYPT 0xE
#define DECRYPT 0xD
#define PACK 0xAB
#define UNPACK 0xBA
#define DONTPACK 0x0
#define DONTCRYPT 0x0

#define INVERT_XOR 0xFFFFFFFFFFFFFFFF

// No need for anything else than std
using namespace std;

__m256i oxia, oxib, oxic, oxid;
__m256i oxa, oxb, oxc, oxd;

// Standard Endless Left BitShift 
uint64_t MoveLeft(uint64_t a, uint8_t shift) {
	uint64_t b = a >> (64 - shift);
	a <<= shift;
	a ^= b;
	return a;
}

// SIMD Endless Left BitShift (4 versions for a, b, c, d options)
__m256i MoveLeftA(__m256i a) {
	auto b = _mm256_srlv_epi64(a, oxia);
	a = _mm256_sllv_epi64(a, oxa);
	a = _mm256_xor_si256(a, b);
	return a;
}

__m256i MoveLeftB(__m256i a) {
	auto b = _mm256_srlv_epi64(a, oxib);
	a = _mm256_sllv_epi64(a, oxb);
	a = _mm256_xor_si256(a, b);
	return a;
}

__m256i MoveLeftC(__m256i a) {
	auto b = _mm256_srlv_epi64(a, oxic);
	a = _mm256_sllv_epi64(a, oxc);
	a = _mm256_xor_si256(a, b);
	return a;
}

__m256i MoveLeftD(__m256i a) {
	auto b = _mm256_srlv_epi64(a, oxid);
	a = _mm256_sllv_epi64(a, oxd);
	a = _mm256_xor_si256(a, b);
	return a;
}

// Standard Endless Left BitShift by reference
void MoveLeft(uint64_t* a, uint8_t shift) {
	uint64_t b = *a >> (64 - shift);
	*a <<= shift;
	*a ^= b;
}

// Standard Endless Right BitShift
uint64_t MoveRight(uint64_t a, uint8_t shift) {
	uint64_t b = a << (64 - shift);
	a >>= shift;
	a ^= b;
	return a;
}

// SIMD Endless Right BitShift (4 versions for a, b, c, d options)
__m256i MoveRightA(__m256i a) {
	auto b = _mm256_sllv_epi64(a, oxia);
	a = _mm256_srlv_epi64(a, oxa);
	a = _mm256_xor_si256(a, b);
	return a;
}

__m256i MoveRightB(__m256i a) {
	auto b = _mm256_sllv_epi64(a, oxib);
	a = _mm256_srlv_epi64(a, oxb);
	a = _mm256_xor_si256(a, b);
	return a;
}

__m256i MoveRightC(__m256i a) {
	auto b = _mm256_sllv_epi64(a, oxic);
	a = _mm256_srlv_epi64(a, oxc);
	a = _mm256_xor_si256(a, b);
	return a;
}

__m256i MoveRightD(__m256i a) {
	auto b = _mm256_sllv_epi64(a, oxid);
	a = _mm256_srlv_epi64(a, oxd);
	a = _mm256_xor_si256(a, b);
	return a;
}

// Standard Endless Right BitShift by reference
void MoveRight(uint64_t* a, uint8_t shift) {
	uint64_t b = *a << (64 - shift);
	*a >>= shift;
	*a ^= b;
}

// XOR Mask (use 2 times to unmask)
uint64_t XORMake(uint64_t a, uint64_t mask) {
	return a ^ mask;
}

// Same, but by reference
void XORMake(uint64_t* a, uint64_t mask) {
	*a ^= mask;
}

// Inverted XOR
uint64_t XORInv(uint64_t a, uint64_t mask) {
	return a ^ (mask ^ INVERT_XOR);
}

// Maybe used in future
union DataSplitter {
	char data[32];
	uint8_t udata[32];
	int64_t split[4];
};

int main(int argc, char* argv[]) {
	// Display help if no arguments
	if (argc < 2) {
		std::cout << "SeqoCrypt " << MAJOR << "." << MINOR << endl;
		std::cout << "\t-d \tdecrypt\n";
		std::cout << "\t-e \tencrypt\n";
		std::cout << "\t-p \tpack\n";
		std::cout << "\t-l \tlimit per pack\n";
		std::cout << "\t-u \tunpack\n";
		std::cout << "\tex.\tcrypter\t[args]\t[in_path]\t[out_path]";
		cin.get();
		return 0;
	}

	// Default values
	uint8_t crypter = DONTCRYPT;
	uint8_t packer = DONTPACK;
	uint32_t limiter = 0x40000000; // IN BYTES

	// Loop through arguments, set the values
	for (auto i = 1; i < argc - 2; i++) {
		switch (argv[i][1]) {
		case 'D':
		case 'd':
			crypter = DECRYPT;
			break;
		case 'E':
		case 'e':
			crypter = ENCRYPT;
			break;
		case 'P':
		case 'p':
			packer = PACK;
			break;
		case 'U':
		case 'u':
			packer = UNPACK;
			break;
		case 'L':
		case 'l':
			limiter = stoi(std::string{ argv[++i] });
			break;
		}
	}

	// path for input and output
	std::string in = argv[argc - 2];
	std::string out = argv[argc - 1];

	// unnecessary, leave for future
	/*
	const string imageTypes[8] = {"png", "jpg", "jpeg", "tga", "tiff", "bmp", "heif", "raw"};
	const string modelTypes[4] = { "gltf", "glb", "fbx", "obj" };
	const string type = "seq";
	const string pack = "lab";
	*/
	//

#ifdef USE_VECTORS
	// prepare vectors for SIMD operations
	auto decade = _mm256_set1_epi64x(0xDECADE);
	auto football = _mm256_set1_epi64x(0xF007BA11);
	auto deadbeef = _mm256_set1_epi64x(0xDEADBEEF);
	oxa = _mm256_set1_epi64x(0xA);
	oxia = _mm256_set1_epi64x(0x40 - 0xA);
	auto cofeebabe = _mm256_set1_epi64x(0xC0FEEBABE);
	oxb = _mm256_set1_epi64x(0xB);
	oxib = _mm256_set1_epi64x(0x40 - 0xB);
	auto declassified = _mm256_set1_epi64x(0xDEC1A551F1ED);
	auto disclosed = _mm256_set1_epi64x(0xD15C105ED);
	oxc = _mm256_set1_epi64x(0xC);
	oxic = _mm256_set1_epi64x(0x40 - 0xC);
	auto obsessed = _mm256_set1_epi64x(0x0B5E55ED);
	oxd = _mm256_set1_epi64x(0xD);
	oxid = _mm256_set1_epi64x(0x40 - 0xD);
	auto idea = _mm256_set1_epi64x(0x1DEA);
#endif // USE_VECTORS

	if (packer == DONTPACK) {

		// ENCRYPTION
		if (crypter == ENCRYPT) {
#ifdef MEASURE_TIME
			// time measurement start
			auto arbeit = (uint64_t)(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
#endif
			// Open file in binary mode
			ifstream file(in, ios::binary);
			// File size
			const auto sz = filesystem::file_size(in);
			//std::cout << "Left: " << (int)leftover << " File_size: " << sz << endl;
#ifdef USE_VECTORS
			// Get the rest of division by 32 and add that to a total file size for SIMD operations
			// uint8_t leftover = sz % 32;
			uint8_t leftover = (uint8_t)(sz - (uint64_t)(sz * 0.03125 + 0.015625));
			const auto totale = sz + 32 - leftover;
			// Create string for input
			string str(totale, 32 - leftover);
#else
			// Same as for SIMD
			uint8_t leftover = sz % 8;
			const auto totale = sz + 8 - leftover;
			string str(totale, 8 - leftover);
#endif // USE_VECTORS
			// Read data to string
			file.read(str.data(), sz);
			file.close();
			auto data = str.data();
			// Size in 64bit ints
			const auto len = (size_t)(str.size() * 0.125 + 0.5);
			// std::cout << " len: " << len << endl;
#ifdef USE_VECTORS
			// Represent byte buffer in 64bit ints buffer
			auto datat = reinterpret_cast<int64_t*>(data);
			// Crypt each 64 bit field with XOR, ADD, Bitshift and more
			for (auto i = 0; i < len; i += 4) {
				// Load into vector
				auto mm = _mm256_loadu_epi64(&datat[i]);
				mm = _mm256_xor_si256(mm, decade);
				mm = _mm256_add_epi64(mm, football);
				mm = _mm256_xor_si256(mm, deadbeef);
				mm = MoveLeftA(mm);
				mm = _mm256_xor_si256(mm, cofeebabe);
				mm = MoveLeftB(mm);
				mm = _mm256_add_epi64(mm, declassified);
				mm = _mm256_xor_si256(mm, disclosed);
				mm = MoveRightC(mm);
				mm = _mm256_xor_si256(mm, obsessed);
				mm = MoveRightD(mm);
				mm = _mm256_add_epi64(mm, idea);
				_mm256_store_si256((__m256i*)(&datat[i]), mm);
				// Store after encryption
			}
#else
			// Same process as for SIMD
			auto datat = reinterpret_cast<uint64_t*>(data);
			for (auto i = 0; i < len; i++) {
				auto mm = datat[i];
				XORMake(&mm, 0xDECADE);
				mm += 0xF007BA11;
				XORMake(&mm, 0xDEADBEEF);
				MoveLeft(&mm, 0xA);
				XORMake(&mm, 0xC0FEEBABE);
				MoveLeft(&mm, 0xB);
				mm += 0xDEC1A551F1ED;
				XORMake(&mm, 0xD15C105ED);
				MoveRight(&mm, 0xC);
				XORMake(&mm, 0x0B5E55ED);
				MoveRight(&mm, 0xD);
				mm += 0x1DEA;
				datat[i] = mm;
			}
#endif // USE_VECTORS
			// Represent 64bit int buffer as byte buffer
			auto datafinal = reinterpret_cast<char*>(datat);
			// Open output file as binary and save it there
			ofstream ofile(out, ios::binary);
			ofile.write(datafinal, totale);
			ofile.close();
#ifdef MEASURE_TIME
			// time measurement output
			const auto gg = (chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() - arbeit) * 0.001;
			std::cout << gg << "ms";
#endif
		}

		if (crypter == DECRYPT) {
#ifdef MEASURE_TIME
			// time measurement start
			auto arbeit = (uint64_t)(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
#endif
			// Open encrypted file, get its size, make string and read it in there, get length in 64bit ints
			ifstream file(in, ios::binary);
			const auto sz = filesystem::file_size(in);
			std::string str(sz, '\0');
			file.read(str.data(), sz);
			file.close();
			auto data = str.data();
			const auto siz3 = str.size();
			const auto len = (size_t)(siz3 * 0.125 + 0.5);
			// std::cout << " len: " << len << endl;
#ifdef USE_VECTORS
			// Represent as 64bit ints buffer and decrypt each 64bit with reversed process
			auto datat = reinterpret_cast<int64_t*>(data);
			for (auto i = 0; i < len; i += 4) {
				auto mm = _mm256_loadu_epi64(&datat[i]);
				mm = _mm256_sub_epi64(mm, idea);
				mm = MoveLeftD(mm);
				mm = _mm256_xor_si256(mm, obsessed);
				mm = MoveLeftC(mm);
				mm = _mm256_xor_si256(mm, disclosed);
				mm = _mm256_sub_epi64(mm, declassified);
				mm = MoveRightB(mm);
				mm = _mm256_xor_si256(mm, cofeebabe);
				mm = MoveRightA(mm);
				mm = _mm256_xor_si256(mm, deadbeef);
				mm = _mm256_sub_epi64(mm, football);
				mm = _mm256_xor_si256(mm, decade);
				_mm256_store_si256((__m256i*)(&datat[i]), mm);
			}
#else
			// Basically the same but with no SIMD
			auto datat = reinterpret_cast<uint64_t*>(data);
			for (auto i = 0; i < len; i++) {
				auto mm = datat[i];
				mm -= 0x1DEA;
				MoveLeft(&mm, 0xD);
				XORMake(&mm, 0x0B5E55ED);
				MoveLeft(&mm, 0xC);
				XORMake(&mm, 0xD15C105ED);
				mm -= 0xDEC1A551F1ED;
				MoveRight(&mm, 0xB);
				XORMake(&mm, 0xC0FEEBABE);
				MoveRight(&mm, 0xA);
				XORMake(&mm, 0xDEADBEEF);
				mm -= 0xF007BA11;
				XORMake(&mm, 0xDECADE);
				datat[i] = mm;
			}
#endif // USE_VECTORS
			// Get the final data as byte buffer
			auto datafinal = reinterpret_cast<char*>(datat);
			const auto leftover = (uint8_t)(datafinal[siz3 - 1]);
			// std::cout << "Left: " << (int)leftover << endl;
			// Save only the real bytes, without fillups to an output file
			const auto totallenfinal = siz3 - leftover;
			ofstream ofile(out, ios::binary);
			ofile.write(datafinal, totallenfinal);
			ofile.close();
#ifdef MEASURE_TIME
			// time measurement output
			const auto gg = (chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() - arbeit) * 0.001;
			std::cout << gg << "ms";
#endif
		}

		if (crypter == DONTCRYPT) {

		}
	}

	if (packer == PACK) {
		// not implemented yet
	}

	return 0;
}