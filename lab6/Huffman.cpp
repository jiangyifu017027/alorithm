#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <cctype>

using namespace std;

// Huffman树的节点
// 定义了Node结构体，表示Huffman树的节点，包含字符数据、频率以及左右子节点的指针
struct Node {
    char data;  // 字符
    int freq;   // 频率
    Node* left;
    Node* right;

    Node(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// 用于比较节点的频率
// 定义了Compare结构体，用于比较节点的频率，作为优先队列的比较函数
struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

// 生成Huffman树
// 实现了buildHuffmanTree函数，根据字符频率构建Huffman树。
// 该函数使用优先队列（最小堆）来存储节点，并不断合并频率最低的两个节点，直到只剩下一个根节点
Node* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    priority_queue<Node*, vector<Node*>, Compare> pq;

    // 创建叶子节点并将其插入优先队列
    for (const auto& pair : freqMap) {
        Node* leaf = new Node(pair.first, pair.second);
        pq.push(leaf);
    }

    // 构建Huffman树
    while (pq.size() > 1) {
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        Node* parent = new Node(',', left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        pq.push(parent);
    }

    return pq.top();
}

// 生成Huffman编码表
// 实现了generateHuffmanCodes函数，根据Huffman树生成字符的Huffman编码。
// 该函数使用递归方法遍历Huffman树，将叶子节点的编码存储在codes映射中。
void generateHuffmanCodes(Node* root, string code, unordered_map<char, string>& codes) {
    if (root == nullptr) {
        return;
    }

    if (root->left == nullptr && root->right == nullptr) {
        codes[root->data] = code;
    }

    generateHuffmanCodes(root->left, code + "0", codes);
    generateHuffmanCodes(root->right, code + "1", codes);
}

// 将字符串编码为01序列
// 实现了encodeString函数，将给定的字符串根据Huffman编码转换为01序列
string encodeString(const string& str, const unordered_map<char, string>& codes) {
    string encodedStr;
    for (char c : str) {
        if (codes.find(c) != codes.end()) {
            encodedStr += codes.at(c);
        }
    }
    return encodedStr;
}

// 计算压缩率
// 实现了calculateCompressionRatio函数，计算压缩率。
// 该函数根据原始字符串的长度和编码后的字符串的长度，以及字符集的长度，计算出压缩率
double calculateCompressionRatio(const string& orignalStr, const string& encodedStr, const int& ch_length) {
    int orignalBits = orignalStr.length() * ceil(log(ch_length) / log(2));
    int encodedBits = encodedStr.length();
    return static_cast<double>(encodedBits) / orignalBits;
}

// 实现了printHuffmanCodesToFile函数，将Huffman编码表打印到文件中
void printHuffmanCodesToFile(const unordered_map<char, string>& codes, const string& filename, const unordered_map<char, int>& freqMap) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& pair : codes) {
            file << pair.first << "  " << freqMap.at(pair.first) << "  " << pair.second << endl;
        }
        file.close();
    } else {
        cout << "Unable to open the file: " << filename << endl;
    }
}

int main() {
    string orignalFilename = "orignal.txt";
    string tableFilename = "table.txt";

    // 读取原始字符串
    ifstream orignalFile(orignalFilename);
    if (!orignalFile.is_open()) {
        cout << "Unable to open the file: " << orignalFilename << endl;
        return 1;
    }

    char c;
    string orignalStr;
    while (orignalFile.get(c)) {
        if (!std::isspace(c)) {
            // 在这里可以处理非空格和非换行符的字符
            orignalStr.push_back(c);
        }
    }
    orignalFile.close();

    // 统计字符频率
    unordered_map<char, int> freqMap;
    for (char c : orignalStr) {
        if (freqMap.find(c) != freqMap.end()) {
            freqMap[c]++;
        } else {
            freqMap[c] = 1;
        }
    }
    int ch_length = freqMap.size();

    // 构建Huffman树
    Node* root = buildHuffmanTree(freqMap);

    // 生成Huffman编码表
    unordered_map<char, string> codes;
    generateHuffmanCodes(root, "", codes);

    // 将原始字符串编码为01序列
    string encodedStr = encodeString(orignalStr, codes);

    // 打印Huffman编码表到文件
    printHuffmanCodesToFile(codes, tableFilename, freqMap);

    // 计算压缩率
    double compressionRatio = calculateCompressionRatio(orignalStr, encodedStr, ch_length);

    // 打印压缩率
    cout << "Compression Ratio: " << compressionRatio * 100 << "%" << endl;

    return 0;
}