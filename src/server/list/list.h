


// Define the structure for a node in the list
typedef struct Node {
    void* data;
    struct Node* next;
} Node;

// Define the List structure
typedef struct {
    Node* head;
} List;

// Initialize an empty list
void initializeList(List* list);

// Add a new element to the end of the list
void addElement(List* list, void* value);

// Remove an element from the list
void removeElement(List* list, void* value);

// Print the elements of the list
void printList(List* list, void (*printFunction)(void*));

// Clear the memory allocated for the list
void clearList(List* list, void (*freeFunction)(void*));

// Example of a function to print an integer
void printInt(void* data);

// Example of a function to free memory of an integer
void freeInt(void* data);




