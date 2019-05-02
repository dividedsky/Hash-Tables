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
  printf("in insert method\n");
  printf("insert key: %s\n", key);
  unsigned int h = hash(key, ht->capacity);
  printf("hashed index is %d\n", h);
  LinkedPair *pair = create_pair(key, value);
  if (ht->storage[h] == NULL) {
    // index is empty
    printf("storage is null\n");
    ht->storage[h] = pair;
  } else {
    // index is not empty, traverse list looking for key or NULL next
    LinkedPair *current = ht->storage[h];
    printf("current:\n");
    printf("\tkey: %s\n", current->key);
    printf("\tvalue: %s\n", current->value);
    printf("\tnext: %p\n", current->next);
    while (current) {
      if (strcmp(current->key, key) == 0) {
        current->value = strdup(value);
        destroy_pair(pair);
        break;
      }
      if (current->next == NULL) {
        // if we've reached the end of the list, add our new pair
        current->next = pair;
        break;
      } 
       else {
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
      found = 1;
    } else {
      previous = current;
      current = current->next;
    }
  }

  if (found) {
    printf("found\n");
    if (previous) {
      previous->next = current->next;
    }
    if (!previous) {
      ht->storage[h] = current->next;
    }
    /* LinkedPair *pair_to_delete = current; */
    destroy_pair(current); // do i need to make a copy?
  }
  /*
  while (current) {
    if (strcmp(current->key, key) == 0) {
      // match found, delete node
      if (previous) {
        previous->next = current->next;
      }
    //  if (current->next) {
   //     current->next = previous;
  //    }
      current->key = NULL;
      current->value = NULL;
 //     current->next = NULL;
//      destroy_pair(current);
    } else {
      previous = current; // I think this will cause problems?
      current = current->next;
    }
  }
*/
}

/*
  Fill this in.

  Should search the entire list of LinkedPairs for existing
  keys.

  Return NULL if the key is not found.
 */
char *hash_table_retrieve(HashTable *ht, char *key)
{
  printf("in retrieval, key is %s\n", key);
  unsigned int h = hash(key, ht->capacity);
  printf("hash is %d\n", h);
  LinkedPair *current;
  if (ht->storage[h] != NULL) {
    printf("not null\n");
    printf("key: %s, val: %s\n", ht->storage[h]->key, ht->storage[h]->value);
    current = ht->storage[h];
  } else {
    return NULL;
  }
  printf("pair initialized. key: %s, value: %s\n", current->key, current->value);
  while (current) {
    if (strcmp(current->key, key) == 0) {
      return current->value;
    } else {
      current = current->next;
      printf("moved to next node. key: %s, value: %s\n", current->key, current->value);
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
        /* free(previous); */
        destroy_pair(previous);
      }
      destroy_pair(current);
      ht->storage[i] = NULL; // i don't think this is necessary
    }
    /* free(ht->storage[i]); // wait, aren't we not supposed to do this? */
    /* destroy_pair(ht->storage[i]); */
    // if not, traverse list and free each pair

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
  // initialize new ht
  HashTable *new_ht = malloc(sizeof(HashTable));
  new_ht->capacity = ht->capacity * 2;
  new_ht->storage = calloc(new_ht->capacity, sizeof(LinkedPair));

  printf("new table initialized\n");
  // loop through every index in old table
  for (int i = 0; i < ht->capacity; i++) {
    // if it is not empty, traverse list
    LinkedPair *current = ht->storage[i];
    while (current) {
      printf("in while loop, current node at %p\n", current);
      // rehash every pair and store in new table
      hash_table_insert(new_ht, current->key, current->value);
      current = current->next;
    }
  }

  // destroy old table
  free(ht);
  return new_ht;
}


#ifndef TESTING
int main(void)
{
  /*
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
  */

    struct HashTable *ht = create_hash_table(8);
hash_table_insert(ht, "key-0", "val-0");
    hash_table_insert(ht, "key-1", "val-1");
    hash_table_insert(ht, "key-2", "val-2");
    hash_table_insert(ht, "key-3", "val-3");
    hash_table_insert(ht, "key-4", "val-4");
    hash_table_insert(ht, "key-5", "val-5");
    hash_table_insert(ht, "key-6", "val-6");
    hash_table_insert(ht, "key-7", "val-7");
    hash_table_insert(ht, "key-8", "val-8");
    hash_table_insert(ht, "key-9", "val-9");

    hash_table_insert(ht, "key-0", "new-val-0");
    hash_table_insert(ht, "key-1", "new-val-1");
    hash_table_insert(ht, "key-2", "new-val-2");
    hash_table_insert(ht, "key-3", "new-val-3");
    hash_table_insert(ht, "key-4", "new-val-4");
    hash_table_insert(ht, "key-5", "new-val-5");
    hash_table_insert(ht, "key-6", "new-val-6");
    hash_table_insert(ht, "key-7", "new-val-7");
    hash_table_insert(ht, "key-8", "new-val-8");
    hash_table_insert(ht, "key-9", "new-val-9");
    printf(hash_table_retrieve(ht, "key-0"));

  destroy_hash_table(ht);

  return 0;
}
#endif
