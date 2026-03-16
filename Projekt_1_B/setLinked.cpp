#include <iostream>
#include <memory>

class Node {
public:
    int value;
    std::shared_ptr<Node> next;

    Node(int val) : value(val), next(nullptr) {}
};

class setLinked {
private:
    std::shared_ptr<Node> head;

public:
    setLinked() : head(nullptr) {}

    void insert(int value) {
        auto newNode = std::make_shared<Node>(value);
        if (!head || head->value > value) {
            newNode->next = head;
            head = newNode;
            return;
        }

        auto current = head;
        while (current->next && current->next->value < value) {
            current = current->next;
        }

        if (!current->next || current->next->value != value) {
            newNode->next = current->next;
            current->next = newNode;
        }
    }

    void remove(int value) {
        if (!head) return;

        if (head->value == value) {
            head = head->next;
            return;
        }

        auto current = head;
        while (current->next && current->next->value != value) {
            current = current->next;
        }

        if (current->next) {
            current->next = current->next->next;
        }
    }

    bool contains(int value) const {
        auto current = head;
        while (current) {
            if (current->value == value) return true;
            current = current->next;
        }
        return false;
    }

    void print() const {
        auto current = head;
        while (current) {
            std::cout << current->value << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};

int main() {
    setLinked mySet;

    mySet.insert(5);
    mySet.insert(3);
    mySet.insert(8);
    mySet.insert(3); // Duplicate, should not be added

    std::cout << "Set after insertions: ";
    mySet.print();

    mySet.remove(3);
    std::cout << "Set after removing 3: ";
    mySet.print();

    std::cout << "Contains 5? " << (mySet.contains(5) ? "Yes" : "No") << std::endl;
    std::cout << "Contains 10? " << (mySet.contains(10) ? "Yes" : "No") << std::endl;

    return 0;
}
