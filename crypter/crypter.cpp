#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>

#define USE_VECTORS

// #ifdef USE_VECTORS
#include <immintrin.h>
// #endif // USE_VECTORS

#define MAJOR 2
#define MINOR 0

// -d = decrypt
// -e = encrypt
// -p = pack
// -l = limit per pack
// -u = unpack

#define ENCRYPT 0xE
#define DECRYPT 0xD
#define PACK 0xAB
#define UNPACK 0xBA
#define DONTPACK 0x0
#define DONTCRYPT 0x0

#define INVERT_XOR 0xFFFFFFFFFFFFFFFF

using namespace std;

__m256i oxia, oxib, oxic, oxid;
__m256i oxa, oxb, oxc, oxd;

uint64_t MoveLeft(uint64_t a, uint8_t shift) {
	uint64_t b = a >> (64 - shift);
	a <<= shift;
	a ^= b;
	return a;
}

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

void MoveLeft(uint64_t* a, uint8_t shift) {
	uint64_t b = *a >> (64 - shift);
	*a <<= shift;
	*a ^= b;
}

uint64_t MoveRight(uint64_t a, uint8_t shift) {
	uint64_t b = a << (64 - shift);
	a >>= shift;
	a ^= b;
	return a;
}

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

void MoveRight(uint64_t* a, uint8_t shift) {
	uint64_t b = *a << (64 - shift);
	*a >>= shift;
	*a ^= b;
}

uint64_t XORMake(uint64_t a, uint64_t mask) {
	return a ^ mask;
}

void XORMake(uint64_t* a, uint64_t mask) {
	*a ^= mask;
}

uint64_t XORInv(uint64_t a, uint64_t mask) {
	return a ^ (mask ^ INVERT_XOR);
}

int main(int argc, char* argv[]) {
	// help
	if (argc < 2) {
		std::cout << "SeqoCrypt " << MAJOR << "." << MINOR << endl;
		std::cout << "\t-d \tdecrypt\n";
		std::cout << "\t-e \tencrypt\n";
		std::cout << "\t-p \tpack\n";
		std::cout << "\t-l \tlimit per pack\n";
		std::cout << "\t-u \tunpack\n";
		cin.get();
		return 0;
	}

	uint8_t crypter = DONTCRYPT;
	uint8_t packer = DONTPACK;
	uint32_t limiter = 0x40000000; // IN BYTES

	// arguments
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

	std::string in = argv[argc - 2];
	std::string out = argv[argc - 1];

	const string imageTypes[8] = { "png", "jpg", "jpeg", "tga", "tiff", "bmp", "heif", "raw" };
	const string modelTypes[4] = { "gltf", "glb", "fbx", "obj" };
	const string type = "seq";
	const string pack = "lab";

	//std::cout << (int)crypter << " " << (int)packer;
#ifdef USE_VECTORS
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
		// make SIMD later
		if (crypter == ENCRYPT) {
			auto arbeit = (uint64_t)(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
			ifstream file(in, ios::binary);
			const auto sz = filesystem::file_size(in);
			//std::cout << "Left: " << (int)leftover << " File_size: " << sz << endl;
#ifdef USE_VECTORS
			uint8_t leftover = sz % 32;
			const auto totale = sz + 32 - leftover;
			string str(totale, 32 - leftover);
#else
			uint8_t leftover = sz % 8;
			const auto totale = sz + 8 - leftover;
			string str(totale, 8 - leftover);
#endif // USE_VECTORS
			file.read(str.data(), sz);
			file.close();
			auto data = str.data();
			const auto len = (size_t)(str.size() * 0.125 + 0.5);
			// std::cout << " len: " << len << endl;
#ifdef USE_VECTORS
			auto datat = reinterpret_cast<int64_t*>(data);
			for (auto i = 0; i < len; i += 4) {
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
			}
#else
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
			auto datafinal = reinterpret_cast<char*>(datat);
			ofstream ofile(out, ios::binary);
			ofile.write(datafinal, totale);
			ofile.close();
			std::cout << (chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() - arbeit) * 0.001 << "ms";
		}

		if (crypter == DECRYPT) {
			auto arbeit = (uint64_t)(chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count());
			ifstream file(in, ios::binary);
			const auto sz = filesystem::file_size(in);
			std::string str(sz, '\0');
			file.read(str.data(), sz);
			file.close();
			auto data = str.data();
			const auto len = (size_t)(str.size() * 0.125 + 0.5);
			// std::cout << " len: " << len << endl;
#ifdef USE_VECTORS
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
			auto datafinal = reinterpret_cast<char*>(datat);
			const auto leftover = (uint8_t)(datafinal[str.size() - 1]);
			// std::cout << "Left: " << (int)leftover << endl;
			const auto totallenfinal = str.size() - leftover;
			ofstream ofile(out, ios::binary);
			ofile.write(datafinal, totallenfinal);
			ofile.close();
			std::cout << (chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() - arbeit) * 0.001 << "ms";
		}

		if (crypter == DONTCRYPT) {

		}
	}

	return 0;
}