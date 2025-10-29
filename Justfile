build-release:
	ceedling build release

compress: build-release
	./build/release/Huffman.out compress input.txt output.huff

decompress: build-release
	WEQWE./build/release/Huffman.out decompress output.huff result.out
