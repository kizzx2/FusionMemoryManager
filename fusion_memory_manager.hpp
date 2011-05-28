// FusionMemoryManager
//
// This code is under the "BSD License"
//
// Copyright (c) 2011, Chris Yuen <chris@kizzx2.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// * Neither the name of the Chris Yuen nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL CHRIS YUEN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef fusion_memory_manager_hpp__
#define fusion_memory_manager_hpp__

#include <boost/type_traits.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/assert.hpp>

#include <map>
#include <cassert>

namespace FusionMemoryManager
{
    namespace Details
    {
        struct FusionMemoryManagerEntry
        {
            FusionMemoryManagerEntry(void * owner, size_t ownerTypeId) :
                owner(owner), ownerTypeId(ownerTypeId)
            {
            }

            void * owner;
            size_t ownerTypeId;
        };

        template <class Begin, class End>
        struct DeleteAsTypeN
        {
            static void Delete(size_t n, void * obj)
            {
                if(Begin::pos::value == n)
                    delete reinterpret_cast<Begin::type *>(obj);
                else
                {
                    DeleteAsTypeN<typename boost::mpl::next<Begin>::type,
                        End>::Delete(n, obj);
                }
            }
        };

        template <class End>
        struct DeleteAsTypeN<End, End>
        {
            static void Delete(size_t, void *)
            {
                throw std::range_error("Type not found in type list.");
            }
        };
    }

    template <class TypeVector>
    class Manager
    {
    public:
        template <typename KeyType, typename OwnerType>
        void Mark(const KeyType * key, OwnerType * owner)
        {
            typedef boost::mpl::find<TypeVector, OwnerType>::type TypeEntry;

            assert(entries.find(reinterpret_cast<const void *>(key)) == entries.end() &&
                "Key already marked.");

            // Error: TypeVector does not contain OwnerType
            BOOST_MPL_ASSERT_NOT((boost::is_same<TypeVector::end, TypeEntry>));

            entries.insert(std::make_pair(reinterpret_cast<const void *>(key),
                Details::FusionMemoryManagerEntry(owner,
                TypeEntry::pos::value)));
        }

        template <typename KeyType>
        void Free(const KeyType * key)
        {
            EntriesType::const_iterator entry = entries.find(
                reinterpret_cast<const void *>(key));
            assert(entry != entries.end() && "Key has not been marked."); 

            Details::DeleteAsTypeN<
                TypeVector::begin, TypeVector::end
                >::Delete(entry->second.ownerTypeId,
                entry->second.owner);

            entries.erase(reinterpret_cast<const void *>(key));

        }

    private:
        typedef std::map<const void *,
            Details::FusionMemoryManagerEntry> EntriesType;
        EntriesType entries;
    };
}

#endif
