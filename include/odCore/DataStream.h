/*
 * DataStream.h
 *
 *  Created on: 30 Dec 2017
 *      Author: zal
 */

#ifndef INCLUDE_DATASTREAM_H_
#define INCLUDE_DATASTREAM_H_

#include <istream>

#include <odCore/Logger.h>

namespace od
{

    /**
     * @brief Lightweight wrapper for a std::stream for reading binary data.
     */
	class DataReader
	{
	public:

	    class Ignore
	    {
	    public:
	        Ignore(size_t n);
	        inline size_t getCount() const { return mCountByte; }
	    private:
	        size_t mCountByte;
	    };

	    template <typename T>
	    class Expect
		{
		public:

	    	Expect(T value) : mValue(value) {}
	    	const T getExpectedValue() const { return mValue; }

		private:
	    	T mValue;
		};

	    DataReader();
		DataReader(std::istream &stream);
		DataReader(const DataReader &dr);

		DataReader &operator=(const DataReader &dr);

		inline void setStream(std::istream &stream) { mStream = &stream; }
		std::istream &getStream();

		template <typename T>
		DataReader &operator>>(T &s)
        {
            readTyped(s);
            return *this;
        }

        // const overload, basically only useful for Expect<T> and Ignore statements
        template <typename T>
        DataReader &operator>>(const T &s)
        {
            readTyped(s);
            return *this;
        }

        // deserializers can specialize this template:
        template <typename T>
        void readTyped(T &s);

		void readTyped(const Ignore &s);

		template <typename T>
		void readTyped(const Expect<T> &s)
        {
            _checkStream();

    		T value;
    		(*this) >> value;

    		if(value != s.getExpectedValue())
    		{
    			Logger::warn() << "Data error: expected value " << s.getExpectedValue() << ", found value " << value;
    		}
    	}

		void read(char *data, size_t size);

		void ignore(size_t n);
		void seek(size_t offset);
		size_t tell();


	private:

        void _checkStream();

		std::istream *mStream;
	};


    class DataWriter
    {
    public:

        DataWriter(std::ostream &out);

        template <typename T>
        DataWriter &operator<<(const T &v)
        {
            writeTyped(v);
            return *this;
        }

        template <typename T>
        void writeTyped(const T &v);

        void write(const char *data, size_t size);


    private:

        std::ostream *mStream;
    };


	class MemoryInputBuffer final : public std::streambuf
	{
	public:

        MemoryInputBuffer(const char *data, size_t size);

		virtual std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) override;
		virtual std::streampos seekpos(std::streampos sp, std::ios_base::openmode which) override;

	};


    class MemoryOutputBuffer final : public std::streambuf
    {
    public:

        MemoryOutputBuffer(char *data, size_t size);

        //virtual std::streambuf::int_type overflow(std::streambuf::int_type ch) override;
        virtual std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) override;
		virtual std::streampos seekpos(std::streampos sp, std::ios_base::openmode which) override;

    };

}

#endif /* INCLUDE_DATASTREAM_H_ */
