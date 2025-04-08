#pragma once

#include "sparta/utils/SpartaException.hpp"

#include <iostream>
#include <fstream>

namespace atlas
{

    class MessageLogger
    {
      public:
        MessageLogger() = default;

        // Prevent copying
        MessageLogger(const MessageLogger &) = delete;
        MessageLogger & operator=(const MessageLogger &) = delete;

        // Prevent moving
        MessageLogger(MessageLogger &&) = delete;
        MessageLogger & operator=(MessageLogger &&) = delete;

        void open(const std::string & filename)
        {
            if (fout_.is_open())
            {
                throw sparta::SpartaException("MessageLogger is already open");
            }

            fout_.open(filename);

            if (!fout_.is_open())
            {
                throw sparta::SpartaException("Failed to open file: " + filename);
            }
        }

        bool enabled() const { return fout_.is_open(); }

        template <typename T> MessageLogger & operator<<(const T & message)
        {
            return write(message);
        }

        template <typename T> MessageLogger & write(const T & message)
        {
            fout_ << message;
            return *this;
        }

        template <typename T> MessageLogger & tryWrite(const T & message)
        {
            if (fout_.is_open())
            {
                fout_ << message;
            }
            return *this;
        }

      private:
        std::ofstream fout_;
    };

} // namespace atlas
