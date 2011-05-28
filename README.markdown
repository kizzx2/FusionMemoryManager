# FusionMemoryManager

## Motivation

### The Situation

You want to create a C run-time shared library (DLL or SO), but you want to write your core logic in C++. For example:

    const char * Concatenate(const char * a, const char * b)
    {
        std::string * result = new std::string(a);
        *result += " "
        *result += b;
        return result->c_str();
    }

The common idiom for a C library is that you will provide a `Free()` function:

    void Free(void * obj)
    {
        delete obj;
    }

### The Problem

Your caller is going to give you back a `const char *`, and you can't delete that. What you want is to delete the actual `std::string` that contains the string.

### The Solution -- FusionMemoryManager to the Rescue

    const char * Concatenate(const char * a, const char * b)
    {
        std::string * result = new std::string(a);
        *result += " "
        *result += b;

        // Assign the "heritage" 
        fusionManager.Mark(result->c_str(), result);

        return result->c_str();
    }

    void Free(void * obj)
    {
        // This will delete the containing std::string
        // Calling the destructor as appropriate
        fusionManager.Free(obj);
    }

## Techniques

- C++ template meta-progarmming
- [Boost.MPL](http://www.boost.org/doc/libs/release/libs/mpl/doc/index.html)
