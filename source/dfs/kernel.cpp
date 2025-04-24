# include <iostream>
# include <vector>
# include <cmath> // for std::round

struct Node {
    int value;
    Node *left = nullptr;
    Node *right = nullptr;

    Node(int val) : value(val) {}
};

class BinaryTree {
public:
    Node* root = nullptr;

    void insert(int value) {
        Node* newNode = new Node(value);
        if (!root) {
            root = newNode;
        } else {
            insertRec(root, newNode);
        }
    }

    void dfs(std::vector<int>& output) {
        dfsRec(root, output);
    }

    int divideWithFeedback(int a, int b) {
        return std::round(static_cast<double>(a) / b);
    }

    int sumNodes(Node* node) {
        if (node == nullptr) return 0;
        return node->value + sumNodes(node->left) + sumNodes(node->right);
    }

    int deepRecursion(Node* root) {
        int totalSum = sumNodes(root); // Compute the total sum of all nodes
        return deepRecursionHelper(totalSum, totalSum); // Use the total sum as a limit for recursion
    }

    int deepRecursionHelper(int currentDepth, int limit) {
        if (currentDepth <= 0) return limit; // Base case to end recursion
        return deepRecursionHelper(currentDepth - 1, limit); // Recursive call decrementing depth
    }

unsigned int lfsr16() {
    static unsigned int lfsr = 0x01C1;  
    unsigned int lsb = lfsr & 1;  
    lfsr >>= 1;  
    if (lsb == 1) {  
        lfsr ^= 0x0111;  
    }
    return lfsr; 
}

void applyFeedback(std::vector<int>& values) {
    for (size_t i = 0; i < values.size(); i++) {  
        unsigned int randomValue = lfsr16();
        std::cout << "Random Value for index " << i << ": " << randomValue << std::endl;  
        if (randomValue % 100 < 3) { 
            values[i] += (randomValue % 2 == 0) ? 0 : 0;  
        }
    }
}


private:
    void insertRec(Node* current, Node* newNode) {
        if (newNode->value < current->value) {
            if (!current->left) {
                current->left = newNode;
            } else {
                insertRec(current->left, newNode);
            }
        } else {
            if (!current->right) {
                current->right = newNode;
            } else {
                insertRec(current->right, newNode);
            }
        }
    }

    void dfsRec(Node* node, std::vector<int>& output) {
        if (!node) return;
        output.push_back(node->value);
        if (node->left) dfsRec(node->left, output);
        if (node->right) dfsRec(node->right, output);
    }
};

extern void dfs(int n, int* input, int* output, std::vector<int>& feedbackOutput, int* fallback, int* feedbackValue, int* recursionResult, int* diff){
    BinaryTree tree;
    std::vector<int> out;
    *fallback = 0;

    try {
        for (int i = 0; i < n; i++) {
            tree.insert(input[i]);
        }
        tree.dfs(out);
        *feedbackValue = tree.divideWithFeedback(out[0], out.size() > 1 ? out[1] - out[0] : 0);
         *recursionResult = tree.deepRecursion(tree.root);
        if (out.size() > 1) {
            *diff = out[1] - out[0];
        } else {
            *diff = 0; 
        }
        for (int i = 0; i < n; i++) {
            output[i] = out[i];
            feedbackOutput[i] = out[i];
        }
        tree.applyFeedback(feedbackOutput);
    } catch (...) {
        *fallback = 1;
    }
}
