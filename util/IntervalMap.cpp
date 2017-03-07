#ifndef INTERVAL_MAP
#define INTERVAL_MAP

#include <map>
#include <limits>

template<class K, class V>
class IntervalMap {
    friend void IntervalMapTest();

private:
    std::map<K,V> m_map;

public:
    IntervalMap( V const& val) {
        m_map.insert(m_map.begin(),std::make_pair(std::numeric_limits<K>::lowest(),val));
    }

    void assign( K const& keyBegin, K const& keyEnd, const V& val ) {
        // empty interval
        if(!std::less<K>()(keyBegin, keyEnd)) return;

        V begin_regionV = this->operator[](keyBegin);
        V end_regionV = this->operator[](keyEnd);

        bool is_lower_start = true;
        if(!(begin_regionV == val))
        {
            m_map[keyBegin] = val;
            is_lower_start = false;
        }

        bool is_lower_end = false;
        if(!(end_regionV == val))
        {
            m_map[keyEnd] = end_regionV;
            is_lower_end = true;
        }

        auto start_itt = is_lower_start ? m_map.lower_bound(keyBegin) : m_map.upper_bound(keyBegin);
        auto end_itt = is_lower_end ? m_map.lower_bound(keyEnd) : m_map.upper_bound(keyEnd);
        m_map.erase(start_itt,end_itt);
    }

    V const& operator[]( K const& key ) const {
        return ( --m_map.upper_bound(key) )->second;
    }
};

#endif
