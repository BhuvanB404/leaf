#pragma once

class ArenaAllocator {
  public:
    inline   explicit ArenaAllocator(size_t bytes)
      :m_size(bytes)
  {
    m_buffer = malloc(m_size);
      m_offset = m_buffer;
  }

  template<typename T>
  inline T* alloc()
    {
      void* offset = m_offset;
      m_offset += sizeof(T);
      return static_cast<T*>(offset);
    }


private:
  size_t m_size;
  void* m_buffer;
  void* m_offset;

};
