#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <fstream>

using namespace std;

// Node of the Huffman tree
struct Node {
    char simbol;
    int frecuencia;
    Node *izq, *der;
};

// Function prototypes
void encode(Node* root, const string& str, unordered_map<char, string> &CodeHuffman);
void decode(Node* root, int &index, string str);
Node* getNode(char simbol, int frecuencia, Node* izq, Node* der);
void generarDOT(Node* root, ofstream &file);
void deleteTree(Node* root);
void CrearTree(const string& text);

// Function to create a new node in the tree
Node* getNode(char simbol, int frecuencia, Node* izq, Node* der) {
    Node* nodo = new Node();
    nodo->simbol = simbol;
    nodo->frecuencia = frecuencia;
    nodo->izq = izq;
    nodo->der = der;
    return nodo;
}

// Comparison object used to order the priority queue
struct comp {
    bool operator()(Node* i, Node* d) {
        return i->frecuencia > d->frecuencia;
    }
};

// Traverse the Huffman tree and save codes in a map
void encode(Node* root, const string& str, unordered_map<char, string> &CodeHuffman) {
    if (root == nullptr)
        return;
    if (!root->izq && !root->der) {
        CodeHuffman[root->simbol] = str;
    }
    encode(root->izq, str + "0", CodeHuffman);
    encode(root->der, str + "1", CodeHuffman);
}

// Traverse the tree and decode the encoded symbols
void decode(Node* root, int &index, string str) {
    if (root == nullptr) {
        return;
    }
    if (!root->izq && !root->der) {
        cout << root->simbol;
        return;
    }
    index++;
    if (index < str.size()) { // Ensure indice es within bounds
        if (str[index] == '0')
            decode(root->izq, index, str);
        else
            decode(root->der, index, str);
    }
}

// Function to generate a DOT file
void generarDOT(Node* root, ofstream &file) {
    if (root == nullptr)
        return;

    // si el node es a leaf, display the symbol, frequency and direction.
    if (!root->izq && !root->der) {
        file << "    \"" << root << "\" [label=\"" << root->simbol << " (" << root->frecuencia << ")\\n" << root << "\"];\n";
    } else {
        // For internal nodes, display an asterisco, and the address
        file << "    \"" << root << R"(" [label="*\n)" << root << "\"];\n"; // Nodo interno
    }

    if (root->izq) {
        file << "    \"" << root << "\" -> \"" << root->izq << "\" [label=\"0\"];\n"; // Etiqueta 0 para la izquierda
        generarDOT(root->izq, file);
    }
    if (root->der) {
        file << "    \"" << root << "\" -> \"" << root->der << "\" [label=\"1\"];\n"; // Etiqueta 1 para la derecha
        generarDOT(root->der, file);
    }
}

// Function to delete the Huffman tree
void deleteTree(Node* root) {
    if (root == nullptr) return;
    deleteTree(root->izq);
    deleteTree(root->der);
    delete root;
}

// Create the Huffman tree and decode given symbols
void CrearTree(const string& text) {
    unordered_map<char, int> frecuencia;
    for (char simbol : text) {
        frecuencia[simbol]++;
    }

    priority_queue<Node*, vector<Node*>, comp> pq;
    for (auto pair : frecuencia) {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    while (pq.size() != 1) {
        Node* izq = pq.top(); pq.pop();
        Node* der = pq.top(); pq.pop();
        int sum = izq->frecuencia + der->frecuencia;
        pq.push(getNode('\0', sum, izq, der)); // An internal node without visible symbol
    }

    Node* root = pq.top();

    unordered_map<char, string> CodeHuffman;
    encode(root, "", CodeHuffman);

    cout << "\nLos códigos de Huffman son:\n";
    for (const auto& pair : CodeHuffman) {
        cout << pair.first << "= " << pair.second << '\n';
    }

    cout << "\nEl mensaje original era:\n" << text << '\n';

    string str;
    for (char simbol : text) {
        str += CodeHuffman[simbol];
    }

    cout << "\nEl texto codificado es:\n" << str << '\n';

    int index = -1;
    cout << "\nEl texto decodificado es:\n";
    while (index < (int)str.size() - 1) {
        decode(root, index, str);
    }
    cout << "\n";

    // Generate DOT file
    ofstream dotFile(R"(C:\Users\Alejandro Castrillon\PycharmProjects\pythonProject\mochila\huffman_tree.dot)");
    dotFile << "digraph G {\n";
    generarDOT(root, dotFile);
    dotFile << "}\n";
    dotFile.close();

    // Optionally, run Graphviz to generate a PNG image
    system(R"(cd "C:\Program Files\Graphviz\bin" && dot.exe -Tpng "C:\Users\Alejandro Castrillon\PycharmProjects\pythonProject\mochila\huffman_tree.dot" -o "C:\Users\Alejandro Castrillon\PycharmProjects\pythonProject\mochila\huffman_tree.png")");

    // Free allocated memory for the tree
    deleteTree(root);
}

int main() {
    cout << "\n-------------------------------------------------";
    cout << "\n Compression y Decompression con el methods de Huffman";
    cout << "\n        Sara Lopez Marin - Alejandro Castrillon";
    cout << "\n                 Alexander Narváez.";
    cout << "\n------------------------------------------------------";

    string text;
    cout << "\n\nIngrese el mensaje que desea comprimir: ";
    getline(cin, text);

    CrearTree(text);
    cout << "\n";
    return 0;
}
