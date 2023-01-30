#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>

#define MAJOR 1
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

uint64_t MoveLeft(uint64_t a, uint8_t shift) {
	uint64_t b = a >> (64 - shift);
	a <<= shift;
	a ^= b;
	return a;
}

uint64_t MoveRight(uint64_t a, uint8_t shift) {
	uint64_t b = a << (64 - shift);
	a >>= shift;
	a ^= b;
	return a;
}

uint64_t XORMake(uint64_t a, uint64_t mask) {
	return a ^ mask;
}

// Use same mask as for XORMake
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

	if (packer == DONTPACK) {
		// make SIMD later
		if (crypter == ENCRYPT) {
			auto arbeit = (uint64_t)(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
			ifstream file(in);
			const auto sz = filesystem::file_size(in);
			auto leftover = sz % 8;
			std::string str(sz + leftover, '\0');
			file.read(str.data(), sz);
			file.close();
			auto data = str.data();
			const auto len = (size_t)(str.size() * 0.125 + 0.5);
			std::cout << " len: " << len << endl;
			auto datat = reinterpret_cast<uint64_t*>(data);
			for (auto i = 0; i < len; i++) {
				auto tmp = datat[i];
				tmp = XORMake(tmp, 0xDECADE);
				tmp += 0xF007BA11;
				tmp = XORMake(tmp, 0xDEADBEEF);
				tmp = MoveLeft(tmp, 0xA);
				tmp = XORMake(tmp, 0xC0FEEBABE);
				tmp = MoveLeft(tmp, 0xB);
				tmp += 0xDEC1A551F1ED;
				tmp = XORMake(tmp, 0xD15C105ED);
				tmp = MoveRight(tmp, 0xC);
				tmp = XORMake(tmp, 0x0B5E55ED);
				tmp = MoveRight(tmp, 0xD);
				tmp += 0x1DEA;
				datat[i] = tmp;
			}
			auto datafinal = reinterpret_cast<char*>(datat);
			ofstream ofile(out);
			ofile.write(datafinal, sz);
			ofile.close();
			std::cout << (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - arbeit) * 0.001;
		}
		if (crypter == DECRYPT) {

		}

		if (crypter == DONTCRYPT) {

		}
	}

	return 0;
}