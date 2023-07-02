#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <bitset>
using namespace std;

// Huffman tree node
struct HuffmanNode
 {
    char data;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char data, int frequency)
    {
        this->data = data;
        this->frequency = frequency;
        left = NULL;
        right = NULL;
    }
};

// Comparison function for priority queue
struct Compare
{
    bool operator()(HuffmanNode* a, HuffmanNode* b)
    {
        return a->frequency > b->frequency;
    }
};

// Utility function to generate Huffman codes
void generateCodes(HuffmanNode* root, string code, unordered_map<char, string>& codes) {
    if (root == nullptr) return;

    // If the node is a leaf node (contains a character)
    if (!root->left && !root->right) {
        codes[root->data] = code;
    }

    if (root->left) {
        generateCodes(root->left, code + "0", codes);
    }
    if (root->right) {
        generateCodes(root->right, code + "1", codes);
    }
}

// Function to build the Huffman tree and return the root node
HuffmanNode* buildHuffmanTree(unordered_map<char, int>& freq) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    for (auto pair : freq) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* newNode = new HuffmanNode('#', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);
    }

    return pq.top();
}

// Function to perform Huffman encoding on the input data
string huffmanEncode(string input) {
    // Calculate frequency of each character
    unordered_map<char, int> freq;
    for (char c : input) {
        freq[c]++;
    }

    // Build the Huffman tree
    HuffmanNode* root = buildHuffmanTree(freq);

    // Generate Huffman codes
    unordered_map<char, string> codes;
    generateCodes(root, "", codes);

    // Encode the input string using Huffman codes
    string encoded;
    for (char c : input) {
        encoded += codes[c];
    }

    return encoded;
}

// Function to write the Huffman codes and compressed data to the output file
void writeCompressedDataToFile(const string& compressedData, const string& outputFileName, const unordered_map<char, string>& codes) {
    ofstream outputFile(outputFileName, ios::binary);
    if (!outputFile) {
        cout << "Error opening output file!" << endl;
        return;
    }

    // Write the Huffman codes to the output file as a header
    int codesSize = codes.size();
    outputFile.write(reinterpret_cast<const char*>(&codesSize), sizeof(codesSize));
    for (const auto& pair : codes) {
        outputFile.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
        int codeSize = pair.second.size();
        outputFile.write(reinterpret_cast<const char*>(&codeSize), sizeof(codeSize));
        outputFile.write(pair.second.c_str(), codeSize);
    }

    // Convert the compressed data to bytes
    string byteBuffer;
    for (char bit : compressedData) {
        byteBuffer += bit;
        if (byteBuffer.size() == 8) {
            char byte = static_cast<char>(bitset<8>(byteBuffer).to_ulong());
            outputFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
            byteBuffer.clear();
        }
    }

    // Write any remaining bits if there are any
    if (!byteBuffer.empty()) {
        // Pad the byte buffer with zeros
        while (byteBuffer.size() < 8) {
            byteBuffer += '0';
        }

        char byte = static_cast<char>(bitset<8>(byteBuffer).to_ulong());
        outputFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
    }

    outputFile.close();
}

// Compress the input file
// By using const references, we ensure that the function handles the files in a read-only manner and provides an additional level of safety by preventing accidental modifications to the file names.
void compressFile(const string& inputFileName, const string& outputFileName)
{
    // Read the input file
    ifstream inputFile(inputFileName);

// If the file specified by the inputFileName string does not exist or cannot be opened, the ifstream constructor will fail to open the file. In such a case, the condition if (!inputFile) will evaluate to true, indicating that there was an error opening the input file.
    if (!inputFile)
    {
        cout << "Error opening input file!" << endl;
        return;
    }

    // Read the contents of the file
    string inputContent((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));
    inputFile.close();
/*
The code string inputContent((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>())) reads the contents of the input file and stores it in the inputContent string.

Here's how it works:
1. istreambuf_iterator<char>(inputFile) creates an iterator istreambuf_iterator<char> that points to the beginning of the input stream of the inputFile.
2. The iterator range (istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()) represents the range of characters in the input stream, from the beginning to the end.
3. The range of characters is used to construct a string inputContent. The constructor of the string class takes two iterators, specifying the range of characters to be used to initialize the string.
4. After reading the contents of the file and storing it in inputContent, inputFile.close() is called to close the input file. This ensures that the file is properly closed and resources are released.
*/
    // Perform Huffman encoding
    string compressedData = huffmanEncode(inputContent);

    // Calculate frequency of each character
    unordered_map<char, int> freq;
    for (char c : inputContent)
    {
        freq[c]++;
    }

    // Generate Huffman codes for writing to the output file
    unordered_map<char, string> codes;
    generateCodes(buildHuffmanTree(freq), "", codes);

    // Write the compressed data to the output file
    writeCompressedDataToFile(compressedData, outputFileName, codes);

    cout << "File compressed successfully!" << endl;
}

int main()
{
    string inputFileName;
    cout<<"Enter the input file name: "; cin>>inputFileName;
    string outputFileName;
    cout<<"Enter the output file name: "; cin>>outputFileName;

    compressFile(inputFileName, outputFileName);

    return 0;
}