// unit test file

/** Implement and test:
 * Given: a queue instace with a size N - constructor
 * When: we check if its empty
 * Then: it returns true
 */

/** Implement and test:
 * Given: a queue with several items with the same priority
 * When: we dequeue it till it's empty
 * Then: it returns the same items in the order of enqueuing
 */

/** Implement and test:
 * Given: a queue with several items with the different priorities
 * When: we dequeue it till it's empty
 * Then: it returns the same items but ordered by their priority (smaller priority is more urgent)
 */

/** Implement and test:
 * Given: a queue
 * When: we enqueue with more items than its capacity and dequeue it till its empty
 * Then: we cannot enqueue items more then its capacity and when we dequeue it we don't see any items we tried to enqueue after it became full
 */