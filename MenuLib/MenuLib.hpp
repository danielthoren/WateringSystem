

// class IStringWrapper
// {
// public:
//   IStringWrapper(uint16_t size)
//     : m_size{size}
//   {}

//   virtual ~IStringWrapper() = default;

//   uint16_t size() const
//   {
//     return m_size;
//   }

//   char &operator[](uint16_t index) = 0;
//   const char &operator[](uint16_t index) const = 0;

//   // Iterators
//   virtual char *begin() = 0;
//   virtual const char *begin() const = 0;
//   virtual char *end() = 0;
//   virtual const char *end() const = 0;

//   // Comparisons
//   virtual bool operator==(const IStringWrapper &rhs) const = 0;

//   bool operator!=(const IStringWrapper &rhs) const
//   {
//     return !(*this == rhs);
//   }

// private:
//   uint16_t m_size;
// };

// class RamStringWrapper : public IStringWrapper
// {
// public:
//   RamStringWrapper(char* pStr, uint16_t size)
//     : IStringWrapper{size},
//       m_pStr{pStr}
//   {}

// private:
//   char* m_pStr{nullptr};
// };
