#ifndef DALE_STLUTILS
#define DALE_STLUTILS

namespace dale {
/*! STL

    STL utility methods.
*/
namespace STL {
/*! Delete each individual element in a container.
 *  @param container The container.
 */
template <typename T>
void deleteElements(T *container) {
    for (typename T::iterator it = container->begin(),
                              et = container->end();
         it != et; ++it) {
        delete (*it);
    }
}

/*! Delete each individual value element in a map.
 *  @param map The map.
 */
template <typename T>
void deleteMapElements(T *map) {
    for (typename T::iterator it = map->begin(), et = map->end();
         it != et; ++it) {
        delete it->second;
    }
}

/*! Delete each individual map value element in a map.
 *  @param map The map.
 */
template <typename T>
void deleteNestedMapElements(T *map) {
    for (typename T::iterator it = map->begin(), et = map->end();
         it != et; ++it) {
        deleteMapElements(&(it->second));
    }
}

/*! Check whether each element of container 1 isEqualTo the
 *  corresponding element from container 2.
 *  @param container1 The first container.
 *  @param container2 The second container.
 */
template <typename T>
bool isEqualTo(T *container1, T *container2) {
    if (container1->size() != container2->size()) {
        return false;
    }

    typename T::iterator iter1, iter2;

    iter1 = container1->begin();
    iter2 = container2->begin();

    while ((iter1 != container1->end()) &&
           (iter2 != container2->end())) {
        if (!(*iter1)->isEqualTo((*iter2))) {
            return false;
        }
        iter1++;
        iter2++;
    }

    return true;
}

/*! Check whether each element of iterator range 1 isEqualTo the
 *  corresponding element from iterator range 2.
 *  @param iter1_begin The beginning iterator for the first range.
 *  @param iter1_end The ending iterator for the first range.
 *  @param iter2_begin The beginning iterator for the second range.
 *  @param iter2_end The ending iterator for the second range.
 */
template <typename T>
bool isEqualToIter(T iter1_begin, T iter1_end, T iter2_begin,
                   T iter2_end) {
    while ((iter1_begin != iter1_end) && (iter2_begin != iter2_end)) {
        if (!(*iter1_begin)->isEqualTo((*iter2_begin))) {
            return false;
        }
        ++iter1_begin;
        ++iter2_begin;
    }

    if ((iter1_begin == iter1_end) && (iter2_begin == iter2_end)) {
        return true;
    }

    return false;
}

/*! Push two elements back onto a container.
 *  @param container The container.
 *  @param val1 The first value.
 *  @param val2 The second value.
 */
template <typename T>
void push_back2(T *container, const typename T::value_type &val1,
                const typename T::value_type &val2) {
    container->push_back(val1);
    container->push_back(val2);
}
}
}

#endif
