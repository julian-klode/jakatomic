/*
 * Copyright (C) 2017 Julian Andres Klode <jak@jak-linux.org>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef JAK_ATOM_LIST
#define JAK_ATOM_LIST

/**
 * A simple singly linked jak_list
 */
struct jak_list {
    struct jak_list *next;
};

/**
 * Atomic list is a data structure similar to the list in that it can
 * be pushed to. It can only be read completely, though, by taking control
 * of it.
 *
 * The primary use case for this data structure is multiple writers sending
 * data to a single reader. The single reader can then take the data with
 * a single atomic compare and exchange, and use it in a non-atomic way.
 */
struct jak_atom_list {
    struct jak_list *jak_list;
};

/**
 * Push a new item to the atomic list.
 *
 * The item will be cast into a jak_list object.
 */
static inline void jak_atom_list_push(struct jak_atom_list *list,
                                      struct jak_list *new_element)
{
    do {
        new_element->next = __atomic_load_n(&list->jak_list, __ATOMIC_SEQ_CST);
    } while (!__atomic_compare_exchange_n(&list->jak_list,
                                          &new_element->next,
                                          new_element,
                                          0,
                                          __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
}

/**
 * Take control over the list in an atomic way.
 *
 * This returns the list stored in the atomic list wrapper, and empties the
 * atomic list wrapper in a single operation.
 */
static inline struct jak_list *jak_atom_list_take(struct jak_atom_list *list)
{
    return __atomic_exchange_n(&list->jak_list, 0, __ATOMIC_SEQ_CST);
}
#endif
