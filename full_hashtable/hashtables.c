#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Hash table key/value pair with linked list pointer.

  Note that an instance of `LinkedPair` is also a node in a linked list.
  More specifically, the `next` field is a pointer pointing to the the 
  next `LinkedPair` in the list of `LinkedPair` nodes. 
 */
typedef struct LinkedPair {
  char *key;
  char *value;
  struct LinkedPair *next;
} LinkedPair;

/*
  Hash table with linked pairs.
 */
typedef struct HashTable {
  int capacity;
  LinkedPair **storage;
} HashTable;

/*
  Create a key/value linked pair to be stored in the hash table.
 */
LinkedPair *create_pair(char *key, char *value)
{
  LinkedPair *pair = malloc(sizeof(LinkedPair));
  pair->key = strdup(key);
  pair->value = strdup(value);
  pair->next = NULL;

  return pair;
}

/*
  Use this function to safely destroy a hashtable pair.
 */
void destroy_pair(LinkedPair *pair)
{
  if (pair != NULL) {
    free(pair->key);
    free(pair->value);
    free(pair);
  }
}

/*
  djb2 hash function

  Do not modify this!
 */
unsigned int hash(char *str, int max)
{
  unsigned long hash = 5381;
  int c;
  unsigned char * u_str = (unsigned char *)str;

  while ((c = *u_str++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash % max;
}

/*
  Fill this in.

  All values in storage should be initialized to NULL
 */
HashTable *create_hash_table(int capacity)
{
  HashTable *ht = malloc(sizeof(HashTable));
  ht->capacity = capacity;
  ht->storage = calloc(capacity, sizeof(LinkedPair *));
  return ht;
}

/*
  Fill this in.

  Inserting values to the same index with different keys should be
  added to the corresponding LinkedPair list.

  Inserting values to the same index with existing keys can overwrite
  the value in th existing LinkedPair list.
 */
void hash_table_insert(HashTable *ht, char *key, char *value)
{
  unsigned int h = hash(key, ht->capacity);
  if (ht->storage[h] == NULL) {
    // index is empty, add pair here
    ht->storage[h] = create_pair(key, value);
  } else {
    // index is not empty, traverse list looking for key or NULL next
    LinkedPair *current = ht->storage[h];
    while (current) {
      if (strcmp(current->key, key) == 0) {
        /* if key matches, overwrite value. previously I just replaced the current value, which
        caused a memory leak. is there another way to do this? */
        free(current->value);
        current->value = strdup(value);
        break;
      }
      if (current->next == NULL) {
        // if we've reached the end of the list, add our new pair
        current->next = create_pair(key, value);
        break;
      } 
       else {
         // if a match is not found, move on to the next node
        current = current->next;
      }
    }
  }
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys and remove matching LinkedPairs safely.

  Don't forget to free any malloc'ed memory!
 */
void hash_table_remove(HashTable *ht, char *key)
{
  unsigned int h = hash(key, ht->capacity);
  LinkedPair *current = ht->storage[h];
  LinkedPair *previous = NULL;
  int found = 0;
  while (current && !found) {
    if (strcmp(current->key, key) == 0) {
      // we've found a match, set found to exit loop
      found = 1;
    } else {
      // otherwise, keep moving down the line
      previous = current;
      current = current->next;
    }
  }

  if (found) {
    if (previous) {
      // if we're not at the head, update the previous node's next pointer
      previous->next = current->next;
    }
    if (!previous) {
      // if we're at the head, add the node here
      ht->storage[h] = current->next;
    }
    destroy_pair(previous);
    destroy_pair(current);
  }
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys.

  Return NULL if the key is not found.
 */
char *hash_table_retrieve(HashTable *ht, char *key)
{
  unsigned int h = hash(key, ht->capacity);
  LinkedPair *current;
  // if the node is null, we'll drop to the end of the function and return null
  if (ht->storage[h] != NULL) {
    current = ht->storage[h];
    while (current) {
      // traverse the list looking for the value
      if (strcmp(current->key, key) == 0) {
        return current->value;
      } else {
        // if this node is not a match, move on to the next one
        current = current->next;
      }
    }
  }
  return NULL;
}

/*
  Fill this in.

  Don't forget to free any malloc'ed memory!
 */
void destroy_hash_table(HashTable *ht)
{
  // loop through all indexes
  for (int i = 0; i < ht->capacity; i++) {
    // if the index is not empty, traverse the list and free everything
    if (ht->storage[i]) {
      LinkedPair *current = ht->storage[i];
      LinkedPair *previous;
      while (current->next != NULL) {
        previous = current;
        current = current->next;
        destroy_pair(previous);
      }
      destroy_pair(current);
      ht->storage[i] = NULL; // i don't remember why this is necessary...
    }
    destroy_pair(ht->storage[i]);
  }

  // free storage;
  free(ht->storage);

  // free ht
  free(ht);
}

/*
  Fill this in.

  Should create a new hash table with double the capacity
  of the original and copy all elements into the new hash table.

  Don't forget to free any malloc'ed memory!
 */
HashTable *hash_table_resize(HashTable *ht)
{
  // initialize new ht with double capacity
  HashTable *new_ht = malloc(sizeof(HashTable));
  new_ht->capacity = ht->capacity * 2;
  new_ht->storage = calloc(new_ht->capacity, sizeof(LinkedPair));

  // loop through every index in old table
  for (int i = 0; i < ht->capacity; i++) {
    // if it is not empty, traverse list
    LinkedPair *current = ht->storage[i];
    while (current) {
      // rehash every pair and store in new table
      hash_table_insert(new_ht, current->key, current->value);
      current = current->next;
    }
  }

  // destroy old table
  destroy_hash_table(ht);
  return new_ht;
}


#ifndef TESTING
int main(void)
{
  struct HashTable *ht = create_hash_table(2);

  hash_table_insert(ht, "line_1", "Tiny hash table\n");
  hash_table_insert(ht, "line_2", "Filled beyond capacity\n");
  hash_table_insert(ht, "line_3", "Linked list saves the day!\n");
  hash_table_insert(ht, "line_3", "new: Linked list saves the day!\n");

  printf("%s", hash_table_retrieve(ht, "line_1"));
  printf("%s", hash_table_retrieve(ht, "line_2"));
  printf("%s", hash_table_retrieve(ht, "line_3"));

  int old_capacity = ht->capacity;
  ht = hash_table_resize(ht);
  int new_capacity = ht->capacity;

  printf("\nResizing hash table from %d to %d.\n", old_capacity, new_capacity);

  destroy_hash_table(ht);

  return 0;
}
#endif
