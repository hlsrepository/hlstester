# include <iostream>
# include <vector>
# include <queue>
# include <stack>
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

    // Breadth-First Search implementation using a queue
    void bfs(std::vector<int>& output) {
        if (!root) return;
        std::queue<Node*> queue;
        queue.push(root);
        while (!queue.empty()) {
            Node* current = queue.front();
            queue.pop();
            output.push_back(current->value);
            if (current->left) queue.push(current->left);
            if (current->right) queue.push(current->right);
        }
    }

    // Pre-Order Traversal (root-left-right)
    void preOrder(std::vector<int>& output) {
        preOrderRec(root, output);
    }

    int divideWithFeedback(int a, int b) {
        return std::round(static_cast<double>(a) / b);
    }

    // Modified sumNodes function: return the sum of the two smallest nodes in the binary tree
    int sumNodes(Node* node) {
        if (node == nullptr) return 0;
        std::stack<Node*> s;
        Node* current = node;
        int count = 0;
        int sum = 0;
        // In-order traversal: visit the smallest nodes first
        while (!s.empty() || current != nullptr) {
            while (current != nullptr) {
                s.push(current);
                current = current->left;
            }
            current = s.top();
            s.pop();
            sum += current->value;
            count++;
            if (count == 2) { // Stop after finding the two smallest nodes
                return sum;
            }
            current = current->right;
        }
        return sum; // If only one node exists, return its value
    }

    int deepRecursion(Node* root) {
        int totalSum = sumNodes(root); // Compute the total sum of the smallest two nodes
        return deepRecursionHelper(totalSum, totalSum); // Use the sum as a recursion limit
    }

    int deepRecursionHelper(int currentDepth, int limit) {
        if (currentDepth <= 0) return limit; // Base case to stop recursion
        return deepRecursionHelper(currentDepth - 1, limit); // Recursive call
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
            if (randomValue % 1000 < 20) { 
                values[i] += (randomValue % 2 == 0) ? 0 : 0;  // Placeholder for feedback
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

    void preOrderRec(Node* node, std::vector<int>& output) {
        if (!node) return;
        output.push_back(node->value);
        preOrderRec(node->left, output);
        preOrderRec(node->right, output);
    }
};

extern void bfs(int n, int* input, int* output, std::vector<int>& feedbackOutput, int* fallback, int* feedbackValue, int* recursionResult, int* diff){
    BinaryTree tree;
    std::vector<int> out;
    std::vector<int> preOrderOut;
    *fallback = 0;

    try {
        for (int i = 0; i < n; i++) {
            tree.insert(input[i]);
        }
        tree.bfs(out);
        tree.preOrder(preOrderOut);
        *feedbackValue = tree.divideWithFeedback(out[0], out.size() > 1 ? out[1] - out[0] + 7 : 0);
        *recursionResult = tree.deepRecursion(tree.root);
        if (out.size() > 1) {
            *diff = out[1] - out[0];
        } else {
            *diff = 0; 
        }
        for (int i = 0; i < n; i++) {
            output[i] = preOrderOut[i];  
            feedbackOutput[i] = preOrderOut[i]; 
        }
        tree.applyFeedback(feedbackOutput);
    } catch (...) {
        *fallback = 1;
    }
}
