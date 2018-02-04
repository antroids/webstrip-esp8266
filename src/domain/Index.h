#ifndef DOMAIN_INDEX_H
#define DOMAIN_INDEX_H

#include <Arduino.h>

typedef uint16_t index_id_t;

template <class T_ENTITY_TYPE, index_id_t V_INDEX_SIZE> class Index {
public:
  static const index_id_t INVALID = 65535;

  static index_id_t addToIndex(T_ENTITY_TYPE *entity) {
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

  Index(T_ENTITY_TYPE *entity) : id(addToIndex(entity)){};
  index_id_t getId() { return id; };

private:
  static T_ENTITY_TYPE *storage[V_INDEX_SIZE];
  static index_id_t count;
  const index_id_t id;
};

template <class T_ENTITY_TYPE, index_id_t V_INDEX_SIZE> index_id_t Index<T_ENTITY_TYPE, V_INDEX_SIZE>::count = 0;
template <class T_ENTITY_TYPE, index_id_t V_INDEX_SIZE> T_ENTITY_TYPE *Index<T_ENTITY_TYPE, V_INDEX_SIZE>::storage[V_INDEX_SIZE];

#endif
