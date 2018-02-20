#ifndef __coffee_logger_CircularTraceWriter_hpp
#define __coffee_logger_CircularTraceWriter_hpp

#include <coffee/logger/Writer.hpp>

namespace coffee {

namespace logger {

class CircularTraceWriter : public Writer {
public:
   static const int MinimalSize = 256;
   static const int NullStream;
   static const int CheckSizePeriod;

   CircularTraceWriter (const std::string& path, const size_t maxSize);
   virtual ~CircularTraceWriter () { closeStream (); }

   int getStream () const noexcept { return m_stream; }
   size_t getLineNo () const noexcept { return m_lineno; }
   size_t getKbytesMaxSize () const noexcept { return m_maxSize; }

protected:
   virtual void apply (const Level::_v level, const std::string& line) noexcept;
   virtual bool wantsToProcess (const Level::_v level) const noexcept;

private:
   std::string m_path;
   size_t m_maxSize;
   int m_stream;
   size_t m_lineno;

   void do_initialize () throw (adt::RuntimeException);

   void openStream () throw (adt::RuntimeException);
   bool oversizedStream () throw (adt::RuntimeException);
   void closeStream () noexcept;
   void renameFile () throw (adt::RuntimeException);
};

}
}

#endif
