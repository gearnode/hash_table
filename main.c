/*
Copyright 2019 Bryan Frimin.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define HT_PRIME_1 127
#define HT_PRIME_2 181

typedef struct {
  char *key;
  char *value;
} ht_item;

typedef struct {
  int size;
  int count;
  ht_item **items;
} ht_hash_table;

static ht_item HT_DELETED_ITEM = {NULL, NULL};

static ht_item *ht_new_item(const char *k, const char *v) {
  ht_item *i = malloc(sizeof(ht_item));

  i->key = strdup(k);
  i->value = strdup(v);
  return i;
}

ht_hash_table *ht_new() {
  ht_hash_table *ht = malloc(sizeof(ht_hash_table));

  ht->size = 53;
  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(ht_item*));

  return ht;
}

static void ht_del_item(ht_item *i) {
  free(i->key);
  free(i->value);
  free(i);
}

void ht_del_hash_table(ht_hash_table *ht) {
  for (int i = 0; i < ht->size; i++) {
    ht_item *item = ht->items[i];
    if (item != NULL && item != &HT_DELETED_ITEM) {
      ht_del_item(item);
    }
  }
  free(ht->items);
  free(ht);
}

static int ht_hash(const char *s, const int a, const int m) {
  long hash = 0;
  const int len_s = strlen(s);

  for (int i = 0; i < len_s; i++) {
    hash += (long)pow(a, len_s - (i+1)) * s[i];
    hash = hash % m;
  }

  return (int)hash;
}

static int ht_get_hash(const char *s, const int num_buckets, const int attempt) {
  const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
  const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);

  return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

void ht_insert(ht_hash_table *ht, const char *key, const char *value) {
  ht_item *item = ht_new_item(key, value);
  int index = ht_get_hash(item->key, ht->size, 0);
  ht_item *cur_item = ht->items[index];

  for (int i = 1; cur_item != NULL; i++) {
    if (cur_item != &HT_DELETED_ITEM) {
      if (strcmp(cur_item->key, key) == 0) {
        ht_del_item(cur_item);
        ht->items[index] = item;
        return;
      }
    }
    index = ht_get_hash(item->key, ht->size, i);
    cur_item = ht->items[index];
  }

  ht->items[index] = item;
  ht->count++;
}

char *ht_search(ht_hash_table *ht, const char *key) {
  int index = ht_get_hash(key, ht->size, 0);
  ht_item *item = ht->items[index];

  for (int i = 1; item != NULL; i++) {
    if (item != &HT_DELETED_ITEM) {
      if (strcmp(item->key, key) == 0) {
        return item->value;
      }
    }
    index = ht_get_hash(key, ht->size, i);
    item = ht->items[index];
  }

  return NULL;
}


void ht_delete(ht_hash_table *ht, const char *key) {
  int index = ht_get_hash(key, ht->size, 0);
  ht_item *item = ht->items[index];

  for (int i = 1; item != NULL; i++) {
    if (item != &HT_DELETED_ITEM) {
      if (strcmp(item->key, key) == 0) {
        ht_del_item(item);
        ht->items[index] = &HT_DELETED_ITEM;
      }
    }
    index = ht_get_hash(key, ht->size, i);
    item = ht->items[index];
  }

  ht->count--;
}

int main() {
   ht_hash_table *person = ht_new();

   ht_insert(person, "first_name", "Bryan");
   puts(ht_search(person, "first_name"));
   ht_insert(person, "first_name", "Gearnode");
   puts(ht_search(person, "first_name"));
   ht_insert(person, "first_name", "foobar");
   puts(ht_search(person, "first_name"));
   ht_delete(person, "first_name");
   puts(ht_search(person, "first_name"));

   ht_insert(person, "first_name", "yo");
   puts(ht_search(person, "first_name"));

   ht_del_hash_table(person);
}
