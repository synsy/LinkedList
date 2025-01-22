//============================================================================
// Name        : LinkedList.cpp
// Author      : Christopher O'Dell
// Version     : 1.0
// Copyright   : Copyright � 2023 SNHU COCE
// Description : Lab 3-2 Lists and Searching
//============================================================================

#include <algorithm>
#include <iostream>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

//============================================================================
// Linked-List class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a linked-list.
 */
class LinkedList {

private:
    //Internal structure for list entries, housekeeping variables
    struct Node {
        Bid bid;
        Node *next;

        // default constructor
        Node() {
            next = nullptr;
        }

        // initialize with a bid
        Node(Bid aBid) {
            bid = aBid;
            next = nullptr;
        }
    };

    Node* head;
    Node* tail;
    int size = 0;

public:
    LinkedList();
    virtual ~LinkedList();
    void Append(Bid bid);
    void Prepend(Bid bid);
    void PrintList();
    void Remove(string bidId);
    Bid Search(string bidId);
    int Size();
};

LinkedList::LinkedList() {
    //set head and tail equal to nullptr
    head = nullptr;
    tail = nullptr;
}

/**
 * Destructor
 */
LinkedList::~LinkedList() {
    // start at the head
    Node* currentNode = head;
    Node* temp;

    // loop over each node, detach from list then delete
    while (currentNode != nullptr) {
        temp = currentNode; // hang on to current node
        currentNode = currentNode->next; // make current the next node
        delete temp; // delete the orphan node
    }
}

/**
 * Append a new bid to the end of the list
 */
void LinkedList::Append(Bid bid) {
    //Create new node
	Node* newNode = new Node(bid);
    //if there is nothing at the head...
	if (head == nullptr) {
        // new node becomes the head and the tail
		head = newNode;
		tail = newNode;
    }
    else {
        // make current tail node point to the new node
		tail->next = newNode;
        // and tail becomes the new node
		tail = newNode;
    }
    //increase size count
    size++;
}

/**
 * Prepend a new bid to the start of the list
 */
void LinkedList::Prepend(Bid bid) {
    // Create new node
    Node* newNode = new Node(bid);
	// if the list is empty
	if (head == nullptr) {
		// new node becomes the head and the tail
		head = newNode;
		tail = newNode;
    }
	else {
		// make the new node point to the head
		newNode->next = head;
        // head now becomes the new node
		head = newNode;
    }
    //increase size count
    size++;
}

/**
 * Simple output of all bids in the list
 */
void LinkedList::PrintList() {
    // start at the head
	Node* currentNode = head;
    // while loop over each node looking for a match
    while (currentNode != nullptr) {
        //output current bidID, title, amount and fund
		std::cout << currentNode->bid.bidId << ": " << currentNode->bid.title << " | " << currentNode->bid.amount << " | " << currentNode->bid.fund << endl;
        //set current equal to next
		currentNode = currentNode->next;
    }
}

/**
 * Remove a specified bid
 *
 * @param bidId The bid id to remove from the list
 */
void LinkedList::Remove(string bidId) {
	// Handle case where list is empty
	if (head == nullptr) {
		return;
	}

    // special case if matching node is the head
	if (head->bid.bidId.compare(bidId) == 0) {
		// hold onto the head node temporarily
		Node* tempNode = head;
        // make head point to the next node in the list
		head = head->next;
		// free up memory held by tempNode
		delete tempNode;
        //decrease size count
        size--;
		// if list becomes empty, set tail to nullptr
		if (head == nullptr) {
			tail = nullptr;
		}
        //return
        return;
    }
    // start at the head
	Node* currentNode = head;
    // while loop over each node looking for a match
    while (currentNode->next != nullptr) {
        // if the next node bidID is equal to the current bidID
        if (currentNode->next->bid.bidId.compare(bidId) == 0) {
            // hold onto the next node temporarily
            Node* tempNode = currentNode->next;
            // make current node point beyond the next node
            currentNode->next = currentNode->next->next;
			// if the next node is the tail, update the tail
			if (tempNode == tail) {
				tail = currentNode;
			}
            // now free up memory held by temp
			delete tempNode;
            // decrease size count
            size--;
            //return
            return;
        }
        // current node is equal to next node
        currentNode = currentNode->next;
    }
}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid LinkedList::Search(string bidId) {
    // special case if matching bid is the head
	if (head->bid.bidId.compare(bidId) == 0) {
		return head->bid;
	}

    // start at the head of the list
	Node* currentNode = head;
    // keep searching until end reached with while loop (current != nullptr)
    while (currentNode != nullptr) {
        // if the current node matches, return current bid
        if (currentNode->bid.bidId.compare(bidId) == 0) {
            return currentNode->bid;
        }
        currentNode = currentNode->next;
    }
    //create new empty bid
    //return empty bid 
    return Bid();
}

/**
 * Returns the current size (number of elements) in the list
 */
int LinkedList::Size() {
    return size;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount
         << " | " << bid.fund << endl;
    return;
}

/**
 * Prompt user for bid information
 *
 * @return Bid struct containing the bid info
 */
Bid getBid() {
    Bid bid;

    cout << "Enter Id: ";
    cin.ignore();
    getline(cin, bid.bidId);

    cout << "Enter title: ";
    getline(cin, bid.title);

    cout << "Enter fund: ";
    cin >> bid.fund;

    cout << "Enter amount: ";
    cin.ignore();
    string strAmount;
    getline(cin, strAmount);
    bid.amount = strToDouble(strAmount, '$');

    return bid;
}

/**
 * Load a CSV file containing bids into a LinkedList
 *
 * @return a LinkedList containing all the bids read
 */
void loadBids(string csvPath, LinkedList *list) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser
    csv::Parser file = csv::Parser(csvPath);

    try {
        // loop to read rows of a CSV file
        for (int i = 0; i < file.rowCount(); i++) {

            // initialize a bid using data from current row (i)
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << bid.bidId << ": " << bid.title << " | " << bid.fund << " | " << bid.amount << endl;

            // add this bid to the end
            list->Append(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 *
 * @param arg[1] path to CSV file to load from (optional)
 * @param arg[2] the bid Id to use when searching the list (optional)
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98109";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales.csv";
        bidKey = "98109";
    }

    clock_t ticks;

    LinkedList bidList;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Enter a Bid" << endl;
        cout << "  2. Load Bids" << endl;
        cout << "  3. Display All Bids" << endl;
        cout << "  4. Find Bid" << endl;
        cout << "  5. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            bid = getBid();
            bidList.Append(bid);
            displayBid(bid);

            break;

        case 2:
            ticks = clock();

            loadBids(csvPath, &bidList);

            cout << bidList.Size() << " bids read" << endl;

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " milliseconds" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 3:
            bidList.PrintList();

            break;

        case 4:
            ticks = clock();

            bid = bidList.Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
            	cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 5:
            bidList.Remove(bidKey);

            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
