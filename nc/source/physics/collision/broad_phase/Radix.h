///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains source code from the article "Radix Sort Revisited".
 *	\file		IceRevisitedRadix.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICERADIXSORT_H__
#define __ICERADIXSORT_H__

#define RADIX_LOCAL_RAM

class RadixSort
{
    public:
        RadixSort();
        ~RadixSort();
        RadixSort(const RadixSort&) = delete;
        RadixSort& operator=(const RadixSort&) = delete;

        // Sorting methods
        RadixSort& Sort(const unsigned* input, unsigned nb, bool signedvalues=true);
        RadixSort& Sort(const float* input, unsigned nb);

        //! Access to results. mRanks is a list of indices in sorted order, i.e. in the order you may further process your data
        unsigned* GetRanks() const { return mRanks; }

        //! mIndices2 gets trashed on calling the sort routine, but otherwise you can recycle it the way you want.
        unsigned* GetRecyclable() const { return mRanks2; }

        // Stats
        unsigned GetUsedRam() const;
        unsigned GetNbTotalCalls() const { return mTotalCalls; }
        unsigned GetNbHits() const { return mNbHits; }

    private:
#ifndef RADIX_LOCAL_RAM
        unsigned* mHistogram; //!< Counters for each byte
        unsigned* mOffset;    //!< Offsets (nearly a cumulative distribution function)
#endif
        unsigned mCurrentSize; //!< Current size of the indices list
        unsigned* mRanks;      //!< Two lists, swapped each pass
        unsigned* mRanks2;
        unsigned mTotalCalls;
        unsigned mNbHits;

        void CheckResize(unsigned nb);
        bool Resize(unsigned nb);
};

#endif