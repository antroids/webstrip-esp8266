#ifndef DOMAIN_STATIC_INDEX_H
#define DOMAIN_STATIC_INDEX_H

#include "../Types.h"
#include <Arduino.h>

template <class T_ENTITY_TYPE, index_id_t V_INDEX_SIZE> class StaticIndex {
public:
  static const index_id_t INVALID = 65535;

  static index_id_t addToIndex(T_ENTITY_TYPE *entity, index_id_t index) {
    if (index >= V_INDEX_SIZE) {
      return INVALID;
    }
    storage[index] = entity;
    return index;
  };
  static index_id_t pushToIndex(T_ENTITY_TYPE *entity) {
    if (count == V_INDEX_SIZE) {
      return INVALID;
    }
    storage[count] = entity;
    return count++;
  };
  static T_ENTITY_TYPE *getFromIndex(index_id_t id) {
    if (id > count - 1) {
      return NULL;
    }
    return storage[id];
  };
  static index_id_t getCount() { return count; };

  index_id_t getId() { return id; };
  index_id_t addToIndex(index_id_t index) { return addToIndex(this, index); }

  StaticIndex(T_ENTITY_TYPE *entity) : id(pushToIndex(entity)) {}
  StaticIndex(T_ENTITY_TYPE *entity, index_id_t id) : id(addToIndex(entity, id)) {}

private:
  static T_ENTITY_TYPE *storage[V_INDEX_SIZE];
  static index_id_t count;
  const index_id_t id;
};

template <class T_ENTITY_TYPE, index_id_t V_INDEX_SIZE> index_id_t StaticIndex<T_ENTITY_TYPE, V_INDEX_SIZE>::count = 0;
template <class T_ENTITY_TYPE, index_id_t V_INDEX_SIZE> T_ENTITY_TYPE *StaticIndex<T_ENTITY_TYPE, V_INDEX_SIZE>::storage[V_INDEX_SIZE];

#endif
