#ifndef DALE_STLUTILS
#define DALE_STLUTILS

namespace dale
{
namespace stl
{
template <typename T>
void deleteElements(T *container)
{
    for (typename T::iterator it = container->begin(),
            et = container->end();
            it != et; ++it) {
        delete (*it);
    }
}

template <typename T>
void deleteMapElements(T *container)
{
    for (typename T::iterator it = container->begin(),
            et = container->end();
            it != et; ++it) {
        delete it->second;
    }
}

template <typename T>
void deleteNestedMapElements(T *container)
{
    for (typename T::iterator it = container->begin(),
            et = container->end();
            it != et; ++it) {
        deleteMapElements(&(it->second));
    }
}

template <typename T>
bool isEqualTo(T *container1, T *container2)
{
    if (container1->size() != container2->size()) {
        return false;
    }

    typename T::iterator iter1, iter2;

    iter1 = container1->begin();
    iter2 = container2->begin();

    while ((iter1 != container1->end())
            && (iter2 != container2->end())) {
        if (!(*iter1)->isEqualTo((*iter2))) {
            return false;
        }
        iter1++;
        iter2++;
    }

    return true;
}

template <typename T>
bool isEqualToIter(T iter1_start, T iter1_end,
                   T iter2_start, T iter2_end)
{
    while ((iter1_start != iter1_end) && (iter2_start != iter2_end)) {
        if (!(*iter1_start)->isEqualTo((*iter2_start))) {
            return false;
        }
        ++iter1_start;
        ++iter2_start;
    }

    if ((iter1_start == iter1_end) && (iter2_start == iter2_end)) {
        return true;
    }

    return false;
}

template <typename T>
void push_back2(T *container, const typename T::value_type &val1,
                const typename T::value_type &val2)
{
    container->push_back(val1);
    container->push_back(val2);
}

template <typename T>
void push_back3(T *container, const typename T::value_type &val1,
                const typename T::value_type &val2,
                const typename T::value_type &val3)
{
    container->push_back(val1);
    container->push_back(val2);
    container->push_back(val3);
}
}
}

#endif
