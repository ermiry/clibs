#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../../include/collections/htab.h"

static size_t htab_generic_hash (const void *key, size_t key_size, size_t table_size) {

	size_t i;
	size_t sum = 0;
	unsigned char *k = (unsigned char *) key;
	for (i = 0; i < key_size; ++i)
		sum = (sum + (int)k[i]) % table_size;
	
	return sum;
}

static int htab_generic_compare (const void *k1, size_t s1, const void *k2, size_t s2) {

	if (!k1 || !s1 || !k2 || !s2) return -1;

	if (s1 != s2) return -1;

	return memcmp (k1, k2, s1);
}

static int htab_generic_copy (void **dst, const void *src, size_t sz) {

	if (!dst || !src || !sz) return -1;

	if (!*dst) *dst = malloc (sz);

	if (!*dst) return -1;

	memcpy (*dst, src, sz);

	return 0;
}

#pragma region internal

static HtabNode *htab_node_new (void) {

	HtabNode *node = (HtabNode *) malloc (sizeof (HtabNode));
	if (node) {
		node->key = NULL;
		node->val = NULL;
		node->next = NULL;
		node->key_size = 0;
		node->val_size = 0;
	}

	return node;

}

static HtabNode *htab_node_create (const void *key, size_t key_size, void *val, size_t val_size) {

	HtabNode *node = htab_node_new ();
	if (node) {
		// TODO: option for custom key copy
		node->key_size = key_size;
		node->key = malloc (node->key_size);
		htab_generic_copy (&node->key, key, node->key_size);

		node->val = val;
		node->val_size = val_size;
	}

	return node;

}

static void htab_node_delete (HtabNode *node, void (*delete_data)(void *data)) {

	if (node) {
		if (node->val) {
			if (delete_data) delete_data (node->val);
		}

		if (node->key) free (node->key);

		free (node);
	}

}

static HtabBucket *htab_bucket_new (void) {

	HtabBucket *bucket = (HtabBucket *) malloc (sizeof (HtabBucket));
	if (bucket) {
		bucket->start = NULL;
		bucket->count = 0;
	}

	return bucket;

}

static void htab_bucket_delete (HtabBucket *bucket, void (*delete_data)(void *data)) {

	if (bucket) {
		while (bucket->count) {
			HtabNode *node = bucket->start;
			bucket->start = bucket->start->next;

			htab_node_delete (node, delete_data);

			bucket->count--;
		}

		free (bucket);
	}

}

static void htab_delete (Htab *htab) {

	if (htab) {
		if (htab->table) free (htab->table);
		free (htab);
	}

}

static Htab *htab_new (void) {

	Htab *htab = (Htab *) malloc (sizeof (Htab));
	if (htab) {
		htab->table = NULL;
		
		htab->size = 0;
		htab->count = 0;

		htab->hash = NULL;
		htab->compare = NULL;

		htab->delete_data = NULL;
	}

	return htab;

}

#pragma endregion

// creates a new htab
// size - how many buckets do you want - more buckets = less collisions
// hash - custom method to hash the key for insertion
// compare - custom method to compare keys
// delete_data - custom method to delete your data, NULL for no delete when htab gets destroyed
Htab *htab_create (size_t size,
	size_t (*hash)(const void *key, size_t key_size, size_t table_size),
	int (*compare)(const void *k1, size_t s1, const void *k2, size_t s2),
	void (*delete_data)(void *data)) {

	Htab *htab = htab_new ();
	if (htab) {
		if (size > 0) {
			htab->size = size;
			
			htab->table = (HtabBucket **) calloc (htab->size, sizeof (HtabBucket));
			if (htab->table) {
				for (size_t i = 0; i < htab->size; i++)
					htab->table[i] = htab_bucket_new ();

				htab->hash = hash ? hash : htab_generic_hash;
				htab->compare = compare ? compare : htab_generic_compare;

				htab->delete_data = delete_data;
			}
		}
	}

	return htab;

}

// returns true if there is at least 1 data associated with the key
// returns false if their is none
bool htab_contains_key (Htab *ht, const void *key, size_t key_size) {

	bool retval = false;

	if (ht && key && key_size) {
		size_t index = ht->hash (key, key_size, ht->size);
		HtabNode *node = ht->table[index]->start;
		while (node && node->key && node->val) {
			if (node->key_size == key_size) {
				if (!ht->compare (key, key_size, node->key, node->key_size)) {
					retval = true;
					break;
				}

				else node = node->next;
			}

			else node = node->next;
		}
	}

	return retval;

}

// inserts a new value to the htab associated with its key
// returns 0 on success, 1 on error
int htab_insert (Htab *ht, const void *key, size_t key_size, void *val, size_t val_size) {

	int retval = 1;

	if (ht && ht->hash && ht->compare && key && key_size && val && val_size) {
		size_t index = ht->hash (key, key_size, ht->size);
		// printf ("size: %ld\n", ht->size);
		// printf ("index: %ld\n", index);

		HtabBucket *bucket = ht->table[index];
		HtabNode *node = bucket->start;
		if (node) {
			while (node->next && ht->compare (key, key_size, node->key, node->key_size))
				node = node->next;

			if (ht->compare (key, key_size, node->key, node->key_size)) {
				node->next = htab_node_create (key, key_size, val, val_size);
				if (node->next) {
					node = node->next;

					bucket->count += 1;
					ht->count += 1;

					retval = 0;
				}
			}
		}

		else {
			node = htab_node_create (key, key_size, val, val_size);
			if (node) {
				bucket->start = node;

				bucket->count += 1;
				ht->count += 1;

				retval = 0;
			}
		}
	}

	return retval;

}

// returns a ptr to the data associated with the key
// returns NULL if no data was found
void *htab_get (Htab *ht, const void *key, size_t key_size) {

	void *retval = NULL;

	if (ht) {
		size_t index = ht->hash (key, key_size, ht->size);
		HtabNode *node = ht->table[index]->start;  
		while (node && node->key && node->val) {
			if (node->key_size == key_size) {
				if (!ht->compare (key, key_size, node->key, node->key_size)) {
					retval = node->val;
					break;
				}

				else {
					node = node->next;
				} 
			}

			else {
				node = node->next;
			}
		}
	}

	return retval;

}

// removes the data associated with the key from the htab
// returns NULL if no data was found with the provided key
void *htab_remove (Htab *ht, const void *key, size_t key_size) {

	void *retval = NULL;

	if (ht && key && ht->hash && ht->compare) {
		size_t index = ht->hash (key, key_size, ht->size);
		HtabBucket *bucket = ht->table[index];
		HtabNode *node = bucket->start;
		HtabNode *prev = NULL;
		while (node) {
			if (!ht->compare (key, key_size, node->key, node->key_size)) {
				if (!prev) ht->table[index]->start = ht->table[index]->start->next;
				else prev->next = node->next;

				retval = node->val;

				htab_node_delete (node, ht->delete_data);

				bucket->count--;
				ht->count--;

				break;
			}

			prev = node;
			node = node->next;
		}
	}

	return retval;

}

void htab_destroy (Htab *ht) {

	if (ht) {
		if (ht->table) {
			for (size_t i = 0; i < ht->size; i++) {
				if (ht->table[i]) {
					htab_bucket_delete (ht->table[i], ht->delete_data);
				}
			}
		}
		
		htab_delete (ht);
	}

}

static void htab_node_print (HtabNode *node) {

	if (node) {
		int *int_key = (int *) node->key;
		int *int_value = (int *) node->val;
		printf ("\tKey %d - Value: %d\n", *int_key, *int_value);
	}

}

static void htab_bucket_print (HtabBucket *bucket, size_t idx) {

	if (bucket) {
		printf ("Bucket <%ld> count: %ld\n", idx, bucket->count);
		HtabNode *n = bucket->start;
		while (n) {
			htab_node_print (n);
			
			n = n->next;
		}
	}

}

void htab_print (Htab *htab) {

	if (htab) {
		printf ("\n\n");
		printf ("Htab's size: %ld\n", htab->size);
		printf ("Htab's count: %ld\n", htab->count);

		for (size_t idx = 0; idx < htab->size; idx++) {
			htab_bucket_print (htab->table[idx], idx);
		}

		printf ("\n\n");
	}

}