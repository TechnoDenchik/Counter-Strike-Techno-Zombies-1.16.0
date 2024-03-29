#pragma once
#include <utility>
#include <list>
#include <map>
#include <optional>

template<class Key, class Value>
class lru_cache
{
public:
    typedef Key key_type;
    typedef Value value_type;
    typedef std::list<key_type> list_type;
    typedef std::map<
    key_type,
    std::pair<value_type, typename list_type::iterator>
    > map_type;

    lru_cache(size_t capacity)
            : m_capacity(capacity)
    {
    }

    ~lru_cache()
    {
    }

    size_t size() const
    {
        return m_map.size();
    }

    size_t capacity() const
    {
        return m_capacity;
    }

    bool empty() const
    {
        return m_map.empty();
    }

    bool contains(const key_type &key) const
    {
        return m_map.find(key) != m_map.end();
    }

    bool contains_and_make_first(const key_type& key)
    {
        auto i = m_map.find(key);
        if (i == m_map.end())
            return false;
        auto i2 = i->second.second;
        m_list.erase(i2);
        m_list.push_front(key);
        auto i3 = m_list.begin();
        i->second.second = i3;
        return true;
    }

    void insert(const key_type &key, const value_type &value)
    {
        auto i = m_map.find(key);
        if(i == m_map.end()){
            // insert item into the cache, but first check if it is full
            while(m_capacity && size() >= m_capacity){
                // cache is full, evict the least recently used item
                evict();
            }

            // insert the new item
            m_list.push_front(key);
            m_map[key] = std::make_pair(value, m_list.begin());
        }
    }

    void remove(const key_type &key)
    {
        auto i = m_map.find(key);
        auto i2 = i->second.second;
        m_list.erase(i2);
        m_map.erase(i);
    }

    std::optional<value_type> get(const key_type &key)
    {
        // lookup value in the cache
        auto i = m_map.find(key);
        if(i == m_map.end()){
            // value not in cache
            return std::nullopt;
        }

        // return the value, but first update its place in the most
        // recently used list
        auto j = i->second.second;
        if(j != m_list.begin()){
            // move item to the front of the most recently used list
            m_list.erase(j);
            m_list.push_front(key);

            // update iterator in map
            j = m_list.begin();
            const value_type &value = i->second.first;
            m_map[key] = std::make_pair(value, j);

            // return the value
            return value;
        }
        else {
            // the item is already at the front of the most recently
            // used list so just return it
            return i->second.first;
        }
    }

    void clear()
    {
        m_map.clear();
        m_list.clear();
    }

    void set_capacity(size_t value)
    {
        m_capacity = value;
    }

    void evict()
    {
        // evict item from the end of most recently used list
        typename list_type::iterator i = --m_list.end();
        m_map.erase(*i);
        m_list.erase(i);
    }

private:
    map_type m_map;
    list_type m_list;
    size_t m_capacity;
};