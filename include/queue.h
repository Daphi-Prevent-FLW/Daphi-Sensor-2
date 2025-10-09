#pragma once


/** Queue 
 * implement the Queue class
 * 
 * Note that since it'll be used for several different tasks (messages, events, etc.) T will be replaced with the corresponding type
 * Also note, that in order to prevent dynamic allocation, the queue is set to a maximal size - capacity. 
*/
template<typename T>
class Queue {
public:
    Queue(uint8_t capacity);
    void enqueue(const T &item);
    T dequeue();
    bool isEmpty();

private:
    bool isFull();
    void sortByPriority();
};

