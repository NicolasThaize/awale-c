#include <stdio.h>
#include <stdlib.h>

#include "list.h"


// Initialize an empty list
void initializeList(List* list) {
    list->head = NULL;
}

// Templated function to add a new element to the end of the list
void addElement(List* list, void* value) {
    // Create a new node
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    newNode->data = value;
    newNode->next = NULL;

    // If the list is empty, set the new node as the head
    if (list->head == NULL) {
        list->head = newNode;
    } else {
        // Otherwise, traverse the list to find the last node and append the new node
        Node* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Templated function to remove an element from the list
void removeElement(List* list, void* value) {
    Node* current = list->head;
    Node* previous = NULL;

    // Traverse the list to find the node with the specified value
    while (current != NULL && current->data != value) {
        previous = current;
        current = current->next;
    }

    // If the node with the specified value is found
    if (current != NULL) {
        // If the node to be removed is the head
        if (previous == NULL) {
            list->head = current->next;
        } else {
            previous->next = current->next;
        }

        // Free the memory of the removed node
        free(current);
    }
}

// Templated function to print the elements of the list
void printList(List* list, void (*printFunction)(void*)) {
    Node* current = list->head;

    while (current != NULL) {
        printFunction(current->data);
        current = current->next;
    }

    printf("\n");
}

// Templated function to clear the memory allocated for the list
void clearList(List* list, void (*freeFunction)(void*)) {
    Node* current = list->head;
    Node* next;

    while (current != NULL) {
        next = current->next;
        freeFunction(current->data);
        free(current);
        current = next;
    }

    list->head = NULL;
}

// Example of a function to print an integer
void printInt(void* data) {
    printf("%d ", *((int*)data));
}

// Example of a function to free memory of an integer
void freeInt(void* data) {
    free(data);
}

int main() {
    List intList;
    initializeList(&intList);

    // Add elements to the list
    int* value1 = malloc(sizeof(int));
    int* value2 = malloc(sizeof(int));
    int* value3 = malloc(sizeof(int));

    *value1 = 5;
    *value2 = 10;
    *value3 = 15;

    addElement(&intList, value1);
    addElement(&intList, value2);
    addElement(&intList, value3);

    // Print the initial list
    printf("Initial List: ");
    printList(&intList, printInt);

    // Remove an element from the list
    removeElement(&intList, value2);

    // Print the updated list
    printf("Updated List: ");
    printList(&intList, printInt);

    // Clear the list and free memory
    clearList(&intList, freeInt);

    return 0;
}



